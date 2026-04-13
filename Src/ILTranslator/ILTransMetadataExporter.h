#pragma once

#include <string>
#include <utility>
#include <vector>

class ILParser;

// 旁路收集与写盘：不改变现有插桩与生成代码语义
class ILTransMetadataExporter
{
public:
	// 每次完整翻译开始前调用，避免同进程多次翻译时脏数据
	static void reset();

	// 由 TCGStateSearch 分支关系分析填充（读/写全局与 branch id 的对应）
	static void recordBranchRelationPairs(
		const std::vector<std::pair<void*, std::string>>& listBranchReadGlobalVar,
		const std::vector<std::pair<void*, std::string>>& listBranchWriteGlobalVar);

	// 写出 obligation_map.json（内含 mcdc_decisions / condition_pairs / decision_pairs）、
	// obligation_map.csv、branch_relation.json、distance_map.json（失败返回 false）
	static bool flush(const std::string& outputDir, const ILParser* parser);
};
