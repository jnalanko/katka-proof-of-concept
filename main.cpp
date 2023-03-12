#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>

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

    T.nodes.push_back(v);
    return v.id;
}


int main(int argc, char** argv){
    string tree_encoding = read_file(argv[1]);

    // Trim trailing whitespace
    while(tree_encoding.back() == ' ' || tree_encoding.back() == '\n') tree_encoding.pop_back();

    traverse_subtree(tree_encoding, 0, tree_encoding.size()-1-1, 0); // Discard the ';' in the end.

    for(Tree::Node v : T.nodes){
        cout << v.id << " " << v.parent_id << endl;
    }
}