#include "SLSubsystem.h"
#include "SLBlock.h"
#include "SLLine.h"
using namespace std;
void SLSubsystem::release()
{
	for(int i = 0;i < blocks.size(); i++)
	{
		blocks[i]->release();
		delete blocks[i];
	}
	blocks.clear();
	for(int i = 0;i < lines.size(); i++)
	{
		lines[i]->release();
		delete lines[i];
	}
	lines.clear();
}
