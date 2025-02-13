

/**
 * main.c
 */
#include <math.h>

# define STUDENT_NUMBER_LENGTH 9
# define INPUT_SIGNAL_REPEAT 100
# define CUT 4

const int student_number_1[STUDENT_NUMBER_LENGTH] = {2,0,2,0,3,7,6,9,6};
const int student_number_2[STUDENT_NUMBER_LENGTH] = {2,0,2,0,3,2,1,7,5};


void generate_input_samples(const int* num, int len,int N, int* signal)
{
    int i = 0;
    for(i=0;i<N*len;i++)
    {
        signal[i] = num[i%len];
    }
}

float calculate_mean(int* signal, int N){

    float sum = 0;
    int i;

    for (i=0; i<N; i++){
        sum += signal[i];
    }

    return sum/(float)N;
}

void zero_mean(const int* signal, float mean, int N, float* zero_mu_signal){

    int i;

    for (i=0; i<N; i++){
        zero_mu_signal[i] = (float)signal[i] - mean;
    }

}

void calculate_sampling_frequency(const int* num1, const int* num2, int len, int N)
{
    int L = len-N;
//    int* arr1 = (int*)malloc(N);
//    int* arr2 = (int*)malloc(N);
    int arr1[CUT];
    int arr2[CUT];

    //Get last N numbers in array
    int i = 0;
    for (i=L;i<len;i++)
    {
        arr1[i-L] = num1[i];
        arr2[i-L] = num2[i];
    }

    //
    int n1 = 0;
    int n2 = 0;

    double test = pow(10.0,2.0);

//    for (i=0;i<N;i++)
//    {
//        n1 += (int)pow(10, N-i+1)*arr1[i];
//        n2 += (int)pow(10, N-i+1)*arr2[i];
//    }

    free(arr1);
    free(arr2);
}

int main(void)
{

    int input_signal_1[STUDENT_NUMBER_LENGTH*INPUT_SIGNAL_REPEAT];
    generate_input_samples(student_number_1,STUDENT_NUMBER_LENGTH,INPUT_SIGNAL_REPEAT,input_signal_1);

    float mean = calculate_mean(input_signal_1, STUDENT_NUMBER_LENGTH*INPUT_SIGNAL_REPEAT);

    float zero_mu_signal_1[STUDENT_NUMBER_LENGTH*INPUT_SIGNAL_REPEAT];
    zero_mean(input_signal_1, mean, STUDENT_NUMBER_LENGTH*INPUT_SIGNAL_REPEAT, zero_mu_signal_1);




    int input_signal_2[STUDENT_NUMBER_LENGTH*INPUT_SIGNAL_REPEAT];
    generate_input_samples(student_number_2, STUDENT_NUMBER_LENGTH,INPUT_SIGNAL_REPEAT, input_signal_2);

    calculate_sampling_frequency(student_number_1,student_number_2,STUDENT_NUMBER_LENGTH,4);

    free(input_signal_1);
	return 0;
}
