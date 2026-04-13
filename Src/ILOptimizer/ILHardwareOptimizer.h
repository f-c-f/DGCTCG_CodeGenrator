#pragma once

#include <string>
/*
暂定该类通过利用平台有关的硬件指令进行优化
目前思路类似于 依据指令进行识别
大部分组件的翻译做代码替换。
即，利用asm内联汇编替换组件的实现
考虑可移植性等多方面的问题，
目前该优化不考虑多编译器的情况，专注针对GCC

该优化针对TriCore、RISC-V平台、ARM平台
x86 以及 其它平台需要进一步的探索与研究

*/


class ILParser;
class ILSchedule;
class MIRModel;

class ILHardwareOptimizer
{
public:
	int optimize(ILSchedule* schedule, ILParser* parser, MIRModel* mirModel, std::string platform);
	ILSchedule* schedule = nullptr;
	ILParser* iLParser = nullptr;
	MIRModel* mirModel = nullptr;
	std::string platform = "";
private:
	int optimize() const;

};
