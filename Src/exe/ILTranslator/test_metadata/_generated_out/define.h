#ifndef _DEFINE_H_
#define _DEFINE_H_
#include <string.h>
#include <math.h>
#include <float.h>
#define RT_PI 3.14159265358979323846
#define RT_PIF 3.1415927F
#define RT_LN_10 2.30258509299404568402
#define RT_LN_10F 2.3025851F
#define RT_LOG10E 0.43429448190325182765
#define RT_LOG10EF 0.43429449F
#define RT_E 2.7182818284590452354
#define RT_EF 2.7182817F
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
#define MAX_int64_T ((int64_T)(9223372036854775807LL))
#define MIN_int64_T ((int64_T)(-9223372036854775807LL-1LL))
#define MAX_uint64_T ((uint64_T)(0xFFFFFFFFFFFFFFFFULL))
typedef signed char int8_T;
typedef unsigned char uint8_T;
typedef short int16_T;
typedef unsigned short uint16_T;
typedef int int32_T;
typedef unsigned int uint32_T;
typedef long long int64_T;
typedef unsigned long long uint64_T;
typedef float real32_T;
typedef double real64_T;
typedef double real_T;
typedef double time_T;
typedef unsigned char boolean_T;
typedef int int_T;
typedef unsigned int uint_T;
typedef unsigned long ulong_T;
typedef unsigned long long ulonglong_T;
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
typedef struct cint64_T cint64_T;
typedef struct cuint64_T cuint64_T;
typedef void * pointer_T;
typedef int ZCEventType;
typedef struct _ssMdlInfo _ssMdlInfo;
typedef uint8_T ZCSigState;
typedef long long longlong_T;
typedef struct int128m_T int128m_T;
typedef struct cint128m_T cint128m_T;
typedef struct uint128m_T uint128m_T;
typedef struct cuint128m_T cuint128m_T;
typedef struct int192m_T int192m_T;
typedef struct cint192m_T cint192m_T;
typedef struct uint192m_T uint192m_T;
typedef struct cuint192m_T cuint192m_T;
typedef struct int256m_T int256m_T;
typedef struct cint256m_T cint256m_T;
typedef struct uint256m_T uint256m_T;
typedef struct cuint256m_T cuint256m_T;
typedef struct tag_RTM_DynamicCompressionMod_T RT_MODEL_DynamicCompressionMo_T;
typedef struct DW_fxpt_lzc_W32_DynamicCompre_T DW_fxpt_lzc_W32_DynamicCompre_T;
typedef struct B_DynamicCompressionModel_T B_DynamicCompressionModel_T;
typedef struct DW_DynamicCompressionModel_T DW_DynamicCompressionModel_T;
typedef struct ConstP_DynamicCompressionMode_T ConstP_DynamicCompressionMode_T;
typedef struct TaskCounters_DynamicCompressionModel_T TaskCounters_DynamicCompressionModel_T;
typedef struct TIME_DynamicCompressionModel_T TIME_DynamicCompressionModel_T;
typedef int PrevZCX_DynamicCompressionMod_T;
struct DW_fxpt_lzc_W32_DynamicCompre_T;
struct B_DynamicCompressionModel_T;
struct DW_DynamicCompressionModel_T;
struct ConstP_DynamicCompressionMode_T;
struct creal32_T;
struct creal64_T;
struct creal_T;
struct cint8_T;
struct cuint8_T;
struct cint16_T;
struct cuint16_T;
struct cint32_T;
struct cuint32_T;
struct cint64_T;
struct cuint64_T;
struct int128m_T;
struct cint128m_T;
struct uint128m_T;
struct cuint128m_T;
struct int192m_T;
struct cint192m_T;
struct uint192m_T;
struct cuint192m_T;
struct int256m_T;
struct cint256m_T;
struct uint256m_T;
struct cuint256m_T;
struct TaskCounters_DynamicCompressionModel_T;
struct TIME_DynamicCompressionModel_T;
struct tag_RTM_DynamicCompressionMod_T;
struct DW_fxpt_lzc_W32_DynamicCompre_T {
    uint8_T Delay_DSTATE;
};
struct B_DynamicCompressionModel_T {
    int32_T Subtract1;
    int32_T Subtract1_n;
    int32_T Subtract1_i;
    int32_T Subtract1_k;
    int32_T Subtract1_a;
    int32_T Subtract1_d;
    int32_T Subtract1_b;
    int32_T Subtract1_nh;
    int32_T Subtract1_ax;
    int32_T Subtract1_ku;
};
struct DW_DynamicCompressionModel_T {
    int32_T DiscreteFIRFilter2_circBuf;
    int32_T DiscreteFIRFilter1_circBuf;
    int32_T DiscreteFIRFilter3_circBuf;
    int32_T DiscreteFIRFilter4_circBuf;
    int32_T DiscreteFIRFilter5_circBuf;
    int32_T ig65fir_circBuf;
    int32_T ram_DSTATE[512];
    int32_T ram_DSTATE_h[512];
    int32_T ram_DSTATE_a[512];
    int32_T ram_DSTATE_n[512];
    int32_T ram_DSTATE_o[512];
    int32_T ram_DSTATE_k[512];
    int32_T ram_DSTATE_c[512];
    int32_T ram_DSTATE_c5[512];
    int32_T ram_DSTATE_hz[512];
    int32_T ram_DSTATE_hp[512];
    int32_T dly_shift_1_DSTATE[26];
    int32_T DiscreteFIRFilter2_states[628];
    int32_T dly_shift_3_DSTATE[138];
    int32_T dly_shift_4_DSTATE[187];
    int32_T dly_shift_5_DSTATE[199];
    int32_T UnitDelay1_DSTATE;
    int32_T ram_data_DSTATE;
    int32_T ram_data_DSTATE_g;
    int32_T UnitDelay1_DSTATE_a;
    int32_T ram_data_DSTATE_n;
    int32_T ram_data_DSTATE_a;
    int32_T UnitDelay1_DSTATE_i;
    int32_T ram_data_DSTATE_f;
    int32_T ram_data_DSTATE_e;
    int32_T UnitDelay1_DSTATE_c;
    int32_T ram_data_DSTATE_c;
    int32_T ram_data_DSTATE_e5;
    int32_T DiscreteFIRFilter1_states[576];
    int32_T DiscreteFIRFilter3_states[352];
    int32_T DiscreteFIRFilter4_states[254];
    int32_T DiscreteFIRFilter5_states[230];
    int32_T UnitDelay1_DSTATE_j;
    int32_T ram_data_DSTATE_l;
    int32_T ram_data_DSTATE_o;
    int32_T UnitDelay_DSTATE;
    int32_T UnitDelay_DSTATE_h;
    int32_T Delay_DSTATE[3];
    int32_T UnitDelay_DSTATE_n;
    int32_T UnitDelay_DSTATE_g;
    int32_T Delay_DSTATE_e[3];
    int32_T UnitDelay_DSTATE_d;
    int32_T UnitDelay_DSTATE_gj;
    int32_T Delay_DSTATE_n[3];
    int32_T UnitDelay_DSTATE_a;
    int32_T UnitDelay_DSTATE_m;
    int32_T Delay_DSTATE_d[3];
    int32_T UnitDelay_DSTATE_gu;
    int32_T UnitDelay_DSTATE_m2;
    int32_T Delay_DSTATE_p[3];
    int32_T ig65fir_states[288];
    int32_T UnitDelay2_DSTATE;
    int32_T UnitDelay2_DSTATE_h;
    int32_T UnitDelay2_DSTATE_hj;
    int32_T UnitDelay2_DSTATE_m;
    int32_T UnitDelay2_DSTATE_e;
    uint32_T Delay2_DSTATE;
    uint32_T Delay2_DSTATE_g;
    uint32_T Delay2_DSTATE_a;
    uint32_T Delay2_DSTATE_n;
    uint32_T Delay2_DSTATE_nl;
    uint16_T Count_reg_DSTATE;
    uint16_T UnitDelay3_DSTATE;
    uint16_T UnitDelay3_DSTATE_d;
    uint16_T UnitDelay3_DSTATE_i;
    uint16_T UnitDelay3_DSTATE_c;
    uint16_T UnitDelay3_DSTATE_o;
    struct DW_fxpt_lzc_W32_DynamicCompre_T fxpt_lzc_W32_pp;
    struct DW_fxpt_lzc_W32_DynamicCompre_T fxpt_lzc_W32_p;
    struct DW_fxpt_lzc_W32_DynamicCompre_T fxpt_lzc_W32_l;
    struct DW_fxpt_lzc_W32_DynamicCompre_T fxpt_lzc_W32_c;
    struct DW_fxpt_lzc_W32_DynamicCompre_T fxpt_lzc_W32;
};
struct ConstP_DynamicCompressionMode_T {
    int64_T ig65fir_Coefficients[289];
    int32_T pooled19[512];
    int32_T pooled20[512];
    int32_T pooled21[512];
    int32_T pooled22[512];
    int32_T pooled23[512];
    int32_T DiscreteFIRFilter2_Coefficients[629];
    int32_T DiscreteFIRFilter1_Coefficients[577];
    int32_T DiscreteFIRFilter3_Coefficients[353];
    int32_T DiscreteFIRFilter4_Coefficients[255];
    int32_T DiscreteFIRFilter5_Coefficients[231];
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
struct cint64_T {
    int64_T re;
    int64_T im;
};
struct cuint64_T {
    uint64_T re;
    uint64_T im;
};
struct int128m_T {
    uint64_T chunks[2];
};
struct cint128m_T {
    struct int128m_T re;
    struct int128m_T im;
};
struct uint128m_T {
    uint64_T chunks[2];
};
struct cuint128m_T {
    struct uint128m_T re;
    struct uint128m_T im;
};
struct int192m_T {
    uint64_T chunks[3];
};
struct cint192m_T {
    struct int192m_T re;
    struct int192m_T im;
};
struct uint192m_T {
    uint64_T chunks[3];
};
struct cuint192m_T {
    struct uint192m_T re;
    struct uint192m_T im;
};
struct int256m_T {
    uint64_T chunks[4];
};
struct cint256m_T {
    struct int256m_T re;
    struct int256m_T im;
};
struct uint256m_T {
    uint64_T chunks[4];
};
struct cuint256m_T {
    struct uint256m_T re;
    struct uint256m_T im;
};
struct TaskCounters_DynamicCompressionModel_T {
    uint16_T TID[2];
};
struct TIME_DynamicCompressionModel_T {
    struct TaskCounters_DynamicCompressionModel_T TaskCounters;
    uint32_T clockTick0;
};
struct tag_RTM_DynamicCompressionMod_T {
    char_T * errorStatus;
    struct B_DynamicCompressionModel_T * blockIO;
    struct DW_DynamicCompressionModel_T * dwork;
    struct TIME_DynamicCompressionModel_T Timing;
    PrevZCX_DynamicCompressionMod_T * prevZCSigState;
};
extern struct tag_RTM_DynamicCompressionMod_T DynamicCompressionModel_instance;
extern struct DW_DynamicCompressionModel_T DynamicCompressionModel_DW_instance;
extern struct B_DynamicCompressionModel_T DynamicCompressionModel_B_instance;
extern PrevZCX_DynamicCompressionMod_T DynamicCompressionModel_PrevZCX_instance;
extern struct ConstP_DynamicCompressionMode_T DynamicCompressionModel_ConstP;
void DynamicCompressionModel_step(RT_MODEL_DynamicCompressionMo_T* DynamicCompressionModel_M, int32_T DynamicCompressionModel_U_Table_Values, boolean_T DynamicCompressionModel_U_In2, int32_T DynamicCompressionModel_U_In3, boolean_T DynamicCompressionModel_U_Table_Values_e, int32_T* DynamicCompressionModel_Y_proc_sig);
void DynamicCompressionModel_init(RT_MODEL_DynamicCompressionMo_T* DynamicCompressionModel_M, int32_T* DynamicCompressionModel_U_Table_Values, boolean_T* DynamicCompressionModel_U_In2, int32_T* DynamicCompressionModel_U_In3, boolean_T* DynamicCompressionModel_U_Table_Values_e, int32_T* DynamicCompressionModel_Y_proc_sig);
int64_T MultiWord2sLong(uint64_T u[]);
void sMultiWordShr(uint64_T u1[], int32_T n1, uint32_T n2, uint64_T y[], int32_T n);
void sMultiWordMul(uint64_T u1[], int32_T n1, uint64_T u2[], int32_T n2, uint64_T y[], int32_T n);
void ssuMultiWordMul(uint64_T u1[], int32_T n1, uint64_T u2[], int32_T n2, uint64_T y[], int32_T n);
void MultiWordSignedWrap(uint64_T u1[], int32_T n1, uint32_T n2, uint64_T y[]);
void MultiWordAdd(uint64_T u1[], uint64_T u2[], uint64_T y[], int32_T n);
void sLong2MultiWord(int64_T u, uint64_T y[], int32_T n);
void mul_wide_s64(int64_T in0, int64_T in1, uint64_T* ptrOutBitsHi, uint64_T* ptrOutBitsLo);
int64_T mul_s64_loSR(int64_T a, int64_T b, uint64_T aShift);
uint32_T DynamicCompressionMo_BitRotate1(uint32_T rtu_u);
uint32_T DynamicCompressionMod_BitRotate(uint32_T rtu_u);
uint32_T DynamicCompression_BitRotate1_l(uint32_T rtu_u);
uint32_T DynamicCompressionM_BitRotate10(uint32_T rtu_u);
uint32_T DynamicCompressionM_BitRotate11(uint32_T rtu_u);
uint32_T DynamicCompressionM_BitRotate12(uint32_T rtu_u);
uint32_T DynamicCompressionM_BitRotate13(uint32_T rtu_u);
uint32_T DynamicCompressionM_BitRotate14(uint32_T rtu_u);
uint32_T DynamicCompressionMo_BitRotate2(uint32_T rtu_u);
uint32_T DynamicCompressionMo_BitRotate3(uint32_T rtu_u);
uint32_T DynamicCompressionMo_BitRotate4(uint32_T rtu_u);
uint32_T DynamicCompressionMo_BitRotate5(uint32_T rtu_u);
uint32_T DynamicCompressionMo_BitRotate6(uint32_T rtu_u);
uint32_T DynamicCompressionMo_BitRotate7(uint32_T rtu_u);
uint32_T DynamicCompressionMo_BitRotate8(uint32_T rtu_u);
uint32_T DynamicCompressionMo_BitRotate9(uint32_T rtu_u);
uint32_T DynamicCompressionM_BitRotate_g(uint32_T rtu_u);
uint32_T DynamicCompression_BitRotate1_a(uint32_T rtu_u);
uint32_T DynamicCompressio_BitRotate10_p(uint32_T rtu_u);
uint32_T DynamicCompressio_BitRotate11_l(uint32_T rtu_u);
uint32_T DynamicCompressio_BitRotate12_m(uint32_T rtu_u);
uint32_T DynamicCompressio_BitRotate13_a(uint32_T rtu_u);
uint32_T DynamicCompressio_BitRotate14_o(uint32_T rtu_u);
uint32_T DynamicCompression_BitRotate2_a(uint32_T rtu_u);
uint32_T DynamicCompression_BitRotate3_d(uint32_T rtu_u);
uint32_T DynamicCompression_BitRotate5_p(uint32_T rtu_u);
uint32_T DynamicCompression_BitRotate6_b(uint32_T rtu_u);
uint32_T DynamicCompression_BitRotate7_h(uint32_T rtu_u);
uint32_T DynamicCompression_BitRotate8_o(uint32_T rtu_u);
uint32_T DynamicCompression_BitRotate9_g(uint32_T rtu_u);
void DynamicCompre_fxpt_lzc_W32_Init(struct DW_fxpt_lzc_W32_DynamicCompre_T* localDW);
uint8_T DynamicCompression_fxpt_lzc_W32(uint32_T rtu_x_in, struct DW_fxpt_lzc_W32_DynamicCompre_T* localDW);
#endif
