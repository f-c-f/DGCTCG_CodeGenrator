#pragma once

#include <map>
#include <string>
#include <vector>


using namespace std;
//记录那些分支读写了全局变量
class StateSearchBasicBranchRelation{
public:
    static void loadStateSearchBranchRelation(vector<pair<int, string>>& TCGHybrid_BranchReadGlobalVar, vector<pair<int, string>>& TCGHybrid_BranchWriteGlobalVar);
};