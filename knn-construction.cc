
/**
 *  Naive baseline for construction a KNN Graph.
 *  Randomly select 100 neighbors from a 10k subset.
 */

#define HyRec
// #define BaseLine
#pragma GCC optimization("O2")
#include <sys/time.h>

#include <algorithm>
#include <ctime>
#include <fstream>
#include <iostream>
#include <numeric>
#include <queue>
#include <random>
#include <string>
#include <vector>

#include "assert.h"
#include "io.h"
#include "kdtree.h"
#include "utils.h"
#include "HyRec.h"

using std::cout;
using std::endl;
using std::string;
using std::vector;

#define _INT_MAX 2147483640

float EuclideanDistance(const vector<float> &lhs, const vector<float> &rhs) {
  float ans = 0.0;
  unsigned lensDim = 100;
  for (unsigned i = 0; i < lensDim; ++i) {
    ans += (lhs[i] - rhs[i]) * (lhs[i] - rhs[i]);
  }
  return ans;
}

vector<uint32_t> CalculateOneKnn(const vector<vector<float>> &data,
                                 const vector<uint32_t> &sample_indexes,
                                 const uint32_t id) {
  std::priority_queue<std::pair<float, uint32_t>> top_candidates;
  float lower_bound = _INT_MAX;
  for (unsigned i = 0; i < sample_indexes.size(); i++) {
    uint32_t sample_id = sample_indexes[i];
    if (id == sample_id) continue;  // skip itself.
    float dist = EuclideanDistance(data[id], data[sample_id]);

    // only keep the top 100
    if (top_candidates.size() < 100 || dist < lower_bound) {
      top_candidates.push(std::make_pair(dist, sample_id));
      if (top_candidates.size() > 100) {
        top_candidates.pop();
      }

      lower_bound = top_candidates.top().first;
    }
  }

  vector<uint32_t> knn;
  while (!top_candidates.empty()) {
    knn.emplace_back(top_candidates.top().second);
    top_candidates.pop();
  }
  std::reverse(knn.begin(), knn.end());
  return knn;
}

void ConstructKnng(const vector<vector<float>> &data,
                   const vector<uint32_t> &sample_indexes,
                   vector<vector<uint32_t>> &knng) {
  knng.resize(data.size());
  int start_ = data.size() * 0; 

#pragma omp parallel for
  for (uint32_t n = start_; n < knng.size(); n++) {
    knng[n] = CalculateOneKnn(data, sample_indexes, n);
  }
  cout << "Construction Done!" << endl;
}

void CalculateOneKnn(
  const tree_model *model, 
  const vector<vector<float>> &data,
  vector<vector<uint32_t>>& knng,
  uint32_t id                                                     
) {
    vector<float> dists(101);
    vector<size_t> neighbors(100);
    find_k_nearests(model,data[id].data(),101,neighbors.data(),dists.data());  
    neighbors.resize(100);
    vector<uint32_t> neighbors_(100);;
    for(int j=0;j<100;j++) neighbors_[j]=neighbors[j];
    knng[id] = std::move(neighbors_);
}

void ConstructKnng(const vector<vector<float>> &data,
                   const vector<float> &data_f,
                   vector<vector<uint32_t>> &knng,
                   int nrows,int ncols
) {
  // time_t start = clock();
  vector<float> indexes(data.size());
  iota(indexes.begin(), indexes.end(), 0); 
  time_t start = clock();
  // printf("row:%d, col:%d\n",(int)data.size(),(int)data[0].size());
  tree_model *model = build_kdtree(data_f.data(),indexes.data(),data.size(),data[0].size(),2);
  printf("Finised Building Tree, Time:%.6lf\n",((double)clock()-start)/CLOCKS_PER_SEC);
  

  vector<float> dists(101);
  knng.resize(data.size());
  cout<<data.size()<<endl;
  int count = 0, end_ = data.size() * 1;
#pragma omp parallel for
  for (size_t i = 0;i<end_;i++) {
    CalculateOneKnn(model,data,knng,i);
    if((count+1)%50==0) {
      printf("complete %.2lf, time: %.2lf\n",100.*count/data.size(),((double)clock()-start)/CLOCKS_PER_SEC);
    }
#pragma omp critical
    {
      count++;
    }    
    // int id = i;
    // vector<float> dists(101);
    // vector<size_t> neighbors(100);
    // find_k_nearests(model,data[id].data(),101,neighbors.data(),dists.data());  
    // neighbors.resize(100);
    // vector<uint32_t> neighbors_(100);;
    // for(int j=0;j<100;j++) neighbors_[j]=neighbors[j];
    // knng[id] = neighbors_;
    // if((i+1)%10==0) {
    //   printf("complete %.2lf, time: %.2lf\n",100.*i/data.size(),((double)clock()-start)/CLOCKS_PER_SEC);
    // }
  }
  cout<<knng.size()<<" "<<knng[0].size()<<endl;
}

int main(int argc, char **argv) {
  timeALL(0);
  string source_path = "dummy-data.bin", labels_path = "groundTruth1w.bin";

  // Also accept other path for source data
  if (argc > 1) {
    source_path = string(argv[1]);
  }

  // Read data points
  vector<vector<float>> nodes;
  ReadBin(source_path, nodes);
  // nodes.resize(100000);

  // Sample points for greedy search
  std::default_random_engine rd;
  std::mt19937 gen(rd());  // Mersenne twister MT19937
  vector<uint32_t> sample_indexes(nodes.size());
  iota(sample_indexes.begin(), sample_indexes.end(), 0);
  shuffle(sample_indexes.begin(), sample_indexes.end(), gen);
  // For evaluation dataset, keep more points
  if (sample_indexes.size()>100000) {
    sample_indexes.resize(100000);
    cout<<"sample size resize"<<endl;
  }
  
  vector<float> data_f(nodes.size()*nodes[0].size());
#pragma omp parallel for  
  for (size_t i_=0;i_<nodes.size();i_++) {
#pragma omp parallel for
    for (size_t j_=0;j_<nodes[0].size();j_++) {
        data_f[(int)nodes[0].size()*i_+j_]=(nodes[i_][j_]);
    }
  }

  // Knng constuction
  vector<vector<uint32_t>> knng, gt;
  cout<<nodes.size()<<" "<<nodes[0].size()<<endl;
/// BaseLine
#ifdef BaseLine
  ConstructKnng(nodes, sample_indexes, knng);
#endif

/// KDTree Method
#ifdef KDTREE  
  ConstructKnng(nodes,data_f,knng,nodes.size(),nodes[0].size());  
#endif
/// HyRec Method
#ifdef HyRec
  ConstructANNHyrec(nodes,knng,100,true);
#endif

// check format
  for (int i = 0; i < NNodes; i++) {
    if(knng[i].size()!=100) {
      cout<<i<<endl;
      exit(-1);
    }
  }

// evaluation
  ReadGroundTruth(labels_path, gt);
  cout<< "Recall: " << Recall(knng,gt)*100 << "%" <<endl;
  // Save to ouput.bin
  SaveKNNG(knng);
  timeALL(1);
  return 0;
}
