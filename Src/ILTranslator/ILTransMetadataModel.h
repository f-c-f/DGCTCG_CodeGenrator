#pragma once

#include <string>
#include <vector>

// 轻量模型：仅用于元数据导出（不参与代码生成语义）

struct ILTransBranchObligationMeta
{
	int bitmap_index = -1;
	std::string obligation_type;
	std::string function_name;
	std::string file_name;
	std::string condition_text;
	int branch_depth = 0;
	int call_depth = 0;
	std::vector<std::string> related_inputs;
	std::vector<std::string> related_globals;
	std::string cbmc_property_label;
};

struct ILTransBranchRelationMeta
{
	int branch_id = -1;
	std::vector<std::string> read_globals;
	std::vector<std::string> write_globals;
	int parent_branch = -1;
	std::vector<int> children_branch;
};

// 以下为 flush 侧写出的附加 JSON/CSV 的语义说明（结构体可选用于后续 C++ 消费）
// obligation_map.json: obligations[]；另含 mcdc_decisions[]（atoms 含 function/file/related_*）、
//   condition_pairs[]、decision_pairs[]（原独立 JSON 已合并至此文件）
// obligation_map.csv: 含 obligation_hint（与 JSON 中 obligation_hint 同义，便于表格/LLM 消费）
// distance_map.json: branch_value_distances | array_distances（来自 TCGStateSearch 或 TCGHybrid 静态表）
