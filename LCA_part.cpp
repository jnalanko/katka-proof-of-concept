#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>
#include <memory>
#include <cmath>
#include <sstream>
#include <unordered_map>
#include <bit>
#include <set>
#include "LCA_support.hh"
#include "Tree.hh"

using namespace std;

struct MEM{
    int64_t left_doc;
    int64_t right_doc;
    int64_t length;
};

vector<MEM> load_MEM_data(const string& filename){
    ifstream in(filename);
    if(!in.good()){
        cerr << "Error loading file: " << filename << endl;
        exit(1);
    }

    vector<MEM> mems;
    string line;
    while(getline(in, line)){
        stringstream ss(line);
        MEM mem;
        ss >> mem.length >> mem.left_doc >> mem.right_doc;
        mems.push_back(mem);
    }

    return mems;
}

vector<int64_t> get_all_ancestors(shared_ptr<Tree> T, int64_t v){
    vector<int64_t> ancestors;
    while(v != 0){
        ancestors.push_back(v);
        v = T->nodes[v].parent_id;
    } ancestors.push_back(v); // Root
    return ancestors;
}


void print_subgraph_in_dot_format(shared_ptr<Tree> T, const vector<int64_t>& LCA_nodes){
    set<int64_t> subgraph_nodes, marked_nodes;
    for(int64_t v : LCA_nodes){
        marked_nodes.insert(v);
        for(int64_t u : get_all_ancestors(T, v)){
            subgraph_nodes.insert(u);
        }
    }

    // Print the DOT graph

    cout << "digraph graphname {" << endl;
    for(int64_t v : subgraph_nodes){

        // Add node
        string color = marked_nodes.count(v) ? "red" : "black";
        cout << "  " << v << " [style=filled, fillcolor=" << color << "];" << endl;

        // Add edge from parent
        cout << "  " << T->nodes[v].parent_id << " -> " << v << ";" << endl;

    }
    cout << "}" << endl;
}

int main(int argc, char** argv){

    string tree_encoding = read_file(argv[1]);
    vector<MEM> MEM_data = load_MEM_data(argv[2]);

    // Trim trailing whitespace
    while(tree_encoding.back() == ' ' || tree_encoding.back() == '\n') tree_encoding.pop_back();

    shared_ptr<Tree> T = make_unique<Tree_Loader>()->load_tree(tree_encoding);
    LCA_support LCA(T);

    // Find the LCAs
    vector<int64_t> LCA_nodes;
    for(MEM mem : MEM_data){
        if(mem.length > 0){
            int64_t lca = LCA.LCA(mem.left_doc, mem.right_doc);
            LCA_nodes.push_back(lca);
        }
    }

    print_subgraph_in_dot_format(T, LCA_nodes);
}