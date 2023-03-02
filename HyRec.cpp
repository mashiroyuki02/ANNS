#include "HyRec.h"
#include <unordered_set>
#include <algorithm>

void ConstructANNHyrec(
    const vector<vector<float>> &data,
    vector<vector<uint32_t>> &knng,
    int K,
    bool earlyStop) 
{
    int N = data.size();
    knng.resize(N);
// #pragma omp parallel for       
    for (size_t i = 0; i < N; i++) {
        vector<uint32_t> anng(K);
        IdxSmp.sampleN(anng,K);
        knng[i] = std::move(anng);
    }
    cout<<"Sample Init KNNG Completed"<<endl;
    
/// use early stop when some nodes's neigbors has not been updated below a threshold
/// early stop

    std::bitset<NNodes> skip;
    std::vector<int> stoptimes(N,0);

    for (size_t epoch = 0; epoch < MAX_EPOCH; epoch ++ ) {
        int changes = 0;
        cout<< "epoch: " << epoch << endl;
#pragma omp parallel for         
        for (size_t id = 0; id < N; id ++) {
            int curchanges = 0;
            if ((id+1)%LOGGAP==0) cout<< "complete: "<< (id+1.)/NNodes*100 << "%" << endl;
            if (skip[id])  continue; // early stop
            std::vector<std::pair<double,uint32_t>> cands;
            std::unordered_set<uint32_t> s;
            assert(knng[id].size()==K);
            for (uint32_t x:knng[id]) {
                assert(x<N);
                cands.emplace_back(L2(data[id],data[x]),x);
                s.insert(x);
                for (uint32_t y:knng[x]) cands.emplace_back(L2<float>(data[id],data[y]),y);
            }
            vector<uint32_t> rdk(K);
#pragma omp critical 
            {            
            IdxSmp.sampleN(rdk,K);
            }
            for (uint32_t x:rdk) cands.emplace_back(L2<float>(data[id],data[x]),x);
            sort(cands.begin(),cands.end());
            cands.erase(std::unique(cands.begin(),cands.end()),cands.end());
            assert(cands.size()>=K);
            cands.resize(K);
            for (uint32_t i = 0; i < K; i ++) {
                knng[id][i] = cands[i].second; 
                if (s.find(cands[i].second) == s.end()) 
#pragma omp critical
                { 
                    changes++;
                    curchanges++;
                }
            }
            if (curchanges < NNodes * UpdThresh) stoptimes[id]++;
            else stoptimes[id]=0;
            if (stoptimes[id] >= STOPTIMES) skip.set(id);
        }
        if (changes < MIN_CHANGES) break;
        cout << changes << " " << endl;
    }
}

void ConstructOneNodeHyrec(
    const vector<vector<float>> &data,
    vector<vector<uint32_t>> &knng,
    int id) 
{
    
} 