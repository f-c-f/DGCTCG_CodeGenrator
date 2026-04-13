#pragma once
#include<string>
#include<vector>
#include<map>

class TCGConfig
{
public:
	int iterationCount = -1;
	std::string mainInitFunction;
	std::string mainExecFunction;
	std::string mainCompositeState;

    
    class TCGInput
    {
    public:
        // Ù–‘
	    std::string name;
	    std::string type;
	    int isAddress = 0;
	    std::vector<int> arraySize;
        bool noTest = false;

        class TCGParameter
        {
        public:
            std::string expression;
	        std::string type;
            std::string min;
            std::string max;
            std::string value;
        };

        std::vector<TCGParameter> parameters;
    };

    std::vector<TCGInput> inputs;
};
