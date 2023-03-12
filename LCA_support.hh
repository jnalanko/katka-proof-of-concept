#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>
#include <memory>
#include <cmath>
#include <unordered_map>
#include <bit>
#include "RMQ_support.hh"
#include "Tree.hh"

class LCA_support{

private:
    void DFS_tour(int64_t v, int64_t depth){
        int64_t tour_index = DFS_tour_nodes.size();

        DFS_tour_nodes.push_back(v);
        DFS_tour_depths->push_back(depth);
        first_index_on_tour[v] = min(first_index_on_tour[v], tour_index);

        for(int64_t u : tree->nodes[v].children_ids){
            DFS_tour(u, depth+1);
            DFS_tour_nodes.push_back(v);
            DFS_tour_depths->push_back(depth);
        }
    }

public:

    shared_ptr<Tree> tree;
    vector<int64_t> DFS_tour_nodes; // List of node ids in tour order
    shared_ptr<vector<int64_t>> DFS_tour_depths; // List of node depths in tour order
    vector<int64_t> first_index_on_tour; // node -> first index on tour
    RMQ_support depth_RMQ;

    LCA_support(shared_ptr<Tree> tree) : tree(tree) {
        DFS_tour_depths = make_shared<vector<int64_t>>();
        first_index_on_tour.resize(tree->nodes.size(), 1e18);
        DFS_tour(0,0);
        depth_RMQ = RMQ_support(DFS_tour_depths);
    }

    int64_t LCA(int64_t v, int64_t u){
        int64_t i = first_index_on_tour[v];
        int64_t j = first_index_on_tour[u];
        if(i > j) swap(i,j);
        int64_t v_pos = first_index_on_tour[v];
        int64_t u_pos = first_index_on_tour[u];
        int64_t min_depth_pos = depth_RMQ.RMQ(min(v_pos, u_pos), max(v_pos, u_pos) + 1);
        return DFS_tour_nodes[min_depth_pos];
    }

};

void test_LCA_support(){
    string tree_encoding = "((L1,L2)V1,(L3,L4,L5)V2,(L6,L7)V3)Root;";
    shared_ptr<Tree> T = make_unique<Tree_Loader>()->load_tree(tree_encoding);
    for(Tree::Node v : T->nodes){
        cout << v.to_string() << endl;
    }
    LCA_support LCA(T);
    cout << LCA.LCA(4,5) << endl;
}