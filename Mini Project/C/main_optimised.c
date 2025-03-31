#include <std.h>
#include "stdio.h"
#include <stdlib.h>
#include "types.h"
#include "evmc6748.h"
#include "evmc6748_timer.h"
#include "evmc6748_i2c.h"
#include "evmc6748_led.h"
#include "evmc6748_dip.h"
#include "framework.h"
#include "maincfg.h"
//#include "data.h"
//#include "data_sos.h"
#include "data_sos_q14.h"
#include "clk.h"

/*
 *  ======== main ========
 */

//#define BUFFER_SIZE 32000        // >4 seconds of audio at 8kHz
#define BUFFER_SIZE 32768
#define BUFFER_MOD 32767
#define BUFFER_SIZE_2 16383

#define Q14_SHIFT 14
#define INT16_MAX_VAL 32767
#define INT16_MIN_VAL -32768

#define NUM_BIQUADS_LOW 7
#define NUM_BIQUADS_BP 8
#define NUM_BIQUADS_HIGH 7

#define LOW_MASK (1 << 0)  // 0b0001
#define BP_MASK (1 << 1)  // 0b0010
#define HIGH_MASK (1 << 2)  // 0b0100

#define DIP_1_MASK (1<<DIP_1)
#define DIP_2_MASK (1<<DIP_2)
#define DIP_6_MASK (1<<DIP_6)
#define DIP_7_MASK (1<<DIP_7)
#define DIP_8_MASK (1<<DIP_8)

uint8_t filter_state = 0;

volatile int16_t buffer[BUFFER_SIZE];           // Circular buffer
volatile int16_t filtered_buffer[BUFFER_SIZE_2];  // Buffer for filtered outputs
volatile uint16_t write_index = 0;              // Circular Buffer write pointer
volatile uint16_t write_index_2 = 0;
volatile uint16_t read_index = 0;               // Buffer read pointer

volatile uint8_t state = 0; //0: off, 1: recording, 2: filtering

volatile int32_t w_low[2 * NUM_BIQUADS_LOW];
volatile int32_t w_bp[2 * NUM_BIQUADS_BP];
volatile int32_t w_high[2 * NUM_BIQUADS_HIGH];

volatile int16_t s16;
volatile int16_t filtered_sample;
volatile int16_t playback_sample;

LgUns startTime, endTime, duration;

void main(void)
{
    // Ensure all elements in history arrays are set to zero
    memset(w_low, 0, sizeof(w_low));
    memset(w_bp, 0, sizeof(w_bp));
    memset(w_high, 0, sizeof(w_high));

    initAll();
    /* fall into DSP/BIOS idle loop */
    return;
}

// PRD to check state of dip switches called every 0.5 seconds
void dipPRD(void)
{
    // Get status of required DIP switches
    uint8_t dip_status1;
    uint8_t dip_status2;
    uint8_t dip_status6;
    uint8_t dip_status7;
    uint8_t dip_status8;

    DIP_get(DIP_1, &dip_status1);
    DIP_get(DIP_2, &dip_status2);
    DIP_get(DIP_6, &dip_status6);
    DIP_get(DIP_7, &dip_status7);
    DIP_get(DIP_8, &dip_status8);

    // Set state and filter variables as defined by switch positions
    if(dip_status1)
    {
        if(dip_status2)
        {
            state = 2;

            if(dip_status6)
            {
                filter_state |= LOW_MASK;
            }
            else
            {
                filter_state &= ~LOW_MASK;
            }

            if(dip_status7)
            {
                filter_state |= BP_MASK;
            }
            else
            {
                filter_state &= ~BP_MASK;
            }

            if(dip_status8)
            {
                filter_state |= HIGH_MASK;
            }
            else
            {
                filter_state &= ~HIGH_MASK;
            }
            // Toggle LED as required
            LED_toggle(LED_1);
        }
        else
        {
            state = 1;
        }
    }
    else
    {
        state = 0;
    }
}

// LED PRD @ 20 Hz
void ledPRD_20Hz(void)
{
    // Toggle both LEDs if in state 1 (recording)
    if (state == 1)
    {
        LED_toggle(LED_1);
        LED_toggle(LED_2);
    }
}

// LED PRED @ 20 Hz
void ledPRD_6Hz(void)
{
    // Toggle LED 2 if in state 2 and at least 1 filter is active
    if (state == 2 && filter_state > 0)
    {
        LED_toggle(LED_2);
    }

}

// Function to apply a single biquad to a signal
//#pragma FUNCTION_OPTIONS(apply_biquad_filter_q14, "--opt_level=3")
int16_t apply_biquad_filter_q14(int16_t *b, int16_t *a, volatile int32_t *w, int16_t gain, int16_t x)
{
    // need to declare new vara which are 32 bit instead now
    // Q14 * Q14 = Q28 (2^28)
    int32_t x_scaled = ((int32_t)x * gain) >> Q14_SHIFT; // shift right to bring back to q14 scale (/2^14)

    int32_t w0 = x_scaled
                 - ((int32_t)a[1] * w[0] >> Q14_SHIFT)
                 - ((int32_t)a[2] * w[1] >> Q14_SHIFT);

    // Compute output
    int32_t y = ((int32_t)b[0] * w0 >> Q14_SHIFT)
              + ((int32_t)b[1] * w[0] >> Q14_SHIFT)
              + ((int32_t)b[2] * w[1] >> Q14_SHIFT);

    // Update delay registers
    w[1] = w[0];
    w[0] = w0;

    // Saturate results
    if (y > INT16_MAX_VAL)
        y = INT16_MAX_VAL;
    else if (y < INT16_MIN_VAL)
        y = INT16_MIN_VAL;

    return (int16_t)y;
}


/* Where:
* - b is a (1xN*3) array of all coefficients
* - a is a (1xN*3) array of all coefficients
* - w is a (1xN*2) array of all stored w values
* - G is a (1xN) array of all gains
* - N is the total number of biquads in cascade
* - x is the input sample
*/
//#pragma FUNCTION_OPTIONS(apply_sos_IIR_filter_q14, "--opt_level=3")
int16_t apply_sos_IIR_filter_q14(int16_t *b, int16_t *a, volatile int32_t *w, int16_t *G, int N, int16_t x)
{
    uint16_t i,j,k;
    int16_t y = 0;
    #pragma UNROLL(8)
    #pragma MUST_ITERATE(7, 8, 1)
    //#pragma LOOP_COUNT(7, 8, 1)
    //#pragma IVDEP
    for (i = j = k = 0; i < N; i++, j+=3, k+=2)
    {
        y = apply_biquad_filter_q14(b + j, a + j, w + k, G[i], x);
        x = y;
    }

    return y;
}

// Filtering software interrupt
void filterSWI0(void)  // SWI0
{
    startTime = CLK_gethtime();
    if (state == 2)
    {
        filtered_sample = 0;
        playback_sample = buffer[read_index];
//        read_index = (read_index + 1) % BUFFER_SIZE;  // Wrap around if needed
        read_index = (read_index + 1) & BUFFER_MOD;  // Wrap around if needed

        // Apply filters as set by switches
         if (filter_state & LOW_MASK)
             filtered_sample += apply_sos_IIR_filter_q14(IIR_low_B, IIR_low_A, w_low, IIR_low_G, NUM_BIQUADS_LOW, playback_sample);
         if (filter_state&BP_MASK)
             filtered_sample += apply_sos_IIR_filter_q14(IIR_bp_B, IIR_bp_A, w_bp, IIR_bp_G, NUM_BIQUADS_BP, playback_sample);
         if (filter_state&HIGH_MASK)
             filtered_sample += apply_sos_IIR_filter_q14(IIR_high_B, IIR_high_A, w_high, IIR_high_G, NUM_BIQUADS_HIGH, playback_sample);

        filtered_buffer[write_index_2] = (int16_t)filtered_sample;
        write_index_2 = (write_index_2 + 1) & BUFFER_SIZE_2;
        write_audio_sample(filtered_sample);
    }
    else if (state == 1)
    {
        buffer[write_index] = s16;
//        write_index = (write_index + 1) % BUFFER_SIZE;
        write_index = (write_index + 1) & BUFFER_MOD;
        write_audio_sample(s16);
    }
    else
    {
        // Output no audio if in state 0
        write_audio_sample(0);
    }

    endTime = CLK_gethtime();
    duration = endTime-startTime;
    LOG_printf(&trace, "Ticks: %d", duration);
}


// Audio input hardware interrupt
void audioHWI(void)
{
    // Read in the audio sample
    s16 = read_audio_sample();
    // If left channel post software interrupt
    // If right channel output nothing
    if (MCASP->RSLOT)
    {
        SWI_post(&SWI0);
    }
    else
    {
        write_audio_sample(0);
    }
}

/**
float apply_biquad_filter(float *b, float *a,  volatile float *w, float gain, volatile float x)
{
    x *= gain;
    float w0 = x - a[1]*w[0] - a[2]*w[1];
    float y = b[0]*w0 + b[1]*w[0] + b[2]*w[1];

    w[1] = w[0];
    w[0] = w0;
//    y *= gain;
    return y;
}


 * Where:
 * - b is a (1xN*3) array of all coefficients
 * - a is a (1xN*3) array of all coefficients
 * - w is a (1xN*2) array of all stored w values
 * - G is a (1xN) array of all gains
 * - N is the total number of biquads in cascade
 * - x is the input sample


float apply_sos_IIR_filter(float *b, float *a,  volatile float *w, float *G, int N, volatile float x)
{
    int i;
    float y;

    for (i=0;i<N;i++)
    {
        y = apply_biquad_filter(b+(3*i), a+(3*i), w+(2*i), G[i], x);
        x = y;
    }

    return y;
}

**/
