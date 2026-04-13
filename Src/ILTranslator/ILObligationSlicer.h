#pragma once

#include <string>
#include <vector>

class ILParser;

// ??? translate???? branchMapId ?????? bitmap_index ??????
class ILObligationSlicer
{
public:
	// ?? Hybrid ??? hybrid_coverage_stall_slice_request.json ?? uncovered_branch_bitmap_indices
	static bool parseHybridSliceRequestFile(const std::string& path, std::vector<int>& outIndices);

	// ???? "12,34,56" ??????????
	static bool parseBitmapIndexListArg(const std::string& arg, std::vector<int>& outIndices);

	// ? branch_relation.json ??? branch_id ???????????????????
	static bool tryExtractBranchRelationSnippet(const std::string& branchRelationJson, int branchId,
		std::string& outSnippet);

	// ??????? outputDir/Slices/slice_<index>.txt ? slice_manifest.txt
	static bool emitSlicesForBitmapIndices(const ILParser* parser, const std::vector<int>& indices,
		const std::string& obligationMapJsonPath, const std::string& branchRelationJsonPath,
		const std::string& outputDir);
};
