#include "ILBatchCalculationComplexUtility.h"
#include <math.h>
#include <string>
#include <string.h>
#include "../Common/Utility.h"

using namespace std;

namespace
{
    const int CONST_MAX_LINE_LEN = 90;
}

int FFT::rfft_1d_f32_Ooura8g::generateDataFile(std::string type, int arraySize, std::string dataVariablePrefix, std::string outputFileName)
{
    int n = arraySize;

    int ipLen = 2 + sqrt(n / 2);
    int* ip = (int*)malloc(ipLen * sizeof(int));
    for(int i = 0; i < ipLen; i++)
        ip[i] = 0;
    int wLen = n / 2;
    float* w = (float*)malloc(wLen * sizeof(float));
    for(int i = 0; i < wLen; i++)
        w[i] = 0;

    generateData(ip, w, n);

    string exePath = getExeDirPath();
    string outputFilePath = exePath + "/temp/" + outputFileName;

    int breakLineLen = 0;
    string outputFileData = "int " + dataVariablePrefix + "_ip[" + to_string(ipLen) + "] = {";
    for(int i = 0; i < ipLen; i++) {
        if(i != 0)
            outputFileData += ", ";
        if(outputFileData.length() - breakLineLen > CONST_MAX_LINE_LEN) {
            outputFileData += "\n";
            breakLineLen = outputFileData.length();
        }
        outputFileData += to_string(ip[i]);
    }
    outputFileData += "};\n";

    outputFileData += "float " + dataVariablePrefix + "_w[" + to_string(wLen) + "] = {";
    for(int i = 0; i < wLen; i++) {
        if(i != 0)
            outputFileData += ", ";
        if(outputFileData.length() - breakLineLen > CONST_MAX_LINE_LEN) {
            outputFileData += "\n";
            breakLineLen = outputFileData.length();
        }
        outputFileData += to_string(w[i]);
    }
    outputFileData += "};\n";

    writeFile(outputFilePath, outputFileData);
    
    free(ip);
    free(w);

    return 1;
}

void FFT::rfft_1d_f32_Ooura8g::generateData(int* ip, float* w, int n)
{
    int nw, nc;
    float xi;
    nw = ip[0];
    if (n > (nw << 2)) {
        nw = n >> 2;
        makewt(nw, ip, w);
    }
    nc = ip[1];
    if (n > (nc << 2)) {
        nc = n >> 2;
        makect(nc, ip, w + nw);
    }
}

static void FFT::rfft_1d_f32_Ooura8g::makewt(int nw, int *ip, float *w)
{
    int j, nwh;
    float delta, x, y;
    
    ip[0] = nw;
    ip[1] = 1;
    if (nw > 2) {
        nwh = nw >> 1;
        delta = atan(1.0) / nwh;
        w[0] = 1;
        w[1] = 0;
        w[nwh] = cos(delta * nwh);
        w[nwh + 1] = w[nwh];
        if (nwh > 2) {
            for (j = 2; j < nwh; j += 2) {
                x = cos(delta * j);
                y = sin(delta * j);
                w[j] = x;
                w[j + 1] = y;
                w[nw - j] = y;
                w[nw - j + 1] = x;
            }
            for (j = nwh - 2; j >= 2; j -= 2) {
                x = w[2 * j];
                y = w[2 * j + 1];
                w[nwh + j] = x;
                w[nwh + j + 1] = y;
            }
            bitrv2(nw, ip + 2, w);
        }
    }
}

static void FFT::rfft_1d_f32_Ooura8g::makect(int nc, int *ip, float *c)
{
    int j, nch;
    float delta;
    
    ip[1] = nc;
    if (nc > 1) {
        nch = nc >> 1;
        delta = atan(1.0) / nch;
        c[0] = cos(delta * nch);
        c[nch] = 0.5 * c[0];
        for (j = 1; j < nch; j++) {
            c[j] = 0.5 * cos(delta * j);
            c[nc - j] = 0.5 * sin(delta * j);
        }
    }
}

static void FFT::rfft_1d_f32_Ooura8g::bitrv2(int n, int *ip, float *a)
{
    int j, j1, k, k1, l, m, m2;
    float xr, xi, yr, yi;
    
    ip[0] = 0;
    l = n;
    m = 1;
    while ((m << 3) < l) {
        l >>= 1;
        for (j = 0; j < m; j++) {
            ip[m + j] = ip[j] + l;
        }
        m <<= 1;
    }
    m2 = 2 * m;
    if ((m << 3) == l) {
        for (k = 0; k < m; k++) {
            for (j = 0; j < k; j++) {
                j1 = 2 * j + ip[k];
                k1 = 2 * k + ip[j];
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += m2;
                k1 += 2 * m2;
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += m2;
                k1 -= m2;
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += m2;
                k1 += 2 * m2;
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
            }
            j1 = 2 * k + m2 + ip[k];
            k1 = j1 + m2;
            xr = a[j1];
            xi = a[j1 + 1];
            yr = a[k1];
            yi = a[k1 + 1];
            a[j1] = yr;
            a[j1 + 1] = yi;
            a[k1] = xr;
            a[k1 + 1] = xi;
        }
    } else {
        for (k = 1; k < m; k++) {
            for (j = 0; j < k; j++) {
                j1 = 2 * j + ip[k];
                k1 = 2 * k + ip[j];
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += m2;
                k1 += m2;
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
            }
        }
    }
}


int DCT::dct_1d_f32_Ooura8g::generateDataFile(std::string type, int arraySize, std::string dataVariablePrefix, std::string outputFileName)
{
    int n = arraySize;

    int ipLen = 2 + sqrt(n / 2);
    int* ip = (int*)malloc(ipLen * sizeof(int));
    for(int i = 0; i < ipLen; i++)
        ip[i] = 0;
    int wLen = n * 5 / 4;
    float* w = (float*)malloc(wLen * sizeof(float));
    for(int i = 0; i < wLen; i++)
        w[i] = 0;
    
    generateData(ip, w, n);
    
    string exePath = getExeDirPath();
    string outputFilePath = exePath + "/temp/" + outputFileName;

    string outputFileData;
    int breakLineLen = 0;
    outputFileData += "int " + dataVariablePrefix + "_ip[" + to_string(ipLen) + "] = {";
    for(int i = 0; i < ipLen; i++) {
        if(i != 0)
            outputFileData += ", ";
        if(outputFileData.length() - breakLineLen > CONST_MAX_LINE_LEN) {
            outputFileData += "\n";
            breakLineLen = outputFileData.length();
        }
        outputFileData += to_string(ip[i]);
    }
    outputFileData += "};\n";

    outputFileData += "float " + dataVariablePrefix + "_w[" + to_string(wLen) + "] = {";
    for(int i = 0; i < wLen; i++) {
        if(i != 0)
            outputFileData += ", ";
        if(outputFileData.length() - breakLineLen > CONST_MAX_LINE_LEN) {
            outputFileData += "\n";
            breakLineLen = outputFileData.length();
        }
        outputFileData += to_string(w[i]);
    }
    outputFileData += "};\n";

    writeFile(outputFilePath, outputFileData);
    
    free(ip);
    free(w);

    return 1;
}

void DCT::dct_1d_f32_Ooura8g::generateData(int* ip, float* w, int n)
{
    int nw, nc;
    nw = ip[0];
    if (n > (nw << 2)) {
        nw = n >> 2;
        makewt(nw, ip, w);
    }
    nc = ip[1];
    if (n > nc) {
        nc = n;
        makect(nc, ip, w + nw);
    }
}

static void DCT::dct_1d_f32_Ooura8g::makewt(int nw, int *ip, float *w)
{
    int j, nwh;
    float delta, x, y;
    
    ip[0] = nw;
    ip[1] = 1;
    if (nw > 2) {
        nwh = nw >> 1;
        delta = atan(1.0) / nwh;
        w[0] = 1;
        w[1] = 0;
        w[nwh] = cos(delta * nwh);
        w[nwh + 1] = w[nwh];
        if (nwh > 2) {
            for (j = 2; j < nwh; j += 2) {
                x = cos(delta * j);
                y = sin(delta * j);
                w[j] = x;
                w[j + 1] = y;
                w[nw - j] = y;
                w[nw - j + 1] = x;
            }
            for (j = nwh - 2; j >= 2; j -= 2) {
                x = w[2 * j];
                y = w[2 * j + 1];
                w[nwh + j] = x;
                w[nwh + j + 1] = y;
            }
            bitrv2(nw, ip + 2, w);
        }
    }
}

static void DCT::dct_1d_f32_Ooura8g::makect(int nc, int *ip, float *c)
{
    int j, nch;
    float delta;
    
    ip[1] = nc;
    if (nc > 1) {
        nch = nc >> 1;
        delta = atan(1.0) / nch;
        c[0] = cos(delta * nch);
        c[nch] = 0.5 * c[0];
        for (j = 1; j < nch; j++) {
            c[j] = 0.5 * cos(delta * j);
            c[nc - j] = 0.5 * sin(delta * j);
        }
    }
}

static void DCT::dct_1d_f32_Ooura8g::bitrv2(int n, int *ip, float *a)
{
    int j, j1, k, k1, l, m, m2;
    float xr, xi, yr, yi;
    
    ip[0] = 0;
    l = n;
    m = 1;
    while ((m << 3) < l) {
        l >>= 1;
        for (j = 0; j < m; j++) {
            ip[m + j] = ip[j] + l;
        }
        m <<= 1;
    }
    m2 = 2 * m;
    if ((m << 3) == l) {
        for (k = 0; k < m; k++) {
            for (j = 0; j < k; j++) {
                j1 = 2 * j + ip[k];
                k1 = 2 * k + ip[j];
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += m2;
                k1 += 2 * m2;
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += m2;
                k1 -= m2;
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += m2;
                k1 += 2 * m2;
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
            }
            j1 = 2 * k + m2 + ip[k];
            k1 = j1 + m2;
            xr = a[j1];
            xi = a[j1 + 1];
            yr = a[k1];
            yi = a[k1 + 1];
            a[j1] = yr;
            a[j1 + 1] = yi;
            a[k1] = xr;
            a[k1 + 1] = xi;
        }
    } else {
        for (k = 1; k < m; k++) {
            for (j = 0; j < k; j++) {
                j1 = 2 * j + ip[k];
                k1 = 2 * k + ip[j];
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += m2;
                k1 += m2;
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
            }
        }
    }
}

