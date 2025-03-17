//#include "hellocfg.h"
#include "stdio.h"
#include <stdlib.h>
#include "types.h"
#include "evmc6748.h"
#include "evmc6748_timer.h"
#include "evmc6748_i2c.h"
#include "evmc6748_led.h"
#include "evmc6748_dip.h"
#include "framework.h"
#include "data.h"

/*
 *  ======== main ========
 */

#define BUFFER_SIZE 32000        // 4 seconds of audio at 8kHz

int16_t buffer[BUFFER_SIZE];     // Circular buffer
uint16_t write_index = 0;        // Buffer write pointer
uint16_t read_index = 0;         // Buffer read pointer

// filter memory

#define MAX_FILTER_LENGTH 33
float y_history[MAX_FILTER_LENGTH] = {0.0};   // past outputs
float x_history[MAX_FILTER_LENGTH] = {0.0};   // past inputs

float B[MAX_FILTER_LENGTH] = {0.0};
float A[MAX_FILTER_LENGTH] = {0.0};

uint8_t state = 0; //0: off, 1: recording, 2: filtering
float playback_sample;

void main(void)
{
    // Ensure all elements in history arrays are set to zero
    memset(y_history, 0, sizeof(y_history));
    memset(x_history, 0, sizeof(x_history));

    memset(B, 0, sizeof(B));
    memset(A, 0, sizeof(A));

    initAll();
    /* fall into DSP/BIOS idle loop */
    return;
}

void dipPRD(void)
{
    // Can we get DIP status with HWI??
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

    if (dip_status1)
    {
        if (dip_status2)
        {
            state = 2;
//            if (dip_status6)
//            {
//                conv(B, MAX_FILTER_LENGTH, IIR_low_B, N_IIR_low_B, B, 0);
//                conv(A, MAX_FILTER_LENGTH, IIR_low_A, N_IIR_low_A, B, 0);
//            }
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

void ledPRD(void)
{
    if (state == 1)
    {
        LED_toggle(LED_1);
        LED_toggle(LED_2);
    }
}

//---------------------------------------------------------
// filtering
//---------------------------------------------------------
float applyIIRFilter(float x, float *x_hist, float *y_hist, const float *b, const float *a, int order) {
    float y = 0.0;
    int i;


    // shift history for input samples
    for (i = order - 1; i > 0; i--) {
        x_hist[i] = x_hist[i - 1];
    }
    x_hist[0] = x;


    // difference equation
    for (i = 0; i < order; i++) {
        y += b[i] * x_hist[i];
        if (i > 0) {
            y -= a[i] * y_hist[i - 1];
        }
    }

    // shift history for output samples
    for (i = order - 1; i > 0; i--) {
        y_hist[i] = y_hist[i - 1];
    }

    y_hist[0] = y;

    return y;
}


void audioHWI(void)
{
    int16_t s16;
    s16 = read_audio_sample();


    switch(state)
    {
        case 0:
            write_audio_sample(0);
            break;
        case 1:
            if (MCASP->RSLOT)
            {
                // Left audio channel
                // Store sample in circular buffer
                buffer[write_index] = s16;
                write_index = (write_index + 1) % BUFFER_SIZE;  // Wrap around if needed
                write_audio_sample(s16);
            }
            else
            {
                // right audio channel
                // Need to output something?
                write_audio_sample(0);
            }
            break;
        case 2:

            if (MCASP->RSLOT)
            {
                // Left audio channel
                // Read from buffer for playback
                playback_sample = (float)buffer[read_index];
                read_index = (read_index + 1) % BUFFER_SIZE;  // Wrap around if needed

//                playback_sample = applyIIRFilter(playback_sample, x_history, y_history, IIR_low_B, IIR_low_A, N_IIR_low_B);

                write_audio_sample((int16_t)playback_sample);

            }
            else
            {
                // right audio channel
                // Need to output something?
                write_audio_sample(0);
            }
            break;
    }
}
