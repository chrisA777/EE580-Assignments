//#include "hellocfg.h"
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
#include "data_sos_2.h"
#include "clk.h"

/*
 *  ======== main ========
 */

//#define BUFFER_SIZE 32000        // 4 seconds of audio at 8kHz
#define BUFFER_SIZE 32767

//#define NUM_BIQUADS_LOW 15
//#define NUM_BIQUADS_BP 16
//#define NUM_BIQUADS_HIGH 8

#define NUM_BIQUADS_LOW 4
#define NUM_BIQUADS_BP 8
#define NUM_BIQUADS_HIGH 4

#define LOW_MASK (1 << 0)  // 0b0001
#define BP_MASK (1 << 1)  // 0b0010
#define HIGH_MASK (1 << 2)  // 0b0100

#define DIP_1_MASK (1<<DIP_1)
#define DIP_2_MASK (1<<DIP_2)
#define DIP_6_MASK (1<<DIP_6)
#define DIP_7_MASK (1<<DIP_7)
#define DIP_8_MASK (1<<DIP_8)

#define STATE_MASK 3
#define FILTERS_MASK 224
#define STATE_0 0
#define STATE_1 1
#define STATE_2 3


uint8_t filter_state = 0;

volatile int16_t buffer[BUFFER_SIZE];     // Circular buffer
volatile uint16_t write_index = 0;        // Buffer write pointer
volatile uint16_t read_index = 0;         // Buffer read pointer

volatile uint8_t state = 0; //0: off, 1: recording, 2: filtering

volatile float w_low[2*NUM_BIQUADS_LOW];
volatile float w_bp[2*NUM_BIQUADS_BP];
volatile float w_high[2*NUM_BIQUADS_HIGH];


volatile int16_t s16;
volatile float filtered_sample;
volatile float playback_sample;
volatile float lp, bp, hp;



LgUns startTime, endTime, duration;

void main(void)
{
    // Ensure all elements in history arrays are set to zero
    memset(w_low, 0, sizeof(w_low));
    memset(w_bp, 0, sizeof(w_bp));
    memset(w_high, 0, sizeof(w_high));

    initAll();
//    LOG_printf(&trace, "State1 mask: %d, state 2 mask: %d", STATE_1_MASK, STATE_2_MASK);
//    LOG_printf(&trace, "1: %d, 2: %d",DIP_1_MASK, DIP_2_MASK);
//    LOG_printf(&trace, "6: %d, 7: %d",DIP_6_MASK, DIP_7_MASK);
//    LOG_printf(&trace, "dip all: %d, state mask: %d",dipAll, dipAll&STATE_MASK);
//    LOG_printf(&trace, "state1: %d, state2: %d",STATE_1_MASK, STATE_2_MASK);
    /* fall into DSP/BIOS idle loop */
    return;
}


void dipPRD(void)
{
//    startTime = CLK_gethtime();
//    uint32_t dipAll;
//    DIP_getAll(&dipAll);
//    dipAll = ~dipAll;

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

//    LOG_printf(&trace, "DIP ALL: %d", (dipAll&DIP_1_MASK)==DIP_1_MASK);
//    LOG_printf(&trace, "dip all: %d, state mask: %d",dipAll, dipAll&STATE_MASK);

//    endTime = CLK_gethtime();
//    duration = endTime-startTime;
//    LOG_printf(&trace, "Ticks: %d", duration);

//    LOG_printf(&trace, "Ticks: %d, %d", dipAll, DIP_1_MASK);

//    if (dipAll&DIP_1_MASK)
    if(dip_status1)
    {
//        if (dipAll&DIP_2_MASK)
        if(dip_status2)
        {
            state = 2;
//            if (dipAll&DIP_6_MASK)
            if(dip_status6)
            {
                filter_state |= LOW_MASK;
            }
            else
            {
                filter_state &= ~LOW_MASK;
            }

//            if (dipAll&DIP_7_MASK)
            if(dip_status7)
            {
                filter_state |= BP_MASK;
            }
            else
            {
                filter_state &= ~BP_MASK;
            }
//            if (dipAll&DIP_8_MASK)
            if(dip_status8)
            {
                filter_state |= HIGH_MASK;
            }
            else
            {
                filter_state &= ~HIGH_MASK;
            }
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

//    if ((dipAll&STATE_MASK) == 3)
//        LED_toggle(LED_1);
}

void ledPRD_20Hz(void)
{
    if (state == 1)
    {
        LED_toggle(LED_1);
        LED_toggle(LED_2);
    }
}

void ledPRD_6Hz(void)
{
//    LOG_printf(&trace,"6hz: %d", (FILTERS_MASK & dipAll));
    if (state == 2 && filter_state > 0)  // At least one filter is active
    {
        LED_toggle(LED_2);
    }

}


/*
 * OR just pass pointer to correct point in array for b,a and w
 *
 */
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

/*
 * Where:
 * - b is a (1xN*3) array of all coefficients
 * - a is a (1xN*3) array of all coefficients
 * - w is a (1xN*2) array of all stored w values
 * - G is a (1xN) array of all gains
 * - N is the total number of biquads in cascade
 * - x is the input sample
 */

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

void filterSWI0(void)  // SWI0
{
    startTime = CLK_gethtime();
    filtered_sample = 0;
    if (state == 2)
    {
        filtered_sample = 0;
        playback_sample = (float)buffer[read_index];
//        read_index = (read_index + 1) % BUFFER_SIZE;  // Wrap around if needed
        read_index = (read_index + 1) & BUFFER_SIZE;  // Wrap around if needed
//        SWI_post(&SWI1);
//        SWI_post(&SWI2);

//        if (dipAll&DIP_6_MASK)
//            filtered_sample += apply_sos_IIR_filter(IIR_low_B, IIR_low_A, w_low, IIR_low_G, NUM_BIQUADS_LOW, playback_sample);
//        if (dipAll&DIP_7_MASK)
//            filtered_sample += apply_sos_IIR_filter(IIR_bp_B, IIR_bp_A, w_bp, IIR_bp_G, NUM_BIQUADS_BP, playback_sample);
//        if (dipAll&DIP_8_MASK)
//            filtered_sample += apply_sos_IIR_filter(IIR_high_B, IIR_high_A, w_high, IIR_high_G, NUM_BIQUADS_HIGH, playback_sample);

         if (filter_state & LOW_MASK)
             filtered_sample += apply_sos_IIR_filter(IIR_low_B, IIR_low_A, w_low, IIR_low_G, NUM_BIQUADS_LOW, playback_sample);
         if (filter_state&BP_MASK)
             filtered_sample += apply_sos_IIR_filter(IIR_bp_B, IIR_bp_A, w_bp, IIR_bp_G, NUM_BIQUADS_BP, playback_sample);
         if (filter_state&HIGH_MASK)
             filtered_sample += apply_sos_IIR_filter(IIR_high_B, IIR_high_A, w_high, IIR_high_G, NUM_BIQUADS_HIGH, playback_sample);


        write_audio_sample((int16_t)filtered_sample);
    }
    else if (state == 1)
    {
        playback_sample = s16;
        buffer[write_index] = s16;
//        write_index = (write_index + 1) % BUFFER_SIZE;
        write_index = (write_index + 1) & BUFFER_SIZE;
        write_audio_sample((int16_t)playback_sample);
    }
    else
    {
        playback_sample = 0;
        write_audio_sample((int16_t)playback_sample);
    }
//    LOG_printf(&trace, "playback: %d, filtered: %d", (int)playback_sample, (int)filtered_sample);

    endTime = CLK_gethtime();
    duration = endTime-startTime;
    LOG_printf(&trace, "Ticks: %d", duration);
}


void audioHWI(void)
{
    s16 = read_audio_sample();
    if (MCASP->RSLOT)
    {
        SWI_post(&SWI0);
//        if (state == 2)
//            write_audio_sample((int16_t)filtered_sample);
//        else
//            write_audio_sample((int16_t)playback_sample);
//        write_audio_sample(0);
    }
    else
    {
        write_audio_sample(0);
    }
}
