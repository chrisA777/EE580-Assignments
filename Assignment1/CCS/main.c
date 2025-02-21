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
float calculate_mean(int *signal, int N)
{

    float result, sum = 0;
    int i;

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
 * - mean : mean to subtract
 * - N : length of signal
 * - zero_mu_signal : output array pointer of resulting zero mean signal
 */
void zero_mean(const int *signal, float mean, int N, float *zero_mu_signal)
{
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
 * - y : output signal array of length M+N-1
 */
void conv(const float *x, int N, const float *b, int M float *y)
{
    int conv_len = M+N-1;
    int n;
    for (n = 0; n < conv_len; n++)
    {
        y[n] = 0.0f;

        int k;
        for  (k = 0; k < M; k++)
        {
            if (n >= k && (n-k)<N)
            {
                y[n] += b[k] * x[n - k];
            }
        }
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
    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        printf("Error opening file %s\n", filename);
        return;
    }

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

    // Calculate mean
    float mean1 = calculate_mean(input_signal_1, SIGNAL_LENGTH);
    float mean2 = calculate_mean(input_signal_2, SIGNAL_LENGTH);

    // Zero mean signals
    float zero_mu_signal_1[SIGNAL_LENGTH];
    float zero_mu_signal_2[SIGNAL_LENGTH];
    zero_mean(input_signal_1, mean1, SIGNAL_LENGTH, zero_mu_signal_1);
    zero_mean(input_signal_2, mean2, SIGNAL_LENGTH, zero_mu_signal_2);

    // Filter signals
    float filtered_signal_1[SIGNAL_LENGTH+N_FIR_B_1-1];
    float filtered_signal_2[SIGNAL_LENGTH+N_FIR_B_2-1];

    conv(zero_mu_signal_1, SIGNAL_LENGTH, b_fir_1, N_FIR_B_1, filtered_signal_1);
    conv(zero_mu_signal_2, SIGNAL_LENGTH, b_fir_2, N_FIR_B_2, filtered_signal_2);

    // Write to file
    write_to_file("filtered_signal_1.txt", filtered_signal_1, SIGNAL_LENGTH);
    write_to_file("filtered_signal_2.txt", filtered_signal_2, SIGNAL_LENGTH);

    return 0;
}
