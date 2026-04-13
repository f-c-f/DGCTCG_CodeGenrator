#pragma once

#include <string>
#include <vector>

// ?? obligation ?????? LLM / ??????? obligation_map ?????
struct ILObligationSlice
{
	int bitmap_index = -1;
	std::string function_name;
	std::string file_name;
	std::string condition_text;
	std::string cbmc_property_label;
	std::string obligation_type;
	std::string control_context_il;
	std::string il_anchor_kind; // "ILBranch" | "Statement" | "unknown"
	std::vector<std::string> related_inputs;
	std::vector<std::string> read_globals;
	std::vector<std::string> write_globals;
	std::string obligation_map_json_snippet;
	std::string branch_relation_json_snippet;
	std::string slice_text;
};
