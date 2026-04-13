#include "Trace.h"


// , unsigned char leftOpndType, unsigned char leftOpndValIdx, unsigned char rightOpndType, unsigned char rightOpndValIdx
OneStep::OneStep(int id, unsigned char type)
{
	this->id = id;
	this->type = type;
}

Trace::Trace()
{
	allUnsignedChars = new std::vector<unsigned char>();
	allChars = new std::vector<char>();
	allUnsignedShorts = new std::vector<unsigned short>();
	allShorts = new std::vector<short>();
	allUnsignedInts = new std::vector<unsigned int>();
	allInts = new std::vector<int>();
	allUnsignedLongLongs = new std::vector<unsigned long long>();
	allLongLongs = new std::vector<long long>();
	allFloats = new std::vector<float>();
	allDoubles = new std::vector<double>();
	allSteps = new std::vector<OneStep*>();
}

Trace::~Trace()
{
	for (std::vector<OneStep*>::iterator itr = allSteps->begin(); itr != allSteps->end();itr++) {
		OneStep* os = *itr;
		delete os;
	}
	delete allUnsignedChars;
	delete allChars;
	delete allUnsignedShorts;
	delete allShorts;
	delete allUnsignedInts;
	delete allInts;
	delete allUnsignedLongLongs;
	delete allLongLongs;
	delete allFloats;
	delete allDoubles;
	delete allSteps;
}

Trace* TraceManager::curr = nullptr;

void TraceManager::BeginBewTrace()
{
	TraceManager::curr = new Trace();
}



