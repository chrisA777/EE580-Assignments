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

void conv(const float *x, int N, const float *b, int M, float *y, int verbose)
{

    // Length of the output convolution result
    int conv_len = M+N-1;
    float *temp = (float *)malloc(conv_len * sizeof(float));
    memset(temp, 0, sizeof(temp));

    int n;
    // Iterate over each output sample
    for (n = 0; n < conv_len; n++)
    {
        // Initialize output value to zero before accumulation
        y[n] = 0.0f;

        int k;
        int first = 1;
        if (verbose)
            printf("\ny[%d] = ",n);
        // Iterate over second array coefficients
        for  (k = 0; k < M; k++)
        {
            // Ensure valid indices:
            // - n >= k ensures no negative indices of x
            // - (n - k) < N ensures input signal length not exceeded
            if (n >= k && (n-k)<N)
            {
                if (verbose)
                {
                    if (!first)
                        printf("+ ");
                    printf("(%.2f * %.2f) ",x[n-k],b[k]);
                }
                // Convolution accumulation
                temp[n] += b[k] * x[n - k];
                first = 0;
                fflush(stdout); // Force printing to console when stepping through loop
            }
        }
        if (verbose)
            printf("= %.2f",y[n]);
        fflush(stdout); // Force printing to console when stepping through loop
    }

    // Copy the result back to the original signal array
    int i;
    for (i = 0; i < conv_len; i++) {
        y[i] = temp[i];
    }
    free(temp);
}

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
            B[0] = 1.0f;
            A[0] = 1.0f;
            if (dip_status6)
            {
                conv(B, MAX_FILTER_LENGTH, IIR_high_B, N_IIR_high_B, B, 0);
                conv(A, MAX_FILTER_LENGTH, IIR_high_A, N_IIR_high_A, B, 0);
            }
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


/*
 * OR just pass pointer to correct point in array for b,a and w
 *
 */
float apply_biquad_filter(float *b, float *a, float *w, float gain, float x)
{
    // TODO: apply gain
    float w0 = x - a[0]*w[0] - a[1]*w[1];
    float y = b[0]*w0 + b[1]*w[0] + b[2]*w[1];

    w[1] = w[0];
    w[0] = w0;
    return y;
}

/*
 * Where:
 * - b is a (1xN*3) array of all coefficients
 * - a is a (1xN*2) array of all coefficients
 * - w is a (1xN*2) array of all stored w values
 * - G is a (1xN) array of all gains
 * - N is the total number of biquads in cascade
 * - x is the input sample
 */
float apply_sos_IIR_filter(float *b, float *a, float *w, float *G, int N,  float x)
{
    int i;
    for (i=1;i<N;i++)
    {
        x = apply_biquad_filter(b+(3*N), a+(2*N), w+(2*N), *(G+N), x);
    }

    return x;
}


//---------------------------------------------------------
// filtering
//---------------------------------------------------------
//float applyIIRFilter(float x, float *x_hist, float *y_hist, const float *b, const float *a, int order) {
//    float y = 0.0;
//    int i;
//
//
//    // shift history for input samples
//    for (i = order - 1; i > 0; i--) {
//        x_hist[i] = x_hist[i - 1];
//    }
//    x_hist[0] = x;
//
//
//    // difference equation
//    for (i = 0; i < order; i++) {
//        y += b[i] * x_hist[i];
//        if (i > 0) {
//            y -= a[i] * y_hist[i - 1];
//        }
//    }
//
//    // shift history for output samples
//    for (i = order - 1; i > 0; i--) {
//        y_hist[i] = y_hist[i - 1];
//    }
//
//    y_hist[0] = y;
//
//    return y;
//}


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
//                playback_sample = applyIIRFilter(playback_sample, x_history, y_history, B, A, MAX_FILTER_LENGTH);

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
