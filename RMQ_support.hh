#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>
#include <memory>
#include <cmath>
#include <unordered_map>

using namespace std;

class RMQ_support{
    public:

    shared_ptr<vector<int64_t>> data;

    vector<vector<int64_t>> precalc;
    // precalc[i][j] = position of the minimum of data[i .. i+2^j)

    RMQ_support() {}
    RMQ_support(shared_ptr<vector<int64_t>> data) : data(data){
        int64_t n = data->size();
        precalc.resize(n);
        for(int64_t i = 0; i < n; i++){
            for(int64_t len = 1; len <= n; len *= 2){
                int64_t end = min(i + len, n); // One past the end
                auto it = std::min_element(data->begin() + i, data->begin() + end);
                int64_t min_pos = it - data->begin();
                precalc[i].push_back(min_pos);
            }
        }
    }

    int64_t log2_floor(int64_t x){ // TODO: O(1)
        int64_t ans = 0;
        int64_t y = 1;
        while(y * 2 < x){
             y *= 2;
             ans++;
        }
        return ans;
    }

    // Returns the position of the minimum in range [i..j)
    int64_t RMQ(int64_t i, int64_t j){
        assert(i < j);
        if(i + 1 == j) return i; // Singleton interval
        int64_t log2floor = log2_floor(j-i);
        int64_t len = ((int64_t) 1) << log2floor;

        int64_t p = precalc[i][log2floor];
        int64_t q = precalc[j - len][log2floor];

        return data->at(p) <= data->at(q) ? p : q;
    }
};

void test_RMQ_support(){
    srand(234);
    auto data = make_shared<vector<int64_t>>();;
    int64_t n = 30;
    for(int64_t i = 0; i < n; i++){
        data->push_back(rand() % 10);
    }
    for(auto x : *data) cout << x << " "; cout << endl;
    RMQ_support RMQ(data);
    for(int64_t i = 0; i < n; i++){
        for(int64_t j = i + 1; j <= n; j++){ // exclusive end
            int64_t correct_answer = std::min_element(data->begin() + i, data->begin() + j) - data->begin();
            int64_t our_answer = RMQ.RMQ(i,j);
            cout << correct_answer << " " << our_answer << endl;
            assert(correct_answer == our_answer);
        }
    }
}
