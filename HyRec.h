#ifndef __HyRec_H__
#define __HyRec_H_

#include <fstream>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>
#include <bitset>

#include "assert.h"
#include "utils.h"

using std::cout;
using std::endl;
using std::string;
using std::vector;

void ConstructANNHyrec(const vector<vector<float>> &data, vector<vector<uint32_t>> &knng, int K=100, bool earlyStop=false);
void ConstructOneNodeHyrec(const vector<vector<float>> &data, vector<vector<uint32_t>> &knng, int id);

#endif