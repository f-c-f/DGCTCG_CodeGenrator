#include <vector>


class OneStep {
public:
	int id = -1;
	unsigned char type = -1;

	unsigned char leftOpndType = -1;
	unsigned char leftOpndValIdx = -1;

	unsigned char rightOpndType = -1;
	unsigned char rightOpndValIdx = -1;

	// , unsigned char leftOpndType = -1, unsigned char leftOpndValIdx = -1, unsigned char rightOpndType = -1, unsigned char rightOpndValIdx = -1
	OneStep(int id = -1, unsigned char type = -1);

};

class Trace {
public:
	std::vector<unsigned char>* allUnsignedChars;
	std::vector<char>* allChars;
	std::vector<unsigned short>* allUnsignedShorts;
	std::vector<short>* allShorts;
	std::vector<unsigned int>* allUnsignedInts;
	std::vector<int>* allInts;
	std::vector<unsigned long long>* allUnsignedLongLongs;
	std::vector<long long>* allLongLongs;
	std::vector<float>* allFloats;
	std::vector<double>* allDoubles;

	std::vector<OneStep*>* allSteps;
	
	Trace();
	~Trace();
};

class TraceManager {
public:

	static Trace* curr;

	static void BeginBewTrace();

};


