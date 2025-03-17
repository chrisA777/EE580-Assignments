//---------------------------------------------------------
// main example 1
//---------------------------------------------------------
#include "hellocfg.h"  // BIOS include file
#include "framework.h"
#include "data.h"

#define BUFFER_SIZE 32000        // 4 seconds of audio at 8kHz

int16_t buffer[BUFFER_SIZE];     // Circular buffer
uint16_t write_index = 0;        // Buffer write pointer
uint16_t read_index = 0;         // Buffer read pointer

// filter memory
float y_history[N_IIR_high_A] = {0.0};   // past outputs
float x_history[N_IIR_high_B] = {0.0};;  // past inputs


//---------------------------------------------------------
//---------------------------------------------------------
void main(void)
{

    // Ensure all elements in history arrays are set to zero
    memset(y_history, 0, sizeof(y_history));
    memset(x_history, 0, sizeof(x_history));

    initAll();
    return;  // Return to BIOS scheduler
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

//---------------------------------------------------------
// Interrupt Handler for Audio Processing
//---------------------------------------------------------
void audioHWI(void) {

    // Read in current sample
    int16_t s16;
    s16 = read_audio_sample();

    // Store sample in circular buffer
    buffer[write_index] = s16;
    write_index = (write_index + 1) % BUFFER_SIZE;  // Wrap around if needed

    // Read from buffer for playback
    float playback_sample = (float)buffer[read_index];
    read_index = (read_index + 1) % BUFFER_SIZE;  // Wrap around if needed


    if (MCASP->RSLOT)
    {
        // THIS IS THE LEFT CHANNEL!!!

        // Apply high-Pass Filter
        playback_sample = applyIIRFilter(playback_sample, x_history, y_history, IIR_high_B, IIR_high_A, N_IIR_high_B);



    }
    else {
        // THIS IS THE RIGHT CHANNEL!!!
        //s16 = process(s16);
    }

    write_audio_sample((int16_t)playback_sample);


}
