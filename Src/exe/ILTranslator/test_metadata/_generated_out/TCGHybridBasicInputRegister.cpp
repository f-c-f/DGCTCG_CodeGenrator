#include "TCGHybridBasicInputRegister.h"
#include "define.h"
#include "TCGHybridBasic.h"
void TCGHybridBasicInputRegister::registAllInputs()
{
    TCGHybrid::registInputs("exec_DynamicCompressionModel_U_Table_Values", "i32", 4);
    TCGHybrid::registInputs("exec_DynamicCompressionModel_U_In2", "u8", 1);
    TCGHybrid::registInputs("exec_DynamicCompressionModel_U_In3", "i32", 4);
    TCGHybrid::registInputs("exec_DynamicCompressionModel_U_Table_Values_e", "u8", 1);
}
