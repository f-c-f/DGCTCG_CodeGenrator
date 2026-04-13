#pragma once

#include <map>
#include <string>
#include <vector>


using namespace std;

class TCGHybridBasicBranchRegister{
public:
    static map<int, string> TCGHybrid_Branch;
    static map<int, int> TCGHybrid_Branch_Depth;

    static void loadTCGHybridBranchData();
};
