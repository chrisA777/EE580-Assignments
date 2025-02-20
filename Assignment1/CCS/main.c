

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

void generate_input_samples(const int *num, int len, int N, int *signal)
{
    int i = 0;
    for (i = 0; i < N * len; i++)
    {
        signal[i] = num[i % len];
    }
}

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

void zero_mean(const int *signal, float mean, int N, float *zero_mu_signal)
{

    int i;

    for (i = 0; i < N; i++)
    {
        zero_mu_signal[i] = (float)signal[i] - mean;
    }
}

void fir_filter(const float *x, float *y, int N, const float *b, int M)
{
    for (int n = 0; n < N; n++)
    {
        y[n] = 0.0f;
        for (int k = 0; k < M; k++)
        {
            if (n >= k)
            {
                y[n] += b[k] * x[n - k];
            }
        }
    }
}

void write_to_file(const char *filename, const float *signal, int length)
{
    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        printf("Error opening file %s\n", filename);
        return;
    }

    for (int i = 0; i < length; i++)
    {
        fprintf(file, "%f\n", signal[i]);
    }

    fclose(file);
}

int main(void)
{

    int input_signal_1[SIGNAL_LENGTH];
    int input_signal_2[SIGNAL_LENGTH];

    // Generate input signals from student numbers
    generate_input_samples(student_number_1, STUDENT_NUMBER_LENGTH, INPUT_SIGNAL_REPEAT, input_signal_1);
    generate_input_samples(student_number_2, STUDENT_NUMBER_LENGTH, INPUT_SIGNAL_REPEAT, input_signal_2);

    // Calc mean
    float mean1 = calculate_mean(input_signal_1, SIGNAL_LENGTH);
    float mean2 = calculate_mean(input_signal_2, SIGNAL_LENGTH);

    // Zero mean signals
    float zero_mu_signal_1[SIGNAL_LENGTH];
    float zero_mu_signal_2[SIGNAL_LENGTH];
    zero_mean(input_signal_1, mean1, SIGNAL_LENGTH, zero_mu_signal_1);
    zero_mean(input_signal_2, mean2, SIGNAL_LENGTH, zero_mu_signal_2);

    // apply filter
    float filtered_signal_1[SIGNAL_LENGTH];
    float filtered_signal_2[SIGNAL_LENGTH];

    fir_filter(zero_mu_signal_1, filtered_signal_1, SIGNAL_LENGTH, b_fir_1, N_FIR_B_1);
    fir_filter(zero_mu_signal_2, filtered_signal_2, SIGNAL_LENGTH, b_fir_2, N_FIR_B_2);

    // write 2 file
    write_to_file("filtered_signal_1.txt", filtered_signal_1, SIGNAL_LENGTH);
    write_to_file("filtered_signal_2.txt", filtered_signal_2, SIGNAL_LENGTH);

    return 0;
}
