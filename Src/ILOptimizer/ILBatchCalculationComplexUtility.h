#pragma once
#include <map>
#include <string>
#include <vector>

namespace FFT
{
    namespace rfft_1d_f32_Ooura8g
    {
        int generateDataFile(std::string type, int arraySize, std::string dataVariablePrefix, std::string outputFileName);
        void generateData(int* ip, float* w, int n);
        static void makect(int nc, int *ip, float *c);
        static void makewt(int nw, int *ip, float *w);
        static void bitrv2(int n, int *ip, float *a);
    }
}

namespace DCT
{
    namespace dct_1d_f32_Ooura8g
    {
        int generateDataFile(std::string type, int arraySize, std::string dataVariablePrefix, std::string outputFileName);
        void generateData(int* ip, float* w, int n);
        static void makect(int nc, int *ip, float *c);
        static void makewt(int nw, int *ip, float *w);
        static void bitrv2(int n, int *ip, float *a);
    }
}
