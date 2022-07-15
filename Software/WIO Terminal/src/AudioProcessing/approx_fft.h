// see here: https://create.arduino.cc/projecthub/abhilashpatel121/approxfft-fastest-fft-function-for-arduino-fd4917
//-----------------------------FFT Function----------------------------------------------//
/*
Code to perform High speed and Accurate FFT on arduino,
setup:

1. in[]     : Data array, 
2. N        : Number of sample (recommended sample size 2,4,8,16,32,64,128,256,512...)
3. Frequency: sampling frequency required as input (Hz)

It will by default return frequency with max aplitude,
if you need complex output or magnitudes uncomment required sections

If sample size is not in power of 2 it will be clipped to lower side of number. 
i.e, for 150 number of samples, code will consider first 128 sample, remaining sample  will be omitted.
For Arduino nano, FFT of more than 256 sample not possible due to mamory limitation 
Code by ABHILASH
Contact: abhilashpatel121@gmail.com
Documentation & details: https://www.instructables.com/member/abhilash_patel/instructables/
*/
float Approx_FFT(int in[], int real[], int imag[], unsigned int N, float Frequency);
