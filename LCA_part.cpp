#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>
#include <memory>
#include <cmath>
#include <unordered_map>
#include <bit>

using namespace std;

// https://stackoverflow.com/questions/2602013/read-whole-ascii-file-into-c-stdstring
string read_file(string filename){
    std::ifstream t(filename);
    if(!t.good()){
        cerr << "Error opening file " << filename << endl;
        exit(1);
    }
    t.seekg(0, std::ios::end);
    size_t size = t.tellg();
    std::string buffer(size, ' ');
    t.seekg(0);
    t.read(&buffer[0], size);
    return buffer;
}

class RMQ_support{
    public:

    shared_ptr<vector<int64_t>> data;

    vector<vector<int64_t>> precalc;
    // precalc[i][j] = position of the minimum of data[i .. i+2^j)

    RMQ_support(shared_ptr<vector<int64_t>> data) : data(data){
        int64_t n = data->size();
        precalc.resize(n);
        for(int64_t i = 0; i < n; i++){
            for(int64_t len = 1; len <= n; len *= 2){
                int64_t end = min(i + len, n); // One past the end
                auto it = std::min_element(data->begin() + i, data->begin() + end);
                int64_t min_pos = it - data->begin();
                precalc[i].push_back(min_pos);
                if(i == 0) cout << precalc[0].back() << " ";
            }
        }
        cout << endl;
    }

    int64_t log2_floor(int64_t x){
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

void test_RMQ(){
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


class Tree{
public:
    struct Node{
        string name;
        int64_t id;
        int64_t parent_id;
        vector<int64_t> children_ids;
        vector<double> children_edge_lengths;
    };

    vector<Node> nodes; // node[0] is the root
};

Tree T;

class LCA_support{

private:
    void DFS_tour(int64_t v, int64_t depth){
        int64_t tour_index = DFS_tour_nodes.size();

        DFS_tour_nodes.push_back(v);
        DFS_tour_depths.push_back(depth);
        first_index_on_tour[v] = min(first_index_on_tour[v], tour_index);

        for(int64_t u : tree->nodes[v].children_ids){
            DFS_tour(u, depth+1);
            DFS_tour_nodes.push_back(v);
            DFS_tour_depths.push_back(depth);
        }
    }

public:

    shared_ptr<Tree> tree;
    vector<int64_t> DFS_tour_nodes; // List of node ids in tour order
    vector<int64_t> DFS_tour_depths; // List of node depths in your order
    vector<int64_t> first_index_on_tour; // node -> first index on tour

    LCA_support(shared_ptr<Tree> tree) : tree(tree) {
        first_index_on_tour.resize(tree->nodes.size(), 1e18);
        DFS_tour(0,0);
    }

    int64_t LCA(int64_t v, int64_t u){
        int64_t i = first_index_on_tour[v];
        int64_t j = first_index_on_tour[u];
        if(i > j) swap(i,j);
        //int64_t tour_pos = 
        return 0; // TODO
    }

};

// Returns next comma at the current level of the tree.
// If not found, returns end + 1
int64_t next_comma(const string& tree_encoding, int64_t start, int64_t end){
    int64_t depth = 0;
    for(int64_t i = start; i <= end; i++){
        if(tree_encoding[i] == '(') depth++;
        if(tree_encoding[i] == ')') depth--;
        if(depth == 0 && tree_encoding[i] == ','){
            return i;
        }
    }
    return end +1;
}
int64_t find_close(const string& tree_encoding, int64_t open){
    int64_t depth = 0;
    for(int64_t i = open + 1; ; i++){
        if(depth == 0 && tree_encoding[i] == ')') return i;
        if(tree_encoding[i] == '(') depth++;
        if(tree_encoding[i] == ')') depth--;
    }
}

int64_t find_colon(const string& tree_encoding, int64_t left, int64_t right){
    for(int64_t i = left; i <= right; i++){
        if(tree_encoding[i] == ':') return i;
    }
    return -1; // Not found
}

// [left, right] is a segment of tree_encoding
// Returns the node id of the root of the subtree
int64_t traverse_subtree(const string& tree_encoding, int64_t left, int64_t right, int64_t parent_id){

    // Create a new node (added to the tree at the end of the function
    Tree::Node v;
    v.parent_id = parent_id;
    v.id = T.nodes.size(); // Create a new id

    // Push the node to the tree. The data will be updated at the end of the function, but
    // we need to push this node here now to reverse the id in the tree.
    T.nodes.push_back(v);

    // Subtree -> Leaf | Internal

    // Is this an internal node or a leaf?
    if(tree_encoding[left] == '('){ // Internal node

        // Internal -> "(" BranchSet ")" Name

        int64_t open = left;
        int64_t close = find_close(tree_encoding, left);
        string name = tree_encoding.substr(close+1, right - (close+1) + 1);
        v.name = name;

        // BranchSet → Branch | Branch "," BranchSet
        int64_t branchset_start = open+1;
        int64_t branchset_end = close-1;

        int64_t branch_start = branchset_start;
        while(true){
            
            int64_t branch_end = next_comma(tree_encoding, branch_start, branchset_end) - 1;
            // Branch → Subtree Length
            // Length → empty | ":" number

            // Where does the subtree string end?
            // Two cases: 
            //   1) Subtree is an internal node. This is the case iff is starts with '('. Then the end is a the matching ')'
            //   2) Substree is a leaf. This is the case if (1) is not the case. Then we search for a ':'. If not found,
            //      then there is no length parameter and the end is at the end of the branch. Otherwise, the end
            //      is at the ':'

            if(tree_encoding[branch_start] == '('){
                // Subtree is an internal node
                int64_t subtree_start = branch_start;
                int64_t subtree_end = find_close(tree_encoding, subtree_start);
                if(subtree_end < branch_end){
                    // Length is present
                    string length = tree_encoding.substr(subtree_end+1+1, branch_end - subtree_end - 1);
                    v.children_edge_lengths.push_back(stod(length));
                }
                int64_t child_id = traverse_subtree(tree_encoding, subtree_start, subtree_end, v.id);
                v.children_ids.push_back(child_id);
            } else{
                // Subtree is a leaf
                int64_t colon_pos = find_colon(tree_encoding, branch_start, branch_end);
                int64_t subtree_start = branch_start;
                int64_t subtree_end = branch_end;
                if(colon_pos != -1){
                    subtree_end = colon_pos - 1;
                    string length = tree_encoding.substr(colon_pos + 1, branch_end - (colon_pos+1) + 1);
                    v.children_edge_lengths.push_back(stod(length));
                }
                int64_t child_id = traverse_subtree(tree_encoding, subtree_start, subtree_end, v.id);
                v.children_ids.push_back(child_id);
            }

            branch_start = branch_end + 1 + 1; // Skip over comma or closing ')'
            if(branch_start >= right) break;

        }
    }  else{
        // Leaf → Name
        v.name = tree_encoding.substr(left, right-left+1);
    }

    T.nodes[v.id] = v; // # Update the node
    return v.id;
}

void dfs_leaf_names_left_to_right(int64_t v_id){
    Tree::Node v = T.nodes[v_id];
    if(v.children_ids.size() == 0){
        cout << v.name << endl;
    }
    for(int64_t u_id : v.children_ids){
        dfs_leaf_names_left_to_right(u_id);
    }
}

void print_graph_in_dot_format(){
    cout << "digraph graphname {" << endl;
    for(Tree::Node v : T.nodes){
        cout << "  " << v.parent_id << " -> " << v.id << ";" << endl;
    }
    cout << "}" << endl;
}


int main(int argc, char** argv){

    string tree_encoding = read_file(argv[1]);

    // Trim trailing whitespace
    while(tree_encoding.back() == ' ' || tree_encoding.back() == '\n') tree_encoding.pop_back();

    traverse_subtree(tree_encoding, 0, tree_encoding.size()-1-1, 0); // Discard the ';' in the end.

    dfs_leaf_names_left_to_right(0);

}