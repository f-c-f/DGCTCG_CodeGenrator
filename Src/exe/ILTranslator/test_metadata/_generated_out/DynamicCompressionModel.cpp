#include <cstring>
#include "TCGHybridBasic.h"
#include "BitmapCov.h"
#include "define.h"
struct tag_RTM_DynamicCompressionMod_T DynamicCompressionModel_instance;
struct DW_DynamicCompressionModel_T DynamicCompressionModel_DW_instance;
struct B_DynamicCompressionModel_T DynamicCompressionModel_B_instance;
PrevZCX_DynamicCompressionMod_T DynamicCompressionModel_PrevZCX_instance;
void DynamicCompressionModel_step(RT_MODEL_DynamicCompressionMo_T* DynamicCompressionModel_M, int32_T DynamicCompressionModel_U_Table_Values, boolean_T DynamicCompressionModel_U_In2, int32_T DynamicCompressionModel_U_In3, boolean_T DynamicCompressionModel_U_Table_Values_e, int32_T* DynamicCompressionModel_Y_proc_sig) {
    BitmapCov::munitBitmap[0] = 1;
    struct B_DynamicCompressionModel_T * DynamicCompressionModel_B = DynamicCompressionModel_M->blockIO;
    struct DW_DynamicCompressionModel_T * DynamicCompressionModel_DW = DynamicCompressionModel_M->dwork;
    struct int128m_T tmp = {0};
    struct int128m_T tmp_0 = {0};
    struct int128m_T tmp_1 = {0};
    struct int128m_T tmp_4 = {0};
    struct int128m_T tmp_5 = {0};
    struct int128m_T tmp_6 = {0};
    struct int128m_T tmp_7 = {0};
    struct int128m_T tmp_8 = {0};
    struct int128m_T tmp_9 = {0};
    struct int128m_T tmp_a = {0};
    struct int128m_T tmp_b = {0};
    struct int128m_T tmp_c = {0};
    struct int128m_T tmp_d = {0};
    struct int128m_T tmp_e = {0};
    struct int128m_T tmp_f = {0};
    int64_T acc1_3 = {0};
    uint64_T tmp_2 = {0};
    uint64_T tmp_3 = {0};
    int32_T rtb_ram[512] = {0};
    int32_T rtb_ram_f2[512] = {0};
    int32_T rtb_ram_gg[512] = {0};
    int32_T rtb_ram_j[512] = {0};
    int32_T rtb_ram_j2[512] = {0};
    int32_T rtb_ram_jd[512] = {0};
    int32_T rtb_ram_k[512] = {0};
    int32_T rtb_ram_m[512] = {0};
    int32_T rtb_ram_mw[512] = {0};
    int32_T rtb_ram_p1[512] = {0};
    int32_T rtb_wr_port[512] = {0};
    int32_T Switch = {0};
    int32_T acc1 = {0};
    int32_T acc1_0 = {0};
    int32_T acc1_1 = {0};
    int32_T acc1_2 = {0};
    int32_T cff = {0};
    int32_T i = {0};
    int32_T j = {0};
    int32_T rtb_Switch = {0};
    int32_T rtb_Switch_c = {0};
    int32_T rtb_Switch_d3 = {0};
    int32_T rtb_Switch_eo = {0};
    int32_T rtb_Switch_g = {0};
    int32_T rtb_Switch_kd = {0};
    int32_T rtb_Switch_m = {0};
    int32_T rtb_Switch_mc = {0};
    int32_T rtb_Switch_mv = {0};
    int32_T rtb_Switch_nj = {0};
    int32_T rtb_X_in_e = {0};
    int32_T rtb_X_in_i = {0};
    int32_T rtb_X_in_l = {0};
    int32_T rtb_X_in_m = {0};
    uint32_T rtb_MultiportSwitch = {0};
    uint32_T rtb_MultiportSwitch_n = {0};
    uint32_T rtb_y = {0};
    uint32_T rtb_y_a = {0};
    uint32_T rtb_y_a1y = {0};
    uint32_T rtb_y_a2t = {0};
    uint32_T rtb_y_av = {0};
    uint32_T rtb_y_aw = {0};
    uint32_T rtb_y_b = {0};
    uint32_T rtb_y_b3 = {0};
    uint32_T rtb_y_bm = {0};
    uint32_T rtb_y_bn = {0};
    uint32_T rtb_y_bo = {0};
    uint32_T rtb_y_bu = {0};
    uint32_T rtb_y_c = {0};
    uint32_T rtb_y_c3 = {0};
    uint32_T rtb_y_ck = {0};
    uint32_T rtb_y_cn = {0};
    uint32_T rtb_y_d = {0};
    uint32_T rtb_y_d1 = {0};
    uint32_T rtb_y_de = {0};
    uint32_T rtb_y_di = {0};
    uint32_T rtb_y_dt = {0};
    uint32_T rtb_y_dz = {0};
    uint32_T rtb_y_e = {0};
    uint32_T rtb_y_et = {0};
    uint32_T rtb_y_ev = {0};
    uint32_T rtb_y_f = {0};
    uint32_T rtb_y_f3 = {0};
    uint32_T rtb_y_fv = {0};
    uint32_T rtb_y_g = {0};
    uint32_T rtb_y_h = {0};
    uint32_T rtb_y_h4 = {0};
    uint32_T rtb_y_h5 = {0};
    uint32_T rtb_y_hc = {0};
    uint32_T rtb_y_he = {0};
    uint32_T rtb_y_hk = {0};
    uint32_T rtb_y_hv = {0};
    uint32_T rtb_y_i = {0};
    uint32_T rtb_y_i1j = {0};
    uint32_T rtb_y_ihg = {0};
    uint32_T rtb_y_ij = {0};
    uint32_T rtb_y_ik = {0};
    uint32_T rtb_y_il = {0};
    uint32_T rtb_y_im = {0};
    uint32_T rtb_y_iu = {0};
    uint32_T rtb_y_j = {0};
    uint32_T rtb_y_j0 = {0};
    uint32_T rtb_y_j3 = {0};
    uint32_T rtb_y_j5 = {0};
    uint32_T rtb_y_jd = {0};
    uint32_T rtb_y_je = {0};
    uint32_T rtb_y_jf = {0};
    uint32_T rtb_y_jg = {0};
    uint32_T rtb_y_jm = {0};
    uint32_T rtb_y_jv = {0};
    uint32_T rtb_y_k = {0};
    uint32_T rtb_y_kc = {0};
    uint32_T rtb_y_kh0 = {0};
    uint32_T rtb_y_kp = {0};
    uint32_T rtb_y_kv = {0};
    uint32_T rtb_y_l = {0};
    uint32_T rtb_y_l3 = {0};
    uint32_T rtb_y_le = {0};
    uint32_T rtb_y_lf = {0};
    uint32_T rtb_y_lgp = {0};
    uint32_T rtb_y_li = {0};
    uint32_T rtb_y_lp = {0};
    uint32_T rtb_y_lv = {0};
    uint32_T rtb_y_m = {0};
    uint32_T rtb_y_mb = {0};
    uint32_T rtb_y_mh = {0};
    uint32_T rtb_y_n = {0};
    uint32_T rtb_y_nd = {0};
    uint32_T rtb_y_nj = {0};
    uint32_T rtb_y_nk = {0};
    uint32_T rtb_y_nn = {0};
    uint32_T rtb_y_nq = {0};
    uint32_T rtb_y_o = {0};
    uint32_T rtb_y_o0u = {0};
    uint32_T rtb_y_o3 = {0};
    uint32_T rtb_y_o5 = {0};
    uint32_T rtb_y_oa = {0};
    uint32_T rtb_y_om = {0};
    uint32_T rtb_y_ov = {0};
    uint32_T rtb_y_oy = {0};
    uint32_T rtb_y_p = {0};
    uint32_T rtb_y_pbx = {0};
    uint32_T rtb_y_pc = {0};
    uint32_T rtb_y_pu = {0};
    uint32_T rtb_y_py = {0};
    uint32_T rtb_y_pyi = {0};
    uint16_T rtb_Switch1_b2 = {0};
    uint16_T rtb_Switch_load = {0};
    uint8_T rtb_DataTypeConversion_a0 = {0};
    uint8_T rtb_DataTypeConversion_by = {0};
    uint8_T rtb_DataTypeConversion_gu = {0};
    uint8_T rtb_DataTypeConversion_m = {0};
    uint8_T rtb_DataTypeConversion_n = {0};
    uint8_T rtb_ExtractDesiredBits_a = {0};
    uint8_T rtb_ExtractDesiredBits_dd = {0};
    uint8_T rtb_ExtractDesiredBits_ez = {0};
    uint8_T rtb_ExtractDesiredBits_l3 = {0};
    uint8_T rtb_ExtractDesiredBits_o = {0};
    uint8_T rtb_ExtractDesiredBits_oj = {0};
    uint8_T rtb_NShifts = {0};
    uint8_T rtb_NShifts_i = {0};
    uint8_T rtb_NShifts_mn = {0};
    uint8_T rtb_NShifts_n = {0};
    uint8_T rtb_Subtract_a = {0};
    uint8_T rtb_Subtract_c = {0};
    uint8_T rtb_Subtract_e = {0};
    uint8_T rtb_Subtract_j = {0};
    uint8_T rtb_Subtract_n = {0};
    boolean_T rtb_CastToBoolean1 = {0};
    rtb_ExtractDesiredBits_oj = (uint8_T)(DynamicCompressionModel_DW->Count_reg_DSTATE >> 9);
    TCGHybrid::updateBranchDistanceValue("rtb_ExtractDesiredBits_oj", rtb_ExtractDesiredBits_oj);
    unsigned char mcdc_114_c_1 = rtb_ExtractDesiredBits_oj == 0;
    if (mcdc_114_c_1) {
        BitmapCov::branchBitmap[0] = 1;
    }
    else {
        BitmapCov::branchBitmap[1] = 1;
    }
    rtb_CastToBoolean1 = (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(rtb_ExtractDesiredBits_oj == 0, 0, 1, 0), -1, 0, 0));
    rtb_Switch_load = (uint16_T)(DynamicCompressionModel_DW->Count_reg_DSTATE & 511);
    memcpy(&rtb_ram[0], &DynamicCompressionModel_DW->ram_DSTATE[0], sizeof(int32_T) << 9U);
    memcpy(&rtb_ram_f2[0], &DynamicCompressionModel_DW->ram_DSTATE[0], sizeof(int32_T) << 9U);
    rtb_ram_f2[rtb_Switch_load] = DynamicCompressionModel_U_Table_Values;
    for (i = 0;BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(i < 512, 2, 1, 1), -3, 0, 1);i++) {
        BitmapCov::branchBitmap[2] = 1;
        if (rtb_CastToBoolean1) {
            BitmapCov::branchBitmap[3] = 1;
            DynamicCompressionModel_DW->ram_DSTATE[i] = rtb_ram_f2[i];
        }
        BitmapCov::branchBitmap[4] = 1;
        rtb_X_in_m = DynamicCompressionModel_DW->ram_DSTATE_h[i];
        rtb_ram_f2[i] = rtb_X_in_m;
        rtb_ram_mw[i] = rtb_X_in_m;
    }
    BitmapCov::branchBitmap[5] = 1;
    rtb_ram_mw[rtb_Switch_load] = DynamicCompressionModel_U_Table_Values;
    for (i = 0;BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(i < 512, 4, 1, 2), -5, 0, 2);i++) {
        BitmapCov::branchBitmap[6] = 1;
        if (rtb_CastToBoolean1) {
            BitmapCov::branchBitmap[7] = 1;
            DynamicCompressionModel_DW->ram_DSTATE_h[i] = rtb_ram_mw[i];
        }
        BitmapCov::branchBitmap[8] = 1;
        rtb_X_in_m = DynamicCompressionModel_DW->ram_DSTATE_a[i];
        rtb_ram_mw[i] = rtb_X_in_m;
        rtb_ram_m[i] = rtb_X_in_m;
    }
    BitmapCov::branchBitmap[9] = 1;
    rtb_ram_m[rtb_Switch_load] = DynamicCompressionModel_U_Table_Values;
    unsigned char mcdc_113_c_1 = rtb_ExtractDesiredBits_oj == 1;
    if (mcdc_113_c_1) {
        BitmapCov::branchBitmap[10] = 1;
    }
    else {
        BitmapCov::branchBitmap[11] = 1;
    }
    rtb_CastToBoolean1 = (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(rtb_ExtractDesiredBits_oj == 1, 6, 1, 3), -7, 0, 3));
    for (i = 0;BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(i < 512, 8, 1, 4), -9, 0, 4);i++) {
        BitmapCov::branchBitmap[12] = 1;
        if (rtb_CastToBoolean1) {
            BitmapCov::branchBitmap[13] = 1;
            DynamicCompressionModel_DW->ram_DSTATE_a[i] = rtb_ram_m[i];
        }
        BitmapCov::branchBitmap[14] = 1;
        rtb_X_in_m = DynamicCompressionModel_DW->ram_DSTATE_n[i];
        rtb_ram_m[i] = rtb_X_in_m;
        rtb_ram_p1[i] = rtb_X_in_m;
    }
    BitmapCov::branchBitmap[15] = 1;
    rtb_ram_p1[rtb_Switch_load] = DynamicCompressionModel_U_Table_Values;
    for (i = 0;BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(i < 512, 10, 1, 5), -11, 0, 5);i++) {
        BitmapCov::branchBitmap[16] = 1;
        if (rtb_CastToBoolean1) {
            BitmapCov::branchBitmap[17] = 1;
            DynamicCompressionModel_DW->ram_DSTATE_n[i] = rtb_ram_p1[i];
        }
        BitmapCov::branchBitmap[18] = 1;
        rtb_X_in_m = DynamicCompressionModel_DW->ram_DSTATE_o[i];
        rtb_ram_p1[i] = rtb_X_in_m;
        rtb_ram_j2[i] = rtb_X_in_m;
    }
    BitmapCov::branchBitmap[19] = 1;
    rtb_ram_j2[rtb_Switch_load] = DynamicCompressionModel_U_Table_Values;
    unsigned char mcdc_112_c_1 = rtb_ExtractDesiredBits_oj == 2;
    if (mcdc_112_c_1) {
        BitmapCov::branchBitmap[20] = 1;
    }
    else {
        BitmapCov::branchBitmap[21] = 1;
    }
    rtb_CastToBoolean1 = (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(rtb_ExtractDesiredBits_oj == 2, 12, 1, 6), -13, 0, 6));
    for (i = 0;BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(i < 512, 14, 1, 7), -15, 0, 7);i++) {
        BitmapCov::branchBitmap[22] = 1;
        if (rtb_CastToBoolean1) {
            BitmapCov::branchBitmap[23] = 1;
            DynamicCompressionModel_DW->ram_DSTATE_o[i] = rtb_ram_j2[i];
        }
        BitmapCov::branchBitmap[24] = 1;
        rtb_X_in_m = DynamicCompressionModel_DW->ram_DSTATE_k[i];
        rtb_ram_j2[i] = rtb_X_in_m;
        rtb_ram_gg[i] = rtb_X_in_m;
    }
    BitmapCov::branchBitmap[25] = 1;
    rtb_ram_gg[rtb_Switch_load] = DynamicCompressionModel_U_Table_Values;
    for (i = 0;BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(i < 512, 16, 1, 8), -17, 0, 8);i++) {
        BitmapCov::branchBitmap[26] = 1;
        if (rtb_CastToBoolean1) {
            BitmapCov::branchBitmap[27] = 1;
            DynamicCompressionModel_DW->ram_DSTATE_k[i] = rtb_ram_gg[i];
        }
        BitmapCov::branchBitmap[28] = 1;
        rtb_X_in_m = DynamicCompressionModel_DW->ram_DSTATE_c[i];
        rtb_ram_gg[i] = rtb_X_in_m;
        rtb_ram_k[i] = rtb_X_in_m;
    }
    BitmapCov::branchBitmap[29] = 1;
    rtb_ram_k[rtb_Switch_load] = DynamicCompressionModel_U_Table_Values;
    unsigned char mcdc_111_c_1 = rtb_ExtractDesiredBits_oj == 3;
    if (mcdc_111_c_1) {
        BitmapCov::branchBitmap[30] = 1;
    }
    else {
        BitmapCov::branchBitmap[31] = 1;
    }
    rtb_CastToBoolean1 = (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(rtb_ExtractDesiredBits_oj == 3, 18, 1, 9), -19, 0, 9));
    for (i = 0;BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(i < 512, 20, 1, 10), -21, 0, 10);i++) {
        BitmapCov::branchBitmap[32] = 1;
        if (rtb_CastToBoolean1) {
            BitmapCov::branchBitmap[33] = 1;
            DynamicCompressionModel_DW->ram_DSTATE_c[i] = rtb_ram_k[i];
        }
        BitmapCov::branchBitmap[34] = 1;
        rtb_X_in_m = DynamicCompressionModel_DW->ram_DSTATE_c5[i];
        rtb_ram_k[i] = rtb_X_in_m;
        rtb_ram_jd[i] = rtb_X_in_m;
    }
    BitmapCov::branchBitmap[35] = 1;
    rtb_ram_jd[rtb_Switch_load] = DynamicCompressionModel_U_Table_Values;
    for (i = 0;BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(i < 512, 22, 1, 11), -23, 0, 11);i++) {
        BitmapCov::branchBitmap[36] = 1;
        if (rtb_CastToBoolean1) {
            BitmapCov::branchBitmap[37] = 1;
            DynamicCompressionModel_DW->ram_DSTATE_c5[i] = rtb_ram_jd[i];
        }
        BitmapCov::branchBitmap[38] = 1;
        rtb_X_in_m = DynamicCompressionModel_DW->ram_DSTATE_hz[i];
        rtb_ram_jd[i] = rtb_X_in_m;
        rtb_ram_j[i] = rtb_X_in_m;
    }
    BitmapCov::branchBitmap[39] = 1;
    rtb_ram_j[rtb_Switch_load] = DynamicCompressionModel_U_Table_Values;
    unsigned char mcdc_110_c_1 = rtb_ExtractDesiredBits_oj == 4;
    if (mcdc_110_c_1) {
        BitmapCov::branchBitmap[40] = 1;
    }
    else {
        BitmapCov::branchBitmap[41] = 1;
    }
    rtb_CastToBoolean1 = (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(rtb_ExtractDesiredBits_oj == 4, 24, 1, 12), -25, 0, 12));
    for (i = 0;BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(i < 512, 26, 1, 13), -27, 0, 13);i++) {
        BitmapCov::branchBitmap[42] = 1;
        if (rtb_CastToBoolean1) {
            BitmapCov::branchBitmap[43] = 1;
            DynamicCompressionModel_DW->ram_DSTATE_hz[i] = rtb_ram_j[i];
        }
        BitmapCov::branchBitmap[44] = 1;
        rtb_X_in_m = DynamicCompressionModel_DW->ram_DSTATE_hp[i];
        rtb_ram_j[i] = rtb_X_in_m;
        rtb_wr_port[i] = rtb_X_in_m;
    }
    BitmapCov::branchBitmap[45] = 1;
    rtb_wr_port[rtb_Switch_load] = DynamicCompressionModel_U_Table_Values;
    if (rtb_CastToBoolean1) {
        BitmapCov::branchBitmap[46] = 1;
        memcpy(&DynamicCompressionModel_DW->ram_DSTATE_hp[0], &rtb_wr_port[0], sizeof(int32_T) << 9U);
    }
    BitmapCov::branchBitmap[47] = 1;
    TCGHybrid::updateBranchDistanceValue("DynamicCompressionModel_DW->dly_shift_1_DSTATE[0]", DynamicCompressionModel_DW->dly_shift_1_DSTATE[0]);
    if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(DynamicCompressionModel_DW->dly_shift_1_DSTATE[0] < 0, 28, 1, 14), -29, 0, 14)) {
        BitmapCov::branchBitmap[48] = 1;
        i = -DynamicCompressionModel_DW->dly_shift_1_DSTATE[0];
    }
    else {
        BitmapCov::branchBitmap[49] = 1;
        i = DynamicCompressionModel_DW->dly_shift_1_DSTATE[0];
    }
    rtb_DataTypeConversion_m = DynamicCompression_fxpt_lzc_W32((uint32_T)i, &DynamicCompressionModel_DW->fxpt_lzc_W32);
    rtb_Subtract_e = (uint8_T)((uint8_T)(rtb_DataTypeConversion_m - 3) & 63);
    TCGHybrid::updateBranchDistanceValue("rtb_Subtract_e", rtb_Subtract_e);
    rtb_ExtractDesiredBits_l3 = (uint8_T)(rtb_Subtract_e & 15);
    rtb_Switch_eo = (int32_T)((DynamicCompressionModel_U_In3 * -45LL) >> 26);
    cff = 1;
    for (j = DynamicCompressionModel_DW->DiscreteFIRFilter2_circBuf;BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(j < 628, 30, 1, 15), -31, 0, 15);j++) {
        BitmapCov::branchBitmap[50] = 1;
        rtb_Switch_eo += (int32_T)(((int64_T)DynamicCompressionModel_DW->DiscreteFIRFilter2_states[j] * DynamicCompressionModel_ConstP.DiscreteFIRFilter2_Coefficients[cff]) >> 28);
        cff++;
    }
    BitmapCov::branchBitmap[51] = 1;
    for (j = 0;BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(j < DynamicCompressionModel_DW->DiscreteFIRFilter2_circBuf, 32, 1, 16), -33, 0, 16);j++) {
        BitmapCov::branchBitmap[52] = 1;
        rtb_Switch_eo += (int32_T)(((int64_T)DynamicCompressionModel_DW->DiscreteFIRFilter2_states[j] * DynamicCompressionModel_ConstP.DiscreteFIRFilter2_Coefficients[cff]) >> 28);
        cff++;
    }
    BitmapCov::branchBitmap[53] = 1;
    if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(rtb_Switch_eo < 0, 34, 1, 17), -35, 0, 17)) {
        BitmapCov::branchBitmap[54] = 1;
        rtb_X_in_m = -rtb_Switch_eo;
    }
    else {
        BitmapCov::branchBitmap[55] = 1;
        rtb_X_in_m = rtb_Switch_eo;
    }
    rtb_DataTypeConversion_by = DynamicCompression_fxpt_lzc_W32((uint32_T)rtb_X_in_m, &DynamicCompressionModel_DW->fxpt_lzc_W32_c);
    rtb_Subtract_j = (uint8_T)((uint8_T)(rtb_DataTypeConversion_by - 3) & 63);
    TCGHybrid::updateBranchDistanceValue("rtb_Subtract_j", rtb_Subtract_j);
    rtb_ExtractDesiredBits_dd = (uint8_T)(rtb_Subtract_j & 15);
    uint8_T tmp1 = (uint8_T)(DynamicCompressionModel_DW->UnitDelay3_DSTATE_d >> 5);
    rtb_NShifts_mn = (uint8_T)((~tmp1) & 15);
    TCGHybrid::updateBranchDistanceValue("DynamicCompressionModel_DW->dly_shift_3_DSTATE[0]", DynamicCompressionModel_DW->dly_shift_3_DSTATE[0]);
    if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(DynamicCompressionModel_DW->dly_shift_3_DSTATE[0] < 0, 36, 1, 18), -37, 0, 18)) {
        BitmapCov::branchBitmap[56] = 1;
        rtb_X_in_i = -DynamicCompressionModel_DW->dly_shift_3_DSTATE[0];
    }
    else {
        BitmapCov::branchBitmap[57] = 1;
        rtb_X_in_i = DynamicCompressionModel_DW->dly_shift_3_DSTATE[0];
    }
    rtb_DataTypeConversion_n = DynamicCompression_fxpt_lzc_W32((uint32_T)rtb_X_in_i, &DynamicCompressionModel_DW->fxpt_lzc_W32_l);
    rtb_Subtract_n = (uint8_T)((uint8_T)(rtb_DataTypeConversion_n - 3) & 63);
    TCGHybrid::updateBranchDistanceValue("rtb_Subtract_n", rtb_Subtract_n);
    rtb_ExtractDesiredBits_a = (uint8_T)(rtb_Subtract_n & 15);
    uint8_T tmp2 = (uint8_T)(DynamicCompressionModel_DW->UnitDelay3_DSTATE_i >> 5);
    rtb_NShifts_n = (uint8_T)((~tmp2) & 15);
    TCGHybrid::updateBranchDistanceValue("DynamicCompressionModel_DW->dly_shift_4_DSTATE[0]", DynamicCompressionModel_DW->dly_shift_4_DSTATE[0]);
    if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(DynamicCompressionModel_DW->dly_shift_4_DSTATE[0] < 0, 38, 1, 19), -39, 0, 19)) {
        BitmapCov::branchBitmap[58] = 1;
        rtb_X_in_l = -DynamicCompressionModel_DW->dly_shift_4_DSTATE[0];
    }
    else {
        BitmapCov::branchBitmap[59] = 1;
        rtb_X_in_l = DynamicCompressionModel_DW->dly_shift_4_DSTATE[0];
    }
    rtb_DataTypeConversion_gu = DynamicCompression_fxpt_lzc_W32((uint32_T)rtb_X_in_l, &DynamicCompressionModel_DW->fxpt_lzc_W32_p);
    rtb_Subtract_a = (uint8_T)((uint8_T)(rtb_DataTypeConversion_gu - 3) & 63);
    TCGHybrid::updateBranchDistanceValue("rtb_Subtract_a", rtb_Subtract_a);
    rtb_ExtractDesiredBits_ez = (uint8_T)(rtb_Subtract_a & 15);
    uint8_T tmp3 = (uint8_T)(DynamicCompressionModel_DW->UnitDelay3_DSTATE_c >> 5);
    rtb_NShifts_i = (uint8_T)((~tmp3) & 15);
    TCGHybrid::updateBranchDistanceValue("DynamicCompressionModel_DW->dly_shift_5_DSTATE[0]", DynamicCompressionModel_DW->dly_shift_5_DSTATE[0]);
    if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(DynamicCompressionModel_DW->dly_shift_5_DSTATE[0] < 0, 40, 1, 20), -41, 0, 20)) {
        BitmapCov::branchBitmap[60] = 1;
        rtb_X_in_e = -DynamicCompressionModel_DW->dly_shift_5_DSTATE[0];
    }
    else {
        BitmapCov::branchBitmap[61] = 1;
        rtb_X_in_e = DynamicCompressionModel_DW->dly_shift_5_DSTATE[0];
    }
    rtb_DataTypeConversion_a0 = DynamicCompression_fxpt_lzc_W32((uint32_T)rtb_X_in_e, &DynamicCompressionModel_DW->fxpt_lzc_W32_pp);
    rtb_Subtract_c = (uint8_T)((uint8_T)(rtb_DataTypeConversion_a0 - 3) & 63);
    TCGHybrid::updateBranchDistanceValue("rtb_Subtract_c", rtb_Subtract_c);
    rtb_ExtractDesiredBits_o = (uint8_T)(rtb_Subtract_c & 15);
    rtb_y_h = DynamicCompressionMod_BitRotate(DynamicCompressionModel_DW->Delay2_DSTATE);
    rtb_y_cn = DynamicCompression_BitRotate1_l(DynamicCompressionModel_DW->Delay2_DSTATE);
    rtb_y_de = DynamicCompressionMo_BitRotate2(DynamicCompressionModel_DW->Delay2_DSTATE);
    rtb_y_bm = DynamicCompressionMo_BitRotate3(DynamicCompressionModel_DW->Delay2_DSTATE);
    rtb_y_mh = DynamicCompressionMo_BitRotate4(DynamicCompressionModel_DW->Delay2_DSTATE);
    rtb_y_oy = DynamicCompressionMo_BitRotate5(DynamicCompressionModel_DW->Delay2_DSTATE);
    rtb_y_di = DynamicCompressionMo_BitRotate6(DynamicCompressionModel_DW->Delay2_DSTATE);
    rtb_y_kc = DynamicCompressionMo_BitRotate7(DynamicCompressionModel_DW->Delay2_DSTATE);
    rtb_y_il = DynamicCompressionMo_BitRotate8(DynamicCompressionModel_DW->Delay2_DSTATE);
    rtb_y_nn = DynamicCompressionMo_BitRotate9(DynamicCompressionModel_DW->Delay2_DSTATE);
    rtb_y_hc = DynamicCompressionM_BitRotate10(DynamicCompressionModel_DW->Delay2_DSTATE);
    rtb_y_c3 = DynamicCompressionM_BitRotate11(DynamicCompressionModel_DW->Delay2_DSTATE);
    rtb_y_aw = DynamicCompressionM_BitRotate12(DynamicCompressionModel_DW->Delay2_DSTATE);
    rtb_y_l3 = DynamicCompressionM_BitRotate13(DynamicCompressionModel_DW->Delay2_DSTATE);
    rtb_y_bu = DynamicCompressionM_BitRotate14(DynamicCompressionModel_DW->Delay2_DSTATE);
    switch(rtb_ExtractDesiredBits_o)
    {
    case 0:
        BitmapCov::branchBitmap[62] = 1;
        {
            break;
        }
    case 1:
        BitmapCov::branchBitmap[63] = 1;
        {
            DynamicCompressionModel_DW->Delay2_DSTATE = rtb_y_h;
            break;
        }
    case 2:
        BitmapCov::branchBitmap[64] = 1;
        {
            DynamicCompressionModel_DW->Delay2_DSTATE = rtb_y_cn;
            break;
        }
    case 3:
        BitmapCov::branchBitmap[65] = 1;
        {
            DynamicCompressionModel_DW->Delay2_DSTATE = rtb_y_de;
            break;
        }
    case 4:
        BitmapCov::branchBitmap[66] = 1;
        {
            DynamicCompressionModel_DW->Delay2_DSTATE = rtb_y_bm;
            break;
        }
    case 5:
        BitmapCov::branchBitmap[67] = 1;
        {
            DynamicCompressionModel_DW->Delay2_DSTATE = rtb_y_mh;
            break;
        }
    case 6:
        BitmapCov::branchBitmap[68] = 1;
        {
            DynamicCompressionModel_DW->Delay2_DSTATE = rtb_y_oy;
            break;
        }
    case 7:
        BitmapCov::branchBitmap[69] = 1;
        {
            DynamicCompressionModel_DW->Delay2_DSTATE = rtb_y_di;
            break;
        }
    case 8:
        BitmapCov::branchBitmap[70] = 1;
        {
            DynamicCompressionModel_DW->Delay2_DSTATE = rtb_y_kc;
            break;
        }
    case 9:
        BitmapCov::branchBitmap[71] = 1;
        {
            DynamicCompressionModel_DW->Delay2_DSTATE = rtb_y_il;
            break;
        }
    case 10:
        BitmapCov::branchBitmap[72] = 1;
        {
            DynamicCompressionModel_DW->Delay2_DSTATE = rtb_y_nn;
            break;
        }
    case 11:
        BitmapCov::branchBitmap[73] = 1;
        {
            DynamicCompressionModel_DW->Delay2_DSTATE = rtb_y_hc;
            break;
        }
    case 12:
        BitmapCov::branchBitmap[74] = 1;
        {
            DynamicCompressionModel_DW->Delay2_DSTATE = rtb_y_c3;
            break;
        }
    case 13:
        BitmapCov::branchBitmap[75] = 1;
        {
            DynamicCompressionModel_DW->Delay2_DSTATE = rtb_y_aw;
            break;
        }
    case 14:
        BitmapCov::branchBitmap[76] = 1;
        {
            DynamicCompressionModel_DW->Delay2_DSTATE = rtb_y_l3;
            break;
        }
    default:
        BitmapCov::branchBitmap[77] = 1;
        {
            DynamicCompressionModel_DW->Delay2_DSTATE = rtb_y_bu;
            break;
        }
    }
    TCGHybrid::updateBranchDistanceValue("rtb_Subtract_c", rtb_Subtract_c);
    if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(rtb_Subtract_c > 15, 42, 1, 21), -43, 0, 21)) {
        BitmapCov::branchBitmap[78] = 1;
        rtb_Switch1_b2 = 0U;
    }
    else {
        BitmapCov::branchBitmap[79] = 1;
        rtb_Switch1_b2 = (uint16_T)((uint8_T)((uint8_T)~rtb_ExtractDesiredBits_o & 15) << 5 | (uint8_T)((uint8_T)(DynamicCompressionModel_DW->Delay2_DSTATE >> 23) & 31));
    }
    uint8_T tmp4 = (uint8_T)(DynamicCompressionModel_DW->UnitDelay3_DSTATE_o >> 5);
    rtb_Subtract_c = (uint8_T)((~tmp4) & 15);
    TCGHybrid::updateBranchDistanceValue("rtb_Subtract_c", rtb_Subtract_c);
    rtb_y_ck = DynamicCompressionM_BitRotate_g(1073741824U);
    rtb_y_b3 = DynamicCompression_BitRotate1_a(1073741824U);
    rtb_y_l = DynamicCompression_BitRotate2_a(1073741824U);
    rtb_y_nd = DynamicCompression_BitRotate3_d(1073741824U);
    rtb_y_et = DynamicCompressionMo_BitRotate1(1073741824U);
    rtb_y_i = DynamicCompression_BitRotate5_p(1073741824U);
    rtb_y_b = DynamicCompression_BitRotate6_b(1073741824U);
    rtb_y_jd = DynamicCompression_BitRotate7_h(1073741824U);
    rtb_y_n = DynamicCompression_BitRotate8_o(1073741824U);
    rtb_MultiportSwitch = DynamicCompression_BitRotate9_g(1073741824U);
    rtb_y_mb = DynamicCompressio_BitRotate10_p(1073741824U);
    rtb_y_d = DynamicCompressio_BitRotate11_l(1073741824U);
    rtb_y_bn = DynamicCompressio_BitRotate12_m(1073741824U);
    rtb_y_ihg = DynamicCompressio_BitRotate13_a(1073741824U);
    rtb_y_g = DynamicCompressio_BitRotate14_o(1073741824U);
    switch(rtb_Subtract_c)
    {
    case 0:
        BitmapCov::branchBitmap[80] = 1;
        {
            rtb_MultiportSwitch_n = 1073741824U;
            break;
        }
    case 1:
        BitmapCov::branchBitmap[81] = 1;
        {
            rtb_MultiportSwitch_n = rtb_y_ck;
            break;
        }
    case 2:
        BitmapCov::branchBitmap[82] = 1;
        {
            rtb_MultiportSwitch_n = rtb_y_b3;
            break;
        }
    case 3:
        BitmapCov::branchBitmap[83] = 1;
        {
            rtb_MultiportSwitch_n = rtb_y_l;
            break;
        }
    case 4:
        BitmapCov::branchBitmap[84] = 1;
        {
            rtb_MultiportSwitch_n = rtb_y_nd;
            break;
        }
    case 5:
        BitmapCov::branchBitmap[85] = 1;
        {
            rtb_MultiportSwitch_n = rtb_y_et;
            break;
        }
    case 6:
        BitmapCov::branchBitmap[86] = 1;
        {
            rtb_MultiportSwitch_n = rtb_y_i;
            break;
        }
    case 7:
        BitmapCov::branchBitmap[87] = 1;
        {
            rtb_MultiportSwitch_n = rtb_y_b;
            break;
        }
    case 8:
        BitmapCov::branchBitmap[88] = 1;
        {
            rtb_MultiportSwitch_n = rtb_y_jd;
            break;
        }
    case 9:
        BitmapCov::branchBitmap[89] = 1;
        {
            rtb_MultiportSwitch_n = rtb_y_n;
            break;
        }
    case 10:
        BitmapCov::branchBitmap[90] = 1;
        {
            rtb_MultiportSwitch_n = rtb_MultiportSwitch;
            break;
        }
    case 11:
        BitmapCov::branchBitmap[91] = 1;
        {
            rtb_MultiportSwitch_n = rtb_y_mb;
            break;
        }
    case 12:
        BitmapCov::branchBitmap[92] = 1;
        {
            rtb_MultiportSwitch_n = rtb_y_d;
            break;
        }
    case 13:
        BitmapCov::branchBitmap[93] = 1;
        {
            rtb_MultiportSwitch_n = rtb_y_bn;
            break;
        }
    case 14:
        BitmapCov::branchBitmap[94] = 1;
        {
            rtb_MultiportSwitch_n = rtb_y_ihg;
            break;
        }
    default:
        BitmapCov::branchBitmap[95] = 1;
        {
            rtb_MultiportSwitch_n = rtb_y_g;
            break;
        }
    }
    rtb_y = DynamicCompressionMo_BitRotate1(rtb_MultiportSwitch_n);
    *DynamicCompressionModel_Y_proc_sig = DynamicCompressionModel_DW->UnitDelay1_DSTATE - (int32_T)(((uint64_T)((uint8_T)((uint8_T)DynamicCompressionModel_DW->UnitDelay3_DSTATE_o & 31)) * rtb_y + rtb_MultiportSwitch_n) >> 2);
    rtb_y = DynamicCompressionMod_BitRotate(32U);
    rtb_y_n = DynamicCompression_BitRotate1_l(32U);
    rtb_y_l = DynamicCompressionMo_BitRotate2(32U);
    rtb_y_g = DynamicCompressionMo_BitRotate3(32U);
    rtb_y_ihg = DynamicCompressionMo_BitRotate4(32U);
    rtb_y_bn = DynamicCompressionMo_BitRotate5(32U);
    rtb_y_d = DynamicCompressionMo_BitRotate6(32U);
    rtb_y_mb = DynamicCompressionMo_BitRotate7(32U);
    rtb_y_b3 = DynamicCompressionMo_BitRotate8(32U);
    rtb_y_ck = DynamicCompressionMo_BitRotate9(32U);
    rtb_y_jd = DynamicCompressionM_BitRotate10(32U);
    rtb_y_b = DynamicCompressionM_BitRotate11(32U);
    rtb_y_i = DynamicCompressionM_BitRotate12(32U);
    rtb_y_et = DynamicCompressionM_BitRotate13(32U);
    rtb_y_nd = DynamicCompressionM_BitRotate14(32U);
    switch(rtb_Subtract_c)
    {
    case 0:
        BitmapCov::branchBitmap[96] = 1;
        {
            rtb_MultiportSwitch = 32U;
            break;
        }
    case 1:
        BitmapCov::branchBitmap[97] = 1;
        {
            rtb_MultiportSwitch = rtb_y;
            break;
        }
    case 2:
        BitmapCov::branchBitmap[98] = 1;
        {
            rtb_MultiportSwitch = rtb_y_n;
            break;
        }
    case 3:
        BitmapCov::branchBitmap[99] = 1;
        {
            rtb_MultiportSwitch = rtb_y_l;
            break;
        }
    case 4:
        BitmapCov::branchBitmap[100] = 1;
        {
            rtb_MultiportSwitch = rtb_y_g;
            break;
        }
    case 5:
        BitmapCov::branchBitmap[101] = 1;
        {
            rtb_MultiportSwitch = rtb_y_ihg;
            break;
        }
    case 6:
        BitmapCov::branchBitmap[102] = 1;
        {
            rtb_MultiportSwitch = rtb_y_bn;
            break;
        }
    case 7:
        BitmapCov::branchBitmap[103] = 1;
        {
            rtb_MultiportSwitch = rtb_y_d;
            break;
        }
    case 8:
        BitmapCov::branchBitmap[104] = 1;
        {
            rtb_MultiportSwitch = rtb_y_mb;
            break;
        }
    case 9:
        BitmapCov::branchBitmap[105] = 1;
        {
            rtb_MultiportSwitch = rtb_y_b3;
            break;
        }
    case 10:
        BitmapCov::branchBitmap[106] = 1;
        {
            rtb_MultiportSwitch = rtb_y_ck;
            break;
        }
    case 11:
        BitmapCov::branchBitmap[107] = 1;
        {
            rtb_MultiportSwitch = rtb_y_jd;
            break;
        }
    case 12:
        BitmapCov::branchBitmap[108] = 1;
        {
            rtb_MultiportSwitch = rtb_y_b;
            break;
        }
    case 13:
        BitmapCov::branchBitmap[109] = 1;
        {
            rtb_MultiportSwitch = rtb_y_i;
            break;
        }
    case 14:
        BitmapCov::branchBitmap[110] = 1;
        {
            rtb_MultiportSwitch = rtb_y_et;
            break;
        }
    default:
        BitmapCov::branchBitmap[111] = 1;
        {
            rtb_MultiportSwitch = rtb_y_nd;
            break;
        }
    }
    if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(*DynamicCompressionModel_Y_proc_sig < 0, 44, 1, 22), -45, 0, 22)) {
        BitmapCov::branchBitmap[112] = 1;
        rtb_Switch = DynamicCompressionModel_DW->ram_data_DSTATE;
    }
    else {
        BitmapCov::branchBitmap[113] = 1;
        tmp_2 = (uint64_T)((int64_T)DynamicCompressionModel_DW->ram_data_DSTATE_g - DynamicCompressionModel_DW->ram_data_DSTATE);
        tmp_3 = rtb_MultiportSwitch;
        ssuMultiWordMul(&tmp_2, 1, &tmp_3, 1, &tmp_1.chunks[0U], 2);
        tmp_2 = (uint64_T)*DynamicCompressionModel_Y_proc_sig;
        sMultiWordMul(&tmp_1.chunks[0U], 2, &tmp_2, 1, &tmp_0.chunks[0U], 2);
        sMultiWordShr(&tmp_0.chunks[0U], 2, 28U, &tmp.chunks[0U], 2);
        rtb_Switch = DynamicCompressionModel_DW->ram_data_DSTATE + (int32_T)MultiWord2sLong(&tmp.chunks[0U]);
    }
    TCGHybrid::updateBranchDistanceValue("rtb_ExtractDesiredBits_oj", rtb_ExtractDesiredBits_oj);
    if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(rtb_ExtractDesiredBits_oj > 4, 46, 1, 23), -47, 0, 23)) {
        BitmapCov::branchBitmap[114] = 1;
        rtb_Switch_nj = DynamicCompressionModel_DW->dly_shift_5_DSTATE[0];
    }
    else {
        BitmapCov::branchBitmap[115] = 1;
        rtb_Switch_nj = 0;
    }
    rtb_y_n = DynamicCompressionM_BitRotate_g(1073741824U);
    rtb_y_jd = DynamicCompression_BitRotate1_a(1073741824U);
    rtb_y_g = DynamicCompression_BitRotate2_a(1073741824U);
    rtb_y_ihg = DynamicCompression_BitRotate3_d(1073741824U);
    rtb_y_bn = DynamicCompressionMo_BitRotate1(1073741824U);
    rtb_y_d = DynamicCompression_BitRotate5_p(1073741824U);
    rtb_y_mb = DynamicCompression_BitRotate6_b(1073741824U);
    rtb_y_b3 = DynamicCompression_BitRotate7_h(1073741824U);
    rtb_y_ck = DynamicCompression_BitRotate8_o(1073741824U);
    rtb_y = DynamicCompression_BitRotate9_g(1073741824U);
    rtb_y_b = DynamicCompressio_BitRotate10_p(1073741824U);
    rtb_y_i = DynamicCompressio_BitRotate11_l(1073741824U);
    rtb_y_et = DynamicCompressio_BitRotate12_m(1073741824U);
    rtb_y_nd = DynamicCompressio_BitRotate13_a(1073741824U);
    rtb_y_l = DynamicCompressio_BitRotate14_o(1073741824U);
    switch(rtb_NShifts_i)
    {
    case 0:
        BitmapCov::branchBitmap[116] = 1;
        {
            rtb_MultiportSwitch_n = 1073741824U;
            break;
        }
    case 1:
        BitmapCov::branchBitmap[117] = 1;
        {
            rtb_MultiportSwitch_n = rtb_y_n;
            break;
        }
    case 2:
        BitmapCov::branchBitmap[118] = 1;
        {
            rtb_MultiportSwitch_n = rtb_y_jd;
            break;
        }
    case 3:
        BitmapCov::branchBitmap[119] = 1;
        {
            rtb_MultiportSwitch_n = rtb_y_g;
            break;
        }
    case 4:
        BitmapCov::branchBitmap[120] = 1;
        {
            rtb_MultiportSwitch_n = rtb_y_ihg;
            break;
        }
    case 5:
        BitmapCov::branchBitmap[121] = 1;
        {
            rtb_MultiportSwitch_n = rtb_y_bn;
            break;
        }
    case 6:
        BitmapCov::branchBitmap[122] = 1;
        {
            rtb_MultiportSwitch_n = rtb_y_d;
            break;
        }
    case 7:
        BitmapCov::branchBitmap[123] = 1;
        {
            rtb_MultiportSwitch_n = rtb_y_mb;
            break;
        }
    case 8:
        BitmapCov::branchBitmap[124] = 1;
        {
            rtb_MultiportSwitch_n = rtb_y_b3;
            break;
        }
    case 9:
        BitmapCov::branchBitmap[125] = 1;
        {
            rtb_MultiportSwitch_n = rtb_y_ck;
            break;
        }
    case 10:
        BitmapCov::branchBitmap[126] = 1;
        {
            rtb_MultiportSwitch_n = rtb_y;
            break;
        }
    case 11:
        BitmapCov::branchBitmap[127] = 1;
        {
            rtb_MultiportSwitch_n = rtb_y_b;
            break;
        }
    case 12:
        BitmapCov::branchBitmap[128] = 1;
        {
            rtb_MultiportSwitch_n = rtb_y_i;
            break;
        }
    case 13:
        BitmapCov::branchBitmap[129] = 1;
        {
            rtb_MultiportSwitch_n = rtb_y_et;
            break;
        }
    case 14:
        BitmapCov::branchBitmap[130] = 1;
        {
            rtb_MultiportSwitch_n = rtb_y_nd;
            break;
        }
    default:
        BitmapCov::branchBitmap[131] = 1;
        {
            rtb_MultiportSwitch_n = rtb_y_l;
            break;
        }
    }
    rtb_y = DynamicCompressionMo_BitRotate1(rtb_MultiportSwitch_n);
    *DynamicCompressionModel_Y_proc_sig = DynamicCompressionModel_DW->UnitDelay1_DSTATE_a - (int32_T)(((uint64_T)((uint8_T)((uint8_T)DynamicCompressionModel_DW->UnitDelay3_DSTATE_c & 31)) * rtb_y + rtb_MultiportSwitch_n) >> 2);
    rtb_y_n = DynamicCompressionMod_BitRotate(32U);
    rtb_y_jd = DynamicCompression_BitRotate1_l(32U);
    rtb_y_g = DynamicCompressionMo_BitRotate2(32U);
    rtb_y_ihg = DynamicCompressionMo_BitRotate3(32U);
    rtb_y_bn = DynamicCompressionMo_BitRotate4(32U);
    rtb_y_d = DynamicCompressionMo_BitRotate5(32U);
    rtb_y_mb = DynamicCompressionMo_BitRotate6(32U);
    rtb_y_b3 = DynamicCompressionMo_BitRotate7(32U);
    rtb_y_ck = DynamicCompressionMo_BitRotate8(32U);
    rtb_y = DynamicCompressionMo_BitRotate9(32U);
    rtb_y_b = DynamicCompressionM_BitRotate10(32U);
    rtb_y_i = DynamicCompressionM_BitRotate11(32U);
    rtb_y_et = DynamicCompressionM_BitRotate12(32U);
    rtb_y_nd = DynamicCompressionM_BitRotate13(32U);
    rtb_y_l = DynamicCompressionM_BitRotate14(32U);
    switch(rtb_NShifts_i)
    {
    case 0:
        BitmapCov::branchBitmap[132] = 1;
        {
            rtb_MultiportSwitch = 32U;
            break;
        }
    case 1:
        BitmapCov::branchBitmap[133] = 1;
        {
            rtb_MultiportSwitch = rtb_y_n;
            break;
        }
    case 2:
        BitmapCov::branchBitmap[134] = 1;
        {
            rtb_MultiportSwitch = rtb_y_jd;
            break;
        }
    case 3:
        BitmapCov::branchBitmap[135] = 1;
        {
            rtb_MultiportSwitch = rtb_y_g;
            break;
        }
    case 4:
        BitmapCov::branchBitmap[136] = 1;
        {
            rtb_MultiportSwitch = rtb_y_ihg;
            break;
        }
    case 5:
        BitmapCov::branchBitmap[137] = 1;
        {
            rtb_MultiportSwitch = rtb_y_bn;
            break;
        }
    case 6:
        BitmapCov::branchBitmap[138] = 1;
        {
            rtb_MultiportSwitch = rtb_y_d;
            break;
        }
    case 7:
        BitmapCov::branchBitmap[139] = 1;
        {
            rtb_MultiportSwitch = rtb_y_mb;
            break;
        }
    case 8:
        BitmapCov::branchBitmap[140] = 1;
        {
            rtb_MultiportSwitch = rtb_y_b3;
            break;
        }
    case 9:
        BitmapCov::branchBitmap[141] = 1;
        {
            rtb_MultiportSwitch = rtb_y_ck;
            break;
        }
    case 10:
        BitmapCov::branchBitmap[142] = 1;
        {
            rtb_MultiportSwitch = rtb_y;
            break;
        }
    case 11:
        BitmapCov::branchBitmap[143] = 1;
        {
            rtb_MultiportSwitch = rtb_y_b;
            break;
        }
    case 12:
        BitmapCov::branchBitmap[144] = 1;
        {
            rtb_MultiportSwitch = rtb_y_i;
            break;
        }
    case 13:
        BitmapCov::branchBitmap[145] = 1;
        {
            rtb_MultiportSwitch = rtb_y_et;
            break;
        }
    case 14:
        BitmapCov::branchBitmap[146] = 1;
        {
            rtb_MultiportSwitch = rtb_y_nd;
            break;
        }
    default:
        BitmapCov::branchBitmap[147] = 1;
        {
            rtb_MultiportSwitch = rtb_y_l;
            break;
        }
    }
    if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(*DynamicCompressionModel_Y_proc_sig < 0, 48, 1, 24), -49, 0, 24)) {
        BitmapCov::branchBitmap[148] = 1;
        rtb_Switch_d3 = DynamicCompressionModel_DW->ram_data_DSTATE_n;
    }
    else {
        BitmapCov::branchBitmap[149] = 1;
        tmp_2 = (uint64_T)((int64_T)DynamicCompressionModel_DW->ram_data_DSTATE_a - DynamicCompressionModel_DW->ram_data_DSTATE_n);
        tmp_3 = rtb_MultiportSwitch;
        ssuMultiWordMul(&tmp_2, 1, &tmp_3, 1, &tmp_4.chunks[0U], 2);
        tmp_2 = (uint64_T)*DynamicCompressionModel_Y_proc_sig;
        sMultiWordMul(&tmp_4.chunks[0U], 2, &tmp_2, 1, &tmp_1.chunks[0U], 2);
        sMultiWordShr(&tmp_1.chunks[0U], 2, 28U, &tmp_0.chunks[0U], 2);
        rtb_Switch_d3 = DynamicCompressionModel_DW->ram_data_DSTATE_n + (int32_T)MultiWord2sLong(&tmp_0.chunks[0U]);
    }
    rtb_y_le = DynamicCompressionMod_BitRotate(DynamicCompressionModel_DW->Delay2_DSTATE_g);
    rtb_y_e = DynamicCompression_BitRotate1_l(DynamicCompressionModel_DW->Delay2_DSTATE_g);
    rtb_y_oa = DynamicCompressionMo_BitRotate2(DynamicCompressionModel_DW->Delay2_DSTATE_g);
    rtb_y_d1 = DynamicCompressionMo_BitRotate3(DynamicCompressionModel_DW->Delay2_DSTATE_g);
    rtb_y_fv = DynamicCompressionMo_BitRotate4(DynamicCompressionModel_DW->Delay2_DSTATE_g);
    rtb_y_m = DynamicCompressionMo_BitRotate5(DynamicCompressionModel_DW->Delay2_DSTATE_g);
    rtb_y_j = DynamicCompressionMo_BitRotate6(DynamicCompressionModel_DW->Delay2_DSTATE_g);
    rtb_y_p = DynamicCompressionMo_BitRotate7(DynamicCompressionModel_DW->Delay2_DSTATE_g);
    rtb_y_f = DynamicCompressionMo_BitRotate8(DynamicCompressionModel_DW->Delay2_DSTATE_g);
    rtb_y_he = DynamicCompressionMo_BitRotate9(DynamicCompressionModel_DW->Delay2_DSTATE_g);
    rtb_y_h4 = DynamicCompressionM_BitRotate10(DynamicCompressionModel_DW->Delay2_DSTATE_g);
    rtb_y_o = DynamicCompressionM_BitRotate11(DynamicCompressionModel_DW->Delay2_DSTATE_g);
    rtb_y_ij = DynamicCompressionM_BitRotate12(DynamicCompressionModel_DW->Delay2_DSTATE_g);
    rtb_y_nj = DynamicCompressionM_BitRotate13(DynamicCompressionModel_DW->Delay2_DSTATE_g);
    rtb_y_je = DynamicCompressionM_BitRotate14(DynamicCompressionModel_DW->Delay2_DSTATE_g);
    switch(rtb_ExtractDesiredBits_ez)
    {
    case 0:
        BitmapCov::branchBitmap[150] = 1;
        {
            break;
        }
    case 1:
        BitmapCov::branchBitmap[151] = 1;
        {
            DynamicCompressionModel_DW->Delay2_DSTATE_g = rtb_y_le;
            break;
        }
    case 2:
        BitmapCov::branchBitmap[152] = 1;
        {
            DynamicCompressionModel_DW->Delay2_DSTATE_g = rtb_y_e;
            break;
        }
    case 3:
        BitmapCov::branchBitmap[153] = 1;
        {
            DynamicCompressionModel_DW->Delay2_DSTATE_g = rtb_y_oa;
            break;
        }
    case 4:
        BitmapCov::branchBitmap[154] = 1;
        {
            DynamicCompressionModel_DW->Delay2_DSTATE_g = rtb_y_d1;
            break;
        }
    case 5:
        BitmapCov::branchBitmap[155] = 1;
        {
            DynamicCompressionModel_DW->Delay2_DSTATE_g = rtb_y_fv;
            break;
        }
    case 6:
        BitmapCov::branchBitmap[156] = 1;
        {
            DynamicCompressionModel_DW->Delay2_DSTATE_g = rtb_y_m;
            break;
        }
    case 7:
        BitmapCov::branchBitmap[157] = 1;
        {
            DynamicCompressionModel_DW->Delay2_DSTATE_g = rtb_y_j;
            break;
        }
    case 8:
        BitmapCov::branchBitmap[158] = 1;
        {
            DynamicCompressionModel_DW->Delay2_DSTATE_g = rtb_y_p;
            break;
        }
    case 9:
        BitmapCov::branchBitmap[159] = 1;
        {
            DynamicCompressionModel_DW->Delay2_DSTATE_g = rtb_y_f;
            break;
        }
    case 10:
        BitmapCov::branchBitmap[160] = 1;
        {
            DynamicCompressionModel_DW->Delay2_DSTATE_g = rtb_y_he;
            break;
        }
    case 11:
        BitmapCov::branchBitmap[161] = 1;
        {
            DynamicCompressionModel_DW->Delay2_DSTATE_g = rtb_y_h4;
            break;
        }
    case 12:
        BitmapCov::branchBitmap[162] = 1;
        {
            DynamicCompressionModel_DW->Delay2_DSTATE_g = rtb_y_o;
            break;
        }
    case 13:
        BitmapCov::branchBitmap[163] = 1;
        {
            DynamicCompressionModel_DW->Delay2_DSTATE_g = rtb_y_ij;
            break;
        }
    case 14:
        BitmapCov::branchBitmap[164] = 1;
        {
            DynamicCompressionModel_DW->Delay2_DSTATE_g = rtb_y_nj;
            break;
        }
    default:
        BitmapCov::branchBitmap[165] = 1;
        {
            DynamicCompressionModel_DW->Delay2_DSTATE_g = rtb_y_je;
            break;
        }
    }
    TCGHybrid::updateBranchDistanceValue("rtb_Subtract_a", rtb_Subtract_a);
    if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(rtb_Subtract_a > 15, 50, 1, 25), -51, 0, 25)) {
        BitmapCov::branchBitmap[166] = 1;
        DynamicCompressionModel_DW->UnitDelay3_DSTATE_c = 0U;
    }
    else {
        BitmapCov::branchBitmap[167] = 1;
        DynamicCompressionModel_DW->UnitDelay3_DSTATE_c = (uint16_T)((uint8_T)((uint8_T)~rtb_ExtractDesiredBits_ez & 15) << 5 | (uint8_T)((uint8_T)(DynamicCompressionModel_DW->Delay2_DSTATE_g >> 23) & 31));
    }
    DynamicCompressionModel_DW->ram_data_DSTATE_a = rtb_ram_gg[(uint16_T)((DynamicCompressionModel_DW->UnitDelay3_DSTATE_c + 1U) & 511U)];
    DynamicCompressionModel_DW->ram_data_DSTATE_n = rtb_ram_k[DynamicCompressionModel_DW->UnitDelay3_DSTATE_c];
    TCGHybrid::updateBranchDistanceValue("rtb_ExtractDesiredBits_oj", rtb_ExtractDesiredBits_oj);
    if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(rtb_ExtractDesiredBits_oj > 4, 52, 1, 26), -53, 0, 26)) {
        BitmapCov::branchBitmap[168] = 1;
        rtb_Switch_kd = DynamicCompressionModel_DW->dly_shift_4_DSTATE[0];
    }
    else {
        BitmapCov::branchBitmap[169] = 1;
        rtb_Switch_kd = 0;
    }
    rtb_y_n = DynamicCompressionM_BitRotate_g(1073741824U);
    rtb_y_jd = DynamicCompression_BitRotate1_a(1073741824U);
    rtb_y_g = DynamicCompression_BitRotate2_a(1073741824U);
    rtb_y_ihg = DynamicCompression_BitRotate3_d(1073741824U);
    rtb_y_bn = DynamicCompressionMo_BitRotate1(1073741824U);
    rtb_y_d = DynamicCompression_BitRotate5_p(1073741824U);
    rtb_y_mb = DynamicCompression_BitRotate6_b(1073741824U);
    rtb_y_b3 = DynamicCompression_BitRotate7_h(1073741824U);
    rtb_y_ck = DynamicCompression_BitRotate8_o(1073741824U);
    rtb_y = DynamicCompression_BitRotate9_g(1073741824U);
    rtb_y_b = DynamicCompressio_BitRotate10_p(1073741824U);
    rtb_y_i = DynamicCompressio_BitRotate11_l(1073741824U);
    rtb_y_et = DynamicCompressio_BitRotate12_m(1073741824U);
    rtb_y_nd = DynamicCompressio_BitRotate13_a(1073741824U);
    rtb_y_l = DynamicCompressio_BitRotate14_o(1073741824U);
    switch(rtb_NShifts_n)
    {
    case 0:
        BitmapCov::branchBitmap[170] = 1;
        {
            rtb_MultiportSwitch_n = 1073741824U;
            break;
        }
    case 1:
        BitmapCov::branchBitmap[171] = 1;
        {
            rtb_MultiportSwitch_n = rtb_y_n;
            break;
        }
    case 2:
        BitmapCov::branchBitmap[172] = 1;
        {
            rtb_MultiportSwitch_n = rtb_y_jd;
            break;
        }
    case 3:
        BitmapCov::branchBitmap[173] = 1;
        {
            rtb_MultiportSwitch_n = rtb_y_g;
            break;
        }
    case 4:
        BitmapCov::branchBitmap[174] = 1;
        {
            rtb_MultiportSwitch_n = rtb_y_ihg;
            break;
        }
    case 5:
        BitmapCov::branchBitmap[175] = 1;
        {
            rtb_MultiportSwitch_n = rtb_y_bn;
            break;
        }
    case 6:
        BitmapCov::branchBitmap[176] = 1;
        {
            rtb_MultiportSwitch_n = rtb_y_d;
            break;
        }
    case 7:
        BitmapCov::branchBitmap[177] = 1;
        {
            rtb_MultiportSwitch_n = rtb_y_mb;
            break;
        }
    case 8:
        BitmapCov::branchBitmap[178] = 1;
        {
            rtb_MultiportSwitch_n = rtb_y_b3;
            break;
        }
    case 9:
        BitmapCov::branchBitmap[179] = 1;
        {
            rtb_MultiportSwitch_n = rtb_y_ck;
            break;
        }
    case 10:
        BitmapCov::branchBitmap[180] = 1;
        {
            rtb_MultiportSwitch_n = rtb_y;
            break;
        }
    case 11:
        BitmapCov::branchBitmap[181] = 1;
        {
            rtb_MultiportSwitch_n = rtb_y_b;
            break;
        }
    case 12:
        BitmapCov::branchBitmap[182] = 1;
        {
            rtb_MultiportSwitch_n = rtb_y_i;
            break;
        }
    case 13:
        BitmapCov::branchBitmap[183] = 1;
        {
            rtb_MultiportSwitch_n = rtb_y_et;
            break;
        }
    case 14:
        BitmapCov::branchBitmap[184] = 1;
        {
            rtb_MultiportSwitch_n = rtb_y_nd;
            break;
        }
    default:
        BitmapCov::branchBitmap[185] = 1;
        {
            rtb_MultiportSwitch_n = rtb_y_l;
            break;
        }
    }
    rtb_y = DynamicCompressionMo_BitRotate1(rtb_MultiportSwitch_n);
    *DynamicCompressionModel_Y_proc_sig = DynamicCompressionModel_DW->UnitDelay1_DSTATE_i - (int32_T)(((uint64_T)((uint8_T)((uint8_T)DynamicCompressionModel_DW->UnitDelay3_DSTATE_i & 31)) * rtb_y + rtb_MultiportSwitch_n) >> 2);
    rtb_y_n = DynamicCompressionMod_BitRotate(32U);
    rtb_y_jd = DynamicCompression_BitRotate1_l(32U);
    rtb_y_g = DynamicCompressionMo_BitRotate2(32U);
    rtb_y_ihg = DynamicCompressionMo_BitRotate3(32U);
    rtb_y_bn = DynamicCompressionMo_BitRotate4(32U);
    rtb_y_d = DynamicCompressionMo_BitRotate5(32U);
    rtb_y_mb = DynamicCompressionMo_BitRotate6(32U);
    rtb_y_b3 = DynamicCompressionMo_BitRotate7(32U);
    rtb_y_ck = DynamicCompressionMo_BitRotate8(32U);
    rtb_y = DynamicCompressionMo_BitRotate9(32U);
    rtb_y_b = DynamicCompressionM_BitRotate10(32U);
    rtb_y_i = DynamicCompressionM_BitRotate11(32U);
    rtb_y_et = DynamicCompressionM_BitRotate12(32U);
    rtb_y_nd = DynamicCompressionM_BitRotate13(32U);
    rtb_y_l = DynamicCompressionM_BitRotate14(32U);
    switch(rtb_NShifts_n)
    {
    case 0:
        BitmapCov::branchBitmap[186] = 1;
        {
            rtb_MultiportSwitch = 32U;
            break;
        }
    case 1:
        BitmapCov::branchBitmap[187] = 1;
        {
            rtb_MultiportSwitch = rtb_y_n;
            break;
        }
    case 2:
        BitmapCov::branchBitmap[188] = 1;
        {
            rtb_MultiportSwitch = rtb_y_jd;
            break;
        }
    case 3:
        BitmapCov::branchBitmap[189] = 1;
        {
            rtb_MultiportSwitch = rtb_y_g;
            break;
        }
    case 4:
        BitmapCov::branchBitmap[190] = 1;
        {
            rtb_MultiportSwitch = rtb_y_ihg;
            break;
        }
    case 5:
        BitmapCov::branchBitmap[191] = 1;
        {
            rtb_MultiportSwitch = rtb_y_bn;
            break;
        }
    case 6:
        BitmapCov::branchBitmap[192] = 1;
        {
            rtb_MultiportSwitch = rtb_y_d;
            break;
        }
    case 7:
        BitmapCov::branchBitmap[193] = 1;
        {
            rtb_MultiportSwitch = rtb_y_mb;
            break;
        }
    case 8:
        BitmapCov::branchBitmap[194] = 1;
        {
            rtb_MultiportSwitch = rtb_y_b3;
            break;
        }
    case 9:
        BitmapCov::branchBitmap[195] = 1;
        {
            rtb_MultiportSwitch = rtb_y_ck;
            break;
        }
    case 10:
        BitmapCov::branchBitmap[196] = 1;
        {
            rtb_MultiportSwitch = rtb_y;
            break;
        }
    case 11:
        BitmapCov::branchBitmap[197] = 1;
        {
            rtb_MultiportSwitch = rtb_y_b;
            break;
        }
    case 12:
        BitmapCov::branchBitmap[198] = 1;
        {
            rtb_MultiportSwitch = rtb_y_i;
            break;
        }
    case 13:
        BitmapCov::branchBitmap[199] = 1;
        {
            rtb_MultiportSwitch = rtb_y_et;
            break;
        }
    case 14:
        BitmapCov::branchBitmap[200] = 1;
        {
            rtb_MultiportSwitch = rtb_y_nd;
            break;
        }
    default:
        BitmapCov::branchBitmap[201] = 1;
        {
            rtb_MultiportSwitch = rtb_y_l;
            break;
        }
    }
    if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(*DynamicCompressionModel_Y_proc_sig < 0, 54, 1, 27), -55, 0, 27)) {
        BitmapCov::branchBitmap[202] = 1;
        rtb_Switch_mv = DynamicCompressionModel_DW->ram_data_DSTATE_f;
    }
    else {
        BitmapCov::branchBitmap[203] = 1;
        tmp_2 = (uint64_T)((int64_T)DynamicCompressionModel_DW->ram_data_DSTATE_e - DynamicCompressionModel_DW->ram_data_DSTATE_f);
        tmp_3 = rtb_MultiportSwitch;
        ssuMultiWordMul(&tmp_2, 1, &tmp_3, 1, &tmp_5.chunks[0U], 2);
        tmp_2 = (uint64_T)*DynamicCompressionModel_Y_proc_sig;
        sMultiWordMul(&tmp_5.chunks[0U], 2, &tmp_2, 1, &tmp_4.chunks[0U], 2);
        sMultiWordShr(&tmp_4.chunks[0U], 2, 28U, &tmp_1.chunks[0U], 2);
        rtb_Switch_mv = DynamicCompressionModel_DW->ram_data_DSTATE_f + (int32_T)MultiWord2sLong(&tmp_1.chunks[0U]);
    }
    rtb_y_av = DynamicCompressionMod_BitRotate(DynamicCompressionModel_DW->Delay2_DSTATE_a);
    rtb_y_a2t = DynamicCompression_BitRotate1_l(DynamicCompressionModel_DW->Delay2_DSTATE_a);
    rtb_y_lgp = DynamicCompressionMo_BitRotate2(DynamicCompressionModel_DW->Delay2_DSTATE_a);
    rtb_y_a = DynamicCompressionMo_BitRotate3(DynamicCompressionModel_DW->Delay2_DSTATE_a);
    rtb_y_pc = DynamicCompressionMo_BitRotate4(DynamicCompressionModel_DW->Delay2_DSTATE_a);
    rtb_y_kh0 = DynamicCompressionMo_BitRotate5(DynamicCompressionModel_DW->Delay2_DSTATE_a);
    rtb_y_nq = DynamicCompressionMo_BitRotate6(DynamicCompressionModel_DW->Delay2_DSTATE_a);
    rtb_y_li = DynamicCompressionMo_BitRotate7(DynamicCompressionModel_DW->Delay2_DSTATE_a);
    rtb_y_j0 = DynamicCompressionMo_BitRotate8(DynamicCompressionModel_DW->Delay2_DSTATE_a);
    rtb_y_nk = DynamicCompressionMo_BitRotate9(DynamicCompressionModel_DW->Delay2_DSTATE_a);
    rtb_y_h5 = DynamicCompressionM_BitRotate10(DynamicCompressionModel_DW->Delay2_DSTATE_a);
    rtb_y_bo = DynamicCompressionM_BitRotate11(DynamicCompressionModel_DW->Delay2_DSTATE_a);
    rtb_y_pu = DynamicCompressionM_BitRotate12(DynamicCompressionModel_DW->Delay2_DSTATE_a);
    rtb_y_k = DynamicCompressionM_BitRotate13(DynamicCompressionModel_DW->Delay2_DSTATE_a);
    rtb_y_ev = DynamicCompressionM_BitRotate14(DynamicCompressionModel_DW->Delay2_DSTATE_a);
    switch(rtb_ExtractDesiredBits_a)
    {
    case 0:
        BitmapCov::branchBitmap[204] = 1;
        {
            break;
        }
    case 1:
        BitmapCov::branchBitmap[205] = 1;
        {
            DynamicCompressionModel_DW->Delay2_DSTATE_a = rtb_y_av;
            break;
        }
    case 2:
        BitmapCov::branchBitmap[206] = 1;
        {
            DynamicCompressionModel_DW->Delay2_DSTATE_a = rtb_y_a2t;
            break;
        }
    case 3:
        BitmapCov::branchBitmap[207] = 1;
        {
            DynamicCompressionModel_DW->Delay2_DSTATE_a = rtb_y_lgp;
            break;
        }
    case 4:
        BitmapCov::branchBitmap[208] = 1;
        {
            DynamicCompressionModel_DW->Delay2_DSTATE_a = rtb_y_a;
            break;
        }
    case 5:
        BitmapCov::branchBitmap[209] = 1;
        {
            DynamicCompressionModel_DW->Delay2_DSTATE_a = rtb_y_pc;
            break;
        }
    case 6:
        BitmapCov::branchBitmap[210] = 1;
        {
            DynamicCompressionModel_DW->Delay2_DSTATE_a = rtb_y_kh0;
            break;
        }
    case 7:
        BitmapCov::branchBitmap[211] = 1;
        {
            DynamicCompressionModel_DW->Delay2_DSTATE_a = rtb_y_nq;
            break;
        }
    case 8:
        BitmapCov::branchBitmap[212] = 1;
        {
            DynamicCompressionModel_DW->Delay2_DSTATE_a = rtb_y_li;
            break;
        }
    case 9:
        BitmapCov::branchBitmap[213] = 1;
        {
            DynamicCompressionModel_DW->Delay2_DSTATE_a = rtb_y_j0;
            break;
        }
    case 10:
        BitmapCov::branchBitmap[214] = 1;
        {
            DynamicCompressionModel_DW->Delay2_DSTATE_a = rtb_y_nk;
            break;
        }
    case 11:
        BitmapCov::branchBitmap[215] = 1;
        {
            DynamicCompressionModel_DW->Delay2_DSTATE_a = rtb_y_h5;
            break;
        }
    case 12:
        BitmapCov::branchBitmap[216] = 1;
        {
            DynamicCompressionModel_DW->Delay2_DSTATE_a = rtb_y_bo;
            break;
        }
    case 13:
        BitmapCov::branchBitmap[217] = 1;
        {
            DynamicCompressionModel_DW->Delay2_DSTATE_a = rtb_y_pu;
            break;
        }
    case 14:
        BitmapCov::branchBitmap[218] = 1;
        {
            DynamicCompressionModel_DW->Delay2_DSTATE_a = rtb_y_k;
            break;
        }
    default:
        BitmapCov::branchBitmap[219] = 1;
        {
            DynamicCompressionModel_DW->Delay2_DSTATE_a = rtb_y_ev;
            break;
        }
    }
    TCGHybrid::updateBranchDistanceValue("rtb_Subtract_n", rtb_Subtract_n);
    if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(rtb_Subtract_n > 15, 56, 1, 28), -57, 0, 28)) {
        BitmapCov::branchBitmap[220] = 1;
        DynamicCompressionModel_DW->UnitDelay3_DSTATE_i = 0U;
    }
    else {
        BitmapCov::branchBitmap[221] = 1;
        DynamicCompressionModel_DW->UnitDelay3_DSTATE_i = (uint16_T)((uint8_T)((uint8_T)~rtb_ExtractDesiredBits_a & 15) << 5 | (uint8_T)((uint8_T)(DynamicCompressionModel_DW->Delay2_DSTATE_a >> 23) & 31));
    }
    DynamicCompressionModel_DW->ram_data_DSTATE_e = rtb_ram_p1[(uint16_T)((DynamicCompressionModel_DW->UnitDelay3_DSTATE_i + 1U) & 511U)];
    DynamicCompressionModel_DW->ram_data_DSTATE_f = rtb_ram_j2[DynamicCompressionModel_DW->UnitDelay3_DSTATE_i];
    TCGHybrid::updateBranchDistanceValue("rtb_ExtractDesiredBits_oj", rtb_ExtractDesiredBits_oj);
    if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(rtb_ExtractDesiredBits_oj > 4, 58, 1, 29), -59, 0, 29)) {
        BitmapCov::branchBitmap[222] = 1;
        rtb_Switch_m = DynamicCompressionModel_DW->dly_shift_3_DSTATE[0];
    }
    else {
        BitmapCov::branchBitmap[223] = 1;
        rtb_Switch_m = 0;
    }
    rtb_y_n = DynamicCompressionM_BitRotate_g(1073741824U);
    rtb_y_jd = DynamicCompression_BitRotate1_a(1073741824U);
    rtb_y_g = DynamicCompression_BitRotate2_a(1073741824U);
    rtb_y_ihg = DynamicCompression_BitRotate3_d(1073741824U);
    rtb_y_bn = DynamicCompressionMo_BitRotate1(1073741824U);
    rtb_y_d = DynamicCompression_BitRotate5_p(1073741824U);
    rtb_y_mb = DynamicCompression_BitRotate6_b(1073741824U);
    rtb_y_b3 = DynamicCompression_BitRotate7_h(1073741824U);
    rtb_y_ck = DynamicCompression_BitRotate8_o(1073741824U);
    rtb_y = DynamicCompression_BitRotate9_g(1073741824U);
    rtb_y_b = DynamicCompressio_BitRotate10_p(1073741824U);
    rtb_y_i = DynamicCompressio_BitRotate11_l(1073741824U);
    rtb_y_et = DynamicCompressio_BitRotate12_m(1073741824U);
    rtb_y_nd = DynamicCompressio_BitRotate13_a(1073741824U);
    rtb_y_l = DynamicCompressio_BitRotate14_o(1073741824U);
    switch(rtb_NShifts_mn)
    {
    case 0:
        BitmapCov::branchBitmap[224] = 1;
        {
            rtb_MultiportSwitch_n = 1073741824U;
            break;
        }
    case 1:
        BitmapCov::branchBitmap[225] = 1;
        {
            rtb_MultiportSwitch_n = rtb_y_n;
            break;
        }
    case 2:
        BitmapCov::branchBitmap[226] = 1;
        {
            rtb_MultiportSwitch_n = rtb_y_jd;
            break;
        }
    case 3:
        BitmapCov::branchBitmap[227] = 1;
        {
            rtb_MultiportSwitch_n = rtb_y_g;
            break;
        }
    case 4:
        BitmapCov::branchBitmap[228] = 1;
        {
            rtb_MultiportSwitch_n = rtb_y_ihg;
            break;
        }
    case 5:
        BitmapCov::branchBitmap[229] = 1;
        {
            rtb_MultiportSwitch_n = rtb_y_bn;
            break;
        }
    case 6:
        BitmapCov::branchBitmap[230] = 1;
        {
            rtb_MultiportSwitch_n = rtb_y_d;
            break;
        }
    case 7:
        BitmapCov::branchBitmap[231] = 1;
        {
            rtb_MultiportSwitch_n = rtb_y_mb;
            break;
        }
    case 8:
        BitmapCov::branchBitmap[232] = 1;
        {
            rtb_MultiportSwitch_n = rtb_y_b3;
            break;
        }
    case 9:
        BitmapCov::branchBitmap[233] = 1;
        {
            rtb_MultiportSwitch_n = rtb_y_ck;
            break;
        }
    case 10:
        BitmapCov::branchBitmap[234] = 1;
        {
            rtb_MultiportSwitch_n = rtb_y;
            break;
        }
    case 11:
        BitmapCov::branchBitmap[235] = 1;
        {
            rtb_MultiportSwitch_n = rtb_y_b;
            break;
        }
    case 12:
        BitmapCov::branchBitmap[236] = 1;
        {
            rtb_MultiportSwitch_n = rtb_y_i;
            break;
        }
    case 13:
        BitmapCov::branchBitmap[237] = 1;
        {
            rtb_MultiportSwitch_n = rtb_y_et;
            break;
        }
    case 14:
        BitmapCov::branchBitmap[238] = 1;
        {
            rtb_MultiportSwitch_n = rtb_y_nd;
            break;
        }
    default:
        BitmapCov::branchBitmap[239] = 1;
        {
            rtb_MultiportSwitch_n = rtb_y_l;
            break;
        }
    }
    rtb_y = DynamicCompressionMo_BitRotate1(rtb_MultiportSwitch_n);
    *DynamicCompressionModel_Y_proc_sig = DynamicCompressionModel_DW->UnitDelay1_DSTATE_c - (int32_T)(((uint64_T)((uint8_T)((uint8_T)DynamicCompressionModel_DW->UnitDelay3_DSTATE_d & 31)) * rtb_y + rtb_MultiportSwitch_n) >> 2);
    unsigned char mcdc_95_c_1 = *DynamicCompressionModel_Y_proc_sig < 0;
    if (mcdc_95_c_1) {
        BitmapCov::branchBitmap[240] = 1;
    }
    else {
        BitmapCov::branchBitmap[241] = 1;
    }
    rtb_CastToBoolean1 = (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(*DynamicCompressionModel_Y_proc_sig < 0, 60, 1, 30), -61, 0, 30));
    rtb_y_n = DynamicCompressionMod_BitRotate(32U);
    rtb_y_jd = DynamicCompression_BitRotate1_l(32U);
    rtb_y_g = DynamicCompressionMo_BitRotate2(32U);
    rtb_y_ihg = DynamicCompressionMo_BitRotate3(32U);
    rtb_y_bn = DynamicCompressionMo_BitRotate4(32U);
    rtb_y_d = DynamicCompressionMo_BitRotate5(32U);
    rtb_y_mb = DynamicCompressionMo_BitRotate6(32U);
    rtb_y_b3 = DynamicCompressionMo_BitRotate7(32U);
    rtb_y_ck = DynamicCompressionMo_BitRotate8(32U);
    rtb_y = DynamicCompressionMo_BitRotate9(32U);
    rtb_y_b = DynamicCompressionM_BitRotate10(32U);
    rtb_y_i = DynamicCompressionM_BitRotate11(32U);
    rtb_y_et = DynamicCompressionM_BitRotate12(32U);
    rtb_y_nd = DynamicCompressionM_BitRotate13(32U);
    rtb_y_l = DynamicCompressionM_BitRotate14(32U);
    switch(rtb_NShifts_mn)
    {
    case 0:
        BitmapCov::branchBitmap[242] = 1;
        {
            rtb_MultiportSwitch = 32U;
            break;
        }
    case 1:
        BitmapCov::branchBitmap[243] = 1;
        {
            rtb_MultiportSwitch = rtb_y_n;
            break;
        }
    case 2:
        BitmapCov::branchBitmap[244] = 1;
        {
            rtb_MultiportSwitch = rtb_y_jd;
            break;
        }
    case 3:
        BitmapCov::branchBitmap[245] = 1;
        {
            rtb_MultiportSwitch = rtb_y_g;
            break;
        }
    case 4:
        BitmapCov::branchBitmap[246] = 1;
        {
            rtb_MultiportSwitch = rtb_y_ihg;
            break;
        }
    case 5:
        BitmapCov::branchBitmap[247] = 1;
        {
            rtb_MultiportSwitch = rtb_y_bn;
            break;
        }
    case 6:
        BitmapCov::branchBitmap[248] = 1;
        {
            rtb_MultiportSwitch = rtb_y_d;
            break;
        }
    case 7:
        BitmapCov::branchBitmap[249] = 1;
        {
            rtb_MultiportSwitch = rtb_y_mb;
            break;
        }
    case 8:
        BitmapCov::branchBitmap[250] = 1;
        {
            rtb_MultiportSwitch = rtb_y_b3;
            break;
        }
    case 9:
        BitmapCov::branchBitmap[251] = 1;
        {
            rtb_MultiportSwitch = rtb_y_ck;
            break;
        }
    case 10:
        BitmapCov::branchBitmap[252] = 1;
        {
            rtb_MultiportSwitch = rtb_y;
            break;
        }
    case 11:
        BitmapCov::branchBitmap[253] = 1;
        {
            rtb_MultiportSwitch = rtb_y_b;
            break;
        }
    case 12:
        BitmapCov::branchBitmap[254] = 1;
        {
            rtb_MultiportSwitch = rtb_y_i;
            break;
        }
    case 13:
        BitmapCov::branchBitmap[255] = 1;
        {
            rtb_MultiportSwitch = rtb_y_et;
            break;
        }
    case 14:
        BitmapCov::branchBitmap[256] = 1;
        {
            rtb_MultiportSwitch = rtb_y_nd;
            break;
        }
    default:
        BitmapCov::branchBitmap[257] = 1;
        {
            rtb_MultiportSwitch = rtb_y_l;
            break;
        }
    }
    tmp_2 = (uint64_T)((int64_T)DynamicCompressionModel_DW->ram_data_DSTATE_e5 - DynamicCompressionModel_DW->ram_data_DSTATE_c);
    tmp_3 = rtb_MultiportSwitch;
    ssuMultiWordMul(&tmp_2, 1, &tmp_3, 1, &tmp_6.chunks[0U], 2);
    tmp_2 = (uint64_T)*DynamicCompressionModel_Y_proc_sig;
    sMultiWordMul(&tmp_6.chunks[0U], 2, &tmp_2, 1, &tmp_5.chunks[0U], 2);
    sMultiWordShr(&tmp_5.chunks[0U], 2, 28U, &tmp_4.chunks[0U], 2);
    *DynamicCompressionModel_Y_proc_sig = DynamicCompressionModel_DW->ram_data_DSTATE_c + (int32_T)MultiWord2sLong(&tmp_4.chunks[0U]);
    if (rtb_CastToBoolean1) {
        BitmapCov::branchBitmap[258] = 1;
        rtb_Switch_mc = DynamicCompressionModel_DW->ram_data_DSTATE_c;
    }
    else {
        BitmapCov::branchBitmap[259] = 1;
        rtb_Switch_mc = *DynamicCompressionModel_Y_proc_sig;
    }
    rtb_y_kp = DynamicCompressionMod_BitRotate(DynamicCompressionModel_DW->Delay2_DSTATE_n);
    rtb_y_jv = DynamicCompression_BitRotate1_l(DynamicCompressionModel_DW->Delay2_DSTATE_n);
    rtb_y_j5 = DynamicCompressionMo_BitRotate2(DynamicCompressionModel_DW->Delay2_DSTATE_n);
    rtb_y_c = DynamicCompressionMo_BitRotate3(DynamicCompressionModel_DW->Delay2_DSTATE_n);
    rtb_y_ov = DynamicCompressionMo_BitRotate4(DynamicCompressionModel_DW->Delay2_DSTATE_n);
    rtb_y_iu = DynamicCompressionMo_BitRotate5(DynamicCompressionModel_DW->Delay2_DSTATE_n);
    rtb_y_jg = DynamicCompressionMo_BitRotate6(DynamicCompressionModel_DW->Delay2_DSTATE_n);
    rtb_y_f3 = DynamicCompressionMo_BitRotate7(DynamicCompressionModel_DW->Delay2_DSTATE_n);
    rtb_y_hv = DynamicCompressionMo_BitRotate8(DynamicCompressionModel_DW->Delay2_DSTATE_n);
    rtb_y_j3 = DynamicCompressionMo_BitRotate9(DynamicCompressionModel_DW->Delay2_DSTATE_n);
    rtb_y_o5 = DynamicCompressionM_BitRotate10(DynamicCompressionModel_DW->Delay2_DSTATE_n);
    rtb_y_o3 = DynamicCompressionM_BitRotate11(DynamicCompressionModel_DW->Delay2_DSTATE_n);
    rtb_y_lv = DynamicCompressionM_BitRotate12(DynamicCompressionModel_DW->Delay2_DSTATE_n);
    rtb_y_jf = DynamicCompressionM_BitRotate13(DynamicCompressionModel_DW->Delay2_DSTATE_n);
    rtb_y_ik = DynamicCompressionM_BitRotate14(DynamicCompressionModel_DW->Delay2_DSTATE_n);
    switch(rtb_ExtractDesiredBits_dd)
    {
    case 0:
        BitmapCov::branchBitmap[260] = 1;
        {
            break;
        }
    case 1:
        BitmapCov::branchBitmap[261] = 1;
        {
            DynamicCompressionModel_DW->Delay2_DSTATE_n = rtb_y_kp;
            break;
        }
    case 2:
        BitmapCov::branchBitmap[262] = 1;
        {
            DynamicCompressionModel_DW->Delay2_DSTATE_n = rtb_y_jv;
            break;
        }
    case 3:
        BitmapCov::branchBitmap[263] = 1;
        {
            DynamicCompressionModel_DW->Delay2_DSTATE_n = rtb_y_j5;
            break;
        }
    case 4:
        BitmapCov::branchBitmap[264] = 1;
        {
            DynamicCompressionModel_DW->Delay2_DSTATE_n = rtb_y_c;
            break;
        }
    case 5:
        BitmapCov::branchBitmap[265] = 1;
        {
            DynamicCompressionModel_DW->Delay2_DSTATE_n = rtb_y_ov;
            break;
        }
    case 6:
        BitmapCov::branchBitmap[266] = 1;
        {
            DynamicCompressionModel_DW->Delay2_DSTATE_n = rtb_y_iu;
            break;
        }
    case 7:
        BitmapCov::branchBitmap[267] = 1;
        {
            DynamicCompressionModel_DW->Delay2_DSTATE_n = rtb_y_jg;
            break;
        }
    case 8:
        BitmapCov::branchBitmap[268] = 1;
        {
            DynamicCompressionModel_DW->Delay2_DSTATE_n = rtb_y_f3;
            break;
        }
    case 9:
        BitmapCov::branchBitmap[269] = 1;
        {
            DynamicCompressionModel_DW->Delay2_DSTATE_n = rtb_y_hv;
            break;
        }
    case 10:
        BitmapCov::branchBitmap[270] = 1;
        {
            DynamicCompressionModel_DW->Delay2_DSTATE_n = rtb_y_j3;
            break;
        }
    case 11:
        BitmapCov::branchBitmap[271] = 1;
        {
            DynamicCompressionModel_DW->Delay2_DSTATE_n = rtb_y_o5;
            break;
        }
    case 12:
        BitmapCov::branchBitmap[272] = 1;
        {
            DynamicCompressionModel_DW->Delay2_DSTATE_n = rtb_y_o3;
            break;
        }
    case 13:
        BitmapCov::branchBitmap[273] = 1;
        {
            DynamicCompressionModel_DW->Delay2_DSTATE_n = rtb_y_lv;
            break;
        }
    case 14:
        BitmapCov::branchBitmap[274] = 1;
        {
            DynamicCompressionModel_DW->Delay2_DSTATE_n = rtb_y_jf;
            break;
        }
    default:
        BitmapCov::branchBitmap[275] = 1;
        {
            DynamicCompressionModel_DW->Delay2_DSTATE_n = rtb_y_ik;
            break;
        }
    }
    TCGHybrid::updateBranchDistanceValue("rtb_Subtract_j", rtb_Subtract_j);
    if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(rtb_Subtract_j > 15, 62, 1, 31), -63, 0, 31)) {
        BitmapCov::branchBitmap[276] = 1;
        DynamicCompressionModel_DW->UnitDelay3_DSTATE_d = 0U;
    }
    else {
        BitmapCov::branchBitmap[277] = 1;
        DynamicCompressionModel_DW->UnitDelay3_DSTATE_d = (uint16_T)((uint8_T)((uint8_T)~rtb_ExtractDesiredBits_dd & 15) << 5 | (uint8_T)((uint8_T)(DynamicCompressionModel_DW->Delay2_DSTATE_n >> 23) & 31));
    }
    DynamicCompressionModel_DW->ram_data_DSTATE_e5 = rtb_ram_mw[(uint16_T)((DynamicCompressionModel_DW->UnitDelay3_DSTATE_d + 1U) & 511U)];
    DynamicCompressionModel_DW->ram_data_DSTATE_c = rtb_ram_m[DynamicCompressionModel_DW->UnitDelay3_DSTATE_d];
    TCGHybrid::updateBranchDistanceValue("rtb_ExtractDesiredBits_oj", rtb_ExtractDesiredBits_oj);
    if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(rtb_ExtractDesiredBits_oj <= 4, 64, 1, 32), -65, 0, 32)) {
        BitmapCov::branchBitmap[278] = 1;
        rtb_Switch_eo = 0;
    }
    BitmapCov::branchBitmap[279] = 1;
    acc1 = (int32_T)((DynamicCompressionModel_U_In3 * 5LL) >> 27);
    cff = 1;
    for (j = DynamicCompressionModel_DW->DiscreteFIRFilter1_circBuf;BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(j < 576, 66, 1, 33), -67, 0, 33);j++) {
        BitmapCov::branchBitmap[280] = 1;
        acc1 += (int32_T)(((int64_T)DynamicCompressionModel_DW->DiscreteFIRFilter1_states[j] * DynamicCompressionModel_ConstP.DiscreteFIRFilter1_Coefficients[cff]) >> 28);
        cff++;
    }
    BitmapCov::branchBitmap[281] = 1;
    for (j = 0;BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(j < DynamicCompressionModel_DW->DiscreteFIRFilter1_circBuf, 68, 1, 34), -69, 0, 34);j++) {
        BitmapCov::branchBitmap[282] = 1;
        acc1 += (int32_T)(((int64_T)DynamicCompressionModel_DW->DiscreteFIRFilter1_states[j] * DynamicCompressionModel_ConstP.DiscreteFIRFilter1_Coefficients[cff]) >> 28);
        cff++;
    }
    BitmapCov::branchBitmap[283] = 1;
    acc1_0 = (int32_T)((DynamicCompressionModel_U_In3 * -319LL) >> 28);
    cff = 1;
    for (j = DynamicCompressionModel_DW->DiscreteFIRFilter3_circBuf;BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(j < 352, 70, 1, 35), -71, 0, 35);j++) {
        BitmapCov::branchBitmap[284] = 1;
        acc1_0 += (int32_T)(((int64_T)DynamicCompressionModel_DW->DiscreteFIRFilter3_states[j] * DynamicCompressionModel_ConstP.DiscreteFIRFilter3_Coefficients[cff]) >> 28);
        cff++;
    }
    BitmapCov::branchBitmap[285] = 1;
    for (j = 0;BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(j < DynamicCompressionModel_DW->DiscreteFIRFilter3_circBuf, 72, 1, 36), -73, 0, 36);j++) {
        BitmapCov::branchBitmap[286] = 1;
        acc1_0 += (int32_T)(((int64_T)DynamicCompressionModel_DW->DiscreteFIRFilter3_states[j] * DynamicCompressionModel_ConstP.DiscreteFIRFilter3_Coefficients[cff]) >> 28);
        cff++;
    }
    BitmapCov::branchBitmap[287] = 1;
    acc1_1 = (int32_T)((DynamicCompressionModel_U_In3 * -13LL) >> 27);
    cff = 1;
    for (j = DynamicCompressionModel_DW->DiscreteFIRFilter4_circBuf;BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(j < 254, 74, 1, 37), -75, 0, 37);j++) {
        BitmapCov::branchBitmap[288] = 1;
        acc1_1 += (int32_T)(((int64_T)DynamicCompressionModel_DW->DiscreteFIRFilter4_states[j] * DynamicCompressionModel_ConstP.DiscreteFIRFilter4_Coefficients[cff]) >> 28);
        cff++;
    }
    BitmapCov::branchBitmap[289] = 1;
    for (j = 0;BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(j < DynamicCompressionModel_DW->DiscreteFIRFilter4_circBuf, 76, 1, 38), -77, 0, 38);j++) {
        BitmapCov::branchBitmap[290] = 1;
        acc1_1 += (int32_T)(((int64_T)DynamicCompressionModel_DW->DiscreteFIRFilter4_states[j] * DynamicCompressionModel_ConstP.DiscreteFIRFilter4_Coefficients[cff]) >> 28);
        cff++;
    }
    BitmapCov::branchBitmap[291] = 1;
    acc1_2 = 0;
    cff = 1;
    for (j = DynamicCompressionModel_DW->DiscreteFIRFilter5_circBuf;BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(j < 230, 78, 1, 39), -79, 0, 39);j++) {
        BitmapCov::branchBitmap[292] = 1;
        acc1_2 += (int32_T)(((int64_T)DynamicCompressionModel_DW->DiscreteFIRFilter5_states[j] * DynamicCompressionModel_ConstP.DiscreteFIRFilter5_Coefficients[cff]) >> 28);
        cff++;
    }
    BitmapCov::branchBitmap[293] = 1;
    for (j = 0;BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(j < DynamicCompressionModel_DW->DiscreteFIRFilter5_circBuf, 80, 1, 40), -81, 0, 40);j++) {
        BitmapCov::branchBitmap[294] = 1;
        acc1_2 += (int32_T)(((int64_T)DynamicCompressionModel_DW->DiscreteFIRFilter5_states[j] * DynamicCompressionModel_ConstP.DiscreteFIRFilter5_Coefficients[cff]) >> 28);
        cff++;
    }
    BitmapCov::branchBitmap[295] = 1;
    rtb_y_n = DynamicCompressionM_BitRotate_g(1073741824U);
    rtb_y_jd = DynamicCompression_BitRotate1_a(1073741824U);
    rtb_y_g = DynamicCompression_BitRotate2_a(1073741824U);
    rtb_y_ihg = DynamicCompression_BitRotate3_d(1073741824U);
    rtb_y_bn = DynamicCompressionMo_BitRotate1(1073741824U);
    rtb_y_d = DynamicCompression_BitRotate5_p(1073741824U);
    rtb_y_mb = DynamicCompression_BitRotate6_b(1073741824U);
    rtb_y_b3 = DynamicCompression_BitRotate7_h(1073741824U);
    rtb_y_ck = DynamicCompression_BitRotate8_o(1073741824U);
    rtb_y = DynamicCompression_BitRotate9_g(1073741824U);
    rtb_y_b = DynamicCompressio_BitRotate10_p(1073741824U);
    rtb_y_i = DynamicCompressio_BitRotate11_l(1073741824U);
    rtb_y_et = DynamicCompressio_BitRotate12_m(1073741824U);
    rtb_y_nd = DynamicCompressio_BitRotate13_a(1073741824U);
    rtb_y_l = DynamicCompressio_BitRotate14_o(1073741824U);
    switch(rtb_NShifts)
    {
    case 0:
        BitmapCov::branchBitmap[296] = 1;
        {
            rtb_MultiportSwitch_n = 1073741824U;
            break;
        }
    case 1:
        BitmapCov::branchBitmap[297] = 1;
        {
            rtb_MultiportSwitch_n = rtb_y_n;
            break;
        }
    case 2:
        BitmapCov::branchBitmap[298] = 1;
        {
            rtb_MultiportSwitch_n = rtb_y_jd;
            break;
        }
    case 3:
        BitmapCov::branchBitmap[299] = 1;
        {
            rtb_MultiportSwitch_n = rtb_y_g;
            break;
        }
    case 4:
        BitmapCov::branchBitmap[300] = 1;
        {
            rtb_MultiportSwitch_n = rtb_y_ihg;
            break;
        }
    case 5:
        BitmapCov::branchBitmap[301] = 1;
        {
            rtb_MultiportSwitch_n = rtb_y_bn;
            break;
        }
    case 6:
        BitmapCov::branchBitmap[302] = 1;
        {
            rtb_MultiportSwitch_n = rtb_y_d;
            break;
        }
    case 7:
        BitmapCov::branchBitmap[303] = 1;
        {
            rtb_MultiportSwitch_n = rtb_y_mb;
            break;
        }
    case 8:
        BitmapCov::branchBitmap[304] = 1;
        {
            rtb_MultiportSwitch_n = rtb_y_b3;
            break;
        }
    case 9:
        BitmapCov::branchBitmap[305] = 1;
        {
            rtb_MultiportSwitch_n = rtb_y_ck;
            break;
        }
    case 10:
        BitmapCov::branchBitmap[306] = 1;
        {
            rtb_MultiportSwitch_n = rtb_y;
            break;
        }
    case 11:
        BitmapCov::branchBitmap[307] = 1;
        {
            rtb_MultiportSwitch_n = rtb_y_b;
            break;
        }
    case 12:
        BitmapCov::branchBitmap[308] = 1;
        {
            rtb_MultiportSwitch_n = rtb_y_i;
            break;
        }
    case 13:
        BitmapCov::branchBitmap[309] = 1;
        {
            rtb_MultiportSwitch_n = rtb_y_et;
            break;
        }
    case 14:
        BitmapCov::branchBitmap[310] = 1;
        {
            rtb_MultiportSwitch_n = rtb_y_nd;
            break;
        }
    default:
        BitmapCov::branchBitmap[311] = 1;
        {
            rtb_MultiportSwitch_n = rtb_y_l;
            break;
        }
    }
    rtb_y = DynamicCompressionMo_BitRotate1(rtb_MultiportSwitch_n);
    *DynamicCompressionModel_Y_proc_sig = DynamicCompressionModel_DW->UnitDelay1_DSTATE_j - (int32_T)(((uint64_T)((uint8_T)((uint8_T)DynamicCompressionModel_DW->UnitDelay3_DSTATE & 31)) * rtb_y + rtb_MultiportSwitch_n) >> 2);
    rtb_y_n = DynamicCompressionMod_BitRotate(32U);
    rtb_y_jd = DynamicCompression_BitRotate1_l(32U);
    rtb_y_g = DynamicCompressionMo_BitRotate2(32U);
    rtb_y_ihg = DynamicCompressionMo_BitRotate3(32U);
    rtb_y_bn = DynamicCompressionMo_BitRotate4(32U);
    rtb_y_d = DynamicCompressionMo_BitRotate5(32U);
    rtb_y_mb = DynamicCompressionMo_BitRotate6(32U);
    rtb_y_b3 = DynamicCompressionMo_BitRotate7(32U);
    rtb_y_ck = DynamicCompressionMo_BitRotate8(32U);
    rtb_y = DynamicCompressionMo_BitRotate9(32U);
    rtb_y_b = DynamicCompressionM_BitRotate10(32U);
    rtb_y_i = DynamicCompressionM_BitRotate11(32U);
    rtb_y_et = DynamicCompressionM_BitRotate12(32U);
    rtb_y_nd = DynamicCompressionM_BitRotate13(32U);
    rtb_y_l = DynamicCompressionM_BitRotate14(32U);
    switch(rtb_NShifts)
    {
    case 0:
        BitmapCov::branchBitmap[312] = 1;
        {
            rtb_MultiportSwitch = 32U;
            break;
        }
    case 1:
        BitmapCov::branchBitmap[313] = 1;
        {
            rtb_MultiportSwitch = rtb_y_n;
            break;
        }
    case 2:
        BitmapCov::branchBitmap[314] = 1;
        {
            rtb_MultiportSwitch = rtb_y_jd;
            break;
        }
    case 3:
        BitmapCov::branchBitmap[315] = 1;
        {
            rtb_MultiportSwitch = rtb_y_g;
            break;
        }
    case 4:
        BitmapCov::branchBitmap[316] = 1;
        {
            rtb_MultiportSwitch = rtb_y_ihg;
            break;
        }
    case 5:
        BitmapCov::branchBitmap[317] = 1;
        {
            rtb_MultiportSwitch = rtb_y_bn;
            break;
        }
    case 6:
        BitmapCov::branchBitmap[318] = 1;
        {
            rtb_MultiportSwitch = rtb_y_d;
            break;
        }
    case 7:
        BitmapCov::branchBitmap[319] = 1;
        {
            rtb_MultiportSwitch = rtb_y_mb;
            break;
        }
    case 8:
        BitmapCov::branchBitmap[320] = 1;
        {
            rtb_MultiportSwitch = rtb_y_b3;
            break;
        }
    case 9:
        BitmapCov::branchBitmap[321] = 1;
        {
            rtb_MultiportSwitch = rtb_y_ck;
            break;
        }
    case 10:
        BitmapCov::branchBitmap[322] = 1;
        {
            rtb_MultiportSwitch = rtb_y;
            break;
        }
    case 11:
        BitmapCov::branchBitmap[323] = 1;
        {
            rtb_MultiportSwitch = rtb_y_b;
            break;
        }
    case 12:
        BitmapCov::branchBitmap[324] = 1;
        {
            rtb_MultiportSwitch = rtb_y_i;
            break;
        }
    case 13:
        BitmapCov::branchBitmap[325] = 1;
        {
            rtb_MultiportSwitch = rtb_y_et;
            break;
        }
    case 14:
        BitmapCov::branchBitmap[326] = 1;
        {
            rtb_MultiportSwitch = rtb_y_nd;
            break;
        }
    default:
        BitmapCov::branchBitmap[327] = 1;
        {
            rtb_MultiportSwitch = rtb_y_l;
            break;
        }
    }
    if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(*DynamicCompressionModel_Y_proc_sig < 0, 82, 1, 41), -83, 0, 41)) {
        BitmapCov::branchBitmap[328] = 1;
        rtb_Switch_g = DynamicCompressionModel_DW->ram_data_DSTATE_l;
    }
    else {
        BitmapCov::branchBitmap[329] = 1;
        tmp_2 = (uint64_T)((int64_T)DynamicCompressionModel_DW->ram_data_DSTATE_o - DynamicCompressionModel_DW->ram_data_DSTATE_l);
        tmp_3 = rtb_MultiportSwitch;
        ssuMultiWordMul(&tmp_2, 1, &tmp_3, 1, &tmp_7.chunks[0U], 2);
        tmp_2 = (uint64_T)*DynamicCompressionModel_Y_proc_sig;
        sMultiWordMul(&tmp_7.chunks[0U], 2, &tmp_2, 1, &tmp_6.chunks[0U], 2);
        sMultiWordShr(&tmp_6.chunks[0U], 2, 28U, &tmp_5.chunks[0U], 2);
        rtb_Switch_g = DynamicCompressionModel_DW->ram_data_DSTATE_l + (int32_T)MultiWord2sLong(&tmp_5.chunks[0U]);
    }
    rtb_y_o0u = DynamicCompressionMod_BitRotate(DynamicCompressionModel_DW->Delay2_DSTATE_nl);
    rtb_y_dt = DynamicCompression_BitRotate1_l(DynamicCompressionModel_DW->Delay2_DSTATE_nl);
    rtb_y_py = DynamicCompressionMo_BitRotate2(DynamicCompressionModel_DW->Delay2_DSTATE_nl);
    rtb_y_pyi = DynamicCompressionMo_BitRotate3(DynamicCompressionModel_DW->Delay2_DSTATE_nl);
    rtb_y_jm = DynamicCompressionMo_BitRotate4(DynamicCompressionModel_DW->Delay2_DSTATE_nl);
    rtb_y_kv = DynamicCompressionMo_BitRotate5(DynamicCompressionModel_DW->Delay2_DSTATE_nl);
    rtb_y_i1j = DynamicCompressionMo_BitRotate6(DynamicCompressionModel_DW->Delay2_DSTATE_nl);
    rtb_y_hk = DynamicCompressionMo_BitRotate7(DynamicCompressionModel_DW->Delay2_DSTATE_nl);
    rtb_y_a1y = DynamicCompressionMo_BitRotate8(DynamicCompressionModel_DW->Delay2_DSTATE_nl);
    rtb_y_om = DynamicCompressionMo_BitRotate9(DynamicCompressionModel_DW->Delay2_DSTATE_nl);
    rtb_y_im = DynamicCompressionM_BitRotate10(DynamicCompressionModel_DW->Delay2_DSTATE_nl);
    rtb_y_pbx = DynamicCompressionM_BitRotate11(DynamicCompressionModel_DW->Delay2_DSTATE_nl);
    rtb_y_lf = DynamicCompressionM_BitRotate12(DynamicCompressionModel_DW->Delay2_DSTATE_nl);
    rtb_y_lp = DynamicCompressionM_BitRotate13(DynamicCompressionModel_DW->Delay2_DSTATE_nl);
    rtb_y_dz = DynamicCompressionM_BitRotate14(DynamicCompressionModel_DW->Delay2_DSTATE_nl);
    switch(rtb_ExtractDesiredBits_l3)
    {
    case 0:
        BitmapCov::branchBitmap[330] = 1;
        {
            break;
        }
    case 1:
        BitmapCov::branchBitmap[331] = 1;
        {
            DynamicCompressionModel_DW->Delay2_DSTATE_nl = rtb_y_o0u;
            break;
        }
    case 2:
        BitmapCov::branchBitmap[332] = 1;
        {
            DynamicCompressionModel_DW->Delay2_DSTATE_nl = rtb_y_dt;
            break;
        }
    case 3:
        BitmapCov::branchBitmap[333] = 1;
        {
            DynamicCompressionModel_DW->Delay2_DSTATE_nl = rtb_y_py;
            break;
        }
    case 4:
        BitmapCov::branchBitmap[334] = 1;
        {
            DynamicCompressionModel_DW->Delay2_DSTATE_nl = rtb_y_pyi;
            break;
        }
    case 5:
        BitmapCov::branchBitmap[335] = 1;
        {
            DynamicCompressionModel_DW->Delay2_DSTATE_nl = rtb_y_jm;
            break;
        }
    case 6:
        BitmapCov::branchBitmap[336] = 1;
        {
            DynamicCompressionModel_DW->Delay2_DSTATE_nl = rtb_y_kv;
            break;
        }
    case 7:
        BitmapCov::branchBitmap[337] = 1;
        {
            DynamicCompressionModel_DW->Delay2_DSTATE_nl = rtb_y_i1j;
            break;
        }
    case 8:
        BitmapCov::branchBitmap[338] = 1;
        {
            DynamicCompressionModel_DW->Delay2_DSTATE_nl = rtb_y_hk;
            break;
        }
    case 9:
        BitmapCov::branchBitmap[339] = 1;
        {
            DynamicCompressionModel_DW->Delay2_DSTATE_nl = rtb_y_a1y;
            break;
        }
    case 10:
        BitmapCov::branchBitmap[340] = 1;
        {
            DynamicCompressionModel_DW->Delay2_DSTATE_nl = rtb_y_om;
            break;
        }
    case 11:
        BitmapCov::branchBitmap[341] = 1;
        {
            DynamicCompressionModel_DW->Delay2_DSTATE_nl = rtb_y_im;
            break;
        }
    case 12:
        BitmapCov::branchBitmap[342] = 1;
        {
            DynamicCompressionModel_DW->Delay2_DSTATE_nl = rtb_y_pbx;
            break;
        }
    case 13:
        BitmapCov::branchBitmap[343] = 1;
        {
            DynamicCompressionModel_DW->Delay2_DSTATE_nl = rtb_y_lf;
            break;
        }
    case 14:
        BitmapCov::branchBitmap[344] = 1;
        {
            DynamicCompressionModel_DW->Delay2_DSTATE_nl = rtb_y_lp;
            break;
        }
    default:
        BitmapCov::branchBitmap[345] = 1;
        {
            DynamicCompressionModel_DW->Delay2_DSTATE_nl = rtb_y_dz;
            break;
        }
    }
    TCGHybrid::updateBranchDistanceValue("rtb_Subtract_e", rtb_Subtract_e);
    if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(rtb_Subtract_e > 15, 84, 1, 42), -85, 0, 42)) {
        BitmapCov::branchBitmap[346] = 1;
        DynamicCompressionModel_DW->UnitDelay3_DSTATE = 0U;
    }
    else {
        BitmapCov::branchBitmap[347] = 1;
        DynamicCompressionModel_DW->UnitDelay3_DSTATE = (uint16_T)((uint8_T)((uint8_T)~rtb_ExtractDesiredBits_l3 & 15) << 5 | (uint8_T)((uint8_T)(DynamicCompressionModel_DW->Delay2_DSTATE_nl >> 23) & 31));
    }
    DynamicCompressionModel_DW->ram_data_DSTATE_o = rtb_ram[(uint16_T)((DynamicCompressionModel_DW->UnitDelay3_DSTATE + 1U) & 511U)];
    DynamicCompressionModel_DW->ram_data_DSTATE_l = rtb_ram_f2[DynamicCompressionModel_DW->UnitDelay3_DSTATE];
    TCGHybrid::updateBranchDistanceValue("rtb_ExtractDesiredBits_oj", rtb_ExtractDesiredBits_oj);
    if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(rtb_ExtractDesiredBits_oj > 4, 86, 1, 43), -87, 0, 43)) {
        BitmapCov::branchBitmap[348] = 1;
        rtb_Switch_c = DynamicCompressionModel_DW->dly_shift_1_DSTATE[0];
    }
    else {
        BitmapCov::branchBitmap[349] = 1;
        rtb_Switch_c = 0;
    }
    TCGHybrid::updateBranchDistanceValue("DynamicCompressionModel_DW->UnitDelay_DSTATE - DynamicCompressionModel_DW->UnitDelay_DSTATE_h", DynamicCompressionModel_DW->UnitDelay_DSTATE - DynamicCompressionModel_DW->UnitDelay_DSTATE_h);
    if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(DynamicCompressionModel_DW->UnitDelay_DSTATE - DynamicCompressionModel_DW->UnitDelay_DSTATE_h >= 0, 88, 1, 44), -89, 0, 44)) {
        BitmapCov::branchBitmap[350] = 1;
        sLong2MultiWord(mul_s64_loSR(9005039113524357LL, (int64_T)DynamicCompressionModel_DW->UnitDelay_DSTATE - DynamicCompressionModel_DW->UnitDelay_DSTATE_h, 21ULL), &tmp_a.chunks[0U], 2);
        MultiWordSignedWrap(&tmp_a.chunks[0U], 2, 63U, &tmp_9.chunks[0U]);
        sLong2MultiWord((int64_T)DynamicCompressionModel_DW->UnitDelay_DSTATE_h << 32, &tmp_b.chunks[0U], 2);
        MultiWordSignedWrap(&tmp_b.chunks[0U], 2, 63U, &tmp_a.chunks[0U]);
        MultiWordAdd(&tmp_9.chunks[0U], &tmp_a.chunks[0U], &tmp_8.chunks[0U], 2);
        MultiWordSignedWrap(&tmp_8.chunks[0U], 2, 63U, &tmp_7.chunks[0U]);
        sMultiWordShr(&tmp_7.chunks[0U], 2, 32U, &tmp_6.chunks[0U], 2);
        DynamicCompressionModel_B->Subtract1_ku = (int32_T)MultiWord2sLong(&tmp_6.chunks[0U]);
        DynamicCompressionModel_DW->UnitDelay_DSTATE = DynamicCompressionModel_B->Subtract1_ku;
    }
    else {
        BitmapCov::branchBitmap[351] = 1;
        acc1_3 = mul_s64_loSR((int64_T)DynamicCompressionModel_DW->UnitDelay_DSTATE_h - DynamicCompressionModel_DW->UnitDelay_DSTATE, 9006983217303385LL, 53ULL);
        if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(((uint64_T)acc1_3 & 4294967296ULL) != 0ULL, 90, 1, 45), -91, 0, 45)) {
            BitmapCov::branchBitmap[352] = 1;
            acc1_3 |= -4294967296LL;
        }
        else {
            BitmapCov::branchBitmap[353] = 1;
            acc1_3 = (int64_T)((uint64_T)acc1_3 & 4294967295ULL);
        }
        DynamicCompressionModel_B->Subtract1_ax = (int32_T)(DynamicCompressionModel_DW->UnitDelay_DSTATE_h - acc1_3);
        DynamicCompressionModel_DW->UnitDelay_DSTATE = DynamicCompressionModel_B->Subtract1_ax;
    }
    TCGHybrid::updateBranchDistanceValue("DynamicCompressionModel_DW->UnitDelay_DSTATE_n - DynamicCompressionModel_DW->UnitDelay_DSTATE_g", DynamicCompressionModel_DW->UnitDelay_DSTATE_n - DynamicCompressionModel_DW->UnitDelay_DSTATE_g);
    if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(DynamicCompressionModel_DW->UnitDelay_DSTATE_n - DynamicCompressionModel_DW->UnitDelay_DSTATE_g >= 0, 92, 1, 46), -93, 0, 46)) {
        BitmapCov::branchBitmap[354] = 1;
        sLong2MultiWord(mul_s64_loSR(9002879490361109LL, (int64_T)DynamicCompressionModel_DW->UnitDelay_DSTATE_n - DynamicCompressionModel_DW->UnitDelay_DSTATE_g, 21ULL), &tmp_b.chunks[0U], 2);
        MultiWordSignedWrap(&tmp_b.chunks[0U], 2, 63U, &tmp_a.chunks[0U]);
        sLong2MultiWord((int64_T)DynamicCompressionModel_DW->UnitDelay_DSTATE_g << 32, &tmp_c.chunks[0U], 2);
        MultiWordSignedWrap(&tmp_c.chunks[0U], 2, 63U, &tmp_b.chunks[0U]);
        MultiWordAdd(&tmp_a.chunks[0U], &tmp_b.chunks[0U], &tmp_9.chunks[0U], 2);
        MultiWordSignedWrap(&tmp_9.chunks[0U], 2, 63U, &tmp_8.chunks[0U]);
        sMultiWordShr(&tmp_8.chunks[0U], 2, 32U, &tmp_7.chunks[0U], 2);
        DynamicCompressionModel_B->Subtract1_nh = (int32_T)MultiWord2sLong(&tmp_7.chunks[0U]);
        DynamicCompressionModel_DW->UnitDelay_DSTATE_n = DynamicCompressionModel_B->Subtract1_nh;
    }
    else {
        BitmapCov::branchBitmap[355] = 1;
        acc1_3 = mul_s64_loSR((int64_T)DynamicCompressionModel_DW->UnitDelay_DSTATE_g - DynamicCompressionModel_DW->UnitDelay_DSTATE_n, 9006911205975667LL, 53ULL);
        if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(((uint64_T)acc1_3 & 4294967296ULL) != 0ULL, 94, 1, 47), -95, 0, 47)) {
            BitmapCov::branchBitmap[356] = 1;
            acc1_3 |= -4294967296LL;
        }
        else {
            BitmapCov::branchBitmap[357] = 1;
            acc1_3 = (int64_T)((uint64_T)acc1_3 & 4294967295ULL);
        }
        DynamicCompressionModel_B->Subtract1_b = (int32_T)(DynamicCompressionModel_DW->UnitDelay_DSTATE_g - acc1_3);
        DynamicCompressionModel_DW->UnitDelay_DSTATE_n = DynamicCompressionModel_B->Subtract1_b;
    }
    TCGHybrid::updateBranchDistanceValue("DynamicCompressionModel_DW->UnitDelay_DSTATE_d - DynamicCompressionModel_DW->UnitDelay_DSTATE_gj", DynamicCompressionModel_DW->UnitDelay_DSTATE_d - DynamicCompressionModel_DW->UnitDelay_DSTATE_gj);
    if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(DynamicCompressionModel_DW->UnitDelay_DSTATE_d - DynamicCompressionModel_DW->UnitDelay_DSTATE_gj >= 0, 96, 1, 48), -97, 0, 48)) {
        BitmapCov::branchBitmap[358] = 1;
        sLong2MultiWord(mul_s64_loSR(9002879490361109LL, (int64_T)DynamicCompressionModel_DW->UnitDelay_DSTATE_d - DynamicCompressionModel_DW->UnitDelay_DSTATE_gj, 21ULL), &tmp_c.chunks[0U], 2);
        MultiWordSignedWrap(&tmp_c.chunks[0U], 2, 63U, &tmp_b.chunks[0U]);
        sLong2MultiWord((int64_T)DynamicCompressionModel_DW->UnitDelay_DSTATE_gj << 32, &tmp_d.chunks[0U], 2);
        MultiWordSignedWrap(&tmp_d.chunks[0U], 2, 63U, &tmp_c.chunks[0U]);
        MultiWordAdd(&tmp_b.chunks[0U], &tmp_c.chunks[0U], &tmp_a.chunks[0U], 2);
        MultiWordSignedWrap(&tmp_a.chunks[0U], 2, 63U, &tmp_9.chunks[0U]);
        sMultiWordShr(&tmp_9.chunks[0U], 2, 32U, &tmp_8.chunks[0U], 2);
        DynamicCompressionModel_B->Subtract1_d = (int32_T)MultiWord2sLong(&tmp_8.chunks[0U]);
        DynamicCompressionModel_DW->UnitDelay_DSTATE_d = DynamicCompressionModel_B->Subtract1_d;
    }
    else {
        BitmapCov::branchBitmap[359] = 1;
        acc1_3 = mul_s64_loSR((int64_T)DynamicCompressionModel_DW->UnitDelay_DSTATE_gj - DynamicCompressionModel_DW->UnitDelay_DSTATE_d, 9006839195223683LL, 53ULL);
        if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(((uint64_T)acc1_3 & 4294967296ULL) != 0ULL, 98, 1, 49), -99, 0, 49)) {
            BitmapCov::branchBitmap[360] = 1;
            acc1_3 |= -4294967296LL;
        }
        else {
            BitmapCov::branchBitmap[361] = 1;
            acc1_3 = (int64_T)((uint64_T)acc1_3 & 4294967295ULL);
        }
        DynamicCompressionModel_B->Subtract1_a = (int32_T)(DynamicCompressionModel_DW->UnitDelay_DSTATE_gj - acc1_3);
        DynamicCompressionModel_DW->UnitDelay_DSTATE_d = DynamicCompressionModel_B->Subtract1_a;
    }
    TCGHybrid::updateBranchDistanceValue("DynamicCompressionModel_DW->UnitDelay_DSTATE_a - DynamicCompressionModel_DW->UnitDelay_DSTATE_m", DynamicCompressionModel_DW->UnitDelay_DSTATE_a - DynamicCompressionModel_DW->UnitDelay_DSTATE_m);
    if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(DynamicCompressionModel_DW->UnitDelay_DSTATE_a - DynamicCompressionModel_DW->UnitDelay_DSTATE_m >= 0, 100, 1, 50), -101, 0, 50)) {
        BitmapCov::branchBitmap[362] = 1;
        sLong2MultiWord(mul_s64_loSR(9002879490361109LL, (int64_T)DynamicCompressionModel_DW->UnitDelay_DSTATE_a - DynamicCompressionModel_DW->UnitDelay_DSTATE_m, 21ULL), &tmp_d.chunks[0U], 2);
        MultiWordSignedWrap(&tmp_d.chunks[0U], 2, 63U, &tmp_c.chunks[0U]);
        sLong2MultiWord((int64_T)DynamicCompressionModel_DW->UnitDelay_DSTATE_m << 32, &tmp_e.chunks[0U], 2);
        MultiWordSignedWrap(&tmp_e.chunks[0U], 2, 63U, &tmp_d.chunks[0U]);
        MultiWordAdd(&tmp_c.chunks[0U], &tmp_d.chunks[0U], &tmp_b.chunks[0U], 2);
        MultiWordSignedWrap(&tmp_b.chunks[0U], 2, 63U, &tmp_a.chunks[0U]);
        sMultiWordShr(&tmp_a.chunks[0U], 2, 32U, &tmp_9.chunks[0U], 2);
        DynamicCompressionModel_B->Subtract1_k = (int32_T)MultiWord2sLong(&tmp_9.chunks[0U]);
        DynamicCompressionModel_DW->UnitDelay_DSTATE_a = DynamicCompressionModel_B->Subtract1_k;
    }
    else {
        BitmapCov::branchBitmap[363] = 1;
        acc1_3 = mul_s64_loSR((int64_T)DynamicCompressionModel_DW->UnitDelay_DSTATE_m - DynamicCompressionModel_DW->UnitDelay_DSTATE_a, 4503383592523715LL, 52ULL);
        if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(((uint64_T)acc1_3 & 4294967296ULL) != 0ULL, 102, 1, 51), -103, 0, 51)) {
            BitmapCov::branchBitmap[364] = 1;
            acc1_3 |= -4294967296LL;
        }
        else {
            BitmapCov::branchBitmap[365] = 1;
            acc1_3 = (int64_T)((uint64_T)acc1_3 & 4294967295ULL);
        }
        DynamicCompressionModel_B->Subtract1_i = (int32_T)(DynamicCompressionModel_DW->UnitDelay_DSTATE_m - acc1_3);
        DynamicCompressionModel_DW->UnitDelay_DSTATE_a = DynamicCompressionModel_B->Subtract1_i;
    }
    TCGHybrid::updateBranchDistanceValue("DynamicCompressionModel_DW->UnitDelay_DSTATE_gu - DynamicCompressionModel_DW->UnitDelay_DSTATE_m2", DynamicCompressionModel_DW->UnitDelay_DSTATE_gu - DynamicCompressionModel_DW->UnitDelay_DSTATE_m2);
    if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(DynamicCompressionModel_DW->UnitDelay_DSTATE_gu - DynamicCompressionModel_DW->UnitDelay_DSTATE_m2 >= 0, 104, 1, 52), -105, 0, 52)) {
        BitmapCov::branchBitmap[366] = 1;
        sLong2MultiWord(mul_s64_loSR(9002879490361109LL, (int64_T)DynamicCompressionModel_DW->UnitDelay_DSTATE_gu - DynamicCompressionModel_DW->UnitDelay_DSTATE_m2, 21ULL), &tmp_e.chunks[0U], 2);
        MultiWordSignedWrap(&tmp_e.chunks[0U], 2, 63U, &tmp_d.chunks[0U]);
        sLong2MultiWord((int64_T)DynamicCompressionModel_DW->UnitDelay_DSTATE_m2 << 32, &tmp_f.chunks[0U], 2);
        MultiWordSignedWrap(&tmp_f.chunks[0U], 2, 63U, &tmp_e.chunks[0U]);
        MultiWordAdd(&tmp_d.chunks[0U], &tmp_e.chunks[0U], &tmp_c.chunks[0U], 2);
        MultiWordSignedWrap(&tmp_c.chunks[0U], 2, 63U, &tmp_b.chunks[0U]);
        sMultiWordShr(&tmp_b.chunks[0U], 2, 32U, &tmp_a.chunks[0U], 2);
        DynamicCompressionModel_B->Subtract1_n = (int32_T)MultiWord2sLong(&tmp_a.chunks[0U]);
        DynamicCompressionModel_DW->UnitDelay_DSTATE_gu = DynamicCompressionModel_B->Subtract1_n;
    }
    else {
        BitmapCov::branchBitmap[367] = 1;
        acc1_3 = mul_s64_loSR((int64_T)DynamicCompressionModel_DW->UnitDelay_DSTATE_m2 - DynamicCompressionModel_DW->UnitDelay_DSTATE_gu, 4503383592523715LL, 52ULL);
        if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(((uint64_T)acc1_3 & 4294967296ULL) != 0ULL, 106, 1, 53), -107, 0, 53)) {
            BitmapCov::branchBitmap[368] = 1;
            acc1_3 |= -4294967296LL;
        }
        else {
            BitmapCov::branchBitmap[369] = 1;
            acc1_3 = (int64_T)((uint64_T)acc1_3 & 4294967295ULL);
        }
        DynamicCompressionModel_B->Subtract1 = (int32_T)(DynamicCompressionModel_DW->UnitDelay_DSTATE_m2 - acc1_3);
        DynamicCompressionModel_DW->UnitDelay_DSTATE_gu = DynamicCompressionModel_B->Subtract1;
    }
    TCGHybrid::updateBranchDistanceValue("rtb_ExtractDesiredBits_oj", rtb_ExtractDesiredBits_oj);
    if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(rtb_ExtractDesiredBits_oj > 4, 108, 1, 54), -109, 0, 54)) {
        BitmapCov::branchBitmap[370] = 1;
        Switch = ((((int32_T)(((int64_T)DynamicCompressionModel_DW->UnitDelay_DSTATE * DynamicCompressionModel_DW->Delay_DSTATE[0]) >> 28) + (int32_T)(((int64_T)DynamicCompressionModel_DW->UnitDelay_DSTATE_n * DynamicCompressionModel_DW->Delay_DSTATE_e[0]) >> 28)) + (int32_T)(((int64_T)DynamicCompressionModel_DW->UnitDelay_DSTATE_d * DynamicCompressionModel_DW->Delay_DSTATE_n[0]) >> 28)) + (int32_T)(((int64_T)DynamicCompressionModel_DW->UnitDelay_DSTATE_a * DynamicCompressionModel_DW->Delay_DSTATE_d[0]) >> 28)) + (int32_T)(((int64_T)DynamicCompressionModel_DW->UnitDelay_DSTATE_gu * DynamicCompressionModel_DW->Delay_DSTATE_p[0]) >> 28);
    }
    else {
        BitmapCov::branchBitmap[371] = 1;
        Switch = 0;
    }
    acc1_3 = (Switch * 22203LL) >> 24;
    cff = 1;
    for (j = DynamicCompressionModel_DW->ig65fir_circBuf;BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(j < 288, 110, 1, 55), -111, 0, 55);j++) {
        BitmapCov::branchBitmap[372] = 1;
        acc1_3 += mul_s64_loSR(DynamicCompressionModel_DW->ig65fir_states[j], DynamicCompressionModel_ConstP.ig65fir_Coefficients[cff], 28ULL);
        cff++;
    }
    BitmapCov::branchBitmap[373] = 1;
    for (j = 0;BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(j < DynamicCompressionModel_DW->ig65fir_circBuf, 112, 1, 56), -113, 0, 56);j++) {
        BitmapCov::branchBitmap[374] = 1;
        acc1_3 += mul_s64_loSR(DynamicCompressionModel_DW->ig65fir_states[j], DynamicCompressionModel_ConstP.ig65fir_Coefficients[cff], 28ULL);
        cff++;
    }
    BitmapCov::branchBitmap[375] = 1;
    *DynamicCompressionModel_Y_proc_sig = (int32_T)(acc1_3 >> 4);
    if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(*DynamicCompressionModel_Y_proc_sig >= 268435456, 114, 1, 57), -115, 0, 57)) {
        BitmapCov::branchBitmap[376] = 1;
        *DynamicCompressionModel_Y_proc_sig = 268435456;
    }
    BitmapCov::branchBitmap[377] = 1;
    if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(*DynamicCompressionModel_Y_proc_sig <= -268435456, 116, 1, 58), -117, 0, 58)) {
        BitmapCov::branchBitmap[378] = 1;
        *DynamicCompressionModel_Y_proc_sig = -268435456;
    }
    BitmapCov::branchBitmap[379] = 1;
    TCGHybrid::updateBranchDistanceValue("(uint16_T)(DynamicCompressionModel_DW->Count_reg_DSTATE + 1)", (uint16_T)(DynamicCompressionModel_DW->Count_reg_DSTATE + 1));
    if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord((uint16_T)(DynamicCompressionModel_DW->Count_reg_DSTATE + 1) > 4095, 118, 1, 59), -119, 0, 59)) {
        BitmapCov::branchBitmap[380] = 1;
        rtb_Switch_load = (uint16_T)((uint16_T)((int16_T)(DynamicCompressionModel_DW->Count_reg_DSTATE + 1U) - 4096) & 4095);
    }
    else {
        BitmapCov::branchBitmap[381] = 1;
        rtb_Switch_load = (uint16_T)(DynamicCompressionModel_DW->Count_reg_DSTATE + 1);
    }
    if (DynamicCompressionModel_U_In2) {
        BitmapCov::branchBitmap[382] = 1;
        if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(DynamicCompressionModel_DW->Count_reg_DSTATE == 2561, 120, 1, 60), -121, 0, 60)) {
            BitmapCov::branchBitmap[383] = 1;
            DynamicCompressionModel_DW->Count_reg_DSTATE = 2560U;
        }
        else {
            BitmapCov::branchBitmap[384] = 1;
            DynamicCompressionModel_DW->Count_reg_DSTATE = rtb_Switch_load;
        }
    }
    BitmapCov::branchBitmap[385] = 1;
    if (DynamicCompressionModel_U_Table_Values_e) {
        BitmapCov::branchBitmap[386] = 1;
        DynamicCompressionModel_DW->Count_reg_DSTATE = 2560U;
    }
    BitmapCov::branchBitmap[387] = 1;
    for (cff = 0;BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(cff < 25, 122, 1, 61), -123, 0, 61);cff++) {
        BitmapCov::branchBitmap[388] = 1;
        DynamicCompressionModel_DW->dly_shift_1_DSTATE[cff] = DynamicCompressionModel_DW->dly_shift_1_DSTATE[cff + 1];
    }
    BitmapCov::branchBitmap[389] = 1;
    DynamicCompressionModel_DW->dly_shift_1_DSTATE[25] = acc1;
    DynamicCompressionModel_DW->DiscreteFIRFilter2_circBuf--;
    TCGHybrid::updateBranchDistanceValue("DynamicCompressionModel_DW->DiscreteFIRFilter2_circBuf", DynamicCompressionModel_DW->DiscreteFIRFilter2_circBuf);
    TCGHybrid::updateBranchDistanceValue("DynamicCompressionModel_DW->DiscreteFIRFilter2_circBuf", DynamicCompressionModel_DW->DiscreteFIRFilter2_circBuf);
    if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(DynamicCompressionModel_DW->DiscreteFIRFilter2_circBuf < 0, 124, 1, 62), -125, 0, 62)) {
        BitmapCov::branchBitmap[390] = 1;
        DynamicCompressionModel_DW->DiscreteFIRFilter2_circBuf = 627;
        TCGHybrid::updateBranchDistanceValue("DynamicCompressionModel_DW->DiscreteFIRFilter2_circBuf", DynamicCompressionModel_DW->DiscreteFIRFilter2_circBuf);
    }
    BitmapCov::branchBitmap[391] = 1;
    DynamicCompressionModel_DW->DiscreteFIRFilter2_states[DynamicCompressionModel_DW->DiscreteFIRFilter2_circBuf] = DynamicCompressionModel_U_In3;
    for (cff = 0;BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(cff < 137, 126, 1, 63), -127, 0, 63);cff++) {
        BitmapCov::branchBitmap[392] = 1;
        DynamicCompressionModel_DW->dly_shift_3_DSTATE[cff] = DynamicCompressionModel_DW->dly_shift_3_DSTATE[cff + 1];
    }
    BitmapCov::branchBitmap[393] = 1;
    DynamicCompressionModel_DW->dly_shift_3_DSTATE[137] = acc1_0;
    for (cff = 0;BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(cff < 186, 128, 1, 64), -129, 0, 64);cff++) {
        BitmapCov::branchBitmap[394] = 1;
        DynamicCompressionModel_DW->dly_shift_4_DSTATE[cff] = DynamicCompressionModel_DW->dly_shift_4_DSTATE[cff + 1];
    }
    BitmapCov::branchBitmap[395] = 1;
    DynamicCompressionModel_DW->dly_shift_4_DSTATE[186] = acc1_1;
    for (cff = 0;BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(cff < 198, 130, 1, 65), -131, 0, 65);cff++) {
        BitmapCov::branchBitmap[396] = 1;
        DynamicCompressionModel_DW->dly_shift_5_DSTATE[cff] = DynamicCompressionModel_DW->dly_shift_5_DSTATE[cff + 1];
    }
    BitmapCov::branchBitmap[397] = 1;
    DynamicCompressionModel_DW->dly_shift_5_DSTATE[198] = acc1_2;
    DynamicCompressionModel_DW->Delay2_DSTATE = (uint32_T)rtb_X_in_e;
    DynamicCompressionModel_DW->UnitDelay3_DSTATE_o = rtb_Switch1_b2;
    DynamicCompressionModel_DW->UnitDelay1_DSTATE = DynamicCompressionModel_DW->UnitDelay2_DSTATE_e;
    DynamicCompressionModel_DW->ram_data_DSTATE = rtb_ram_j[rtb_Switch1_b2];
    DynamicCompressionModel_DW->ram_data_DSTATE_g = rtb_ram_jd[(uint16_T)((rtb_Switch1_b2 + 1U) & 511U)];
    DynamicCompressionModel_DW->UnitDelay1_DSTATE_a = DynamicCompressionModel_DW->UnitDelay2_DSTATE_m;
    DynamicCompressionModel_DW->Delay2_DSTATE_g = (uint32_T)rtb_X_in_l;
    DynamicCompressionModel_DW->UnitDelay1_DSTATE_i = DynamicCompressionModel_DW->UnitDelay2_DSTATE_hj;
    DynamicCompressionModel_DW->Delay2_DSTATE_a = (uint32_T)rtb_X_in_i;
    DynamicCompressionModel_DW->UnitDelay1_DSTATE_c = DynamicCompressionModel_DW->UnitDelay2_DSTATE_h;
    DynamicCompressionModel_DW->Delay2_DSTATE_n = (uint32_T)rtb_X_in_m;
    DynamicCompressionModel_DW->DiscreteFIRFilter1_circBuf--;
    TCGHybrid::updateBranchDistanceValue("DynamicCompressionModel_DW->DiscreteFIRFilter1_circBuf", DynamicCompressionModel_DW->DiscreteFIRFilter1_circBuf);
    TCGHybrid::updateBranchDistanceValue("DynamicCompressionModel_DW->DiscreteFIRFilter1_circBuf", DynamicCompressionModel_DW->DiscreteFIRFilter1_circBuf);
    if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(DynamicCompressionModel_DW->DiscreteFIRFilter1_circBuf < 0, 132, 1, 66), -133, 0, 66)) {
        BitmapCov::branchBitmap[398] = 1;
        DynamicCompressionModel_DW->DiscreteFIRFilter1_circBuf = 575;
        TCGHybrid::updateBranchDistanceValue("DynamicCompressionModel_DW->DiscreteFIRFilter1_circBuf", DynamicCompressionModel_DW->DiscreteFIRFilter1_circBuf);
    }
    BitmapCov::branchBitmap[399] = 1;
    DynamicCompressionModel_DW->DiscreteFIRFilter1_states[DynamicCompressionModel_DW->DiscreteFIRFilter1_circBuf] = DynamicCompressionModel_U_In3;
    DynamicCompressionModel_DW->DiscreteFIRFilter3_circBuf--;
    TCGHybrid::updateBranchDistanceValue("DynamicCompressionModel_DW->DiscreteFIRFilter3_circBuf", DynamicCompressionModel_DW->DiscreteFIRFilter3_circBuf);
    TCGHybrid::updateBranchDistanceValue("DynamicCompressionModel_DW->DiscreteFIRFilter3_circBuf", DynamicCompressionModel_DW->DiscreteFIRFilter3_circBuf);
    if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(DynamicCompressionModel_DW->DiscreteFIRFilter3_circBuf < 0, 134, 1, 67), -135, 0, 67)) {
        BitmapCov::branchBitmap[400] = 1;
        DynamicCompressionModel_DW->DiscreteFIRFilter3_circBuf = 351;
        TCGHybrid::updateBranchDistanceValue("DynamicCompressionModel_DW->DiscreteFIRFilter3_circBuf", DynamicCompressionModel_DW->DiscreteFIRFilter3_circBuf);
    }
    BitmapCov::branchBitmap[401] = 1;
    DynamicCompressionModel_DW->DiscreteFIRFilter3_states[DynamicCompressionModel_DW->DiscreteFIRFilter3_circBuf] = DynamicCompressionModel_U_In3;
    DynamicCompressionModel_DW->DiscreteFIRFilter4_circBuf--;
    TCGHybrid::updateBranchDistanceValue("DynamicCompressionModel_DW->DiscreteFIRFilter4_circBuf", DynamicCompressionModel_DW->DiscreteFIRFilter4_circBuf);
    TCGHybrid::updateBranchDistanceValue("DynamicCompressionModel_DW->DiscreteFIRFilter4_circBuf", DynamicCompressionModel_DW->DiscreteFIRFilter4_circBuf);
    if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(DynamicCompressionModel_DW->DiscreteFIRFilter4_circBuf < 0, 136, 1, 68), -137, 0, 68)) {
        BitmapCov::branchBitmap[402] = 1;
        DynamicCompressionModel_DW->DiscreteFIRFilter4_circBuf = 253;
        TCGHybrid::updateBranchDistanceValue("DynamicCompressionModel_DW->DiscreteFIRFilter4_circBuf", DynamicCompressionModel_DW->DiscreteFIRFilter4_circBuf);
    }
    BitmapCov::branchBitmap[403] = 1;
    DynamicCompressionModel_DW->DiscreteFIRFilter4_states[DynamicCompressionModel_DW->DiscreteFIRFilter4_circBuf] = DynamicCompressionModel_U_In3;
    DynamicCompressionModel_DW->DiscreteFIRFilter5_circBuf--;
    TCGHybrid::updateBranchDistanceValue("DynamicCompressionModel_DW->DiscreteFIRFilter5_circBuf", DynamicCompressionModel_DW->DiscreteFIRFilter5_circBuf);
    TCGHybrid::updateBranchDistanceValue("DynamicCompressionModel_DW->DiscreteFIRFilter5_circBuf", DynamicCompressionModel_DW->DiscreteFIRFilter5_circBuf);
    if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(DynamicCompressionModel_DW->DiscreteFIRFilter5_circBuf < 0, 138, 1, 69), -139, 0, 69)) {
        BitmapCov::branchBitmap[404] = 1;
        DynamicCompressionModel_DW->DiscreteFIRFilter5_circBuf = 229;
        TCGHybrid::updateBranchDistanceValue("DynamicCompressionModel_DW->DiscreteFIRFilter5_circBuf", DynamicCompressionModel_DW->DiscreteFIRFilter5_circBuf);
    }
    BitmapCov::branchBitmap[405] = 1;
    DynamicCompressionModel_DW->DiscreteFIRFilter5_states[DynamicCompressionModel_DW->DiscreteFIRFilter5_circBuf] = DynamicCompressionModel_U_In3;
    DynamicCompressionModel_DW->UnitDelay1_DSTATE_j = DynamicCompressionModel_DW->UnitDelay2_DSTATE;
    DynamicCompressionModel_DW->Delay2_DSTATE_nl = (uint32_T)i;
    DynamicCompressionModel_DW->UnitDelay_DSTATE_h = rtb_Switch_g;
    DynamicCompressionModel_DW->UnitDelay_DSTATE_g = rtb_Switch_mc;
    DynamicCompressionModel_DW->UnitDelay_DSTATE_gj = rtb_Switch_mv;
    DynamicCompressionModel_DW->UnitDelay_DSTATE_m = rtb_Switch_d3;
    DynamicCompressionModel_DW->UnitDelay_DSTATE_m2 = rtb_Switch;
    DynamicCompressionModel_DW->Delay_DSTATE[0] = DynamicCompressionModel_DW->Delay_DSTATE[1];
    DynamicCompressionModel_DW->Delay_DSTATE_e[0] = DynamicCompressionModel_DW->Delay_DSTATE_e[1];
    DynamicCompressionModel_DW->Delay_DSTATE_n[0] = DynamicCompressionModel_DW->Delay_DSTATE_n[1];
    DynamicCompressionModel_DW->Delay_DSTATE_d[0] = DynamicCompressionModel_DW->Delay_DSTATE_d[1];
    DynamicCompressionModel_DW->Delay_DSTATE_p[0] = DynamicCompressionModel_DW->Delay_DSTATE_p[1];
    DynamicCompressionModel_DW->Delay_DSTATE[1] = DynamicCompressionModel_DW->Delay_DSTATE[2];
    DynamicCompressionModel_DW->Delay_DSTATE_e[1] = DynamicCompressionModel_DW->Delay_DSTATE_e[2];
    DynamicCompressionModel_DW->Delay_DSTATE_n[1] = DynamicCompressionModel_DW->Delay_DSTATE_n[2];
    DynamicCompressionModel_DW->Delay_DSTATE_d[1] = DynamicCompressionModel_DW->Delay_DSTATE_d[2];
    DynamicCompressionModel_DW->Delay_DSTATE_p[1] = DynamicCompressionModel_DW->Delay_DSTATE_p[2];
    DynamicCompressionModel_DW->Delay_DSTATE[2] = rtb_Switch_c;
    DynamicCompressionModel_DW->Delay_DSTATE_e[2] = rtb_Switch_eo;
    DynamicCompressionModel_DW->Delay_DSTATE_n[2] = rtb_Switch_m;
    DynamicCompressionModel_DW->Delay_DSTATE_d[2] = rtb_Switch_kd;
    DynamicCompressionModel_DW->Delay_DSTATE_p[2] = rtb_Switch_nj;
    DynamicCompressionModel_DW->ig65fir_circBuf--;
    TCGHybrid::updateBranchDistanceValue("DynamicCompressionModel_DW->ig65fir_circBuf", DynamicCompressionModel_DW->ig65fir_circBuf);
    TCGHybrid::updateBranchDistanceValue("DynamicCompressionModel_DW->ig65fir_circBuf", DynamicCompressionModel_DW->ig65fir_circBuf);
    if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(DynamicCompressionModel_DW->ig65fir_circBuf < 0, 140, 1, 70), -141, 0, 70)) {
        BitmapCov::branchBitmap[406] = 1;
        DynamicCompressionModel_DW->ig65fir_circBuf = 287;
        TCGHybrid::updateBranchDistanceValue("DynamicCompressionModel_DW->ig65fir_circBuf", DynamicCompressionModel_DW->ig65fir_circBuf);
    }
    BitmapCov::branchBitmap[407] = 1;
    DynamicCompressionModel_DW->ig65fir_states[DynamicCompressionModel_DW->ig65fir_circBuf] = Switch;
    DynamicCompressionModel_DW->UnitDelay2_DSTATE = i;
    DynamicCompressionModel_DW->UnitDelay2_DSTATE_h = rtb_X_in_m;
    DynamicCompressionModel_DW->UnitDelay2_DSTATE_hj = rtb_X_in_i;
    DynamicCompressionModel_DW->UnitDelay2_DSTATE_m = rtb_X_in_l;
    DynamicCompressionModel_DW->UnitDelay2_DSTATE_e = rtb_X_in_e;
}
void DynamicCompressionModel_init(RT_MODEL_DynamicCompressionMo_T* DynamicCompressionModel_M, int32_T* DynamicCompressionModel_U_Table_Values, boolean_T* DynamicCompressionModel_U_In2, int32_T* DynamicCompressionModel_U_In3, boolean_T* DynamicCompressionModel_U_Table_Values_e, int32_T* DynamicCompressionModel_Y_proc_sig) {
    BitmapCov::munitBitmap[1] = 1;
    (void)memset((void*)DynamicCompressionModel_M, 0, sizeof(RT_MODEL_DynamicCompressionMo_T));
    DynamicCompressionModel_M->blockIO = &DynamicCompressionModel_B_instance;
    DynamicCompressionModel_M->dwork = &DynamicCompressionModel_DW_instance;
    DynamicCompressionModel_M->prevZCSigState = &DynamicCompressionModel_PrevZCX_instance;
    struct DW_DynamicCompressionModel_T * DynamicCompressionModel_DW = DynamicCompressionModel_M->dwork;
    struct B_DynamicCompressionModel_T * DynamicCompressionModel_B = DynamicCompressionModel_M->blockIO;
    (void)memset(((void*)DynamicCompressionModel_B), 0, sizeof(B_DynamicCompressionModel_T));
    (void)memset((void*)DynamicCompressionModel_DW, 0, sizeof(DW_DynamicCompressionModel_T));
    *DynamicCompressionModel_U_Table_Values = 0;
    *DynamicCompressionModel_U_In2 = false;
    *DynamicCompressionModel_U_In3 = 0;
    *DynamicCompressionModel_U_Table_Values_e = false;
    *DynamicCompressionModel_Y_proc_sig = 0;
    int32_T i = {0};
    int32_T pooled19 = {0};
    DynamicCompressionModel_DW->Count_reg_DSTATE = 2560U;
    for (i = 0;BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(i < 512, 142, 1, 71), -143, 0, 71);i++) {
        BitmapCov::branchBitmap[408] = 1;
        pooled19 = DynamicCompressionModel_ConstP.pooled19[i];
        DynamicCompressionModel_DW->ram_DSTATE[i] = pooled19;
        DynamicCompressionModel_DW->ram_DSTATE_h[i] = pooled19;
        pooled19 = DynamicCompressionModel_ConstP.pooled20[i];
        DynamicCompressionModel_DW->ram_DSTATE_a[i] = pooled19;
        DynamicCompressionModel_DW->ram_DSTATE_n[i] = pooled19;
        pooled19 = DynamicCompressionModel_ConstP.pooled21[i];
        DynamicCompressionModel_DW->ram_DSTATE_o[i] = pooled19;
        DynamicCompressionModel_DW->ram_DSTATE_k[i] = pooled19;
        pooled19 = DynamicCompressionModel_ConstP.pooled22[i];
        DynamicCompressionModel_DW->ram_DSTATE_c[i] = pooled19;
        DynamicCompressionModel_DW->ram_DSTATE_c5[i] = pooled19;
        pooled19 = DynamicCompressionModel_ConstP.pooled23[i];
        DynamicCompressionModel_DW->ram_DSTATE_hz[i] = pooled19;
        DynamicCompressionModel_DW->ram_DSTATE_hp[i] = pooled19;
    }
    BitmapCov::branchBitmap[409] = 1;
    DynamicCompressionModel_DW->UnitDelay3_DSTATE = 1U;
    DynamicCompressionModel_DW->DiscreteFIRFilter2_circBuf = 0;
    TCGHybrid::updateBranchDistanceValue("DynamicCompressionModel_DW->DiscreteFIRFilter2_circBuf", DynamicCompressionModel_DW->DiscreteFIRFilter2_circBuf);
    memset(&DynamicCompressionModel_DW->DiscreteFIRFilter2_states[0], 0, 628U * sizeof(int32_T));
    DynamicCompressionModel_DW->UnitDelay3_DSTATE_d = 1U;
    DynamicCompressionModel_DW->UnitDelay3_DSTATE_i = 1U;
    DynamicCompressionModel_DW->UnitDelay3_DSTATE_c = 1U;
    DynamicCompressionModel_DW->UnitDelay3_DSTATE_o = 1U;
    DynamicCompressionModel_DW->UnitDelay1_DSTATE = 268435456;
    DynamicCompressionModel_DW->UnitDelay1_DSTATE_a = 268435456;
    DynamicCompressionModel_DW->UnitDelay1_DSTATE_i = 268435456;
    DynamicCompressionModel_DW->UnitDelay1_DSTATE_c = 268435456;
    DynamicCompressionModel_DW->DiscreteFIRFilter1_circBuf = 0;
    TCGHybrid::updateBranchDistanceValue("DynamicCompressionModel_DW->DiscreteFIRFilter1_circBuf", DynamicCompressionModel_DW->DiscreteFIRFilter1_circBuf);
    memset(&DynamicCompressionModel_DW->DiscreteFIRFilter1_states[0], 0, 576U * sizeof(int32_T));
    DynamicCompressionModel_DW->DiscreteFIRFilter3_circBuf = 0;
    TCGHybrid::updateBranchDistanceValue("DynamicCompressionModel_DW->DiscreteFIRFilter3_circBuf", DynamicCompressionModel_DW->DiscreteFIRFilter3_circBuf);
    memset(&DynamicCompressionModel_DW->DiscreteFIRFilter3_states[0], 0, 352U * sizeof(int32_T));
    DynamicCompressionModel_DW->DiscreteFIRFilter4_circBuf = 0;
    TCGHybrid::updateBranchDistanceValue("DynamicCompressionModel_DW->DiscreteFIRFilter4_circBuf", DynamicCompressionModel_DW->DiscreteFIRFilter4_circBuf);
    memset(&DynamicCompressionModel_DW->DiscreteFIRFilter4_states[0], 0, 254U * sizeof(int32_T));
    DynamicCompressionModel_DW->DiscreteFIRFilter5_circBuf = 0;
    TCGHybrid::updateBranchDistanceValue("DynamicCompressionModel_DW->DiscreteFIRFilter5_circBuf", DynamicCompressionModel_DW->DiscreteFIRFilter5_circBuf);
    memset(&DynamicCompressionModel_DW->DiscreteFIRFilter5_states[0], 0, 230U * sizeof(int32_T));
    DynamicCompressionModel_DW->UnitDelay1_DSTATE_j = 268435456;
    DynamicCompressionModel_DW->UnitDelay_DSTATE = 268435456;
    DynamicCompressionModel_DW->UnitDelay_DSTATE_h = 268435456;
    DynamicCompressionModel_DW->UnitDelay_DSTATE_n = 268435456;
    DynamicCompressionModel_DW->UnitDelay_DSTATE_g = 268435456;
    DynamicCompressionModel_DW->UnitDelay_DSTATE_d = 268435456;
    DynamicCompressionModel_DW->UnitDelay_DSTATE_gj = 268435456;
    DynamicCompressionModel_DW->UnitDelay_DSTATE_a = 268435456;
    DynamicCompressionModel_DW->UnitDelay_DSTATE_m = 268435456;
    DynamicCompressionModel_DW->UnitDelay_DSTATE_gu = 268435456;
    DynamicCompressionModel_DW->UnitDelay_DSTATE_m2 = 268435456;
    DynamicCompressionModel_DW->ig65fir_circBuf = 0;
    TCGHybrid::updateBranchDistanceValue("DynamicCompressionModel_DW->ig65fir_circBuf", DynamicCompressionModel_DW->ig65fir_circBuf);
    memset(&DynamicCompressionModel_DW->ig65fir_states[0], 0, 288U * sizeof(int32_T));
    DynamicCompressionModel_DW->UnitDelay2_DSTATE = 268435456;
    DynamicCompressionModel_DW->UnitDelay2_DSTATE_h = 268435456;
    DynamicCompressionModel_DW->UnitDelay2_DSTATE_hj = 268435456;
    DynamicCompressionModel_DW->UnitDelay2_DSTATE_m = 268435456;
    DynamicCompressionModel_DW->UnitDelay2_DSTATE_e = 268435456;
    DynamicCompre_fxpt_lzc_W32_Init(&DynamicCompressionModel_DW->fxpt_lzc_W32);
    DynamicCompre_fxpt_lzc_W32_Init(&DynamicCompressionModel_DW->fxpt_lzc_W32_c);
    DynamicCompre_fxpt_lzc_W32_Init(&DynamicCompressionModel_DW->fxpt_lzc_W32_l);
    DynamicCompre_fxpt_lzc_W32_Init(&DynamicCompressionModel_DW->fxpt_lzc_W32_p);
    DynamicCompre_fxpt_lzc_W32_Init(&DynamicCompressionModel_DW->fxpt_lzc_W32_pp);
}
int64_T MultiWord2sLong(uint64_T u[]) {
    BitmapCov::munitBitmap[2] = 1;
    return (int64_T)u[0];
}
void sMultiWordShr(uint64_T u1[], int32_T n1, uint32_T n2, uint64_T y[], int32_T n) {
    BitmapCov::munitBitmap[3] = 1;
    uint64_T ys = {0};
    int32_T i = {0};
    int32_T i1 = {0};
    int32_T nb = {0};
    nb = (int32_T)(n2 >> 6);
    i = 0;
    unsigned char mcdc_68_c_1 = (u1[n1 - 1] & 9223372036854775808ULL) != 0ULL;
    if (mcdc_68_c_1) {
        BitmapCov::branchBitmap[410] = 1;
    }
    else {
        BitmapCov::branchBitmap[411] = 1;
    }
    ys = BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord((u1[n1 - 1] & 9223372036854775808ULL) != 0ULL, 144, 1, 72), -145, 0, 72) ? MAX_uint64_T : 0ULL;
    if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(nb < n1, 146, 1, 73), -147, 0, 73)) {
        BitmapCov::branchBitmap[412] = 1;
        int32_T nc = {0};
        uint32_T nr = {0};
        nc = n + nb;
        if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(nc > n1, 148, 1, 74), -149, 0, 74)) {
            BitmapCov::branchBitmap[413] = 1;
            nc = n1;
        }
        BitmapCov::branchBitmap[414] = 1;
        nr = n2 - ((uint32_T)nb << 6);
        if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(nr > 0U, 150, 1, 75), -151, 0, 75)) {
            BitmapCov::branchBitmap[415] = 1;
            uint64_T u1i = {0};
            u1i = u1[nb];
            for (i1 = nb + 1;BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(i1 < nc, 152, 1, 76), -153, 0, 76);i1++) {
                BitmapCov::branchBitmap[416] = 1;
                uint64_T yi = {0};
                yi = u1i >> nr;
                u1i = u1[i1];
                y[i] = u1i << (64U - nr) | yi;
                i++;
            }
            BitmapCov::branchBitmap[417] = 1;
            unsigned char mcdc_64_c_1 = nc < n1;
            if (mcdc_64_c_1) {
                BitmapCov::branchBitmap[418] = 1;
            }
            else {
                BitmapCov::branchBitmap[419] = 1;
            }
            y[i] = (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(nc < n1, 154, 1, 77), -155, 0, 77) ? u1[nc] : ys) << (64U - nr) | u1i >> nr;
            i++;
        }
        else {
            BitmapCov::branchBitmap[420] = 1;
            for (i1 = nb;BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(i1 < nc, 156, 1, 78), -157, 0, 78);i1++) {
                BitmapCov::branchBitmap[421] = 1;
                y[i] = u1[i1];
                i++;
            }
            BitmapCov::branchBitmap[422] = 1;
        }
    }
    BitmapCov::branchBitmap[423] = 1;
    while (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(i < n, 158, 1, 79), -159, 0, 79)) {
        BitmapCov::branchBitmap[424] = 1;
        y[i] = ys;
        i++;
    }
    BitmapCov::branchBitmap[425] = 1;
}
void sMultiWordMul(uint64_T u1[], int32_T n1, uint64_T u2[], int32_T n2, uint64_T y[], int32_T n) {
    BitmapCov::munitBitmap[4] = 1;
    uint64_T cb = {0};
    uint64_T cb1 = {0};
    uint64_T yk = {0};
    int32_T i = {0};
    int32_T j = {0};
    int32_T k = {0};
    boolean_T isNegative1 = {0};
    boolean_T isNegative2 = {0};
    unsigned char mcdc_63_c_1 = (u1[n1 - 1] & 9223372036854775808ULL) != 0ULL;
    if (mcdc_63_c_1) {
        BitmapCov::branchBitmap[426] = 1;
    }
    else {
        BitmapCov::branchBitmap[427] = 1;
    }
    isNegative1 = (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord((u1[n1 - 1] & 9223372036854775808ULL) != 0ULL, 160, 1, 80), -161, 0, 80));
    unsigned char mcdc_62_c_1 = (u2[n2 - 1] & 9223372036854775808ULL) != 0ULL;
    if (mcdc_62_c_1) {
        BitmapCov::branchBitmap[428] = 1;
    }
    else {
        BitmapCov::branchBitmap[429] = 1;
    }
    isNegative2 = (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord((u2[n2 - 1] & 9223372036854775808ULL) != 0ULL, 162, 1, 81), -163, 0, 81));
    cb1 = 1ULL;
    for (k = 0;BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(k < n, 164, 1, 82), -165, 0, 82);k++) {
        BitmapCov::branchBitmap[430] = 1;
        y[k] = 0ULL;
    }
    BitmapCov::branchBitmap[431] = 1;
    for (i = 0;BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(i < n1, 166, 1, 83), -167, 0, 83);i++) {
        BitmapCov::branchBitmap[432] = 1;
        uint64_T a0 = {0};
        uint64_T a1 = {0};
        uint64_T cb2 = {0};
        uint64_T u1i = {0};
        int32_T ni = {0};
        cb = 0ULL;
        u1i = u1[i];
        if (isNegative1) {
            BitmapCov::branchBitmap[433] = 1;
            u1i = ~u1i + cb1;
            unsigned char mcdc_61_c_1 = u1i < cb1;
            if (mcdc_61_c_1) {
                BitmapCov::branchBitmap[434] = 1;
            }
            else {
                BitmapCov::branchBitmap[435] = 1;
            }
            cb1 = (uint64_T)(BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(u1i < cb1, 168, 1, 84), -169, 0, 84));
        }
        BitmapCov::branchBitmap[436] = 1;
        a1 = u1i >> 32U;
        a0 = u1i & 4294967295ULL;
        cb2 = 1ULL;
        ni = n - i;
        unsigned char mcdc_60_c_1 = n2 <= ni;
        if (mcdc_60_c_1) {
            BitmapCov::branchBitmap[437] = 1;
        }
        else {
            BitmapCov::branchBitmap[438] = 1;
        }
        ni = BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(n2 <= ni, 170, 1, 85), -171, 0, 85) ? n2 : ni;
        k = i;
        for (j = 0;BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(j < ni, 172, 1, 86), -173, 0, 86);j++) {
            BitmapCov::branchBitmap[439] = 1;
            uint64_T b1 = {0};
            uint64_T w01 = {0};
            uint64_T w10 = {0};
            u1i = u2[j];
            if (isNegative2) {
                BitmapCov::branchBitmap[440] = 1;
                u1i = ~u1i + cb2;
                unsigned char mcdc_59_c_1 = u1i < cb2;
                if (mcdc_59_c_1) {
                    BitmapCov::branchBitmap[441] = 1;
                }
                else {
                    BitmapCov::branchBitmap[442] = 1;
                }
                cb2 = (uint64_T)(BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(u1i < cb2, 174, 1, 87), -175, 0, 87));
            }
            BitmapCov::branchBitmap[443] = 1;
            b1 = u1i >> 32U;
            u1i &= 4294967295ULL;
            w10 = a1 * u1i;
            w01 = a0 * b1;
            yk = y[k] + cb;
            unsigned char mcdc_58_c_1 = yk < cb;
            if (mcdc_58_c_1) {
                BitmapCov::branchBitmap[444] = 1;
            }
            else {
                BitmapCov::branchBitmap[445] = 1;
            }
            cb = (uint64_T)(BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(yk < cb, 176, 1, 88), -177, 0, 88));
            u1i *= a0;
            yk += u1i;
            unsigned char mcdc_57_c_1 = yk < u1i;
            if (mcdc_57_c_1) {
                BitmapCov::branchBitmap[446] = 1;
            }
            else {
                BitmapCov::branchBitmap[447] = 1;
            }
            cb += (uint64_T)(BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(yk < u1i, 178, 1, 89), -179, 0, 89));
            u1i = w10 << 32U;
            yk += u1i;
            unsigned char mcdc_56_c_1 = yk < u1i;
            if (mcdc_56_c_1) {
                BitmapCov::branchBitmap[448] = 1;
            }
            else {
                BitmapCov::branchBitmap[449] = 1;
            }
            cb += (uint64_T)(BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(yk < u1i, 180, 1, 90), -181, 0, 90));
            u1i = w01 << 32U;
            yk += u1i;
            unsigned char mcdc_55_c_1 = yk < u1i;
            if (mcdc_55_c_1) {
                BitmapCov::branchBitmap[450] = 1;
            }
            else {
                BitmapCov::branchBitmap[451] = 1;
            }
            cb += (uint64_T)(BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(yk < u1i, 182, 1, 91), -183, 0, 91));
            y[k] = yk;
            cb += w10 >> 32U;
            cb += w01 >> 32U;
            cb += a1 * b1;
            k++;
        }
        BitmapCov::branchBitmap[452] = 1;
        if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(k < n, 184, 1, 92), -185, 0, 92)) {
            BitmapCov::branchBitmap[453] = 1;
            y[k] = cb;
        }
        BitmapCov::branchBitmap[454] = 1;
    }
    BitmapCov::branchBitmap[455] = 1;
    if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(isNegative1 != isNegative2, 186, 1, 93), -187, 0, 93)) {
        BitmapCov::branchBitmap[456] = 1;
        cb = 1ULL;
        for (k = 0;BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(k < n, 188, 1, 94), -189, 0, 94);k++) {
            BitmapCov::branchBitmap[457] = 1;
            yk = ~y[k] + cb;
            y[k] = yk;
            unsigned char mcdc_52_c_1 = yk < cb;
            if (mcdc_52_c_1) {
                BitmapCov::branchBitmap[458] = 1;
            }
            else {
                BitmapCov::branchBitmap[459] = 1;
            }
            cb = (uint64_T)(BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(yk < cb, 190, 1, 95), -191, 0, 95));
        }
        BitmapCov::branchBitmap[460] = 1;
    }
    BitmapCov::branchBitmap[461] = 1;
}
void ssuMultiWordMul(uint64_T u1[], int32_T n1, uint64_T u2[], int32_T n2, uint64_T y[], int32_T n) {
    BitmapCov::munitBitmap[5] = 1;
    uint64_T cb = {0};
    uint64_T cb1 = {0};
    uint64_T yk = {0};
    int32_T i = {0};
    int32_T j = {0};
    int32_T k = {0};
    boolean_T isNegative1 = {0};
    unsigned char mcdc_51_c_1 = (u1[n1 - 1] & 9223372036854775808ULL) != 0ULL;
    if (mcdc_51_c_1) {
        BitmapCov::branchBitmap[462] = 1;
    }
    else {
        BitmapCov::branchBitmap[463] = 1;
    }
    isNegative1 = (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord((u1[n1 - 1] & 9223372036854775808ULL) != 0ULL, 192, 1, 96), -193, 0, 96));
    cb1 = 1ULL;
    for (k = 0;BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(k < n, 194, 1, 97), -195, 0, 97);k++) {
        BitmapCov::branchBitmap[464] = 1;
        y[k] = 0ULL;
    }
    BitmapCov::branchBitmap[465] = 1;
    for (i = 0;BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(i < n1, 196, 1, 98), -197, 0, 98);i++) {
        BitmapCov::branchBitmap[466] = 1;
        uint64_T a0 = {0};
        uint64_T a1 = {0};
        uint64_T u1i = {0};
        int32_T ni = {0};
        cb = 0ULL;
        u1i = u1[i];
        if (isNegative1) {
            BitmapCov::branchBitmap[467] = 1;
            u1i = ~u1i + cb1;
            unsigned char mcdc_50_c_1 = u1i < cb1;
            if (mcdc_50_c_1) {
                BitmapCov::branchBitmap[468] = 1;
            }
            else {
                BitmapCov::branchBitmap[469] = 1;
            }
            cb1 = (uint64_T)(BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(u1i < cb1, 198, 1, 99), -199, 0, 99));
        }
        BitmapCov::branchBitmap[470] = 1;
        a1 = u1i >> 32U;
        a0 = u1i & 4294967295ULL;
        ni = n - i;
        unsigned char mcdc_49_c_1 = n2 <= ni;
        if (mcdc_49_c_1) {
            BitmapCov::branchBitmap[471] = 1;
        }
        else {
            BitmapCov::branchBitmap[472] = 1;
        }
        ni = BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(n2 <= ni, 200, 1, 100), -201, 0, 100) ? n2 : ni;
        k = i;
        for (j = 0;BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(j < ni, 202, 1, 101), -203, 0, 101);j++) {
            BitmapCov::branchBitmap[473] = 1;
            uint64_T b1 = {0};
            uint64_T w01 = {0};
            uint64_T w10 = {0};
            u1i = u2[j];
            b1 = u1i >> 32U;
            u1i &= 4294967295ULL;
            w10 = a1 * u1i;
            w01 = a0 * b1;
            yk = y[k] + cb;
            unsigned char mcdc_48_c_1 = yk < cb;
            if (mcdc_48_c_1) {
                BitmapCov::branchBitmap[474] = 1;
            }
            else {
                BitmapCov::branchBitmap[475] = 1;
            }
            cb = (uint64_T)(BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(yk < cb, 204, 1, 102), -205, 0, 102));
            u1i *= a0;
            yk += u1i;
            unsigned char mcdc_47_c_1 = yk < u1i;
            if (mcdc_47_c_1) {
                BitmapCov::branchBitmap[476] = 1;
            }
            else {
                BitmapCov::branchBitmap[477] = 1;
            }
            cb += (uint64_T)(BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(yk < u1i, 206, 1, 103), -207, 0, 103));
            u1i = w10 << 32U;
            yk += u1i;
            unsigned char mcdc_46_c_1 = yk < u1i;
            if (mcdc_46_c_1) {
                BitmapCov::branchBitmap[478] = 1;
            }
            else {
                BitmapCov::branchBitmap[479] = 1;
            }
            cb += (uint64_T)(BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(yk < u1i, 208, 1, 104), -209, 0, 104));
            u1i = w01 << 32U;
            yk += u1i;
            unsigned char mcdc_45_c_1 = yk < u1i;
            if (mcdc_45_c_1) {
                BitmapCov::branchBitmap[480] = 1;
            }
            else {
                BitmapCov::branchBitmap[481] = 1;
            }
            cb += (uint64_T)(BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(yk < u1i, 210, 1, 105), -211, 0, 105));
            y[k] = yk;
            cb += w10 >> 32U;
            cb += w01 >> 32U;
            cb += a1 * b1;
            k++;
        }
        BitmapCov::branchBitmap[482] = 1;
        if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(k < n, 212, 1, 106), -213, 0, 106)) {
            BitmapCov::branchBitmap[483] = 1;
            y[k] = cb;
        }
        BitmapCov::branchBitmap[484] = 1;
    }
    BitmapCov::branchBitmap[485] = 1;
    if (isNegative1) {
        BitmapCov::branchBitmap[486] = 1;
        cb = 1ULL;
        for (k = 0;BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(k < n, 214, 1, 107), -215, 0, 107);k++) {
            BitmapCov::branchBitmap[487] = 1;
            yk = ~y[k] + cb;
            y[k] = yk;
            unsigned char mcdc_43_c_1 = yk < cb;
            if (mcdc_43_c_1) {
                BitmapCov::branchBitmap[488] = 1;
            }
            else {
                BitmapCov::branchBitmap[489] = 1;
            }
            cb = (uint64_T)(BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(yk < cb, 216, 1, 108), -217, 0, 108));
        }
        BitmapCov::branchBitmap[490] = 1;
    }
    BitmapCov::branchBitmap[491] = 1;
}
void MultiWordSignedWrap(uint64_T u1[], int32_T n1, uint32_T n2, uint64_T y[]) {
    BitmapCov::munitBitmap[6] = 1;
    int32_T i = {0};
    int32_T n1m1 = {0};
    n1m1 = n1 - 1;
    for (i = 0;BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(i < n1m1, 218, 1, 109), -219, 0, 109);i++) {
        BitmapCov::branchBitmap[492] = 1;
        y[i] = u1[i];
    }
    BitmapCov::branchBitmap[493] = 1;
    uint64_T mask = {0};
    uint64_T ys = {0};
    mask = 1ULL << (63U - n2);
    unsigned char mcdc_42_c_1 = (u1[n1 - 1] & mask) != 0ULL;
    if (mcdc_42_c_1) {
        BitmapCov::branchBitmap[494] = 1;
    }
    else {
        BitmapCov::branchBitmap[495] = 1;
    }
    ys = BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord((u1[n1 - 1] & mask) != 0ULL, 220, 1, 110), -221, 0, 110) ? MAX_uint64_T : 0ULL;
    mask = (mask << 1U) - 1ULL;
    y[n1 - 1] = (u1[n1 - 1] & mask) | (~mask & ys);
}
void MultiWordAdd(uint64_T u1[], uint64_T u2[], uint64_T y[], int32_T n) {
    BitmapCov::munitBitmap[7] = 1;
    uint64_T carry = 0ULL;
    int32_T i = {0};
    for (i = 0;BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(i < n, 222, 1, 111), -223, 0, 111);i++) {
        BitmapCov::branchBitmap[496] = 1;
        uint64_T u1i = {0};
        uint64_T yi = {0};
        u1i = u1[i];
        yi = (u1i + u2[i]) + carry;
        y[i] = yi;
        unsigned char mcdc_39_c_1 = carry != 0ULL;
        if (mcdc_39_c_1) {
            BitmapCov::branchBitmap[497] = 1;
        }
        else {
            BitmapCov::branchBitmap[498] = 1;
        }
        unsigned char mcdc_40_c_1 = yi <= u1i;
        if (mcdc_40_c_1) {
            BitmapCov::branchBitmap[499] = 1;
        }
        else {
            BitmapCov::branchBitmap[500] = 1;
        }
        unsigned char mcdc_41_c_1 = yi < u1i;
        if (mcdc_41_c_1) {
            BitmapCov::branchBitmap[501] = 1;
        }
        else {
            BitmapCov::branchBitmap[502] = 1;
        }
        carry = BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(carry != 0ULL, 224, 1, 112), -225, 0, 112) ? (uint64_T)(BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(yi <= u1i, 226, 1, 113), -227, 0, 113)) : (uint64_T)(BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(yi < u1i, 228, 1, 114), -229, 0, 114));
    }
    BitmapCov::branchBitmap[503] = 1;
}
void sLong2MultiWord(int64_T u, uint64_T y[], int32_T n) {
    BitmapCov::munitBitmap[8] = 1;
    uint64_T yi = {0};
    int32_T i = {0};
    y[0] = (uint64_T)u;
    unsigned char mcdc_38_c_1 = u < 0LL;
    if (mcdc_38_c_1) {
        BitmapCov::branchBitmap[504] = 1;
    }
    else {
        BitmapCov::branchBitmap[505] = 1;
    }
    yi = BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(u < 0LL, 230, 1, 115), -231, 0, 115) ? MAX_uint64_T : 0ULL;
    for (i = 1;BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(i < n, 232, 1, 116), -233, 0, 116);i++) {
        BitmapCov::branchBitmap[506] = 1;
        y[i] = yi;
    }
    BitmapCov::branchBitmap[507] = 1;
}
void mul_wide_s64(int64_T in0, int64_T in1, uint64_T* ptrOutBitsHi, uint64_T* ptrOutBitsLo) {
    BitmapCov::munitBitmap[9] = 1;
    uint64_T absIn0 = {0};
    uint64_T absIn1 = {0};
    uint64_T in0Hi = {0};
    uint64_T in0Lo = {0};
    uint64_T in1Hi = {0};
    uint64_T productHiLo = {0};
    uint64_T productLoHi = {0};
    unsigned char mcdc_37_c_1 = in0 < 0LL;
    if (mcdc_37_c_1) {
        BitmapCov::branchBitmap[508] = 1;
    }
    else {
        BitmapCov::branchBitmap[509] = 1;
    }
    absIn0 = BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(in0 < 0LL, 234, 1, 117), -235, 0, 117) ? ~(uint64_T)in0 + 1ULL : (uint64_T)in0;
    unsigned char mcdc_36_c_1 = in1 < 0LL;
    if (mcdc_36_c_1) {
        BitmapCov::branchBitmap[510] = 1;
    }
    else {
        BitmapCov::branchBitmap[511] = 1;
    }
    absIn1 = BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(in1 < 0LL, 236, 1, 118), -237, 0, 118) ? ~(uint64_T)in1 + 1ULL : (uint64_T)in1;
    in0Hi = absIn0 >> 32ULL;
    in0Lo = absIn0 & 4294967295ULL;
    in1Hi = absIn1 >> 32ULL;
    absIn0 = absIn1 & 4294967295ULL;
    productHiLo = in0Hi * absIn0;
    productLoHi = in0Lo * in1Hi;
    absIn0 *= in0Lo;
    absIn1 = 0ULL;
    in0Lo = (productLoHi << 32ULL) + absIn0;
    if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(in0Lo < absIn0, 238, 1, 119), -239, 0, 119)) {
        BitmapCov::branchBitmap[512] = 1;
        absIn1 = 1ULL;
    }
    BitmapCov::branchBitmap[513] = 1;
    absIn0 = in0Lo;
    in0Lo += productHiLo << 32ULL;
    if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(in0Lo < absIn0, 240, 1, 120), -241, 0, 120)) {
        BitmapCov::branchBitmap[514] = 1;
        absIn1++;
    }
    BitmapCov::branchBitmap[515] = 1;
    absIn0 = (((productLoHi >> 32ULL) + (productHiLo >> 32ULL)) + in0Hi * in1Hi) + absIn1;
    unsigned char mcdc_33_c_1 = in0 != 0LL;
    unsigned char mcdc_33_c_2 = in1 != 0LL;
    unsigned char mcdc_33_c_3 = (in0 > 0LL) != (in1 > 0LL);
    if (mcdc_33_c_1) {
        BitmapCov::branchBitmap[516] = 1;
    }
    else {
        BitmapCov::branchBitmap[517] = 1;
    }
    if (mcdc_33_c_2) {
        BitmapCov::branchBitmap[518] = 1;
    }
    else {
        BitmapCov::branchBitmap[519] = 1;
    }
    if (mcdc_33_c_3) {
        BitmapCov::branchBitmap[520] = 1;
    }
    else {
        BitmapCov::branchBitmap[521] = 1;
    }
    if (BitmapCov::CondAndMCDCRecord((BitmapCov::CondAndMCDCRecord(in0 != 0LL, 242, 1, 121))&((BitmapCov::CondAndMCDCRecord(in1 != 0LL, 244, 2, 121))&(BitmapCov::CondAndMCDCRecord((in0 > 0LL) != (in1 > 0LL), 246, 3, 121))), -243, 0, 121)) {
        BitmapCov::branchBitmap[522] = 1;
        absIn0 = ~absIn0;
        in0Lo = ~in0Lo;
        in0Lo++;
        if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(in0Lo == 0ULL, 248, 1, 122), -245, 0, 122)) {
            BitmapCov::branchBitmap[523] = 1;
            absIn0++;
        }
        BitmapCov::branchBitmap[524] = 1;
    }
    BitmapCov::branchBitmap[525] = 1;
    *ptrOutBitsHi = absIn0;
    *ptrOutBitsLo = in0Lo;
}
int64_T mul_s64_loSR(int64_T a, int64_T b, uint64_T aShift) {
    BitmapCov::munitBitmap[10] = 1;
    uint64_T u64_chi = {0};
    uint64_T u64_clo = {0};
    mul_wide_s64(a, b, &u64_chi, &u64_clo);
    u64_clo = u64_chi << (64ULL - aShift) | u64_clo >> aShift;
    return (int64_T)u64_clo;
}
uint32_T DynamicCompressionMo_BitRotate1(uint32_T rtu_u) {
    BitmapCov::munitBitmap[11] = 1;
    return rtu_u >> 5 | rtu_u << 27;
}
uint32_T DynamicCompressionMod_BitRotate(uint32_T rtu_u) {
    BitmapCov::munitBitmap[12] = 1;
    return rtu_u << 1 | rtu_u >> 31;
}
uint32_T DynamicCompression_BitRotate1_l(uint32_T rtu_u) {
    BitmapCov::munitBitmap[13] = 1;
    return rtu_u << 2 | rtu_u >> 30;
}
uint32_T DynamicCompressionM_BitRotate10(uint32_T rtu_u) {
    BitmapCov::munitBitmap[14] = 1;
    return rtu_u << 11 | rtu_u >> 21;
}
uint32_T DynamicCompressionM_BitRotate11(uint32_T rtu_u) {
    BitmapCov::munitBitmap[15] = 1;
    return rtu_u << 12 | rtu_u >> 20;
}
uint32_T DynamicCompressionM_BitRotate12(uint32_T rtu_u) {
    BitmapCov::munitBitmap[16] = 1;
    return rtu_u << 13 | rtu_u >> 19;
}
uint32_T DynamicCompressionM_BitRotate13(uint32_T rtu_u) {
    BitmapCov::munitBitmap[17] = 1;
    return rtu_u << 14 | rtu_u >> 18;
}
uint32_T DynamicCompressionM_BitRotate14(uint32_T rtu_u) {
    BitmapCov::munitBitmap[18] = 1;
    return rtu_u << 15 | rtu_u >> 17;
}
uint32_T DynamicCompressionMo_BitRotate2(uint32_T rtu_u) {
    BitmapCov::munitBitmap[19] = 1;
    return rtu_u << 3 | rtu_u >> 29;
}
uint32_T DynamicCompressionMo_BitRotate3(uint32_T rtu_u) {
    BitmapCov::munitBitmap[20] = 1;
    return rtu_u << 4 | rtu_u >> 28;
}
uint32_T DynamicCompressionMo_BitRotate4(uint32_T rtu_u) {
    BitmapCov::munitBitmap[21] = 1;
    return rtu_u << 5 | rtu_u >> 27;
}
uint32_T DynamicCompressionMo_BitRotate5(uint32_T rtu_u) {
    BitmapCov::munitBitmap[22] = 1;
    return rtu_u << 6 | rtu_u >> 26;
}
uint32_T DynamicCompressionMo_BitRotate6(uint32_T rtu_u) {
    BitmapCov::munitBitmap[23] = 1;
    return rtu_u << 7 | rtu_u >> 25;
}
uint32_T DynamicCompressionMo_BitRotate7(uint32_T rtu_u) {
    BitmapCov::munitBitmap[24] = 1;
    return rtu_u << 8 | rtu_u >> 24;
}
uint32_T DynamicCompressionMo_BitRotate8(uint32_T rtu_u) {
    BitmapCov::munitBitmap[25] = 1;
    return rtu_u << 9 | rtu_u >> 23;
}
uint32_T DynamicCompressionMo_BitRotate9(uint32_T rtu_u) {
    BitmapCov::munitBitmap[26] = 1;
    return rtu_u << 10 | rtu_u >> 22;
}
uint32_T DynamicCompressionM_BitRotate_g(uint32_T rtu_u) {
    BitmapCov::munitBitmap[27] = 1;
    return rtu_u >> 1 | rtu_u << 31;
}
uint32_T DynamicCompression_BitRotate1_a(uint32_T rtu_u) {
    BitmapCov::munitBitmap[28] = 1;
    return rtu_u >> 2 | rtu_u << 30;
}
uint32_T DynamicCompressio_BitRotate10_p(uint32_T rtu_u) {
    BitmapCov::munitBitmap[29] = 1;
    return rtu_u >> 11 | rtu_u << 21;
}
uint32_T DynamicCompressio_BitRotate11_l(uint32_T rtu_u) {
    BitmapCov::munitBitmap[30] = 1;
    return rtu_u >> 12 | rtu_u << 20;
}
uint32_T DynamicCompressio_BitRotate12_m(uint32_T rtu_u) {
    BitmapCov::munitBitmap[31] = 1;
    return rtu_u >> 13 | rtu_u << 19;
}
uint32_T DynamicCompressio_BitRotate13_a(uint32_T rtu_u) {
    BitmapCov::munitBitmap[32] = 1;
    return rtu_u >> 14 | rtu_u << 18;
}
uint32_T DynamicCompressio_BitRotate14_o(uint32_T rtu_u) {
    BitmapCov::munitBitmap[33] = 1;
    return rtu_u >> 15 | rtu_u << 17;
}
uint32_T DynamicCompression_BitRotate2_a(uint32_T rtu_u) {
    BitmapCov::munitBitmap[34] = 1;
    return rtu_u >> 3 | rtu_u << 29;
}
uint32_T DynamicCompression_BitRotate3_d(uint32_T rtu_u) {
    BitmapCov::munitBitmap[35] = 1;
    return rtu_u >> 4 | rtu_u << 28;
}
uint32_T DynamicCompression_BitRotate5_p(uint32_T rtu_u) {
    BitmapCov::munitBitmap[36] = 1;
    return rtu_u >> 6 | rtu_u << 26;
}
uint32_T DynamicCompression_BitRotate6_b(uint32_T rtu_u) {
    BitmapCov::munitBitmap[37] = 1;
    return rtu_u >> 7 | rtu_u << 25;
}
uint32_T DynamicCompression_BitRotate7_h(uint32_T rtu_u) {
    BitmapCov::munitBitmap[38] = 1;
    return rtu_u >> 8 | rtu_u << 24;
}
uint32_T DynamicCompression_BitRotate8_o(uint32_T rtu_u) {
    BitmapCov::munitBitmap[39] = 1;
    return rtu_u >> 9 | rtu_u << 23;
}
uint32_T DynamicCompression_BitRotate9_g(uint32_T rtu_u) {
    BitmapCov::munitBitmap[40] = 1;
    return rtu_u >> 10 | rtu_u << 22;
}
void DynamicCompre_fxpt_lzc_W32_Init(struct DW_fxpt_lzc_W32_DynamicCompre_T* localDW) {
    BitmapCov::munitBitmap[41] = 1;
    localDW->Delay_DSTATE = 3U;
}
uint8_T DynamicCompression_fxpt_lzc_W32(uint32_T rtu_x_in, struct DW_fxpt_lzc_W32_DynamicCompre_T* localDW) {
    BitmapCov::munitBitmap[42] = 1;
    uint8_T rty_lzc_out_0 = {0};
    rty_lzc_out_0 = localDW->Delay_DSTATE;
    if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord((uint8_T)((uint8_T)rtu_x_in & 1) != 0, 250, 1, 123), -247, 0, 123)) {
        BitmapCov::branchBitmap[526] = 1;
        localDW->Delay_DSTATE = 31U;
    }
    else {
        BitmapCov::branchBitmap[527] = 1;
        localDW->Delay_DSTATE = 32U;
    }
    if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord((uint8_T)((uint8_T)(rtu_x_in >> 8) & 1) != 0, 252, 1, 124), -249, 0, 124)) {
        BitmapCov::branchBitmap[528] = 1;
        localDW->Delay_DSTATE = 23U;
    }
    else if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord((uint8_T)((uint8_T)(rtu_x_in >> 7) & 1) != 0, 254, 1, 125), -251, 0, 125)) {
        BitmapCov::branchBitmap[529] = 1;
        localDW->Delay_DSTATE = 24U;
    }
    else if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord((uint8_T)((uint8_T)(rtu_x_in >> 6) & 1) != 0, 256, 1, 126), -253, 0, 126)) {
        BitmapCov::branchBitmap[530] = 1;
        localDW->Delay_DSTATE = 25U;
    }
    else if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord((uint8_T)((uint8_T)(rtu_x_in >> 5) & 1) != 0, 258, 1, 127), -255, 0, 127)) {
        BitmapCov::branchBitmap[531] = 1;
        localDW->Delay_DSTATE = 26U;
    }
    else if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord((uint8_T)((uint8_T)(rtu_x_in >> 4) & 1) != 0, 260, 1, 128), -257, 0, 128)) {
        BitmapCov::branchBitmap[532] = 1;
        localDW->Delay_DSTATE = 27U;
    }
    else if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord((uint8_T)((uint8_T)(rtu_x_in >> 3) & 1) != 0, 262, 1, 129), -259, 0, 129)) {
        BitmapCov::branchBitmap[533] = 1;
        localDW->Delay_DSTATE = 28U;
    }
    else if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord((uint8_T)((uint8_T)(rtu_x_in >> 2) & 1) != 0, 264, 1, 130), -261, 0, 130)) {
        BitmapCov::branchBitmap[534] = 1;
        localDW->Delay_DSTATE = 29U;
    }
    else if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord((uint8_T)((uint8_T)(rtu_x_in >> 1) & 1) != 0, 266, 1, 131), -263, 0, 131)) {
        BitmapCov::branchBitmap[535] = 1;
        localDW->Delay_DSTATE = 30U;
    }
    BitmapCov::branchBitmap[536] = 1;
    if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord((uint8_T)((uint8_T)(rtu_x_in >> 16) & 1) != 0, 268, 1, 132), -265, 0, 132)) {
        BitmapCov::branchBitmap[537] = 1;
        localDW->Delay_DSTATE = 15U;
    }
    else if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord((uint8_T)((uint8_T)(rtu_x_in >> 15) & 1) != 0, 270, 1, 133), -267, 0, 133)) {
        BitmapCov::branchBitmap[538] = 1;
        localDW->Delay_DSTATE = 16U;
    }
    else if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord((uint8_T)((uint8_T)(rtu_x_in >> 14) & 1) != 0, 272, 1, 134), -269, 0, 134)) {
        BitmapCov::branchBitmap[539] = 1;
        localDW->Delay_DSTATE = 17U;
    }
    else if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord((uint8_T)((uint8_T)(rtu_x_in >> 13) & 1) != 0, 274, 1, 135), -271, 0, 135)) {
        BitmapCov::branchBitmap[540] = 1;
        localDW->Delay_DSTATE = 18U;
    }
    else if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord((uint8_T)((uint8_T)(rtu_x_in >> 12) & 1) != 0, 276, 1, 136), -273, 0, 136)) {
        BitmapCov::branchBitmap[541] = 1;
        localDW->Delay_DSTATE = 19U;
    }
    else if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord((uint8_T)((uint8_T)(rtu_x_in >> 11) & 1) != 0, 278, 1, 137), -275, 0, 137)) {
        BitmapCov::branchBitmap[542] = 1;
        localDW->Delay_DSTATE = 20U;
    }
    else if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord((uint8_T)((uint8_T)(rtu_x_in >> 10) & 1) != 0, 280, 1, 138), -277, 0, 138)) {
        BitmapCov::branchBitmap[543] = 1;
        localDW->Delay_DSTATE = 21U;
    }
    else if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord((uint8_T)((uint8_T)(rtu_x_in >> 9) & 1) != 0, 282, 1, 139), -279, 0, 139)) {
        BitmapCov::branchBitmap[544] = 1;
        localDW->Delay_DSTATE = 22U;
    }
    BitmapCov::branchBitmap[545] = 1;
    if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord((rtu_x_in >> 24 & 1U) != 0U, 284, 1, 140), -281, 0, 140)) {
        BitmapCov::branchBitmap[546] = 1;
        localDW->Delay_DSTATE = 7U;
    }
    else if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord((uint8_T)((uint8_T)(rtu_x_in >> 23) & 1) != 0, 286, 1, 141), -283, 0, 141)) {
        BitmapCov::branchBitmap[547] = 1;
        localDW->Delay_DSTATE = 8U;
    }
    else if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord((uint8_T)((uint8_T)(rtu_x_in >> 22) & 1) != 0, 288, 1, 142), -285, 0, 142)) {
        BitmapCov::branchBitmap[548] = 1;
        localDW->Delay_DSTATE = 9U;
    }
    else if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord((uint8_T)((uint8_T)(rtu_x_in >> 21) & 1) != 0, 290, 1, 143), -287, 0, 143)) {
        BitmapCov::branchBitmap[549] = 1;
        localDW->Delay_DSTATE = 10U;
    }
    else if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord((uint8_T)((uint8_T)(rtu_x_in >> 20) & 1) != 0, 292, 1, 144), -289, 0, 144)) {
        BitmapCov::branchBitmap[550] = 1;
        localDW->Delay_DSTATE = 11U;
    }
    else if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord((uint8_T)((uint8_T)(rtu_x_in >> 19) & 1) != 0, 294, 1, 145), -291, 0, 145)) {
        BitmapCov::branchBitmap[551] = 1;
        localDW->Delay_DSTATE = 12U;
    }
    else if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord((uint8_T)((uint8_T)(rtu_x_in >> 18) & 1) != 0, 296, 1, 146), -293, 0, 146)) {
        BitmapCov::branchBitmap[552] = 1;
        localDW->Delay_DSTATE = 13U;
    }
    else if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord((uint8_T)((uint8_T)(rtu_x_in >> 17) & 1) != 0, 298, 1, 147), -295, 0, 147)) {
        BitmapCov::branchBitmap[553] = 1;
        localDW->Delay_DSTATE = 14U;
    }
    BitmapCov::branchBitmap[554] = 1;
    if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord(rtu_x_in >> 31 != 0U, 300, 1, 148), -297, 0, 148)) {
        BitmapCov::branchBitmap[555] = 1;
        localDW->Delay_DSTATE = 0U;
    }
    else if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord((rtu_x_in >> 30 & 1U) != 0U, 302, 1, 149), -299, 0, 149)) {
        BitmapCov::branchBitmap[556] = 1;
        localDW->Delay_DSTATE = 1U;
    }
    else if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord((rtu_x_in >> 29 & 1U) != 0U, 304, 1, 150), -301, 0, 150)) {
        BitmapCov::branchBitmap[557] = 1;
        localDW->Delay_DSTATE = 2U;
    }
    else if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord((rtu_x_in >> 28 & 1U) != 0U, 306, 1, 151), -303, 0, 151)) {
        BitmapCov::branchBitmap[558] = 1;
        localDW->Delay_DSTATE = 3U;
    }
    else if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord((rtu_x_in >> 27 & 1U) != 0U, 308, 1, 152), -305, 0, 152)) {
        BitmapCov::branchBitmap[559] = 1;
        localDW->Delay_DSTATE = 4U;
    }
    else if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord((rtu_x_in >> 26 & 1U) != 0U, 310, 1, 153), -307, 0, 153)) {
        BitmapCov::branchBitmap[560] = 1;
        localDW->Delay_DSTATE = 5U;
    }
    else if (BitmapCov::CondAndMCDCRecord(BitmapCov::CondAndMCDCRecord((rtu_x_in >> 25 & 1U) != 0U, 312, 1, 154), -309, 0, 154)) {
        BitmapCov::branchBitmap[561] = 1;
        localDW->Delay_DSTATE = 6U;
    }
    BitmapCov::branchBitmap[562] = 1;
    return rty_lzc_out_0;
}