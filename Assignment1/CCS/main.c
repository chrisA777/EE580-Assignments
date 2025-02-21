/**
 * main.c
 */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "data.h"

#define STUDENT_NUMBER_LENGTH 9
#define INPUT_SIGNAL_REPEAT 100
#define SIGNAL_LENGTH (STUDENT_NUMBER_LENGTH * INPUT_SIGNAL_REPEAT)

const int student_number_1[STUDENT_NUMBER_LENGTH] = {2, 0, 2, 0, 3, 7, 6, 9, 6};
const int student_number_2[STUDENT_NUMBER_LENGTH] = {2, 0, 2, 0, 3, 2, 1, 7, 5};


/* Generates array of {num} array of length {len} repeated {N} times and assigns it to {signal}.
 *
 * Parameters:
 * - num : Array of integers to be repeated
 * - len : Length of num array
 * - N : Number of times to repeat num
 * - signal : Output signal of repeated num
 */
void generate_input_samples(const int *num, int len, int N, int *signal)
{
    int i = 0;
    for (i = 0; i < N * len; i++)
    {
        signal[i] = num[i % len];
    }
}

/* Calculates mean of integer array.
 *
 * Parameters:
 * - signal : signal to calculate mean of
 * - N : length of signal
 */
float calculate_mean(const int *signal, int N)
{
    float result, sum = 0;
    int i;
    // Accumulate signal -- risk of overflow if sum>FLOAT MAX
    for (i = 0; i < N; i++)
    {
        sum += signal[i];
    }

    result = sum / N;
    i = 0;
    return result;
}

/* Subtracts mean from signal.
 *
 * Parameters:
 * - signal : signal to subtract from
 * - N : length of signal
 * - zero_mu_signal : output array pointer of resulting zero mean signal
 */
void zero_mean(const int *signal, int N, float *zero_mu_signal)
{
    float mean = calculate_mean(signal,N);
    // subtract mean from each sample in signal array
    int i;
    for (i = 0; i < N; i++)
    {
        zero_mu_signal[i] = (float)signal[i] - mean;
    }
}

/* Convolves signal x with b.
 *
 * Parameters:
 * - x : first input array
 * - N : length of x array
 * - b : second input array
 * - M : length of b array
 * - y : output signal array of length => M+N-1
 */
void conv(const float *x, int N, const float *b, int M, float *y, int verbose)
{
    // Length of the output convolution result
    int conv_len = M+N-1;
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
                y[n] += b[k] * x[n - k];
                first = 0;
            }
        }
        if (verbose)
            printf("= %.2f",y[n]);
    }
}

/* Writes signal to file.
 *
 * Parameters:
 * - filename : Name of file to write signal to
 * - signal : signal array pointer
 * - length : length of signal
 */
void write_to_file(const char *filename, const float *signal, int length)
{
    // Open file and return if error
    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        printf("Error opening file %s\n", filename);
        return;
    }

    // Loop through signal and save each sample to a new line in file
    int i;
    for (i = 0; i < length; i++)
    {
        fprintf(file, "%f\n", signal[i]);
    }

    fclose(file);
}

int main(void)
{
    // Initialise signals
    int input_signal_1[SIGNAL_LENGTH];
    int input_signal_2[SIGNAL_LENGTH];

    // Generate input signals from student numbers
    generate_input_samples(student_number_1, STUDENT_NUMBER_LENGTH, INPUT_SIGNAL_REPEAT, input_signal_1);
    generate_input_samples(student_number_2, STUDENT_NUMBER_LENGTH, INPUT_SIGNAL_REPEAT, input_signal_2);

    // Zero mean signals
    float zero_mu_signal_1[SIGNAL_LENGTH];
    float zero_mu_signal_2[SIGNAL_LENGTH];
    zero_mean(input_signal_1, SIGNAL_LENGTH, zero_mu_signal_1);
    zero_mean(input_signal_2, SIGNAL_LENGTH, zero_mu_signal_2);

    // Test convolution
    float test_signal_1[4] = {1,2,3,4};
    float test_signal_2[4] = {5,6,7,8};
    float test_output[7];
    conv(test_signal_1, 4, test_signal_2, 4, test_output,1);

    // Filter signals
    float filtered_signal_1[SIGNAL_LENGTH+N_FIR_B_1-1];
    float filtered_signal_2[SIGNAL_LENGTH+N_FIR_B_2-1];
    conv(zero_mu_signal_1, SIGNAL_LENGTH, b_fir_1, N_FIR_B_1, filtered_signal_1,0);
    conv(zero_mu_signal_2, SIGNAL_LENGTH, b_fir_2, N_FIR_B_2, filtered_signal_2,0);

    // Write to file
    write_to_file("filtered_signal_1.txt", filtered_signal_1, SIGNAL_LENGTH+N_FIR_B_1-1);
    write_to_file("filtered_signal_2.txt", filtered_signal_2, SIGNAL_LENGTH+N_FIR_B_2-1);

    return 0;
}
