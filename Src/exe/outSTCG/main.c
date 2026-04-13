#include "math"
#include "float"
#define RT_PI 3.14159265358979323846
#define RT_PIF 3.1415927F
#define RT_LN_10 2.30258509299404568402
#define RT_LN_10F 2.3025851F
#define RT_LOG10E 0.43429448190325182765
#define RT_LOG10EF 0.43429449F
#define RT_E 2.7182818284590452354
#define RT_EF 2.7182817F
#define false 0
#define true 1
#define NULL 0
#define UNUSED_PARAMETER(x) (void)(x)
#define rtmGetErrorStatus(rtm) ((rtm)->errorStatus)
#define rtmSetErrorStatus(rtm,val) ((rtm)->errorStatus=(val))
#define POS_ZCSIG 1
#define MAX_int8_T ((int8_T)(127))
#define MIN_int8_T ((int8_T)(-128))
#define MAX_uint8_T ((uint8_T)(255U))
#define MAX_int16_T ((int16_T)(32767))
#define MIN_int16_T ((int16_T)(-32768))
#define MAX_uint16_T ((uint16_T)(65535U))
#define MAX_int32_T ((int32_T)(2147483647))
#define MIN_int32_T ((int32_T)(-2147483647-1))
#define MAX_uint32_T ((uint32_T)(0xFFFFFFFFU))
typedef signed char int8_T;
typedef unsigned char uint8_T;
typedef short int16_T;
typedef unsigned short uint16_T;
typedef int int32_T;
typedef unsigned int uint32_T;
typedef float real32_T;
typedef double real64_T;
typedef double real_T;
typedef double time_T;
typedef unsigned char boolean_T;
typedef int int_T;
typedef unsigned int uint_T;
typedef unsigned long ulong_T;
typedef char char_T;
typedef unsigned char uchar_T;
typedef char_T byte_T;
typedef struct creal32_T creal32_T;
typedef struct creal64_T creal64_T;
typedef struct creal_T creal_T;
typedef struct cint8_T cint8_T;
typedef struct cuint8_T cuint8_T;
typedef struct cint16_T cint16_T;
typedef struct cuint16_T cuint16_T;
typedef struct cint32_T cint32_T;
typedef struct cuint32_T cuint32_T;
typedef void * pointer_T;
typedef int ZCEventType;
typedef struct _ssMdlInfo _ssMdlInfo;
typedef uint8_T ZCSigState;
typedef struct tag_RTM_AM_Mod_Demod_T RT_MODEL_AM_Mod_Demod_T;
typedef struct DW_AM_Mod_Demod_T DW_AM_Mod_Demod_T;
typedef struct ConstP_AM_Mod_Demod_T ConstP_AM_Mod_Demod_T;
typedef struct TaskCounters_AM_Mod_Demod_T TaskCounters_AM_Mod_Demod_T;
typedef struct TIME_AM_Mod_Demod_T TIME_AM_Mod_Demod_T;
typedef int PrevZCX_AM_Mod_Demod_T;
struct DW_AM_Mod_Demod_T;
struct ConstP_AM_Mod_Demod_T;
struct creal32_T;
struct creal64_T;
struct creal_T;
struct cint8_T;
struct cuint8_T;
struct cint16_T;
struct cuint16_T;
struct cint32_T;
struct cuint32_T;
struct TaskCounters_AM_Mod_Demod_T;
struct TIME_AM_Mod_Demod_T;
struct B_AM_Mod_Demod_T;
struct tag_RTM_AM_Mod_Demod_T;
struct DW_AM_Mod_Demod_T {
    real_T BodyDelay120_DSTATE;
    real_T BodyDelay119_DSTATE;
    real_T BodyDelay118_DSTATE;
    real_T BodyDelay117_DSTATE;
    real_T BodyDelay116_DSTATE;
    real_T BodyDelay115_DSTATE;
    real_T BodyDelay114_DSTATE;
    real_T BodyDelay113_DSTATE;
    real_T BodyDelay112_DSTATE;
    real_T BodyDelay111_DSTATE;
    real_T BodyDelay110_DSTATE;
    real_T BodyDelay109_DSTATE;
    real_T BodyDelay108_DSTATE;
    real_T BodyDelay107_DSTATE;
    real_T BodyDelay106_DSTATE;
    real_T BodyDelay105_DSTATE;
    real_T BodyDelay104_DSTATE;
    real_T BodyDelay103_DSTATE;
    real_T BodyDelay102_DSTATE;
    real_T BodyDelay101_DSTATE;
    real_T BodyDelay100_DSTATE;
    real_T BodyDelay99_DSTATE;
    real_T BodyDelay98_DSTATE;
    real_T BodyDelay97_DSTATE;
    real_T BodyDelay96_DSTATE;
    real_T BodyDelay95_DSTATE;
    real_T BodyDelay94_DSTATE;
    real_T BodyDelay93_DSTATE;
    real_T BodyDelay92_DSTATE;
    real_T BodyDelay91_DSTATE;
    real_T BodyDelay90_DSTATE;
    real_T BodyDelay89_DSTATE;
    real_T BodyDelay88_DSTATE;
    real_T BodyDelay87_DSTATE;
    real_T BodyDelay86_DSTATE;
    real_T BodyDelay85_DSTATE;
    real_T BodyDelay84_DSTATE;
    real_T BodyDelay83_DSTATE;
    real_T BodyDelay82_DSTATE;
    real_T BodyDelay81_DSTATE;
    real_T BodyDelay80_DSTATE;
    real_T BodyDelay79_DSTATE;
    real_T BodyDelay78_DSTATE;
    real_T BodyDelay77_DSTATE;
    real_T BodyDelay76_DSTATE;
    real_T BodyDelay75_DSTATE;
    real_T BodyDelay74_DSTATE;
    real_T BodyDelay73_DSTATE;
    real_T BodyDelay72_DSTATE;
    real_T BodyDelay71_DSTATE;
    real_T BodyDelay70_DSTATE;
    real_T BodyDelay69_DSTATE;
    real_T BodyDelay68_DSTATE;
    real_T BodyDelay67_DSTATE;
    real_T BodyDelay66_DSTATE;
    real_T BodyDelay65_DSTATE;
    real_T BodyDelay64_DSTATE;
    real_T BodyDelay63_DSTATE;
    real_T BodyDelay62_DSTATE;
    real_T BodyDelay61_DSTATE;
    real_T BodyDelay60_DSTATE;
    real_T BodyDelay59_DSTATE;
    real_T BodyDelay58_DSTATE;
    real_T BodyDelay57_DSTATE;
    real_T BodyDelay56_DSTATE;
    real_T BodyDelay55_DSTATE;
    real_T BodyDelay54_DSTATE;
    real_T BodyDelay53_DSTATE;
    real_T BodyDelay52_DSTATE;
    real_T BodyDelay51_DSTATE;
    real_T BodyDelay50_DSTATE;
    real_T BodyDelay49_DSTATE;
    real_T BodyDelay48_DSTATE;
    real_T BodyDelay47_DSTATE;
    real_T BodyDelay46_DSTATE;
    real_T BodyDelay45_DSTATE;
    real_T BodyDelay44_DSTATE;
    real_T BodyDelay43_DSTATE;
    real_T BodyDelay42_DSTATE;
    real_T BodyDelay41_DSTATE;
    real_T BodyDelay40_DSTATE;
    real_T BodyDelay39_DSTATE;
    real_T BodyDelay38_DSTATE;
    real_T BodyDelay37_DSTATE;
    real_T BodyDelay36_DSTATE;
    real_T BodyDelay35_DSTATE;
    real_T BodyDelay34_DSTATE;
    real_T BodyDelay33_DSTATE;
    real_T BodyDelay32_DSTATE;
    real_T BodyDelay31_DSTATE;
    real_T BodyDelay30_DSTATE;
    real_T BodyDelay29_DSTATE;
    real_T BodyDelay28_DSTATE;
    real_T BodyDelay27_DSTATE;
    real_T BodyDelay26_DSTATE;
    real_T BodyDelay25_DSTATE;
    real_T BodyDelay24_DSTATE;
    real_T BodyDelay23_DSTATE;
    real_T BodyDelay22_DSTATE;
    real_T BodyDelay21_DSTATE;
    real_T BodyDelay20_DSTATE;
    real_T BodyDelay19_DSTATE;
    real_T BodyDelay18_DSTATE;
    real_T BodyDelay17_DSTATE;
    real_T BodyDelay16_DSTATE;
    real_T BodyDelay15_DSTATE;
    real_T BodyDelay14_DSTATE;
    real_T BodyDelay13_DSTATE;
    real_T BodyDelay12_DSTATE;
    real_T BodyDelay11_DSTATE;
    real_T BodyDelay10_DSTATE;
    real_T BodyDelay9_DSTATE;
    real_T BodyDelay8_DSTATE;
    real_T BodyDelay7_DSTATE;
    real_T BodyDelay6_DSTATE;
    real_T BodyDelay5_DSTATE;
    real_T BodyDelay4_DSTATE;
    real_T BodyDelay3_DSTATE;
    real_T BodyDelay2_DSTATE;
    real_T CarrierWave_AccFreqNorm;
    real_T BodyDelay121_DSTATE;
    real_T CarrierWaveCOS_AccFreqNorm;
    real_T DigitalFilter_states[29];
    real_T CarrierWaveSIN_AccFreqNorm;
    real_T DigitalFilter_states_h[29];
    real_T CarrierWave1_AccFreqNorm;
    real_T BodyDelay2_DSTATE_c;
    real_T BodyDelay3_DSTATE_i;
    real_T BodyDelay4_DSTATE_l;
    real_T BodyDelay5_DSTATE_p;
    real_T BodyDelay6_DSTATE_i;
    real_T BodyDelay7_DSTATE_n;
    real_T BodyDelay8_DSTATE_p;
    real_T BodyDelay9_DSTATE_i;
    real_T BodyDelay10_DSTATE_b;
    real_T BodyDelay11_DSTATE_l;
    real_T BodyDelay12_DSTATE_c;
    real_T BodyDelay13_DSTATE_d;
    real_T BodyDelay14_DSTATE_k;
    real_T BodyDelay15_DSTATE_j;
    real_T BodyDelay16_DSTATE_p;
    real_T BodyDelay17_DSTATE_g;
    real_T BodyDelay18_DSTATE_d;
    real_T BodyDelay19_DSTATE_p;
    real_T BodyDelay20_DSTATE_m;
    real_T BodyDelay21_DSTATE_m;
    real_T BodyDelay22_DSTATE_b;
    real_T BodyDelay23_DSTATE_l;
    real_T BodyDelay24_DSTATE_d;
    real_T BodyDelay25_DSTATE_p;
    real_T BodyDelay26_DSTATE_l;
    real_T BodyDelay27_DSTATE_p;
    real_T BodyDelay28_DSTATE_b;
    real_T BodyDelay29_DSTATE_j;
    real_T BodyDelay30_DSTATE_h;
    real_T BodyDelay31_DSTATE_c;
    real_T BodyDelay32_DSTATE_p;
    real_T BodyDelay33_DSTATE_f;
    real_T BodyDelay34_DSTATE_d;
    real_T BodyDelay35_DSTATE_h;
    real_T BodyDelay36_DSTATE_j;
    real_T BodyDelay37_DSTATE_m;
    real_T BodyDelay38_DSTATE_o;
    real_T BodyDelay39_DSTATE_j;
    real_T BodyDelay40_DSTATE_g;
    real_T BodyDelay41_DSTATE_i;
    real_T BodyDelay42_DSTATE_b;
    real_T BodyDelay43_DSTATE_l;
    real_T BodyDelay44_DSTATE_m;
    real_T BodyDelay45_DSTATE_m;
    real_T BodyDelay46_DSTATE_k;
    real_T BodyDelay47_DSTATE_h;
    real_T BodyDelay48_DSTATE_o;
    real_T BodyDelay49_DSTATE_g;
    real_T BodyDelay50_DSTATE_f;
    real_T BodyDelay51_DSTATE_n;
    real_T BodyDelay52_DSTATE_f;
    real_T BodyDelay53_DSTATE_m;
    real_T BodyDelay54_DSTATE_i;
    real_T BodyDelay55_DSTATE_j;
    real_T BodyDelay56_DSTATE_a;
    real_T BodyDelay57_DSTATE_e;
    real_T BodyDelay58_DSTATE_c;
    real_T BodyDelay59_DSTATE_o;
    real_T BodyDelay60_DSTATE_p;
    real_T BodyDelay61_DSTATE_k;
    real_T BodyDelay62_DSTATE_j;
    real_T BodyDelay63_DSTATE_p;
    real_T BodyDelay64_DSTATE_h;
    real_T BodyDelay65_DSTATE_p;
    real_T BodyDelay66_DSTATE_e;
    real_T BodyDelay67_DSTATE_i;
    real_T BodyDelay68_DSTATE_d;
    real_T BodyDelay69_DSTATE_e;
    real_T BodyDelay70_DSTATE_a;
    real_T BodyDelay71_DSTATE_o;
    real_T BodyDelay72_DSTATE_e;
    real_T BodyDelay73_DSTATE_m;
    real_T BodyDelay74_DSTATE_l;
    real_T BodyDelay75_DSTATE_f;
    real_T BodyDelay76_DSTATE_i;
    real_T BodyDelay77_DSTATE_i;
    real_T BodyDelay78_DSTATE_h;
    real_T BodyDelay79_DSTATE_i;
    real_T BodyDelay80_DSTATE_i;
    real_T BodyDelay81_DSTATE_d;
    real_T BodyDelay82_DSTATE_g;
    real_T BodyDelay83_DSTATE_h;
    real_T BodyDelay84_DSTATE_i;
    real_T BodyDelay85_DSTATE_g;
    real_T BodyDelay86_DSTATE_b;
    real_T BodyDelay87_DSTATE_j;
    real_T BodyDelay88_DSTATE_c;
    real_T BodyDelay89_DSTATE_n;
    real_T BodyDelay90_DSTATE_h;
    real_T BodyDelay91_DSTATE_d;
    real_T BodyDelay92_DSTATE_k;
    real_T BodyDelay93_DSTATE_a;
    real_T BodyDelay94_DSTATE_l;
    real_T BodyDelay95_DSTATE_l;
    real_T BodyDelay96_DSTATE_m;
    real_T BodyDelay97_DSTATE_c;
    real_T BodyDelay98_DSTATE_j;
    real_T BodyDelay99_DSTATE_p;
    real_T BodyDelay100_DSTATE_k;
    real_T BodyDelay101_DSTATE_a;
    real_T BodyDelay102_DSTATE_e;
    real_T BodyDelay103_DSTATE_o;
    real_T BodyDelay104_DSTATE_d;
    real_T BodyDelay105_DSTATE_l;
    real_T BodyDelay106_DSTATE_a;
    real_T BodyDelay107_DSTATE_n;
    real_T BodyDelay108_DSTATE_l;
    real_T BodyDelay109_DSTATE_c;
    real_T BodyDelay110_DSTATE_m;
    real_T BodyDelay111_DSTATE_h;
    real_T BodyDelay112_DSTATE_l;
    real_T BodyDelay113_DSTATE_k;
    real_T BodyDelay114_DSTATE_h;
    real_T BodyDelay115_DSTATE_g;
    real_T BodyDelay116_DSTATE_k;
    real_T BodyDelay117_DSTATE_a;
    real_T BodyDelay118_DSTATE_p;
    real_T BodyDelay119_DSTATE_d;
    real_T BodyDelay120_DSTATE_f;
    real_T BodyDelay121_DSTATE_p;
    real_T DigitalFilter_simContextBuf[58];
    real_T DigitalFilter_simRevCoeff[30];
    real_T DigitalFilter_simContextBuf_n[58];
    real_T DigitalFilter_simRevCoeff_o[30];
};
struct ConstP_AM_Mod_Demod_T {
    real_T pooled65[30];
};
struct creal32_T {
    real32_T re;
    real32_T im;
};
struct creal64_T {
    real64_T re;
    real64_T im;
};
struct creal_T {
    real_T re;
    real_T im;
};
struct cint8_T {
    int8_T re;
    int8_T im;
};
struct cuint8_T {
    uint8_T re;
    uint8_T im;
};
struct cint16_T {
    int16_T re;
    int16_T im;
};
struct cuint16_T {
    uint16_T re;
    uint16_T im;
};
struct cint32_T {
    int32_T re;
    int32_T im;
};
struct cuint32_T {
    uint32_T re;
    uint32_T im;
};
struct TaskCounters_AM_Mod_Demod_T {
    uint16_T TID[2];
};
struct TIME_AM_Mod_Demod_T {
    struct TaskCounters_AM_Mod_Demod_T TaskCounters;
    uint32_T clockTick0;
};
struct B_AM_Mod_Demod_T {
    int TempMember;
};
struct tag_RTM_AM_Mod_Demod_T {
    char_T * errorStatus;
    struct B_AM_Mod_Demod_T * blockIO;
    struct DW_AM_Mod_Demod_T * dwork;
    struct TIME_AM_Mod_Demod_T Timing;
    PrevZCX_AM_Mod_Demod_T * prevZCSigState;
};
struct tag_RTM_AM_Mod_Demod_T AM_Mod_Demod_instance;
struct DW_AM_Mod_Demod_T AM_Mod_Demod_DW_instance;
struct B_AM_Mod_Demod_T AM_Mod_Demod_B_instance;
PrevZCX_AM_Mod_Demod_T AM_Mod_Demod_PrevZCX_instance;
struct ConstP_AM_Mod_Demod_T AM_Mod_Demod_ConstP = {0.00033946031449093929, 0.0017938694272224755, 0.005323982842641563, 0.010986856458809906, 0.016579767935947568, 0.017374586553218191, 0.0083209097467344551, -0.011485493200517788, -0.034702484066064784, -0.045889487185520036, -0.028029947425010509, 0.026849430377878146, 0.10933596399505258, 0.193119570861793, 0.24599539137195209, 0.24599539137195209, 0.193119570861793, 0.10933596399505258, 0.026849430377878146, -0.028029947425010509, -0.045889487185520036, -0.034702484066064784, -0.011485493200517788, 0.0083209097467344551, 0.017374586553218191, 0.016579767935947568, 0.010986856458809906, 0.005323982842641563, 0.0017938694272224755, 0.00033946031449093929};
void AM_Mod_Demod_step(RT_MODEL_AM_Mod_Demod_T* AM_Mod_Demod_M, real_T AM_Mod_Demod_U_In1, real_T AM_Mod_Demod_Y_Out2[2], real_T* AM_Mod_Demod_Y_Out1, real_T* AM_Mod_Demod_Y_Out5, real_T* AM_Mod_Demod_Y_Out6, real_T* AM_Mod_Demod_Y_Out7);
void AM_Mod_Demod_init(RT_MODEL_AM_Mod_Demod_T* AM_Mod_Demod_M, real_T AM_Mod_Demod_Y_Out2[2], real_T* AM_Mod_Demod_U_In1, real_T* AM_Mod_Demod_Y_Out1, real_T* AM_Mod_Demod_Y_Out5, real_T* AM_Mod_Demod_Y_Out6, real_T* AM_Mod_Demod_Y_Out7);
void AM_Mod_Demod_step(RT_MODEL_AM_Mod_Demod_T* AM_Mod_Demod_M, real_T AM_Mod_Demod_U_In1, real_T AM_Mod_Demod_Y_Out2[2], real_T* AM_Mod_Demod_Y_Out1, real_T* AM_Mod_Demod_Y_Out5, real_T* AM_Mod_Demod_Y_Out6, real_T* AM_Mod_Demod_Y_Out7) {
    struct DW_AM_Mod_Demod_T * AM_Mod_Demod_DW = AM_Mod_Demod_M->dwork;
    real_T SquareRoot2 = {0};
    real_T rtb_BodyDelay100_m = {0};
    real_T rtb_BodyDelay101_n = {0};
    real_T rtb_BodyDelay102_b = {0};
    real_T rtb_BodyDelay103_i = {0};
    real_T rtb_BodyDelay104_l = {0};
    real_T rtb_BodyDelay105_c = {0};
    real_T rtb_BodyDelay106_j = {0};
    real_T rtb_BodyDelay107_n = {0};
    real_T rtb_BodyDelay108_a = {0};
    real_T rtb_BodyDelay109_e = {0};
    real_T rtb_BodyDelay10_c = {0};
    real_T rtb_BodyDelay110_b = {0};
    real_T rtb_BodyDelay111_f = {0};
    real_T rtb_BodyDelay112_i = {0};
    real_T rtb_BodyDelay113_m = {0};
    real_T rtb_BodyDelay114_h = {0};
    real_T rtb_BodyDelay115_n = {0};
    real_T rtb_BodyDelay116_l = {0};
    real_T rtb_BodyDelay117_o = {0};
    real_T rtb_BodyDelay118_m = {0};
    real_T rtb_BodyDelay119_m = {0};
    real_T rtb_BodyDelay11_h = {0};
    real_T rtb_BodyDelay120 = {0};
    real_T rtb_BodyDelay120_g = {0};
    real_T rtb_BodyDelay12_l = {0};
    real_T rtb_BodyDelay13_j = {0};
    real_T rtb_BodyDelay14_j = {0};
    real_T rtb_BodyDelay15_n = {0};
    real_T rtb_BodyDelay16_g = {0};
    real_T rtb_BodyDelay17_n = {0};
    real_T rtb_BodyDelay18_g = {0};
    real_T rtb_BodyDelay19_g = {0};
    real_T rtb_BodyDelay2 = {0};
    real_T rtb_BodyDelay20_m = {0};
    real_T rtb_BodyDelay21_m = {0};
    real_T rtb_BodyDelay22_m = {0};
    real_T rtb_BodyDelay23_c = {0};
    real_T rtb_BodyDelay24_o = {0};
    real_T rtb_BodyDelay25_m = {0};
    real_T rtb_BodyDelay26_b = {0};
    real_T rtb_BodyDelay27_b = {0};
    real_T rtb_BodyDelay28_p = {0};
    real_T rtb_BodyDelay29_g = {0};
    real_T rtb_BodyDelay30_m = {0};
    real_T rtb_BodyDelay31_l = {0};
    real_T rtb_BodyDelay32_j = {0};
    real_T rtb_BodyDelay33_f = {0};
    real_T rtb_BodyDelay34_o = {0};
    real_T rtb_BodyDelay35_l = {0};
    real_T rtb_BodyDelay36_j = {0};
    real_T rtb_BodyDelay37_c = {0};
    real_T rtb_BodyDelay38_a = {0};
    real_T rtb_BodyDelay39_d = {0};
    real_T rtb_BodyDelay40_g = {0};
    real_T rtb_BodyDelay41_n = {0};
    real_T rtb_BodyDelay42_p = {0};
    real_T rtb_BodyDelay43_p = {0};
    real_T rtb_BodyDelay44_p = {0};
    real_T rtb_BodyDelay45_b = {0};
    real_T rtb_BodyDelay46_c = {0};
    real_T rtb_BodyDelay47_f = {0};
    real_T rtb_BodyDelay48_p = {0};
    real_T rtb_BodyDelay49_n = {0};
    real_T rtb_BodyDelay4_n = {0};
    real_T rtb_BodyDelay50_g = {0};
    real_T rtb_BodyDelay51_i = {0};
    real_T rtb_BodyDelay52_e = {0};
    real_T rtb_BodyDelay53_m = {0};
    real_T rtb_BodyDelay54_f = {0};
    real_T rtb_BodyDelay55_c = {0};
    real_T rtb_BodyDelay56_p = {0};
    real_T rtb_BodyDelay57_l = {0};
    real_T rtb_BodyDelay58_d = {0};
    real_T rtb_BodyDelay59_a = {0};
    real_T rtb_BodyDelay5_j = {0};
    real_T rtb_BodyDelay60_g = {0};
    real_T rtb_BodyDelay61_g = {0};
    real_T rtb_BodyDelay62_e = {0};
    real_T rtb_BodyDelay63_g = {0};
    real_T rtb_BodyDelay64_e = {0};
    real_T rtb_BodyDelay65_p = {0};
    real_T rtb_BodyDelay66_f = {0};
    real_T rtb_BodyDelay67_h = {0};
    real_T rtb_BodyDelay68_l = {0};
    real_T rtb_BodyDelay69_c = {0};
    real_T rtb_BodyDelay6_n = {0};
    real_T rtb_BodyDelay70_n = {0};
    real_T rtb_BodyDelay71_e = {0};
    real_T rtb_BodyDelay72_a = {0};
    real_T rtb_BodyDelay73_b = {0};
    real_T rtb_BodyDelay74_l = {0};
    real_T rtb_BodyDelay75_m = {0};
    real_T rtb_BodyDelay76_c = {0};
    real_T rtb_BodyDelay77_e = {0};
    real_T rtb_BodyDelay78_p = {0};
    real_T rtb_BodyDelay79_j = {0};
    real_T rtb_BodyDelay7_g = {0};
    real_T rtb_BodyDelay80_d = {0};
    real_T rtb_BodyDelay81_g = {0};
    real_T rtb_BodyDelay82_l = {0};
    real_T rtb_BodyDelay83_p = {0};
    real_T rtb_BodyDelay84_o = {0};
    real_T rtb_BodyDelay85_h = {0};
    real_T rtb_BodyDelay86_a = {0};
    real_T rtb_BodyDelay87_n = {0};
    real_T rtb_BodyDelay88_e = {0};
    real_T rtb_BodyDelay89_c = {0};
    real_T rtb_BodyDelay8_j = {0};
    real_T rtb_BodyDelay90_m = {0};
    real_T rtb_BodyDelay91_j = {0};
    real_T rtb_BodyDelay92_h = {0};
    real_T rtb_BodyDelay93_j = {0};
    real_T rtb_BodyDelay94_i = {0};
    real_T rtb_BodyDelay95_b = {0};
    real_T rtb_BodyDelay96_a = {0};
    real_T rtb_BodyDelay97_c = {0};
    real_T rtb_BodyDelay98_c = {0};
    real_T rtb_BodyDelay99_n = {0};
    real_T rtb_BodyDelay9_p = {0};
    real_T rtb_Square2 = {0};
    real_T rtb_b2 = {0};
    int32_T n = {0};
    int32_T srcIdx = {0};
    rtb_BodyDelay120 = AM_Mod_Demod_DW->BodyDelay120_DSTATE;
    rtb_BodyDelay2 = AM_Mod_Demod_DW->BodyDelay2_DSTATE;
    rtb_b2 = -9.3050702284968865E-5 * AM_Mod_Demod_DW->BodyDelay2_DSTATE;
    *AM_Mod_Demod_Y_Out7 = sin(AM_Mod_Demod_DW->CarrierWave_AccFreqNorm);
    AM_Mod_Demod_DW->CarrierWave_AccFreqNorm += 0.62831853071795862;
    if (AM_Mod_Demod_DW->CarrierWave_AccFreqNorm >= 6.2831853071795862) {
        __CPROVER_cover(1);
        AM_Mod_Demod_DW->CarrierWave_AccFreqNorm -= 6.2831853071795862;
    }
    else if (AM_Mod_Demod_DW->CarrierWave_AccFreqNorm < 0.0) {
        __CPROVER_cover(2);
        AM_Mod_Demod_DW->CarrierWave_AccFreqNorm += 6.2831853071795862;
    }
    __CPROVER_cover(3);
    *AM_Mod_Demod_Y_Out7 *= AM_Mod_Demod_U_In1;
    AM_Mod_Demod_DW->BodyDelay2_DSTATE = *AM_Mod_Demod_Y_Out7* * AM_Mod_Demod_Y_Out7;
    rtb_b2 = (((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((-4.5662663818355689E-5 * AM_Mod_Demod_DW->BodyDelay2_DSTATE + rtb_b2) + -0.00017610623360101414 * AM_Mod_Demod_DW->BodyDelay3_DSTATE) + -0.00029337939435011953 * AM_Mod_Demod_DW->BodyDelay4_DSTATE) + -0.00044423670008644848 * AM_Mod_Demod_DW->BodyDelay5_DSTATE) + -0.00062136931524911987 * AM_Mod_Demod_DW->BodyDelay6_DSTATE) + -0.00080927381631219612 * AM_Mod_Demod_DW->BodyDelay7_DSTATE) + -0.00098345746573073466 * AM_Mod_Demod_DW->BodyDelay8_DSTATE) + -0.001111253998127056 * AM_Mod_Demod_DW->BodyDelay9_DSTATE) + -0.0011538991042739546 * AM_Mod_Demod_DW->BodyDelay10_DSTATE) + -0.0010706706574851095 * AM_Mod_Demod_DW->BodyDelay11_DSTATE) + -0.00082435891040477931 * AM_Mod_Demod_DW->BodyDelay12_DSTATE) + -0.00038780628029214185 * AM_Mod_Demod_DW->BodyDelay13_DSTATE) + 0.00024913305035715656 * AM_Mod_Demod_DW->BodyDelay14_DSTATE) + 0.0010735520035639234 * AM_Mod_Demod_DW->BodyDelay15_DSTATE) + 0.0020453098658091431 * AM_Mod_Demod_DW->BodyDelay16_DSTATE) + 0.003095742162170359 * AM_Mod_Demod_DW->BodyDelay17_DSTATE) + 0.0041303381252319688 * AM_Mod_Demod_DW->BodyDelay18_DSTATE) + 0.00503574931805473 * AM_Mod_Demod_DW->BodyDelay19_DSTATE) + 0.0056908511343630765 * AM_Mod_Demod_DW->BodyDelay20_DSTATE) + 0.0059813089687688441 * AM_Mod_Demod_DW->BodyDelay21_DSTATE) + 0.0058160412426258795 * AM_Mod_Demod_DW->BodyDelay22_DSTATE) + 0.0051435978324832722 * AM_Mod_Demod_DW->BodyDelay23_DSTATE) + 0.0039660325233911858 * AM_Mod_Demod_DW->BodyDelay24_DSTATE) + 0.0023479356927531688 * AM_Mod_Demod_DW->BodyDelay25_DSTATE) + 0.00041841180713779517 * AM_Mod_Demod_DW->BodyDelay26_DSTATE) + -0.0016354030026831713 * AM_Mod_Demod_DW->BodyDelay27_DSTATE) + -0.0035838734457672174 * AM_Mod_Demod_DW->BodyDelay28_DSTATE) + -0.0051785086311871885 * AM_Mod_Demod_DW->BodyDelay29_DSTATE) + -0.0061818605411153834 * AM_Mod_Demod_DW->BodyDelay30_DSTATE) + -0.0064004232313101652 * AM_Mod_Demod_DW->BodyDelay31_DSTATE) + -0.0057164828702958286 * AM_Mod_Demod_DW->BodyDelay32_DSTATE) + -0.0041142914417041185 * AM_Mod_Demod_DW->BodyDelay33_DSTATE) + -0.0016963616291432795 * AM_Mod_Demod_DW->BodyDelay34_DSTATE) + 0.0013138264428854381 * AM_Mod_Demod_DW->BodyDelay35_DSTATE) + 0.0045847769966693638 * AM_Mod_Demod_DW->BodyDelay36_DSTATE) + 0.007706061797677086 * AM_Mod_Demod_DW->BodyDelay37_DSTATE) + 0.010231649301542016 * AM_Mod_Demod_DW->BodyDelay38_DSTATE) + 0.011733690154980873 * AM_Mod_Demod_DW->BodyDelay39_DSTATE) + 0.011860929797117791 * AM_Mod_Demod_DW->BodyDelay40_DSTATE) + 0.010394902058635835 * AM_Mod_Demod_DW->BodyDelay41_DSTATE) + 0.0072963585071920281 * AM_Mod_Demod_DW->BodyDelay42_DSTATE) + 0.0027351285363456158 * AM_Mod_Demod_DW->BodyDelay43_DSTATE) + -0.0029022640829190851 * AM_Mod_Demod_DW->BodyDelay44_DSTATE) + -0.0090303613490534569 * AM_Mod_Demod_DW->BodyDelay45_DSTATE) + -0.014908346162248804 * AM_Mod_Demod_DW->BodyDelay46_DSTATE) + -0.0197064977574164 * AM_Mod_Demod_DW->BodyDelay47_DSTATE) + -0.022589791907730208 * AM_Mod_Demod_DW->BodyDelay48_DSTATE) + -0.022810077984179462 * AM_Mod_Demod_DW->BodyDelay49_DSTATE) + -0.019796855049425213 * AM_Mod_Demod_DW->BodyDelay50_DSTATE) + -0.013235897185452272 * AM_Mod_Demod_DW->BodyDelay51_DSTATE) + -0.0031259793051983376 * AM_Mod_Demod_DW->BodyDelay52_DSTATE) + 0.010194294947491437 * AM_Mod_Demod_DW->BodyDelay53_DSTATE) + 0.026054244278894356 * AM_Mod_Demod_DW->BodyDelay54_DSTATE) + 0.043494554520832826 * AM_Mod_Demod_DW->BodyDelay55_DSTATE) + 0.061343218745072557 * AM_Mod_Demod_DW->BodyDelay56_DSTATE) + 0.078317178333283943 * AM_Mod_Demod_DW->BodyDelay57_DSTATE) + 0.093139403576610449 * AM_Mod_Demod_DW->BodyDelay58_DSTATE) + 0.10465933913932809 * AM_Mod_Demod_DW->BodyDelay59_DSTATE) + 0.11196373625413442 * AM_Mod_Demod_DW->BodyDelay60_DSTATE) + 0.11446594680127516 * AM_Mod_Demod_DW->BodyDelay61_DSTATE) + 0.11196373625413442 * AM_Mod_Demod_DW->BodyDelay62_DSTATE) + 0.10465933913932809 * AM_Mod_Demod_DW->BodyDelay63_DSTATE) + 0.093139403576610449 * AM_Mod_Demod_DW->BodyDelay64_DSTATE) + 0.078317178333283943 * AM_Mod_Demod_DW->BodyDelay65_DSTATE) + 0.061343218745072557 * AM_Mod_Demod_DW->BodyDelay66_DSTATE) + 0.043494554520832826 * AM_Mod_Demod_DW->BodyDelay67_DSTATE) + 0.026054244278894356 * AM_Mod_Demod_DW->BodyDelay68_DSTATE) + 0.010194294947491437 * AM_Mod_Demod_DW->BodyDelay69_DSTATE) + -0.0031259793051983376 * AM_Mod_Demod_DW->BodyDelay70_DSTATE) + -0.013235897185452272 * AM_Mod_Demod_DW->BodyDelay71_DSTATE) + -0.019796855049425213 * AM_Mod_Demod_DW->BodyDelay72_DSTATE) + -0.022810077984179462 * AM_Mod_Demod_DW->BodyDelay73_DSTATE) + -0.022589791907730208 * AM_Mod_Demod_DW->BodyDelay74_DSTATE) + -0.0197064977574164 * AM_Mod_Demod_DW->BodyDelay75_DSTATE) + -0.014908346162248804 * AM_Mod_Demod_DW->BodyDelay76_DSTATE) + -0.0090303613490534569 * AM_Mod_Demod_DW->BodyDelay77_DSTATE) + -0.0029022640829190851 * AM_Mod_Demod_DW->BodyDelay78_DSTATE) + 0.0027351285363456158 * AM_Mod_Demod_DW->BodyDelay79_DSTATE) + 0.0072963585071920281 * AM_Mod_Demod_DW->BodyDelay80_DSTATE) + 0.010394902058635835 * AM_Mod_Demod_DW->BodyDelay81_DSTATE) + 0.011860929797117791 * AM_Mod_Demod_DW->BodyDelay82_DSTATE) + 0.011733690154980873 * AM_Mod_Demod_DW->BodyDelay83_DSTATE) + 0.010231649301542016 * AM_Mod_Demod_DW->BodyDelay84_DSTATE) + 0.007706061797677086 * AM_Mod_Demod_DW->BodyDelay85_DSTATE) + 0.0045847769966693638 * AM_Mod_Demod_DW->BodyDelay86_DSTATE) + 0.0013138264428854381 * AM_Mod_Demod_DW->BodyDelay87_DSTATE) + -0.0016963616291432795 * AM_Mod_Demod_DW->BodyDelay88_DSTATE) + -0.0041142914417041185 * AM_Mod_Demod_DW->BodyDelay89_DSTATE) + -0.0057164828702958286 * AM_Mod_Demod_DW->BodyDelay90_DSTATE) + -0.0064004232313101652 * AM_Mod_Demod_DW->BodyDelay91_DSTATE) + -0.0061818605411153834 * AM_Mod_Demod_DW->BodyDelay92_DSTATE) + -0.0051785086311871885 * AM_Mod_Demod_DW->BodyDelay93_DSTATE) + -0.0035838734457672174 * AM_Mod_Demod_DW->BodyDelay94_DSTATE) + -0.0016354030026831713 * AM_Mod_Demod_DW->BodyDelay95_DSTATE) + 0.00041841180713779517 * AM_Mod_Demod_DW->BodyDelay96_DSTATE) + 0.0023479356927531688 * AM_Mod_Demod_DW->BodyDelay97_DSTATE) + 0.0039660325233911858 * AM_Mod_Demod_DW->BodyDelay98_DSTATE) + 0.0051435978324832722 * AM_Mod_Demod_DW->BodyDelay99_DSTATE) + 0.0058160412426258795 * AM_Mod_Demod_DW->BodyDelay100_DSTATE) + 0.0059813089687688441 * AM_Mod_Demod_DW->BodyDelay101_DSTATE) + 0.0056908511343630765 * AM_Mod_Demod_DW->BodyDelay102_DSTATE) + 0.00503574931805473 * AM_Mod_Demod_DW->BodyDelay103_DSTATE) + 0.0041303381252319688 * AM_Mod_Demod_DW->BodyDelay104_DSTATE) + 0.003095742162170359 * AM_Mod_Demod_DW->BodyDelay105_DSTATE) + 0.0020453098658091431 * AM_Mod_Demod_DW->BodyDelay106_DSTATE) + 0.0010735520035639234 * AM_Mod_Demod_DW->BodyDelay107_DSTATE) + 0.00024913305035715656 * AM_Mod_Demod_DW->BodyDelay108_DSTATE) + -0.00038780628029214185 * AM_Mod_Demod_DW->BodyDelay109_DSTATE) + -0.00082435891040477931 * AM_Mod_Demod_DW->BodyDelay110_DSTATE) + -0.0010706706574851095 * AM_Mod_Demod_DW->BodyDelay111_DSTATE) + -0.0011538991042739546 * AM_Mod_Demod_DW->BodyDelay112_DSTATE) + -0.001111253998127056 * AM_Mod_Demod_DW->BodyDelay113_DSTATE) + -0.00098345746573073466 * AM_Mod_Demod_DW->BodyDelay114_DSTATE) + -0.00080927381631219612 * AM_Mod_Demod_DW->BodyDelay115_DSTATE) + -0.00062136931524911987 * AM_Mod_Demod_DW->BodyDelay116_DSTATE) + -0.00044423670008644848 * AM_Mod_Demod_DW->BodyDelay117_DSTATE) + -0.00029337939435011953 * AM_Mod_Demod_DW->BodyDelay118_DSTATE) + -0.00017610623360101414 * AM_Mod_Demod_DW->BodyDelay119_DSTATE) + -9.3050702284968865E-5 * AM_Mod_Demod_DW->BodyDelay120_DSTATE) + -4.5662663818355689E-5 * AM_Mod_Demod_DW->BodyDelay121_DSTATE;
    if (rtb_b2 < 0.0) {
        __CPROVER_cover(4);
        rtb_b2 = -sqrt(fabs(rtb_b2));
    }
    else {
        __CPROVER_cover(5);
        rtb_b2 = sqrt(rtb_b2);
    }
    *AM_Mod_Demod_Y_Out1 = (rtb_b2 + 1.0) * 2.0;
    AM_Mod_Demod_Y_Out2[0] = AM_Mod_Demod_U_In1;
    AM_Mod_Demod_Y_Out2[1] = *AM_Mod_Demod_Y_Out7;
    rtb_b2 = sin(AM_Mod_Demod_DW->CarrierWaveCOS_AccFreqNorm);
    AM_Mod_Demod_DW->CarrierWaveCOS_AccFreqNorm += 0.62831853071795862;
    if (AM_Mod_Demod_DW->CarrierWaveCOS_AccFreqNorm >= 6.2831853071795862) {
        __CPROVER_cover(6);
        AM_Mod_Demod_DW->CarrierWaveCOS_AccFreqNorm -= 6.2831853071795862;
    }
    else if (AM_Mod_Demod_DW->CarrierWaveCOS_AccFreqNorm < 0.0) {
        __CPROVER_cover(7);
        AM_Mod_Demod_DW->CarrierWaveCOS_AccFreqNorm += 6.2831853071795862;
    }
    __CPROVER_cover(8);
    SquareRoot2 = *AM_Mod_Demod_Y_Out7 * rtb_b2;
    for (srcIdx = 0;srcIdx < 30;srcIdx++) {
        __CPROVER_cover(9);
        AM_Mod_Demod_DW->DigitalFilter_simRevCoeff[29 - srcIdx] = AM_Mod_Demod_ConstP.pooled65[srcIdx];
    }
    __CPROVER_cover(10);
    for (srcIdx = 0;srcIdx < 29;srcIdx++) {
        __CPROVER_cover(11);
        AM_Mod_Demod_DW->DigitalFilter_simContextBuf[28 - srcIdx] = AM_Mod_Demod_DW->DigitalFilter_states[srcIdx];
    }
    __CPROVER_cover(12);
    AM_Mod_Demod_DW->DigitalFilter_simContextBuf[29] = SquareRoot2;
    for (srcIdx = 0;srcIdx < 1;srcIdx++) {
        __CPROVER_cover(13);
        rtb_b2 = 0.0;
        for (n = 0;n < 30;n++) {
            __CPROVER_cover(14);
            rtb_b2 += AM_Mod_Demod_DW->DigitalFilter_simRevCoeff[n] * AM_Mod_Demod_DW->DigitalFilter_simContextBuf[n];
        }
        __CPROVER_cover(15);
    }
    __CPROVER_cover(16);
    for (srcIdx = 27;srcIdx >= 0;srcIdx--) {
        __CPROVER_cover(17);
        AM_Mod_Demod_DW->DigitalFilter_states[srcIdx + 1] = AM_Mod_Demod_DW->DigitalFilter_states[srcIdx];
    }
    __CPROVER_cover(18);
    AM_Mod_Demod_DW->DigitalFilter_states[0] = SquareRoot2;
    rtb_Square2 = rtb_b2 * rtb_b2;
    rtb_b2 = sin(AM_Mod_Demod_DW->CarrierWaveSIN_AccFreqNorm);
    AM_Mod_Demod_DW->CarrierWaveSIN_AccFreqNorm += 0.62831853071795862;
    if (AM_Mod_Demod_DW->CarrierWaveSIN_AccFreqNorm >= 6.2831853071795862) {
        __CPROVER_cover(19);
        AM_Mod_Demod_DW->CarrierWaveSIN_AccFreqNorm -= 6.2831853071795862;
    }
    else if (AM_Mod_Demod_DW->CarrierWaveSIN_AccFreqNorm < 0.0) {
        __CPROVER_cover(20);
        AM_Mod_Demod_DW->CarrierWaveSIN_AccFreqNorm += 6.2831853071795862;
    }
    __CPROVER_cover(21);
    SquareRoot2 = *AM_Mod_Demod_Y_Out7 * rtb_b2;
    for (srcIdx = 0;srcIdx < 30;srcIdx++) {
        __CPROVER_cover(22);
        AM_Mod_Demod_DW->DigitalFilter_simRevCoeff_o[29 - srcIdx] = AM_Mod_Demod_ConstP.pooled65[srcIdx];
    }
    __CPROVER_cover(23);
    for (srcIdx = 0;srcIdx < 29;srcIdx++) {
        __CPROVER_cover(24);
        AM_Mod_Demod_DW->DigitalFilter_simContextBuf_n[28 - srcIdx] = AM_Mod_Demod_DW->DigitalFilter_states_h[srcIdx];
    }
    __CPROVER_cover(25);
    AM_Mod_Demod_DW->DigitalFilter_simContextBuf_n[29] = SquareRoot2;
    for (srcIdx = 0;srcIdx < 1;srcIdx++) {
        __CPROVER_cover(26);
        rtb_b2 = 0.0;
        for (n = 0;n < 30;n++) {
            __CPROVER_cover(27);
            rtb_b2 += AM_Mod_Demod_DW->DigitalFilter_simRevCoeff_o[n] * AM_Mod_Demod_DW->DigitalFilter_simContextBuf_n[n];
        }
        __CPROVER_cover(28);
    }
    __CPROVER_cover(29);
    for (srcIdx = 27;srcIdx >= 0;srcIdx--) {
        __CPROVER_cover(30);
        AM_Mod_Demod_DW->DigitalFilter_states_h[srcIdx + 1] = AM_Mod_Demod_DW->DigitalFilter_states_h[srcIdx];
    }
    __CPROVER_cover(31);
    AM_Mod_Demod_DW->DigitalFilter_states_h[0] = SquareRoot2;
    rtb_b2 = rtb_b2 * rtb_b2 + rtb_Square2;
    if (rtb_b2 < 0.0) {
        __CPROVER_cover(32);
        *AM_Mod_Demod_Y_Out5 = -sqrt(fabs(rtb_b2));
    }
    else {
        __CPROVER_cover(33);
        *AM_Mod_Demod_Y_Out5 = sqrt(rtb_b2);
    }
    rtb_b2 = sin(AM_Mod_Demod_DW->CarrierWave1_AccFreqNorm);
    AM_Mod_Demod_DW->CarrierWave1_AccFreqNorm += 0.62831853071795862;
    if (AM_Mod_Demod_DW->CarrierWave1_AccFreqNorm >= 6.2831853071795862) {
        __CPROVER_cover(34);
        AM_Mod_Demod_DW->CarrierWave1_AccFreqNorm -= 6.2831853071795862;
    }
    else if (AM_Mod_Demod_DW->CarrierWave1_AccFreqNorm < 0.0) {
        __CPROVER_cover(35);
        AM_Mod_Demod_DW->CarrierWave1_AccFreqNorm += 6.2831853071795862;
    }
    __CPROVER_cover(36);
    rtb_b2 *= *AM_Mod_Demod_Y_Out7;
    SquareRoot2 = AM_Mod_Demod_DW->BodyDelay2_DSTATE_c;
    rtb_Square2 = AM_Mod_Demod_DW->BodyDelay3_DSTATE_i;
    rtb_BodyDelay4_n = AM_Mod_Demod_DW->BodyDelay4_DSTATE_l;
    rtb_BodyDelay5_j = AM_Mod_Demod_DW->BodyDelay5_DSTATE_p;
    rtb_BodyDelay6_n = AM_Mod_Demod_DW->BodyDelay6_DSTATE_i;
    rtb_BodyDelay7_g = AM_Mod_Demod_DW->BodyDelay7_DSTATE_n;
    rtb_BodyDelay8_j = AM_Mod_Demod_DW->BodyDelay8_DSTATE_p;
    rtb_BodyDelay9_p = AM_Mod_Demod_DW->BodyDelay9_DSTATE_i;
    rtb_BodyDelay10_c = AM_Mod_Demod_DW->BodyDelay10_DSTATE_b;
    rtb_BodyDelay11_h = AM_Mod_Demod_DW->BodyDelay11_DSTATE_l;
    rtb_BodyDelay12_l = AM_Mod_Demod_DW->BodyDelay12_DSTATE_c;
    rtb_BodyDelay13_j = AM_Mod_Demod_DW->BodyDelay13_DSTATE_d;
    rtb_BodyDelay14_j = AM_Mod_Demod_DW->BodyDelay14_DSTATE_k;
    rtb_BodyDelay15_n = AM_Mod_Demod_DW->BodyDelay15_DSTATE_j;
    rtb_BodyDelay16_g = AM_Mod_Demod_DW->BodyDelay16_DSTATE_p;
    rtb_BodyDelay17_n = AM_Mod_Demod_DW->BodyDelay17_DSTATE_g;
    rtb_BodyDelay18_g = AM_Mod_Demod_DW->BodyDelay18_DSTATE_d;
    rtb_BodyDelay19_g = AM_Mod_Demod_DW->BodyDelay19_DSTATE_p;
    rtb_BodyDelay20_m = AM_Mod_Demod_DW->BodyDelay20_DSTATE_m;
    rtb_BodyDelay21_m = AM_Mod_Demod_DW->BodyDelay21_DSTATE_m;
    rtb_BodyDelay22_m = AM_Mod_Demod_DW->BodyDelay22_DSTATE_b;
    rtb_BodyDelay23_c = AM_Mod_Demod_DW->BodyDelay23_DSTATE_l;
    rtb_BodyDelay24_o = AM_Mod_Demod_DW->BodyDelay24_DSTATE_d;
    rtb_BodyDelay25_m = AM_Mod_Demod_DW->BodyDelay25_DSTATE_p;
    rtb_BodyDelay26_b = AM_Mod_Demod_DW->BodyDelay26_DSTATE_l;
    rtb_BodyDelay27_b = AM_Mod_Demod_DW->BodyDelay27_DSTATE_p;
    rtb_BodyDelay28_p = AM_Mod_Demod_DW->BodyDelay28_DSTATE_b;
    rtb_BodyDelay29_g = AM_Mod_Demod_DW->BodyDelay29_DSTATE_j;
    rtb_BodyDelay30_m = AM_Mod_Demod_DW->BodyDelay30_DSTATE_h;
    rtb_BodyDelay31_l = AM_Mod_Demod_DW->BodyDelay31_DSTATE_c;
    rtb_BodyDelay32_j = AM_Mod_Demod_DW->BodyDelay32_DSTATE_p;
    rtb_BodyDelay33_f = AM_Mod_Demod_DW->BodyDelay33_DSTATE_f;
    rtb_BodyDelay34_o = AM_Mod_Demod_DW->BodyDelay34_DSTATE_d;
    rtb_BodyDelay35_l = AM_Mod_Demod_DW->BodyDelay35_DSTATE_h;
    rtb_BodyDelay36_j = AM_Mod_Demod_DW->BodyDelay36_DSTATE_j;
    rtb_BodyDelay37_c = AM_Mod_Demod_DW->BodyDelay37_DSTATE_m;
    rtb_BodyDelay38_a = AM_Mod_Demod_DW->BodyDelay38_DSTATE_o;
    rtb_BodyDelay39_d = AM_Mod_Demod_DW->BodyDelay39_DSTATE_j;
    rtb_BodyDelay40_g = AM_Mod_Demod_DW->BodyDelay40_DSTATE_g;
    rtb_BodyDelay41_n = AM_Mod_Demod_DW->BodyDelay41_DSTATE_i;
    rtb_BodyDelay42_p = AM_Mod_Demod_DW->BodyDelay42_DSTATE_b;
    rtb_BodyDelay43_p = AM_Mod_Demod_DW->BodyDelay43_DSTATE_l;
    rtb_BodyDelay44_p = AM_Mod_Demod_DW->BodyDelay44_DSTATE_m;
    rtb_BodyDelay45_b = AM_Mod_Demod_DW->BodyDelay45_DSTATE_m;
    rtb_BodyDelay46_c = AM_Mod_Demod_DW->BodyDelay46_DSTATE_k;
    rtb_BodyDelay47_f = AM_Mod_Demod_DW->BodyDelay47_DSTATE_h;
    rtb_BodyDelay48_p = AM_Mod_Demod_DW->BodyDelay48_DSTATE_o;
    rtb_BodyDelay49_n = AM_Mod_Demod_DW->BodyDelay49_DSTATE_g;
    rtb_BodyDelay50_g = AM_Mod_Demod_DW->BodyDelay50_DSTATE_f;
    rtb_BodyDelay51_i = AM_Mod_Demod_DW->BodyDelay51_DSTATE_n;
    rtb_BodyDelay52_e = AM_Mod_Demod_DW->BodyDelay52_DSTATE_f;
    rtb_BodyDelay53_m = AM_Mod_Demod_DW->BodyDelay53_DSTATE_m;
    rtb_BodyDelay54_f = AM_Mod_Demod_DW->BodyDelay54_DSTATE_i;
    rtb_BodyDelay55_c = AM_Mod_Demod_DW->BodyDelay55_DSTATE_j;
    rtb_BodyDelay56_p = AM_Mod_Demod_DW->BodyDelay56_DSTATE_a;
    rtb_BodyDelay57_l = AM_Mod_Demod_DW->BodyDelay57_DSTATE_e;
    rtb_BodyDelay58_d = AM_Mod_Demod_DW->BodyDelay58_DSTATE_c;
    rtb_BodyDelay59_a = AM_Mod_Demod_DW->BodyDelay59_DSTATE_o;
    rtb_BodyDelay60_g = AM_Mod_Demod_DW->BodyDelay60_DSTATE_p;
    rtb_BodyDelay61_g = AM_Mod_Demod_DW->BodyDelay61_DSTATE_k;
    rtb_BodyDelay62_e = AM_Mod_Demod_DW->BodyDelay62_DSTATE_j;
    rtb_BodyDelay63_g = AM_Mod_Demod_DW->BodyDelay63_DSTATE_p;
    rtb_BodyDelay64_e = AM_Mod_Demod_DW->BodyDelay64_DSTATE_h;
    rtb_BodyDelay65_p = AM_Mod_Demod_DW->BodyDelay65_DSTATE_p;
    rtb_BodyDelay66_f = AM_Mod_Demod_DW->BodyDelay66_DSTATE_e;
    rtb_BodyDelay67_h = AM_Mod_Demod_DW->BodyDelay67_DSTATE_i;
    rtb_BodyDelay68_l = AM_Mod_Demod_DW->BodyDelay68_DSTATE_d;
    rtb_BodyDelay69_c = AM_Mod_Demod_DW->BodyDelay69_DSTATE_e;
    rtb_BodyDelay70_n = AM_Mod_Demod_DW->BodyDelay70_DSTATE_a;
    rtb_BodyDelay71_e = AM_Mod_Demod_DW->BodyDelay71_DSTATE_o;
    rtb_BodyDelay72_a = AM_Mod_Demod_DW->BodyDelay72_DSTATE_e;
    rtb_BodyDelay73_b = AM_Mod_Demod_DW->BodyDelay73_DSTATE_m;
    rtb_BodyDelay74_l = AM_Mod_Demod_DW->BodyDelay74_DSTATE_l;
    rtb_BodyDelay75_m = AM_Mod_Demod_DW->BodyDelay75_DSTATE_f;
    rtb_BodyDelay76_c = AM_Mod_Demod_DW->BodyDelay76_DSTATE_i;
    rtb_BodyDelay77_e = AM_Mod_Demod_DW->BodyDelay77_DSTATE_i;
    rtb_BodyDelay78_p = AM_Mod_Demod_DW->BodyDelay78_DSTATE_h;
    rtb_BodyDelay79_j = AM_Mod_Demod_DW->BodyDelay79_DSTATE_i;
    rtb_BodyDelay80_d = AM_Mod_Demod_DW->BodyDelay80_DSTATE_i;
    rtb_BodyDelay81_g = AM_Mod_Demod_DW->BodyDelay81_DSTATE_d;
    rtb_BodyDelay82_l = AM_Mod_Demod_DW->BodyDelay82_DSTATE_g;
    rtb_BodyDelay83_p = AM_Mod_Demod_DW->BodyDelay83_DSTATE_h;
    rtb_BodyDelay84_o = AM_Mod_Demod_DW->BodyDelay84_DSTATE_i;
    rtb_BodyDelay85_h = AM_Mod_Demod_DW->BodyDelay85_DSTATE_g;
    rtb_BodyDelay86_a = AM_Mod_Demod_DW->BodyDelay86_DSTATE_b;
    rtb_BodyDelay87_n = AM_Mod_Demod_DW->BodyDelay87_DSTATE_j;
    rtb_BodyDelay88_e = AM_Mod_Demod_DW->BodyDelay88_DSTATE_c;
    rtb_BodyDelay89_c = AM_Mod_Demod_DW->BodyDelay89_DSTATE_n;
    rtb_BodyDelay90_m = AM_Mod_Demod_DW->BodyDelay90_DSTATE_h;
    rtb_BodyDelay91_j = AM_Mod_Demod_DW->BodyDelay91_DSTATE_d;
    rtb_BodyDelay92_h = AM_Mod_Demod_DW->BodyDelay92_DSTATE_k;
    rtb_BodyDelay93_j = AM_Mod_Demod_DW->BodyDelay93_DSTATE_a;
    rtb_BodyDelay94_i = AM_Mod_Demod_DW->BodyDelay94_DSTATE_l;
    rtb_BodyDelay95_b = AM_Mod_Demod_DW->BodyDelay95_DSTATE_l;
    rtb_BodyDelay96_a = AM_Mod_Demod_DW->BodyDelay96_DSTATE_m;
    rtb_BodyDelay97_c = AM_Mod_Demod_DW->BodyDelay97_DSTATE_c;
    rtb_BodyDelay98_c = AM_Mod_Demod_DW->BodyDelay98_DSTATE_j;
    rtb_BodyDelay99_n = AM_Mod_Demod_DW->BodyDelay99_DSTATE_p;
    rtb_BodyDelay100_m = AM_Mod_Demod_DW->BodyDelay100_DSTATE_k;
    rtb_BodyDelay101_n = AM_Mod_Demod_DW->BodyDelay101_DSTATE_a;
    rtb_BodyDelay102_b = AM_Mod_Demod_DW->BodyDelay102_DSTATE_e;
    rtb_BodyDelay103_i = AM_Mod_Demod_DW->BodyDelay103_DSTATE_o;
    rtb_BodyDelay104_l = AM_Mod_Demod_DW->BodyDelay104_DSTATE_d;
    rtb_BodyDelay105_c = AM_Mod_Demod_DW->BodyDelay105_DSTATE_l;
    rtb_BodyDelay106_j = AM_Mod_Demod_DW->BodyDelay106_DSTATE_a;
    rtb_BodyDelay107_n = AM_Mod_Demod_DW->BodyDelay107_DSTATE_n;
    rtb_BodyDelay108_a = AM_Mod_Demod_DW->BodyDelay108_DSTATE_l;
    rtb_BodyDelay109_e = AM_Mod_Demod_DW->BodyDelay109_DSTATE_c;
    rtb_BodyDelay110_b = AM_Mod_Demod_DW->BodyDelay110_DSTATE_m;
    rtb_BodyDelay111_f = AM_Mod_Demod_DW->BodyDelay111_DSTATE_h;
    rtb_BodyDelay112_i = AM_Mod_Demod_DW->BodyDelay112_DSTATE_l;
    rtb_BodyDelay113_m = AM_Mod_Demod_DW->BodyDelay113_DSTATE_k;
    rtb_BodyDelay114_h = AM_Mod_Demod_DW->BodyDelay114_DSTATE_h;
    rtb_BodyDelay115_n = AM_Mod_Demod_DW->BodyDelay115_DSTATE_g;
    rtb_BodyDelay116_l = AM_Mod_Demod_DW->BodyDelay116_DSTATE_k;
    rtb_BodyDelay117_o = AM_Mod_Demod_DW->BodyDelay117_DSTATE_a;
    rtb_BodyDelay118_m = AM_Mod_Demod_DW->BodyDelay118_DSTATE_p;
    rtb_BodyDelay119_m = AM_Mod_Demod_DW->BodyDelay119_DSTATE_d;
    rtb_BodyDelay120_g = AM_Mod_Demod_DW->BodyDelay120_DSTATE_f;
    *AM_Mod_Demod_Y_Out6 = ((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((-9.3050702284968865E-5 * AM_Mod_Demod_DW->BodyDelay2_DSTATE_c + -4.5662663818355689E-5 * rtb_b2) + -0.00017610623360101414 * AM_Mod_Demod_DW->BodyDelay3_DSTATE_i) + -0.00029337939435011953 * AM_Mod_Demod_DW->BodyDelay4_DSTATE_l) + -0.00044423670008644848 * AM_Mod_Demod_DW->BodyDelay5_DSTATE_p) + -0.00062136931524911987 * AM_Mod_Demod_DW->BodyDelay6_DSTATE_i) + -0.00080927381631219612 * AM_Mod_Demod_DW->BodyDelay7_DSTATE_n) + -0.00098345746573073466 * AM_Mod_Demod_DW->BodyDelay8_DSTATE_p) + -0.001111253998127056 * AM_Mod_Demod_DW->BodyDelay9_DSTATE_i) + -0.0011538991042739546 * AM_Mod_Demod_DW->BodyDelay10_DSTATE_b) + -0.0010706706574851095 * AM_Mod_Demod_DW->BodyDelay11_DSTATE_l) + -0.00082435891040477931 * AM_Mod_Demod_DW->BodyDelay12_DSTATE_c) + -0.00038780628029214185 * AM_Mod_Demod_DW->BodyDelay13_DSTATE_d) + 0.00024913305035715656 * AM_Mod_Demod_DW->BodyDelay14_DSTATE_k) + 0.0010735520035639234 * AM_Mod_Demod_DW->BodyDelay15_DSTATE_j) + 0.0020453098658091431 * AM_Mod_Demod_DW->BodyDelay16_DSTATE_p) + 0.003095742162170359 * AM_Mod_Demod_DW->BodyDelay17_DSTATE_g) + 0.0041303381252319688 * AM_Mod_Demod_DW->BodyDelay18_DSTATE_d) + 0.00503574931805473 * AM_Mod_Demod_DW->BodyDelay19_DSTATE_p) + 0.0056908511343630765 * AM_Mod_Demod_DW->BodyDelay20_DSTATE_m) + 0.0059813089687688441 * AM_Mod_Demod_DW->BodyDelay21_DSTATE_m) + 0.0058160412426258795 * AM_Mod_Demod_DW->BodyDelay22_DSTATE_b) + 0.0051435978324832722 * AM_Mod_Demod_DW->BodyDelay23_DSTATE_l) + 0.0039660325233911858 * AM_Mod_Demod_DW->BodyDelay24_DSTATE_d) + 0.0023479356927531688 * AM_Mod_Demod_DW->BodyDelay25_DSTATE_p) + 0.00041841180713779517 * AM_Mod_Demod_DW->BodyDelay26_DSTATE_l) + -0.0016354030026831713 * AM_Mod_Demod_DW->BodyDelay27_DSTATE_p) + -0.0035838734457672174 * AM_Mod_Demod_DW->BodyDelay28_DSTATE_b) + -0.0051785086311871885 * AM_Mod_Demod_DW->BodyDelay29_DSTATE_j) + -0.0061818605411153834 * AM_Mod_Demod_DW->BodyDelay30_DSTATE_h) + -0.0064004232313101652 * AM_Mod_Demod_DW->BodyDelay31_DSTATE_c) + -0.0057164828702958286 * AM_Mod_Demod_DW->BodyDelay32_DSTATE_p) + -0.0041142914417041185 * AM_Mod_Demod_DW->BodyDelay33_DSTATE_f) + -0.0016963616291432795 * AM_Mod_Demod_DW->BodyDelay34_DSTATE_d) + 0.0013138264428854381 * AM_Mod_Demod_DW->BodyDelay35_DSTATE_h) + 0.0045847769966693638 * AM_Mod_Demod_DW->BodyDelay36_DSTATE_j) + 0.007706061797677086 * AM_Mod_Demod_DW->BodyDelay37_DSTATE_m) + 0.010231649301542016 * AM_Mod_Demod_DW->BodyDelay38_DSTATE_o) + 0.011733690154980873 * AM_Mod_Demod_DW->BodyDelay39_DSTATE_j) + 0.011860929797117791 * AM_Mod_Demod_DW->BodyDelay40_DSTATE_g) + 0.010394902058635835 * AM_Mod_Demod_DW->BodyDelay41_DSTATE_i) + 0.0072963585071920281 * AM_Mod_Demod_DW->BodyDelay42_DSTATE_b) + 0.0027351285363456158 * AM_Mod_Demod_DW->BodyDelay43_DSTATE_l) + -0.0029022640829190851 * AM_Mod_Demod_DW->BodyDelay44_DSTATE_m) + -0.0090303613490534569 * AM_Mod_Demod_DW->BodyDelay45_DSTATE_m) + -0.014908346162248804 * AM_Mod_Demod_DW->BodyDelay46_DSTATE_k) + -0.0197064977574164 * AM_Mod_Demod_DW->BodyDelay47_DSTATE_h) + -0.022589791907730208 * AM_Mod_Demod_DW->BodyDelay48_DSTATE_o) + -0.022810077984179462 * AM_Mod_Demod_DW->BodyDelay49_DSTATE_g) + -0.019796855049425213 * AM_Mod_Demod_DW->BodyDelay50_DSTATE_f) + -0.013235897185452272 * AM_Mod_Demod_DW->BodyDelay51_DSTATE_n) + -0.0031259793051983376 * AM_Mod_Demod_DW->BodyDelay52_DSTATE_f) + 0.010194294947491437 * AM_Mod_Demod_DW->BodyDelay53_DSTATE_m) + 0.026054244278894356 * AM_Mod_Demod_DW->BodyDelay54_DSTATE_i) + 0.043494554520832826 * AM_Mod_Demod_DW->BodyDelay55_DSTATE_j) + 0.061343218745072557 * AM_Mod_Demod_DW->BodyDelay56_DSTATE_a) + 0.078317178333283943 * AM_Mod_Demod_DW->BodyDelay57_DSTATE_e) + 0.093139403576610449 * AM_Mod_Demod_DW->BodyDelay58_DSTATE_c) + 0.10465933913932809 * AM_Mod_Demod_DW->BodyDelay59_DSTATE_o) + 0.11196373625413442 * AM_Mod_Demod_DW->BodyDelay60_DSTATE_p) + 0.11446594680127516 * AM_Mod_Demod_DW->BodyDelay61_DSTATE_k) + 0.11196373625413442 * AM_Mod_Demod_DW->BodyDelay62_DSTATE_j) + 0.10465933913932809 * AM_Mod_Demod_DW->BodyDelay63_DSTATE_p) + 0.093139403576610449 * AM_Mod_Demod_DW->BodyDelay64_DSTATE_h) + 0.078317178333283943 * AM_Mod_Demod_DW->BodyDelay65_DSTATE_p) + 0.061343218745072557 * AM_Mod_Demod_DW->BodyDelay66_DSTATE_e) + 0.043494554520832826 * AM_Mod_Demod_DW->BodyDelay67_DSTATE_i) + 0.026054244278894356 * AM_Mod_Demod_DW->BodyDelay68_DSTATE_d) + 0.010194294947491437 * AM_Mod_Demod_DW->BodyDelay69_DSTATE_e) + -0.0031259793051983376 * AM_Mod_Demod_DW->BodyDelay70_DSTATE_a) + -0.013235897185452272 * AM_Mod_Demod_DW->BodyDelay71_DSTATE_o) + -0.019796855049425213 * AM_Mod_Demod_DW->BodyDelay72_DSTATE_e) + -0.022810077984179462 * AM_Mod_Demod_DW->BodyDelay73_DSTATE_m) + -0.022589791907730208 * AM_Mod_Demod_DW->BodyDelay74_DSTATE_l) + -0.0197064977574164 * AM_Mod_Demod_DW->BodyDelay75_DSTATE_f) + -0.014908346162248804 * AM_Mod_Demod_DW->BodyDelay76_DSTATE_i) + -0.0090303613490534569 * AM_Mod_Demod_DW->BodyDelay77_DSTATE_i) + -0.0029022640829190851 * AM_Mod_Demod_DW->BodyDelay78_DSTATE_h) + 0.0027351285363456158 * AM_Mod_Demod_DW->BodyDelay79_DSTATE_i) + 0.0072963585071920281 * AM_Mod_Demod_DW->BodyDelay80_DSTATE_i) + 0.010394902058635835 * AM_Mod_Demod_DW->BodyDelay81_DSTATE_d) + 0.011860929797117791 * AM_Mod_Demod_DW->BodyDelay82_DSTATE_g) + 0.011733690154980873 * AM_Mod_Demod_DW->BodyDelay83_DSTATE_h) + 0.010231649301542016 * AM_Mod_Demod_DW->BodyDelay84_DSTATE_i) + 0.007706061797677086 * AM_Mod_Demod_DW->BodyDelay85_DSTATE_g) + 0.0045847769966693638 * AM_Mod_Demod_DW->BodyDelay86_DSTATE_b) + 0.0013138264428854381 * AM_Mod_Demod_DW->BodyDelay87_DSTATE_j) + -0.0016963616291432795 * AM_Mod_Demod_DW->BodyDelay88_DSTATE_c) + -0.0041142914417041185 * AM_Mod_Demod_DW->BodyDelay89_DSTATE_n) + -0.0057164828702958286 * AM_Mod_Demod_DW->BodyDelay90_DSTATE_h) + -0.0064004232313101652 * AM_Mod_Demod_DW->BodyDelay91_DSTATE_d) + -0.0061818605411153834 * AM_Mod_Demod_DW->BodyDelay92_DSTATE_k) + -0.0051785086311871885 * AM_Mod_Demod_DW->BodyDelay93_DSTATE_a) + -0.0035838734457672174 * AM_Mod_Demod_DW->BodyDelay94_DSTATE_l) + -0.0016354030026831713 * AM_Mod_Demod_DW->BodyDelay95_DSTATE_l) + 0.00041841180713779517 * AM_Mod_Demod_DW->BodyDelay96_DSTATE_m) + 0.0023479356927531688 * AM_Mod_Demod_DW->BodyDelay97_DSTATE_c) + 0.0039660325233911858 * AM_Mod_Demod_DW->BodyDelay98_DSTATE_j) + 0.0051435978324832722 * AM_Mod_Demod_DW->BodyDelay99_DSTATE_p) + 0.0058160412426258795 * AM_Mod_Demod_DW->BodyDelay100_DSTATE_k) + 0.0059813089687688441 * AM_Mod_Demod_DW->BodyDelay101_DSTATE_a) + 0.0056908511343630765 * AM_Mod_Demod_DW->BodyDelay102_DSTATE_e) + 0.00503574931805473 * AM_Mod_Demod_DW->BodyDelay103_DSTATE_o) + 0.0041303381252319688 * AM_Mod_Demod_DW->BodyDelay104_DSTATE_d) + 0.003095742162170359 * AM_Mod_Demod_DW->BodyDelay105_DSTATE_l) + 0.0020453098658091431 * AM_Mod_Demod_DW->BodyDelay106_DSTATE_a) + 0.0010735520035639234 * AM_Mod_Demod_DW->BodyDelay107_DSTATE_n) + 0.00024913305035715656 * AM_Mod_Demod_DW->BodyDelay108_DSTATE_l) + -0.00038780628029214185 * AM_Mod_Demod_DW->BodyDelay109_DSTATE_c) + -0.00082435891040477931 * AM_Mod_Demod_DW->BodyDelay110_DSTATE_m) + -0.0010706706574851095 * AM_Mod_Demod_DW->BodyDelay111_DSTATE_h) + -0.0011538991042739546 * AM_Mod_Demod_DW->BodyDelay112_DSTATE_l) + -0.001111253998127056 * AM_Mod_Demod_DW->BodyDelay113_DSTATE_k) + -0.00098345746573073466 * AM_Mod_Demod_DW->BodyDelay114_DSTATE_h) + -0.00080927381631219612 * AM_Mod_Demod_DW->BodyDelay115_DSTATE_g) + -0.00062136931524911987 * AM_Mod_Demod_DW->BodyDelay116_DSTATE_k) + -0.00044423670008644848 * AM_Mod_Demod_DW->BodyDelay117_DSTATE_a) + -0.00029337939435011953 * AM_Mod_Demod_DW->BodyDelay118_DSTATE_p) + -0.00017610623360101414 * AM_Mod_Demod_DW->BodyDelay119_DSTATE_d) + -9.3050702284968865E-5 * AM_Mod_Demod_DW->BodyDelay120_DSTATE_f) + -4.5662663818355689E-5 * AM_Mod_Demod_DW->BodyDelay121_DSTATE_p) * 2.0 + 0.5;
    AM_Mod_Demod_DW->BodyDelay120_DSTATE = AM_Mod_Demod_DW->BodyDelay119_DSTATE;
    AM_Mod_Demod_DW->BodyDelay119_DSTATE = AM_Mod_Demod_DW->BodyDelay118_DSTATE;
    AM_Mod_Demod_DW->BodyDelay118_DSTATE = AM_Mod_Demod_DW->BodyDelay117_DSTATE;
    AM_Mod_Demod_DW->BodyDelay117_DSTATE = AM_Mod_Demod_DW->BodyDelay116_DSTATE;
    AM_Mod_Demod_DW->BodyDelay116_DSTATE = AM_Mod_Demod_DW->BodyDelay115_DSTATE;
    AM_Mod_Demod_DW->BodyDelay115_DSTATE = AM_Mod_Demod_DW->BodyDelay114_DSTATE;
    AM_Mod_Demod_DW->BodyDelay114_DSTATE = AM_Mod_Demod_DW->BodyDelay113_DSTATE;
    AM_Mod_Demod_DW->BodyDelay113_DSTATE = AM_Mod_Demod_DW->BodyDelay112_DSTATE;
    AM_Mod_Demod_DW->BodyDelay112_DSTATE = AM_Mod_Demod_DW->BodyDelay111_DSTATE;
    AM_Mod_Demod_DW->BodyDelay111_DSTATE = AM_Mod_Demod_DW->BodyDelay110_DSTATE;
    AM_Mod_Demod_DW->BodyDelay110_DSTATE = AM_Mod_Demod_DW->BodyDelay109_DSTATE;
    AM_Mod_Demod_DW->BodyDelay109_DSTATE = AM_Mod_Demod_DW->BodyDelay108_DSTATE;
    AM_Mod_Demod_DW->BodyDelay108_DSTATE = AM_Mod_Demod_DW->BodyDelay107_DSTATE;
    AM_Mod_Demod_DW->BodyDelay107_DSTATE = AM_Mod_Demod_DW->BodyDelay106_DSTATE;
    AM_Mod_Demod_DW->BodyDelay106_DSTATE = AM_Mod_Demod_DW->BodyDelay105_DSTATE;
    AM_Mod_Demod_DW->BodyDelay105_DSTATE = AM_Mod_Demod_DW->BodyDelay104_DSTATE;
    AM_Mod_Demod_DW->BodyDelay104_DSTATE = AM_Mod_Demod_DW->BodyDelay103_DSTATE;
    AM_Mod_Demod_DW->BodyDelay103_DSTATE = AM_Mod_Demod_DW->BodyDelay102_DSTATE;
    AM_Mod_Demod_DW->BodyDelay102_DSTATE = AM_Mod_Demod_DW->BodyDelay101_DSTATE;
    AM_Mod_Demod_DW->BodyDelay101_DSTATE = AM_Mod_Demod_DW->BodyDelay100_DSTATE;
    AM_Mod_Demod_DW->BodyDelay100_DSTATE = AM_Mod_Demod_DW->BodyDelay99_DSTATE;
    AM_Mod_Demod_DW->BodyDelay99_DSTATE = AM_Mod_Demod_DW->BodyDelay98_DSTATE;
    AM_Mod_Demod_DW->BodyDelay98_DSTATE = AM_Mod_Demod_DW->BodyDelay97_DSTATE;
    AM_Mod_Demod_DW->BodyDelay97_DSTATE = AM_Mod_Demod_DW->BodyDelay96_DSTATE;
    AM_Mod_Demod_DW->BodyDelay96_DSTATE = AM_Mod_Demod_DW->BodyDelay95_DSTATE;
    AM_Mod_Demod_DW->BodyDelay95_DSTATE = AM_Mod_Demod_DW->BodyDelay94_DSTATE;
    AM_Mod_Demod_DW->BodyDelay94_DSTATE = AM_Mod_Demod_DW->BodyDelay93_DSTATE;
    AM_Mod_Demod_DW->BodyDelay93_DSTATE = AM_Mod_Demod_DW->BodyDelay92_DSTATE;
    AM_Mod_Demod_DW->BodyDelay92_DSTATE = AM_Mod_Demod_DW->BodyDelay91_DSTATE;
    AM_Mod_Demod_DW->BodyDelay91_DSTATE = AM_Mod_Demod_DW->BodyDelay90_DSTATE;
    AM_Mod_Demod_DW->BodyDelay90_DSTATE = AM_Mod_Demod_DW->BodyDelay89_DSTATE;
    AM_Mod_Demod_DW->BodyDelay89_DSTATE = AM_Mod_Demod_DW->BodyDelay88_DSTATE;
    AM_Mod_Demod_DW->BodyDelay88_DSTATE = AM_Mod_Demod_DW->BodyDelay87_DSTATE;
    AM_Mod_Demod_DW->BodyDelay87_DSTATE = AM_Mod_Demod_DW->BodyDelay86_DSTATE;
    AM_Mod_Demod_DW->BodyDelay86_DSTATE = AM_Mod_Demod_DW->BodyDelay85_DSTATE;
    AM_Mod_Demod_DW->BodyDelay85_DSTATE = AM_Mod_Demod_DW->BodyDelay84_DSTATE;
    AM_Mod_Demod_DW->BodyDelay84_DSTATE = AM_Mod_Demod_DW->BodyDelay83_DSTATE;
    AM_Mod_Demod_DW->BodyDelay83_DSTATE = AM_Mod_Demod_DW->BodyDelay82_DSTATE;
    AM_Mod_Demod_DW->BodyDelay82_DSTATE = AM_Mod_Demod_DW->BodyDelay81_DSTATE;
    AM_Mod_Demod_DW->BodyDelay81_DSTATE = AM_Mod_Demod_DW->BodyDelay80_DSTATE;
    AM_Mod_Demod_DW->BodyDelay80_DSTATE = AM_Mod_Demod_DW->BodyDelay79_DSTATE;
    AM_Mod_Demod_DW->BodyDelay79_DSTATE = AM_Mod_Demod_DW->BodyDelay78_DSTATE;
    AM_Mod_Demod_DW->BodyDelay78_DSTATE = AM_Mod_Demod_DW->BodyDelay77_DSTATE;
    AM_Mod_Demod_DW->BodyDelay77_DSTATE = AM_Mod_Demod_DW->BodyDelay76_DSTATE;
    AM_Mod_Demod_DW->BodyDelay76_DSTATE = AM_Mod_Demod_DW->BodyDelay75_DSTATE;
    AM_Mod_Demod_DW->BodyDelay75_DSTATE = AM_Mod_Demod_DW->BodyDelay74_DSTATE;
    AM_Mod_Demod_DW->BodyDelay74_DSTATE = AM_Mod_Demod_DW->BodyDelay73_DSTATE;
    AM_Mod_Demod_DW->BodyDelay73_DSTATE = AM_Mod_Demod_DW->BodyDelay72_DSTATE;
    AM_Mod_Demod_DW->BodyDelay72_DSTATE = AM_Mod_Demod_DW->BodyDelay71_DSTATE;
    AM_Mod_Demod_DW->BodyDelay71_DSTATE = AM_Mod_Demod_DW->BodyDelay70_DSTATE;
    AM_Mod_Demod_DW->BodyDelay70_DSTATE = AM_Mod_Demod_DW->BodyDelay69_DSTATE;
    AM_Mod_Demod_DW->BodyDelay69_DSTATE = AM_Mod_Demod_DW->BodyDelay68_DSTATE;
    AM_Mod_Demod_DW->BodyDelay68_DSTATE = AM_Mod_Demod_DW->BodyDelay67_DSTATE;
    AM_Mod_Demod_DW->BodyDelay67_DSTATE = AM_Mod_Demod_DW->BodyDelay66_DSTATE;
    AM_Mod_Demod_DW->BodyDelay66_DSTATE = AM_Mod_Demod_DW->BodyDelay65_DSTATE;
    AM_Mod_Demod_DW->BodyDelay65_DSTATE = AM_Mod_Demod_DW->BodyDelay64_DSTATE;
    AM_Mod_Demod_DW->BodyDelay64_DSTATE = AM_Mod_Demod_DW->BodyDelay63_DSTATE;
    AM_Mod_Demod_DW->BodyDelay63_DSTATE = AM_Mod_Demod_DW->BodyDelay62_DSTATE;
    AM_Mod_Demod_DW->BodyDelay62_DSTATE = AM_Mod_Demod_DW->BodyDelay61_DSTATE;
    AM_Mod_Demod_DW->BodyDelay61_DSTATE = AM_Mod_Demod_DW->BodyDelay60_DSTATE;
    AM_Mod_Demod_DW->BodyDelay60_DSTATE = AM_Mod_Demod_DW->BodyDelay59_DSTATE;
    AM_Mod_Demod_DW->BodyDelay59_DSTATE = AM_Mod_Demod_DW->BodyDelay58_DSTATE;
    AM_Mod_Demod_DW->BodyDelay58_DSTATE = AM_Mod_Demod_DW->BodyDelay57_DSTATE;
    AM_Mod_Demod_DW->BodyDelay57_DSTATE = AM_Mod_Demod_DW->BodyDelay56_DSTATE;
    AM_Mod_Demod_DW->BodyDelay56_DSTATE = AM_Mod_Demod_DW->BodyDelay55_DSTATE;
    AM_Mod_Demod_DW->BodyDelay55_DSTATE = AM_Mod_Demod_DW->BodyDelay54_DSTATE;
    AM_Mod_Demod_DW->BodyDelay54_DSTATE = AM_Mod_Demod_DW->BodyDelay53_DSTATE;
    AM_Mod_Demod_DW->BodyDelay53_DSTATE = AM_Mod_Demod_DW->BodyDelay52_DSTATE;
    AM_Mod_Demod_DW->BodyDelay52_DSTATE = AM_Mod_Demod_DW->BodyDelay51_DSTATE;
    AM_Mod_Demod_DW->BodyDelay51_DSTATE = AM_Mod_Demod_DW->BodyDelay50_DSTATE;
    AM_Mod_Demod_DW->BodyDelay50_DSTATE = AM_Mod_Demod_DW->BodyDelay49_DSTATE;
    AM_Mod_Demod_DW->BodyDelay49_DSTATE = AM_Mod_Demod_DW->BodyDelay48_DSTATE;
    AM_Mod_Demod_DW->BodyDelay48_DSTATE = AM_Mod_Demod_DW->BodyDelay47_DSTATE;
    AM_Mod_Demod_DW->BodyDelay47_DSTATE = AM_Mod_Demod_DW->BodyDelay46_DSTATE;
    AM_Mod_Demod_DW->BodyDelay46_DSTATE = AM_Mod_Demod_DW->BodyDelay45_DSTATE;
    AM_Mod_Demod_DW->BodyDelay45_DSTATE = AM_Mod_Demod_DW->BodyDelay44_DSTATE;
    AM_Mod_Demod_DW->BodyDelay44_DSTATE = AM_Mod_Demod_DW->BodyDelay43_DSTATE;
    AM_Mod_Demod_DW->BodyDelay43_DSTATE = AM_Mod_Demod_DW->BodyDelay42_DSTATE;
    AM_Mod_Demod_DW->BodyDelay42_DSTATE = AM_Mod_Demod_DW->BodyDelay41_DSTATE;
    AM_Mod_Demod_DW->BodyDelay41_DSTATE = AM_Mod_Demod_DW->BodyDelay40_DSTATE;
    AM_Mod_Demod_DW->BodyDelay40_DSTATE = AM_Mod_Demod_DW->BodyDelay39_DSTATE;
    AM_Mod_Demod_DW->BodyDelay39_DSTATE = AM_Mod_Demod_DW->BodyDelay38_DSTATE;
    AM_Mod_Demod_DW->BodyDelay38_DSTATE = AM_Mod_Demod_DW->BodyDelay37_DSTATE;
    AM_Mod_Demod_DW->BodyDelay37_DSTATE = AM_Mod_Demod_DW->BodyDelay36_DSTATE;
    AM_Mod_Demod_DW->BodyDelay36_DSTATE = AM_Mod_Demod_DW->BodyDelay35_DSTATE;
    AM_Mod_Demod_DW->BodyDelay35_DSTATE = AM_Mod_Demod_DW->BodyDelay34_DSTATE;
    AM_Mod_Demod_DW->BodyDelay34_DSTATE = AM_Mod_Demod_DW->BodyDelay33_DSTATE;
    AM_Mod_Demod_DW->BodyDelay33_DSTATE = AM_Mod_Demod_DW->BodyDelay32_DSTATE;
    AM_Mod_Demod_DW->BodyDelay32_DSTATE = AM_Mod_Demod_DW->BodyDelay31_DSTATE;
    AM_Mod_Demod_DW->BodyDelay31_DSTATE = AM_Mod_Demod_DW->BodyDelay30_DSTATE;
    AM_Mod_Demod_DW->BodyDelay30_DSTATE = AM_Mod_Demod_DW->BodyDelay29_DSTATE;
    AM_Mod_Demod_DW->BodyDelay29_DSTATE = AM_Mod_Demod_DW->BodyDelay28_DSTATE;
    AM_Mod_Demod_DW->BodyDelay28_DSTATE = AM_Mod_Demod_DW->BodyDelay27_DSTATE;
    AM_Mod_Demod_DW->BodyDelay27_DSTATE = AM_Mod_Demod_DW->BodyDelay26_DSTATE;
    AM_Mod_Demod_DW->BodyDelay26_DSTATE = AM_Mod_Demod_DW->BodyDelay25_DSTATE;
    AM_Mod_Demod_DW->BodyDelay25_DSTATE = AM_Mod_Demod_DW->BodyDelay24_DSTATE;
    AM_Mod_Demod_DW->BodyDelay24_DSTATE = AM_Mod_Demod_DW->BodyDelay23_DSTATE;
    AM_Mod_Demod_DW->BodyDelay23_DSTATE = AM_Mod_Demod_DW->BodyDelay22_DSTATE;
    AM_Mod_Demod_DW->BodyDelay22_DSTATE = AM_Mod_Demod_DW->BodyDelay21_DSTATE;
    AM_Mod_Demod_DW->BodyDelay21_DSTATE = AM_Mod_Demod_DW->BodyDelay20_DSTATE;
    AM_Mod_Demod_DW->BodyDelay20_DSTATE = AM_Mod_Demod_DW->BodyDelay19_DSTATE;
    AM_Mod_Demod_DW->BodyDelay19_DSTATE = AM_Mod_Demod_DW->BodyDelay18_DSTATE;
    AM_Mod_Demod_DW->BodyDelay18_DSTATE = AM_Mod_Demod_DW->BodyDelay17_DSTATE;
    AM_Mod_Demod_DW->BodyDelay17_DSTATE = AM_Mod_Demod_DW->BodyDelay16_DSTATE;
    AM_Mod_Demod_DW->BodyDelay16_DSTATE = AM_Mod_Demod_DW->BodyDelay15_DSTATE;
    AM_Mod_Demod_DW->BodyDelay15_DSTATE = AM_Mod_Demod_DW->BodyDelay14_DSTATE;
    AM_Mod_Demod_DW->BodyDelay14_DSTATE = AM_Mod_Demod_DW->BodyDelay13_DSTATE;
    AM_Mod_Demod_DW->BodyDelay13_DSTATE = AM_Mod_Demod_DW->BodyDelay12_DSTATE;
    AM_Mod_Demod_DW->BodyDelay12_DSTATE = AM_Mod_Demod_DW->BodyDelay11_DSTATE;
    AM_Mod_Demod_DW->BodyDelay11_DSTATE = AM_Mod_Demod_DW->BodyDelay10_DSTATE;
    AM_Mod_Demod_DW->BodyDelay10_DSTATE = AM_Mod_Demod_DW->BodyDelay9_DSTATE;
    AM_Mod_Demod_DW->BodyDelay9_DSTATE = AM_Mod_Demod_DW->BodyDelay8_DSTATE;
    AM_Mod_Demod_DW->BodyDelay8_DSTATE = AM_Mod_Demod_DW->BodyDelay7_DSTATE;
    AM_Mod_Demod_DW->BodyDelay7_DSTATE = AM_Mod_Demod_DW->BodyDelay6_DSTATE;
    AM_Mod_Demod_DW->BodyDelay6_DSTATE = AM_Mod_Demod_DW->BodyDelay5_DSTATE;
    AM_Mod_Demod_DW->BodyDelay5_DSTATE = AM_Mod_Demod_DW->BodyDelay4_DSTATE;
    AM_Mod_Demod_DW->BodyDelay4_DSTATE = AM_Mod_Demod_DW->BodyDelay3_DSTATE;
    AM_Mod_Demod_DW->BodyDelay3_DSTATE = rtb_BodyDelay2;
    AM_Mod_Demod_DW->BodyDelay121_DSTATE = rtb_BodyDelay120;
    AM_Mod_Demod_DW->BodyDelay2_DSTATE_c = rtb_b2;
    AM_Mod_Demod_DW->BodyDelay3_DSTATE_i = SquareRoot2;
    AM_Mod_Demod_DW->BodyDelay4_DSTATE_l = rtb_Square2;
    AM_Mod_Demod_DW->BodyDelay5_DSTATE_p = rtb_BodyDelay4_n;
    AM_Mod_Demod_DW->BodyDelay6_DSTATE_i = rtb_BodyDelay5_j;
    AM_Mod_Demod_DW->BodyDelay7_DSTATE_n = rtb_BodyDelay6_n;
    AM_Mod_Demod_DW->BodyDelay8_DSTATE_p = rtb_BodyDelay7_g;
    AM_Mod_Demod_DW->BodyDelay9_DSTATE_i = rtb_BodyDelay8_j;
    AM_Mod_Demod_DW->BodyDelay10_DSTATE_b = rtb_BodyDelay9_p;
    AM_Mod_Demod_DW->BodyDelay11_DSTATE_l = rtb_BodyDelay10_c;
    AM_Mod_Demod_DW->BodyDelay12_DSTATE_c = rtb_BodyDelay11_h;
    AM_Mod_Demod_DW->BodyDelay13_DSTATE_d = rtb_BodyDelay12_l;
    AM_Mod_Demod_DW->BodyDelay14_DSTATE_k = rtb_BodyDelay13_j;
    AM_Mod_Demod_DW->BodyDelay15_DSTATE_j = rtb_BodyDelay14_j;
    AM_Mod_Demod_DW->BodyDelay16_DSTATE_p = rtb_BodyDelay15_n;
    AM_Mod_Demod_DW->BodyDelay17_DSTATE_g = rtb_BodyDelay16_g;
    AM_Mod_Demod_DW->BodyDelay18_DSTATE_d = rtb_BodyDelay17_n;
    AM_Mod_Demod_DW->BodyDelay19_DSTATE_p = rtb_BodyDelay18_g;
    AM_Mod_Demod_DW->BodyDelay20_DSTATE_m = rtb_BodyDelay19_g;
    AM_Mod_Demod_DW->BodyDelay21_DSTATE_m = rtb_BodyDelay20_m;
    AM_Mod_Demod_DW->BodyDelay22_DSTATE_b = rtb_BodyDelay21_m;
    AM_Mod_Demod_DW->BodyDelay23_DSTATE_l = rtb_BodyDelay22_m;
    AM_Mod_Demod_DW->BodyDelay24_DSTATE_d = rtb_BodyDelay23_c;
    AM_Mod_Demod_DW->BodyDelay25_DSTATE_p = rtb_BodyDelay24_o;
    AM_Mod_Demod_DW->BodyDelay26_DSTATE_l = rtb_BodyDelay25_m;
    AM_Mod_Demod_DW->BodyDelay27_DSTATE_p = rtb_BodyDelay26_b;
    AM_Mod_Demod_DW->BodyDelay28_DSTATE_b = rtb_BodyDelay27_b;
    AM_Mod_Demod_DW->BodyDelay29_DSTATE_j = rtb_BodyDelay28_p;
    AM_Mod_Demod_DW->BodyDelay30_DSTATE_h = rtb_BodyDelay29_g;
    AM_Mod_Demod_DW->BodyDelay31_DSTATE_c = rtb_BodyDelay30_m;
    AM_Mod_Demod_DW->BodyDelay32_DSTATE_p = rtb_BodyDelay31_l;
    AM_Mod_Demod_DW->BodyDelay33_DSTATE_f = rtb_BodyDelay32_j;
    AM_Mod_Demod_DW->BodyDelay34_DSTATE_d = rtb_BodyDelay33_f;
    AM_Mod_Demod_DW->BodyDelay35_DSTATE_h = rtb_BodyDelay34_o;
    AM_Mod_Demod_DW->BodyDelay36_DSTATE_j = rtb_BodyDelay35_l;
    AM_Mod_Demod_DW->BodyDelay37_DSTATE_m = rtb_BodyDelay36_j;
    AM_Mod_Demod_DW->BodyDelay38_DSTATE_o = rtb_BodyDelay37_c;
    AM_Mod_Demod_DW->BodyDelay39_DSTATE_j = rtb_BodyDelay38_a;
    AM_Mod_Demod_DW->BodyDelay40_DSTATE_g = rtb_BodyDelay39_d;
    AM_Mod_Demod_DW->BodyDelay41_DSTATE_i = rtb_BodyDelay40_g;
    AM_Mod_Demod_DW->BodyDelay42_DSTATE_b = rtb_BodyDelay41_n;
    AM_Mod_Demod_DW->BodyDelay43_DSTATE_l = rtb_BodyDelay42_p;
    AM_Mod_Demod_DW->BodyDelay44_DSTATE_m = rtb_BodyDelay43_p;
    AM_Mod_Demod_DW->BodyDelay45_DSTATE_m = rtb_BodyDelay44_p;
    AM_Mod_Demod_DW->BodyDelay46_DSTATE_k = rtb_BodyDelay45_b;
    AM_Mod_Demod_DW->BodyDelay47_DSTATE_h = rtb_BodyDelay46_c;
    AM_Mod_Demod_DW->BodyDelay48_DSTATE_o = rtb_BodyDelay47_f;
    AM_Mod_Demod_DW->BodyDelay49_DSTATE_g = rtb_BodyDelay48_p;
    AM_Mod_Demod_DW->BodyDelay50_DSTATE_f = rtb_BodyDelay49_n;
    AM_Mod_Demod_DW->BodyDelay51_DSTATE_n = rtb_BodyDelay50_g;
    AM_Mod_Demod_DW->BodyDelay52_DSTATE_f = rtb_BodyDelay51_i;
    AM_Mod_Demod_DW->BodyDelay53_DSTATE_m = rtb_BodyDelay52_e;
    AM_Mod_Demod_DW->BodyDelay54_DSTATE_i = rtb_BodyDelay53_m;
    AM_Mod_Demod_DW->BodyDelay55_DSTATE_j = rtb_BodyDelay54_f;
    AM_Mod_Demod_DW->BodyDelay56_DSTATE_a = rtb_BodyDelay55_c;
    AM_Mod_Demod_DW->BodyDelay57_DSTATE_e = rtb_BodyDelay56_p;
    AM_Mod_Demod_DW->BodyDelay58_DSTATE_c = rtb_BodyDelay57_l;
    AM_Mod_Demod_DW->BodyDelay59_DSTATE_o = rtb_BodyDelay58_d;
    AM_Mod_Demod_DW->BodyDelay60_DSTATE_p = rtb_BodyDelay59_a;
    AM_Mod_Demod_DW->BodyDelay61_DSTATE_k = rtb_BodyDelay60_g;
    AM_Mod_Demod_DW->BodyDelay62_DSTATE_j = rtb_BodyDelay61_g;
    AM_Mod_Demod_DW->BodyDelay63_DSTATE_p = rtb_BodyDelay62_e;
    AM_Mod_Demod_DW->BodyDelay64_DSTATE_h = rtb_BodyDelay63_g;
    AM_Mod_Demod_DW->BodyDelay65_DSTATE_p = rtb_BodyDelay64_e;
    AM_Mod_Demod_DW->BodyDelay66_DSTATE_e = rtb_BodyDelay65_p;
    AM_Mod_Demod_DW->BodyDelay67_DSTATE_i = rtb_BodyDelay66_f;
    AM_Mod_Demod_DW->BodyDelay68_DSTATE_d = rtb_BodyDelay67_h;
    AM_Mod_Demod_DW->BodyDelay69_DSTATE_e = rtb_BodyDelay68_l;
    AM_Mod_Demod_DW->BodyDelay70_DSTATE_a = rtb_BodyDelay69_c;
    AM_Mod_Demod_DW->BodyDelay71_DSTATE_o = rtb_BodyDelay70_n;
    AM_Mod_Demod_DW->BodyDelay72_DSTATE_e = rtb_BodyDelay71_e;
    AM_Mod_Demod_DW->BodyDelay73_DSTATE_m = rtb_BodyDelay72_a;
    AM_Mod_Demod_DW->BodyDelay74_DSTATE_l = rtb_BodyDelay73_b;
    AM_Mod_Demod_DW->BodyDelay75_DSTATE_f = rtb_BodyDelay74_l;
    AM_Mod_Demod_DW->BodyDelay76_DSTATE_i = rtb_BodyDelay75_m;
    AM_Mod_Demod_DW->BodyDelay77_DSTATE_i = rtb_BodyDelay76_c;
    AM_Mod_Demod_DW->BodyDelay78_DSTATE_h = rtb_BodyDelay77_e;
    AM_Mod_Demod_DW->BodyDelay79_DSTATE_i = rtb_BodyDelay78_p;
    AM_Mod_Demod_DW->BodyDelay80_DSTATE_i = rtb_BodyDelay79_j;
    AM_Mod_Demod_DW->BodyDelay81_DSTATE_d = rtb_BodyDelay80_d;
    AM_Mod_Demod_DW->BodyDelay82_DSTATE_g = rtb_BodyDelay81_g;
    AM_Mod_Demod_DW->BodyDelay83_DSTATE_h = rtb_BodyDelay82_l;
    AM_Mod_Demod_DW->BodyDelay84_DSTATE_i = rtb_BodyDelay83_p;
    AM_Mod_Demod_DW->BodyDelay85_DSTATE_g = rtb_BodyDelay84_o;
    AM_Mod_Demod_DW->BodyDelay86_DSTATE_b = rtb_BodyDelay85_h;
    AM_Mod_Demod_DW->BodyDelay87_DSTATE_j = rtb_BodyDelay86_a;
    AM_Mod_Demod_DW->BodyDelay88_DSTATE_c = rtb_BodyDelay87_n;
    AM_Mod_Demod_DW->BodyDelay89_DSTATE_n = rtb_BodyDelay88_e;
    AM_Mod_Demod_DW->BodyDelay90_DSTATE_h = rtb_BodyDelay89_c;
    AM_Mod_Demod_DW->BodyDelay91_DSTATE_d = rtb_BodyDelay90_m;
    AM_Mod_Demod_DW->BodyDelay92_DSTATE_k = rtb_BodyDelay91_j;
    AM_Mod_Demod_DW->BodyDelay93_DSTATE_a = rtb_BodyDelay92_h;
    AM_Mod_Demod_DW->BodyDelay94_DSTATE_l = rtb_BodyDelay93_j;
    AM_Mod_Demod_DW->BodyDelay95_DSTATE_l = rtb_BodyDelay94_i;
    AM_Mod_Demod_DW->BodyDelay96_DSTATE_m = rtb_BodyDelay95_b;
    AM_Mod_Demod_DW->BodyDelay97_DSTATE_c = rtb_BodyDelay96_a;
    AM_Mod_Demod_DW->BodyDelay98_DSTATE_j = rtb_BodyDelay97_c;
    AM_Mod_Demod_DW->BodyDelay99_DSTATE_p = rtb_BodyDelay98_c;
    AM_Mod_Demod_DW->BodyDelay100_DSTATE_k = rtb_BodyDelay99_n;
    AM_Mod_Demod_DW->BodyDelay101_DSTATE_a = rtb_BodyDelay100_m;
    AM_Mod_Demod_DW->BodyDelay102_DSTATE_e = rtb_BodyDelay101_n;
    AM_Mod_Demod_DW->BodyDelay103_DSTATE_o = rtb_BodyDelay102_b;
    AM_Mod_Demod_DW->BodyDelay104_DSTATE_d = rtb_BodyDelay103_i;
    AM_Mod_Demod_DW->BodyDelay105_DSTATE_l = rtb_BodyDelay104_l;
    AM_Mod_Demod_DW->BodyDelay106_DSTATE_a = rtb_BodyDelay105_c;
    AM_Mod_Demod_DW->BodyDelay107_DSTATE_n = rtb_BodyDelay106_j;
    AM_Mod_Demod_DW->BodyDelay108_DSTATE_l = rtb_BodyDelay107_n;
    AM_Mod_Demod_DW->BodyDelay109_DSTATE_c = rtb_BodyDelay108_a;
    AM_Mod_Demod_DW->BodyDelay110_DSTATE_m = rtb_BodyDelay109_e;
    AM_Mod_Demod_DW->BodyDelay111_DSTATE_h = rtb_BodyDelay110_b;
    AM_Mod_Demod_DW->BodyDelay112_DSTATE_l = rtb_BodyDelay111_f;
    AM_Mod_Demod_DW->BodyDelay113_DSTATE_k = rtb_BodyDelay112_i;
    AM_Mod_Demod_DW->BodyDelay114_DSTATE_h = rtb_BodyDelay113_m;
    AM_Mod_Demod_DW->BodyDelay115_DSTATE_g = rtb_BodyDelay114_h;
    AM_Mod_Demod_DW->BodyDelay116_DSTATE_k = rtb_BodyDelay115_n;
    AM_Mod_Demod_DW->BodyDelay117_DSTATE_a = rtb_BodyDelay116_l;
    AM_Mod_Demod_DW->BodyDelay118_DSTATE_p = rtb_BodyDelay117_o;
    AM_Mod_Demod_DW->BodyDelay119_DSTATE_d = rtb_BodyDelay118_m;
    AM_Mod_Demod_DW->BodyDelay120_DSTATE_f = rtb_BodyDelay119_m;
    AM_Mod_Demod_DW->BodyDelay121_DSTATE_p = rtb_BodyDelay120_g;
}
void AM_Mod_Demod_init(RT_MODEL_AM_Mod_Demod_T* AM_Mod_Demod_M, real_T AM_Mod_Demod_Y_Out2[2], real_T* AM_Mod_Demod_U_In1, real_T* AM_Mod_Demod_Y_Out1, real_T* AM_Mod_Demod_Y_Out5, real_T* AM_Mod_Demod_Y_Out6, real_T* AM_Mod_Demod_Y_Out7) {
    (void)memset((void*)AM_Mod_Demod_M, 0, sizeof(RT_MODEL_AM_Mod_Demod_T));
    AM_Mod_Demod_M->blockIO = &AM_Mod_Demod_B_instance;
    AM_Mod_Demod_M->dwork = &AM_Mod_Demod_DW_instance;
    AM_Mod_Demod_M->prevZCSigState = &AM_Mod_Demod_PrevZCX_instance;
    struct DW_AM_Mod_Demod_T * AM_Mod_Demod_DW = AM_Mod_Demod_M->dwork;
    AM_Mod_Demod_DW->BodyDelay120_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay119_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay118_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay117_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay116_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay115_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay114_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay113_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay112_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay111_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay110_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay109_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay108_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay107_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay106_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay105_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay104_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay103_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay102_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay101_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay100_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay99_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay98_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay97_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay96_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay95_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay94_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay93_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay92_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay91_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay90_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay89_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay88_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay87_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay86_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay85_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay84_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay83_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay82_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay81_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay80_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay79_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay78_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay77_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay76_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay75_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay74_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay73_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay72_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay71_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay70_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay69_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay68_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay67_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay66_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay65_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay64_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay63_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay62_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay61_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay60_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay59_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay58_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay57_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay56_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay55_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay54_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay53_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay52_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay51_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay50_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay49_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay48_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay47_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay46_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay45_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay44_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay43_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay42_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay41_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay40_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay39_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay38_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay37_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay36_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay35_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay34_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay33_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay32_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay31_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay30_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay29_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay28_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay27_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay26_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay25_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay24_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay23_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay22_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay21_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay20_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay19_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay18_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay17_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay16_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay15_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay14_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay13_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay12_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay11_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay10_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay9_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay8_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay7_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay6_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay5_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay4_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay3_DSTATE = 0.0;
    AM_Mod_Demod_DW->BodyDelay2_DSTATE = 0.0;
    AM_Mod_Demod_DW->CarrierWave_AccFreqNorm = 0.0;
    AM_Mod_Demod_DW->BodyDelay121_DSTATE = 0.0;
    AM_Mod_Demod_DW->CarrierWaveCOS_AccFreqNorm = 0.0;
    int32_T i = {0};
    for (i = 0;i < 29;i++) {
        __CPROVER_cover(37);
        AM_Mod_Demod_DW->DigitalFilter_states[i] = 0.0;
    }
    __CPROVER_cover(38);
    AM_Mod_Demod_DW->CarrierWaveSIN_AccFreqNorm = 0.0;
    int32_T i_dup1 = {0};
    for (i = 0;i < 29;i++) {
        __CPROVER_cover(39);
        AM_Mod_Demod_DW->DigitalFilter_states_h[i] = 0.0;
    }
    __CPROVER_cover(40);
    AM_Mod_Demod_DW->CarrierWave1_AccFreqNorm = 0.0;
    AM_Mod_Demod_DW->BodyDelay2_DSTATE_c = 0.0;
    AM_Mod_Demod_DW->BodyDelay3_DSTATE_i = 0.0;
    AM_Mod_Demod_DW->BodyDelay4_DSTATE_l = 0.0;
    AM_Mod_Demod_DW->BodyDelay5_DSTATE_p = 0.0;
    AM_Mod_Demod_DW->BodyDelay6_DSTATE_i = 0.0;
    AM_Mod_Demod_DW->BodyDelay7_DSTATE_n = 0.0;
    AM_Mod_Demod_DW->BodyDelay8_DSTATE_p = 0.0;
    AM_Mod_Demod_DW->BodyDelay9_DSTATE_i = 0.0;
    AM_Mod_Demod_DW->BodyDelay10_DSTATE_b = 0.0;
    AM_Mod_Demod_DW->BodyDelay11_DSTATE_l = 0.0;
    AM_Mod_Demod_DW->BodyDelay12_DSTATE_c = 0.0;
    AM_Mod_Demod_DW->BodyDelay13_DSTATE_d = 0.0;
    AM_Mod_Demod_DW->BodyDelay14_DSTATE_k = 0.0;
    AM_Mod_Demod_DW->BodyDelay15_DSTATE_j = 0.0;
    AM_Mod_Demod_DW->BodyDelay16_DSTATE_p = 0.0;
    AM_Mod_Demod_DW->BodyDelay17_DSTATE_g = 0.0;
    AM_Mod_Demod_DW->BodyDelay18_DSTATE_d = 0.0;
    AM_Mod_Demod_DW->BodyDelay19_DSTATE_p = 0.0;
    AM_Mod_Demod_DW->BodyDelay20_DSTATE_m = 0.0;
    AM_Mod_Demod_DW->BodyDelay21_DSTATE_m = 0.0;
    AM_Mod_Demod_DW->BodyDelay22_DSTATE_b = 0.0;
    AM_Mod_Demod_DW->BodyDelay23_DSTATE_l = 0.0;
    AM_Mod_Demod_DW->BodyDelay24_DSTATE_d = 0.0;
    AM_Mod_Demod_DW->BodyDelay25_DSTATE_p = 0.0;
    AM_Mod_Demod_DW->BodyDelay26_DSTATE_l = 0.0;
    AM_Mod_Demod_DW->BodyDelay27_DSTATE_p = 0.0;
    AM_Mod_Demod_DW->BodyDelay28_DSTATE_b = 0.0;
    AM_Mod_Demod_DW->BodyDelay29_DSTATE_j = 0.0;
    AM_Mod_Demod_DW->BodyDelay30_DSTATE_h = 0.0;
    AM_Mod_Demod_DW->BodyDelay31_DSTATE_c = 0.0;
    AM_Mod_Demod_DW->BodyDelay32_DSTATE_p = 0.0;
    AM_Mod_Demod_DW->BodyDelay33_DSTATE_f = 0.0;
    AM_Mod_Demod_DW->BodyDelay34_DSTATE_d = 0.0;
    AM_Mod_Demod_DW->BodyDelay35_DSTATE_h = 0.0;
    AM_Mod_Demod_DW->BodyDelay36_DSTATE_j = 0.0;
    AM_Mod_Demod_DW->BodyDelay37_DSTATE_m = 0.0;
    AM_Mod_Demod_DW->BodyDelay38_DSTATE_o = 0.0;
    AM_Mod_Demod_DW->BodyDelay39_DSTATE_j = 0.0;
    AM_Mod_Demod_DW->BodyDelay40_DSTATE_g = 0.0;
    AM_Mod_Demod_DW->BodyDelay41_DSTATE_i = 0.0;
    AM_Mod_Demod_DW->BodyDelay42_DSTATE_b = 0.0;
    AM_Mod_Demod_DW->BodyDelay43_DSTATE_l = 0.0;
    AM_Mod_Demod_DW->BodyDelay44_DSTATE_m = 0.0;
    AM_Mod_Demod_DW->BodyDelay45_DSTATE_m = 0.0;
    AM_Mod_Demod_DW->BodyDelay46_DSTATE_k = 0.0;
    AM_Mod_Demod_DW->BodyDelay47_DSTATE_h = 0.0;
    AM_Mod_Demod_DW->BodyDelay48_DSTATE_o = 0.0;
    AM_Mod_Demod_DW->BodyDelay49_DSTATE_g = 0.0;
    AM_Mod_Demod_DW->BodyDelay50_DSTATE_f = 0.0;
    AM_Mod_Demod_DW->BodyDelay51_DSTATE_n = 0.0;
    AM_Mod_Demod_DW->BodyDelay52_DSTATE_f = 0.0;
    AM_Mod_Demod_DW->BodyDelay53_DSTATE_m = 0.0;
    AM_Mod_Demod_DW->BodyDelay54_DSTATE_i = 0.0;
    AM_Mod_Demod_DW->BodyDelay55_DSTATE_j = 0.0;
    AM_Mod_Demod_DW->BodyDelay56_DSTATE_a = 0.0;
    AM_Mod_Demod_DW->BodyDelay57_DSTATE_e = 0.0;
    AM_Mod_Demod_DW->BodyDelay58_DSTATE_c = 0.0;
    AM_Mod_Demod_DW->BodyDelay59_DSTATE_o = 0.0;
    AM_Mod_Demod_DW->BodyDelay60_DSTATE_p = 0.0;
    AM_Mod_Demod_DW->BodyDelay61_DSTATE_k = 0.0;
    AM_Mod_Demod_DW->BodyDelay62_DSTATE_j = 0.0;
    AM_Mod_Demod_DW->BodyDelay63_DSTATE_p = 0.0;
    AM_Mod_Demod_DW->BodyDelay64_DSTATE_h = 0.0;
    AM_Mod_Demod_DW->BodyDelay65_DSTATE_p = 0.0;
    AM_Mod_Demod_DW->BodyDelay66_DSTATE_e = 0.0;
    AM_Mod_Demod_DW->BodyDelay67_DSTATE_i = 0.0;
    AM_Mod_Demod_DW->BodyDelay68_DSTATE_d = 0.0;
    AM_Mod_Demod_DW->BodyDelay69_DSTATE_e = 0.0;
    AM_Mod_Demod_DW->BodyDelay70_DSTATE_a = 0.0;
    AM_Mod_Demod_DW->BodyDelay71_DSTATE_o = 0.0;
    AM_Mod_Demod_DW->BodyDelay72_DSTATE_e = 0.0;
    AM_Mod_Demod_DW->BodyDelay73_DSTATE_m = 0.0;
    AM_Mod_Demod_DW->BodyDelay74_DSTATE_l = 0.0;
    AM_Mod_Demod_DW->BodyDelay75_DSTATE_f = 0.0;
    AM_Mod_Demod_DW->BodyDelay76_DSTATE_i = 0.0;
    AM_Mod_Demod_DW->BodyDelay77_DSTATE_i = 0.0;
    AM_Mod_Demod_DW->BodyDelay78_DSTATE_h = 0.0;
    AM_Mod_Demod_DW->BodyDelay79_DSTATE_i = 0.0;
    AM_Mod_Demod_DW->BodyDelay80_DSTATE_i = 0.0;
    AM_Mod_Demod_DW->BodyDelay81_DSTATE_d = 0.0;
    AM_Mod_Demod_DW->BodyDelay82_DSTATE_g = 0.0;
    AM_Mod_Demod_DW->BodyDelay83_DSTATE_h = 0.0;
    AM_Mod_Demod_DW->BodyDelay84_DSTATE_i = 0.0;
    AM_Mod_Demod_DW->BodyDelay85_DSTATE_g = 0.0;
    AM_Mod_Demod_DW->BodyDelay86_DSTATE_b = 0.0;
    AM_Mod_Demod_DW->BodyDelay87_DSTATE_j = 0.0;
    AM_Mod_Demod_DW->BodyDelay88_DSTATE_c = 0.0;
    AM_Mod_Demod_DW->BodyDelay89_DSTATE_n = 0.0;
    AM_Mod_Demod_DW->BodyDelay90_DSTATE_h = 0.0;
    AM_Mod_Demod_DW->BodyDelay91_DSTATE_d = 0.0;
    AM_Mod_Demod_DW->BodyDelay92_DSTATE_k = 0.0;
    AM_Mod_Demod_DW->BodyDelay93_DSTATE_a = 0.0;
    AM_Mod_Demod_DW->BodyDelay94_DSTATE_l = 0.0;
    AM_Mod_Demod_DW->BodyDelay95_DSTATE_l = 0.0;
    AM_Mod_Demod_DW->BodyDelay96_DSTATE_m = 0.0;
    AM_Mod_Demod_DW->BodyDelay97_DSTATE_c = 0.0;
    AM_Mod_Demod_DW->BodyDelay98_DSTATE_j = 0.0;
    AM_Mod_Demod_DW->BodyDelay99_DSTATE_p = 0.0;
    AM_Mod_Demod_DW->BodyDelay100_DSTATE_k = 0.0;
    AM_Mod_Demod_DW->BodyDelay101_DSTATE_a = 0.0;
    AM_Mod_Demod_DW->BodyDelay102_DSTATE_e = 0.0;
    AM_Mod_Demod_DW->BodyDelay103_DSTATE_o = 0.0;
    AM_Mod_Demod_DW->BodyDelay104_DSTATE_d = 0.0;
    AM_Mod_Demod_DW->BodyDelay105_DSTATE_l = 0.0;
    AM_Mod_Demod_DW->BodyDelay106_DSTATE_a = 0.0;
    AM_Mod_Demod_DW->BodyDelay107_DSTATE_n = 0.0;
    AM_Mod_Demod_DW->BodyDelay108_DSTATE_l = 0.0;
    AM_Mod_Demod_DW->BodyDelay109_DSTATE_c = 0.0;
    AM_Mod_Demod_DW->BodyDelay110_DSTATE_m = 0.0;
    AM_Mod_Demod_DW->BodyDelay111_DSTATE_h = 0.0;
    AM_Mod_Demod_DW->BodyDelay112_DSTATE_l = 0.0;
    AM_Mod_Demod_DW->BodyDelay113_DSTATE_k = 0.0;
    AM_Mod_Demod_DW->BodyDelay114_DSTATE_h = 0.0;
    AM_Mod_Demod_DW->BodyDelay115_DSTATE_g = 0.0;
    AM_Mod_Demod_DW->BodyDelay116_DSTATE_k = 0.0;
    AM_Mod_Demod_DW->BodyDelay117_DSTATE_a = 0.0;
    AM_Mod_Demod_DW->BodyDelay118_DSTATE_p = 0.0;
    AM_Mod_Demod_DW->BodyDelay119_DSTATE_d = 0.0;
    AM_Mod_Demod_DW->BodyDelay120_DSTATE_f = 0.0;
    AM_Mod_Demod_DW->BodyDelay121_DSTATE_p = 0.0;
    int32_T i_dup2 = {0};
    for (i = 0;i < 58;i++) {
        __CPROVER_cover(41);
        AM_Mod_Demod_DW->DigitalFilter_simContextBuf[i] = 0.0;
    }
    __CPROVER_cover(42);
    int32_T i_dup3 = {0};
    for (i = 0;i < 30;i++) {
        __CPROVER_cover(43);
        AM_Mod_Demod_DW->DigitalFilter_simRevCoeff[i] = 0.0;
    }
    __CPROVER_cover(44);
    int32_T i_dup4 = {0};
    for (i = 0;i < 58;i++) {
        __CPROVER_cover(45);
        AM_Mod_Demod_DW->DigitalFilter_simContextBuf_n[i] = 0.0;
    }
    __CPROVER_cover(46);
    int32_T i_dup5 = {0};
    for (i = 0;i < 30;i++) {
        __CPROVER_cover(47);
        AM_Mod_Demod_DW->DigitalFilter_simRevCoeff_o[i] = 0.0;
    }
    __CPROVER_cover(48);
    *AM_Mod_Demod_U_In1 = 0.0;
    AM_Mod_Demod_Y_Out2[0] = 0.0;
    AM_Mod_Demod_Y_Out2[1] = 0.0;
    *AM_Mod_Demod_Y_Out1 = 0.0;
    *AM_Mod_Demod_Y_Out5 = 0.0;
    *AM_Mod_Demod_Y_Out6 = 0.0;
    *AM_Mod_Demod_Y_Out7 = 0.0;
    int32_T i_dup6 = {0};
    AM_Mod_Demod_DW->CarrierWave_AccFreqNorm = 0.0;
    AM_Mod_Demod_DW->CarrierWaveCOS_AccFreqNorm = 0.0;
    AM_Mod_Demod_DW->CarrierWaveSIN_AccFreqNorm = 1.5707963267948966;
    AM_Mod_Demod_DW->CarrierWave1_AccFreqNorm = 0.0;
    for (i = 0;i < 29;i++) {
        __CPROVER_cover(49);
        AM_Mod_Demod_DW->DigitalFilter_states[i] = 0.0;
        AM_Mod_Demod_DW->DigitalFilter_states_h[i] = 0.0;
    }
    __CPROVER_cover(50);
}

void globalVariableInit() {
    AM_Mod_Demod_instance.Timing.TaskCounters.TID; // Need to be modified: Variable AM_Mod_Demod_instance.Timing.TaskCounters.TID
    AM_Mod_Demod_instance.Timing.clockTick0; // Need to be modified: Variable AM_Mod_Demod_instance.Timing.clockTick0
    AM_Mod_Demod_DW_instance.BodyDelay120_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay120_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay119_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay119_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay118_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay118_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay117_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay117_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay116_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay116_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay115_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay115_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay114_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay114_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay113_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay113_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay112_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay112_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay111_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay111_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay110_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay110_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay109_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay109_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay108_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay108_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay107_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay107_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay106_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay106_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay105_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay105_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay104_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay104_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay103_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay103_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay102_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay102_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay101_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay101_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay100_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay100_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay99_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay99_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay98_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay98_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay97_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay97_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay96_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay96_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay95_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay95_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay94_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay94_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay93_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay93_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay92_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay92_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay91_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay91_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay90_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay90_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay89_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay89_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay88_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay88_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay87_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay87_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay86_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay86_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay85_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay85_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay84_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay84_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay83_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay83_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay82_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay82_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay81_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay81_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay80_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay80_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay79_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay79_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay78_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay78_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay77_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay77_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay76_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay76_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay75_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay75_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay74_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay74_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay73_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay73_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay72_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay72_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay71_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay71_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay70_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay70_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay69_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay69_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay68_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay68_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay67_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay67_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay66_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay66_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay65_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay65_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay64_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay64_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay63_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay63_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay62_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay62_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay61_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay61_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay60_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay60_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay59_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay59_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay58_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay58_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay57_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay57_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay56_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay56_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay55_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay55_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay54_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay54_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay53_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay53_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay52_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay52_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay51_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay51_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay50_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay50_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay49_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay49_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay48_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay48_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay47_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay47_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay46_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay46_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay45_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay45_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay44_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay44_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay43_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay43_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay42_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay42_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay41_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay41_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay40_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay40_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay39_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay39_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay38_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay38_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay37_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay37_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay36_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay36_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay35_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay35_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay34_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay34_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay33_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay33_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay32_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay32_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay31_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay31_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay30_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay30_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay29_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay29_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay28_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay28_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay27_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay27_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay26_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay26_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay25_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay25_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay24_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay24_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay23_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay23_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay22_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay22_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay21_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay21_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay20_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay20_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay19_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay19_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay18_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay18_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay17_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay17_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay16_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay16_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay15_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay15_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay14_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay14_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay13_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay13_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay12_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay12_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay11_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay11_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay10_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay10_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay9_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay9_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay8_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay8_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay7_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay7_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay6_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay6_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay5_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay5_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay4_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay4_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay3_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay3_DSTATE
    AM_Mod_Demod_DW_instance.BodyDelay2_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay2_DSTATE
    AM_Mod_Demod_DW_instance.CarrierWave_AccFreqNorm; // Need to be modified: Variable AM_Mod_Demod_DW_instance.CarrierWave_AccFreqNorm
    AM_Mod_Demod_DW_instance.BodyDelay121_DSTATE; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay121_DSTATE
    AM_Mod_Demod_DW_instance.CarrierWaveCOS_AccFreqNorm; // Need to be modified: Variable AM_Mod_Demod_DW_instance.CarrierWaveCOS_AccFreqNorm
    AM_Mod_Demod_DW_instance.DigitalFilter_states; // Need to be modified: Variable AM_Mod_Demod_DW_instance.DigitalFilter_states
    AM_Mod_Demod_DW_instance.CarrierWaveSIN_AccFreqNorm; // Need to be modified: Variable AM_Mod_Demod_DW_instance.CarrierWaveSIN_AccFreqNorm
    AM_Mod_Demod_DW_instance.DigitalFilter_states_h; // Need to be modified: Variable AM_Mod_Demod_DW_instance.DigitalFilter_states_h
    AM_Mod_Demod_DW_instance.CarrierWave1_AccFreqNorm; // Need to be modified: Variable AM_Mod_Demod_DW_instance.CarrierWave1_AccFreqNorm
    AM_Mod_Demod_DW_instance.BodyDelay2_DSTATE_c; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay2_DSTATE_c
    AM_Mod_Demod_DW_instance.BodyDelay3_DSTATE_i; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay3_DSTATE_i
    AM_Mod_Demod_DW_instance.BodyDelay4_DSTATE_l; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay4_DSTATE_l
    AM_Mod_Demod_DW_instance.BodyDelay5_DSTATE_p; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay5_DSTATE_p
    AM_Mod_Demod_DW_instance.BodyDelay6_DSTATE_i; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay6_DSTATE_i
    AM_Mod_Demod_DW_instance.BodyDelay7_DSTATE_n; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay7_DSTATE_n
    AM_Mod_Demod_DW_instance.BodyDelay8_DSTATE_p; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay8_DSTATE_p
    AM_Mod_Demod_DW_instance.BodyDelay9_DSTATE_i; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay9_DSTATE_i
    AM_Mod_Demod_DW_instance.BodyDelay10_DSTATE_b; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay10_DSTATE_b
    AM_Mod_Demod_DW_instance.BodyDelay11_DSTATE_l; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay11_DSTATE_l
    AM_Mod_Demod_DW_instance.BodyDelay12_DSTATE_c; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay12_DSTATE_c
    AM_Mod_Demod_DW_instance.BodyDelay13_DSTATE_d; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay13_DSTATE_d
    AM_Mod_Demod_DW_instance.BodyDelay14_DSTATE_k; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay14_DSTATE_k
    AM_Mod_Demod_DW_instance.BodyDelay15_DSTATE_j; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay15_DSTATE_j
    AM_Mod_Demod_DW_instance.BodyDelay16_DSTATE_p; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay16_DSTATE_p
    AM_Mod_Demod_DW_instance.BodyDelay17_DSTATE_g; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay17_DSTATE_g
    AM_Mod_Demod_DW_instance.BodyDelay18_DSTATE_d; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay18_DSTATE_d
    AM_Mod_Demod_DW_instance.BodyDelay19_DSTATE_p; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay19_DSTATE_p
    AM_Mod_Demod_DW_instance.BodyDelay20_DSTATE_m; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay20_DSTATE_m
    AM_Mod_Demod_DW_instance.BodyDelay21_DSTATE_m; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay21_DSTATE_m
    AM_Mod_Demod_DW_instance.BodyDelay22_DSTATE_b; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay22_DSTATE_b
    AM_Mod_Demod_DW_instance.BodyDelay23_DSTATE_l; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay23_DSTATE_l
    AM_Mod_Demod_DW_instance.BodyDelay24_DSTATE_d; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay24_DSTATE_d
    AM_Mod_Demod_DW_instance.BodyDelay25_DSTATE_p; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay25_DSTATE_p
    AM_Mod_Demod_DW_instance.BodyDelay26_DSTATE_l; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay26_DSTATE_l
    AM_Mod_Demod_DW_instance.BodyDelay27_DSTATE_p; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay27_DSTATE_p
    AM_Mod_Demod_DW_instance.BodyDelay28_DSTATE_b; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay28_DSTATE_b
    AM_Mod_Demod_DW_instance.BodyDelay29_DSTATE_j; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay29_DSTATE_j
    AM_Mod_Demod_DW_instance.BodyDelay30_DSTATE_h; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay30_DSTATE_h
    AM_Mod_Demod_DW_instance.BodyDelay31_DSTATE_c; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay31_DSTATE_c
    AM_Mod_Demod_DW_instance.BodyDelay32_DSTATE_p; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay32_DSTATE_p
    AM_Mod_Demod_DW_instance.BodyDelay33_DSTATE_f; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay33_DSTATE_f
    AM_Mod_Demod_DW_instance.BodyDelay34_DSTATE_d; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay34_DSTATE_d
    AM_Mod_Demod_DW_instance.BodyDelay35_DSTATE_h; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay35_DSTATE_h
    AM_Mod_Demod_DW_instance.BodyDelay36_DSTATE_j; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay36_DSTATE_j
    AM_Mod_Demod_DW_instance.BodyDelay37_DSTATE_m; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay37_DSTATE_m
    AM_Mod_Demod_DW_instance.BodyDelay38_DSTATE_o; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay38_DSTATE_o
    AM_Mod_Demod_DW_instance.BodyDelay39_DSTATE_j; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay39_DSTATE_j
    AM_Mod_Demod_DW_instance.BodyDelay40_DSTATE_g; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay40_DSTATE_g
    AM_Mod_Demod_DW_instance.BodyDelay41_DSTATE_i; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay41_DSTATE_i
    AM_Mod_Demod_DW_instance.BodyDelay42_DSTATE_b; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay42_DSTATE_b
    AM_Mod_Demod_DW_instance.BodyDelay43_DSTATE_l; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay43_DSTATE_l
    AM_Mod_Demod_DW_instance.BodyDelay44_DSTATE_m; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay44_DSTATE_m
    AM_Mod_Demod_DW_instance.BodyDelay45_DSTATE_m; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay45_DSTATE_m
    AM_Mod_Demod_DW_instance.BodyDelay46_DSTATE_k; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay46_DSTATE_k
    AM_Mod_Demod_DW_instance.BodyDelay47_DSTATE_h; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay47_DSTATE_h
    AM_Mod_Demod_DW_instance.BodyDelay48_DSTATE_o; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay48_DSTATE_o
    AM_Mod_Demod_DW_instance.BodyDelay49_DSTATE_g; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay49_DSTATE_g
    AM_Mod_Demod_DW_instance.BodyDelay50_DSTATE_f; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay50_DSTATE_f
    AM_Mod_Demod_DW_instance.BodyDelay51_DSTATE_n; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay51_DSTATE_n
    AM_Mod_Demod_DW_instance.BodyDelay52_DSTATE_f; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay52_DSTATE_f
    AM_Mod_Demod_DW_instance.BodyDelay53_DSTATE_m; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay53_DSTATE_m
    AM_Mod_Demod_DW_instance.BodyDelay54_DSTATE_i; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay54_DSTATE_i
    AM_Mod_Demod_DW_instance.BodyDelay55_DSTATE_j; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay55_DSTATE_j
    AM_Mod_Demod_DW_instance.BodyDelay56_DSTATE_a; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay56_DSTATE_a
    AM_Mod_Demod_DW_instance.BodyDelay57_DSTATE_e; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay57_DSTATE_e
    AM_Mod_Demod_DW_instance.BodyDelay58_DSTATE_c; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay58_DSTATE_c
    AM_Mod_Demod_DW_instance.BodyDelay59_DSTATE_o; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay59_DSTATE_o
    AM_Mod_Demod_DW_instance.BodyDelay60_DSTATE_p; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay60_DSTATE_p
    AM_Mod_Demod_DW_instance.BodyDelay61_DSTATE_k; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay61_DSTATE_k
    AM_Mod_Demod_DW_instance.BodyDelay62_DSTATE_j; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay62_DSTATE_j
    AM_Mod_Demod_DW_instance.BodyDelay63_DSTATE_p; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay63_DSTATE_p
    AM_Mod_Demod_DW_instance.BodyDelay64_DSTATE_h; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay64_DSTATE_h
    AM_Mod_Demod_DW_instance.BodyDelay65_DSTATE_p; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay65_DSTATE_p
    AM_Mod_Demod_DW_instance.BodyDelay66_DSTATE_e; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay66_DSTATE_e
    AM_Mod_Demod_DW_instance.BodyDelay67_DSTATE_i; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay67_DSTATE_i
    AM_Mod_Demod_DW_instance.BodyDelay68_DSTATE_d; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay68_DSTATE_d
    AM_Mod_Demod_DW_instance.BodyDelay69_DSTATE_e; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay69_DSTATE_e
    AM_Mod_Demod_DW_instance.BodyDelay70_DSTATE_a; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay70_DSTATE_a
    AM_Mod_Demod_DW_instance.BodyDelay71_DSTATE_o; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay71_DSTATE_o
    AM_Mod_Demod_DW_instance.BodyDelay72_DSTATE_e; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay72_DSTATE_e
    AM_Mod_Demod_DW_instance.BodyDelay73_DSTATE_m; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay73_DSTATE_m
    AM_Mod_Demod_DW_instance.BodyDelay74_DSTATE_l; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay74_DSTATE_l
    AM_Mod_Demod_DW_instance.BodyDelay75_DSTATE_f; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay75_DSTATE_f
    AM_Mod_Demod_DW_instance.BodyDelay76_DSTATE_i; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay76_DSTATE_i
    AM_Mod_Demod_DW_instance.BodyDelay77_DSTATE_i; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay77_DSTATE_i
    AM_Mod_Demod_DW_instance.BodyDelay78_DSTATE_h; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay78_DSTATE_h
    AM_Mod_Demod_DW_instance.BodyDelay79_DSTATE_i; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay79_DSTATE_i
    AM_Mod_Demod_DW_instance.BodyDelay80_DSTATE_i; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay80_DSTATE_i
    AM_Mod_Demod_DW_instance.BodyDelay81_DSTATE_d; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay81_DSTATE_d
    AM_Mod_Demod_DW_instance.BodyDelay82_DSTATE_g; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay82_DSTATE_g
    AM_Mod_Demod_DW_instance.BodyDelay83_DSTATE_h; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay83_DSTATE_h
    AM_Mod_Demod_DW_instance.BodyDelay84_DSTATE_i; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay84_DSTATE_i
    AM_Mod_Demod_DW_instance.BodyDelay85_DSTATE_g; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay85_DSTATE_g
    AM_Mod_Demod_DW_instance.BodyDelay86_DSTATE_b; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay86_DSTATE_b
    AM_Mod_Demod_DW_instance.BodyDelay87_DSTATE_j; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay87_DSTATE_j
    AM_Mod_Demod_DW_instance.BodyDelay88_DSTATE_c; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay88_DSTATE_c
    AM_Mod_Demod_DW_instance.BodyDelay89_DSTATE_n; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay89_DSTATE_n
    AM_Mod_Demod_DW_instance.BodyDelay90_DSTATE_h; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay90_DSTATE_h
    AM_Mod_Demod_DW_instance.BodyDelay91_DSTATE_d; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay91_DSTATE_d
    AM_Mod_Demod_DW_instance.BodyDelay92_DSTATE_k; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay92_DSTATE_k
    AM_Mod_Demod_DW_instance.BodyDelay93_DSTATE_a; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay93_DSTATE_a
    AM_Mod_Demod_DW_instance.BodyDelay94_DSTATE_l; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay94_DSTATE_l
    AM_Mod_Demod_DW_instance.BodyDelay95_DSTATE_l; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay95_DSTATE_l
    AM_Mod_Demod_DW_instance.BodyDelay96_DSTATE_m; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay96_DSTATE_m
    AM_Mod_Demod_DW_instance.BodyDelay97_DSTATE_c; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay97_DSTATE_c
    AM_Mod_Demod_DW_instance.BodyDelay98_DSTATE_j; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay98_DSTATE_j
    AM_Mod_Demod_DW_instance.BodyDelay99_DSTATE_p; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay99_DSTATE_p
    AM_Mod_Demod_DW_instance.BodyDelay100_DSTATE_k; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay100_DSTATE_k
    AM_Mod_Demod_DW_instance.BodyDelay101_DSTATE_a; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay101_DSTATE_a
    AM_Mod_Demod_DW_instance.BodyDelay102_DSTATE_e; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay102_DSTATE_e
    AM_Mod_Demod_DW_instance.BodyDelay103_DSTATE_o; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay103_DSTATE_o
    AM_Mod_Demod_DW_instance.BodyDelay104_DSTATE_d; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay104_DSTATE_d
    AM_Mod_Demod_DW_instance.BodyDelay105_DSTATE_l; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay105_DSTATE_l
    AM_Mod_Demod_DW_instance.BodyDelay106_DSTATE_a; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay106_DSTATE_a
    AM_Mod_Demod_DW_instance.BodyDelay107_DSTATE_n; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay107_DSTATE_n
    AM_Mod_Demod_DW_instance.BodyDelay108_DSTATE_l; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay108_DSTATE_l
    AM_Mod_Demod_DW_instance.BodyDelay109_DSTATE_c; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay109_DSTATE_c
    AM_Mod_Demod_DW_instance.BodyDelay110_DSTATE_m; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay110_DSTATE_m
    AM_Mod_Demod_DW_instance.BodyDelay111_DSTATE_h; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay111_DSTATE_h
    AM_Mod_Demod_DW_instance.BodyDelay112_DSTATE_l; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay112_DSTATE_l
    AM_Mod_Demod_DW_instance.BodyDelay113_DSTATE_k; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay113_DSTATE_k
    AM_Mod_Demod_DW_instance.BodyDelay114_DSTATE_h; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay114_DSTATE_h
    AM_Mod_Demod_DW_instance.BodyDelay115_DSTATE_g; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay115_DSTATE_g
    AM_Mod_Demod_DW_instance.BodyDelay116_DSTATE_k; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay116_DSTATE_k
    AM_Mod_Demod_DW_instance.BodyDelay117_DSTATE_a; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay117_DSTATE_a
    AM_Mod_Demod_DW_instance.BodyDelay118_DSTATE_p; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay118_DSTATE_p
    AM_Mod_Demod_DW_instance.BodyDelay119_DSTATE_d; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay119_DSTATE_d
    AM_Mod_Demod_DW_instance.BodyDelay120_DSTATE_f; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay120_DSTATE_f
    AM_Mod_Demod_DW_instance.BodyDelay121_DSTATE_p; // Need to be modified: Variable AM_Mod_Demod_DW_instance.BodyDelay121_DSTATE_p
    AM_Mod_Demod_DW_instance.DigitalFilter_simContextBuf; // Need to be modified: Variable AM_Mod_Demod_DW_instance.DigitalFilter_simContextBuf
    AM_Mod_Demod_DW_instance.DigitalFilter_simRevCoeff; // Need to be modified: Variable AM_Mod_Demod_DW_instance.DigitalFilter_simRevCoeff
    AM_Mod_Demod_DW_instance.DigitalFilter_simContextBuf_n; // Need to be modified: Variable AM_Mod_Demod_DW_instance.DigitalFilter_simContextBuf_n
    AM_Mod_Demod_DW_instance.DigitalFilter_simRevCoeff_o; // Need to be modified: Variable AM_Mod_Demod_DW_instance.DigitalFilter_simRevCoeff_o
    AM_Mod_Demod_B_instance.TempMember; // Need to be modified: Variable AM_Mod_Demod_B_instance.TempMember
    AM_Mod_Demod_PrevZCX_instance; // Need to be modified: Variable AM_Mod_Demod_PrevZCX_instance
    AM_Mod_Demod_ConstP.pooled65; // Need to be modified: Variable AM_Mod_Demod_ConstP.pooled65
}
int main() {
    AM_Mod_Demod_init(&AM_Mod_Demod_instance, init_AM_Mod_Demod_Y_Out2, &init_AM_Mod_Demod_U_In1, &init_AM_Mod_Demod_Y_Out1, &init_AM_Mod_Demod_Y_Out5, &init_AM_Mod_Demod_Y_Out6, &init_AM_Mod_Demod_Y_Out7);
    globalVariableInit();
    real_T exec_AM_Mod_Demod_U_In1;
    real_T exec_AM_Mod_Demod_Y_Out2[2];
    real_T exec_AM_Mod_Demod_Y_Out1;
    real_T exec_AM_Mod_Demod_Y_Out5;
    real_T exec_AM_Mod_Demod_Y_Out6;
    real_T exec_AM_Mod_Demod_Y_Out7;
    __CPROVER_input("exec_AM_Mod_Demod_U_In1", exec_AM_Mod_Demod_U_In1);
    __CPROVER_input("exec_AM_Mod_Demod_Y_Out2[0]", exec_AM_Mod_Demod_Y_Out2[0]);
    __CPROVER_input("exec_AM_Mod_Demod_Y_Out2[1]", exec_AM_Mod_Demod_Y_Out2[1]);
    return 0;
}