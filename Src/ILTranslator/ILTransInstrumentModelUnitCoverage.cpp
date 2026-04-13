#include "ILTransInstrumentModelUnitCoverage.h"
#include "ILTransInstrumentBitmapHelper.h"
#include "../ILBasic/ILRef.h"

int InsertModelUnitCoverageCollection(const ILCalculate* calculate, ILTransFile* file)
{
	int id = ILTransInstrumentBitmapHelper::munitBitmapCount;
	ILTransInstrumentBitmapHelper::munitBitmapCount++;
	for (ILRef* iref : calculate->refs) {
		std::map<std::string, int>::iterator itr = ILTransInstrumentBitmapHelper::munitIndexMap.find(iref->path);
		if (itr == ILTransInstrumentBitmapHelper::munitIndexMap.end()) {
			ILTransInstrumentBitmapHelper::munitIndexMap[iref->path] = id;
		}
		// else {
		//	id = itr->second;
		// }
	}
	ILTransInstrumentBitmapHelper::munitDesc[id] = calculate->name;
	file->transCodeFunction += "BitmapCov::munitBitmap[" + std::to_string(id) + "] = 1;\n";
	return 0;
}


