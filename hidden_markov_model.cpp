// hidden_markov_model.cpp
// Song Li
// 12/6/2021

#include <iostream>
#include <utility>
#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <sstream>

using namespace std;

using std::cin; using std::cout; using std::endl;
using std::vector;
using std::make_pair;
using std::map;
using std::string;

class Node {
public:
    Node(int num, double initProb): num_(num), initProb_(initProb){}
    int getNum() {return num_;}
    double getInitprob() {return initProb_;}

    void setTransition(Node* node, double prob){
        auto ret = transitions_.insert(make_pair(node, prob));
        if(!ret.second)
            cout << "This transition alrady exists: prob = " << transitions_[node] << endl;
    }
    
    void setEmission(char ch, double prob){
        auto ret = emissions_.insert(make_pair(ch, prob));
        if(!ret.second)
            cout << "This emission alrady exists: prob = " << emissions_[ch] << endl;
    }

    double getTransitionProb(Node* node){
        return transitions_[node];
    }

    double getEmissionProb(char ch){
        return emissions_[ch];
    }

    bool checkTransition(Node* node){
        if(this->getTransitionProb(node) == 0.0){
            return false;
        }
        return true;
    }

    void print(){
        cout << num_;
    }

private:
    int num_;
    double initProb_;
    map<Node*, double> transitions_;
    map<char, double> emissions_;
};

vector<vector<Node*>> CartesianProduct(vector<vector<Node*>> node_lsts, vector<Node*> node_lst){
    vector<vector<Node*>> result;
    if(node_lsts.empty()){
        for(int i = 0; i < node_lst.size(); i++){
            vector<Node*> temp;
            temp.push_back(node_lst[i]);
            result.push_back(temp);
        }
    }
    else{
        for(int i = 0; i < node_lsts.size(); i++){
            vector<Node*> temp = node_lsts[i];
            for(int j = 0; j < node_lst.size(); j++){
                temp.push_back(node_lst[j]);
                result.push_back(temp);
                temp.pop_back();
            }

        }
    }
    return result;
}

void FindMostProbablePath(vector<Node*> nodes, char seq[], int seq_size){
    // output emission sequence -> emission conditions
    vector<vector<Node*>> node_lsts;
    for(int i = 0; i < seq_size; i++){
        vector<Node*> node_lst;
        for(const auto& e: nodes){
            if(e->getEmissionProb(seq[i]) != 0.0){
                node_lst.push_back(e);
            }
        }
        node_lsts.push_back(node_lst);
    }

    // print emission conditions
    cout << "Emission Conditions:" << endl;
    for(int i = 0; i < node_lsts.size(); i++){
        cout << seq[i] << ": ";
        for(int j = 0; j < node_lsts[i].size(); j++){
            node_lsts[i][j]->print();
        }
        cout << endl;
    }
    cout << endl;

    /*
    */

    // execute cartesian product to find all paths
    vector<vector<Node*>> paths;
    for(const auto& e: node_lsts){
        paths = CartesianProduct(paths, e); // successively cartesian product each of the element
    }

    // print all paths
    cout << "All Paths:" << endl;
    for(const auto& path: paths){
        for(const auto& e: path){
            e->print();   
        }
        cout << endl;
    }
    cout << endl;

    /*
    */

    // remove invalid paths
    bool found = false;
    for(auto it = paths.begin(); it != paths.end();){
        for(int i = 0; i < (*it).size()-1; i++){
            if((*it)[i]->checkTransition((*it)[i+1]) == false){
                found = true;
                i = (*it).size()-1; // exit inner loop
            }
        }

        // below statement mark the paths end at 4 as to be removed
        if((*it).back()->getNum() != 4){
            found = true;
        }
        
        if(found){
            it = paths.erase(it); // syntax to erase from a vector
            found = false; //debug*//
        }
        else{
            it++;
        }
    }

    // print valid paths
    cout << "Valid Paths:" << endl;
    for(const auto& path: paths){
        for(const auto& e: path){
            e->print();
        }
        cout << endl;
    }
    cout << endl;

    /*
    */

    // compute the probabilities of valid paths
    vector<double> probs;
    for(const auto& path: paths){
        double prob = 1.0;
        for(int i = 0; i < path.size()-1; i++){
            prob = prob * path[i]->getEmissionProb(seq[i]);
            prob = prob * path[i]->getTransitionProb(path[i+1]);
        }
        // this one is excluded from the loop above
        prob = prob * path[path.size()-1]->getEmissionProb(seq[path.size()-1]);
        prob = prob * path[0]->getInitprob(); // multiply initial probability
        probs.push_back(prob);
    }

    // print valid paths with probabilities
    cout << "Valid Paths with Probabilities:" << endl;
    for(int i = 0; i < paths.size(); i++){
        for(const auto& e: paths[i]){
            e->print();
        }
        cout << ", "<< probs[i] << endl;
    }
    cout << endl;

    /*
    */

    // identify the path with the highiest probabilty
    double max = 0.0;
    int index = 0;
    for(int i = 0; i < probs.size(); i++){
        if(probs[i] > max){
            max = probs[i];
            index = i;
        }
    }

    // print the path with the highiest probabilty
    cout << "The Most Probable Path:" << endl;
    for(const auto& e: paths[index]){
            e->print();
    }
    cout << ", " << probs[index] << endl;

}

void spaces(int num){
    for(int i = 0; i < num; i++){
        cout << " ";
    }
}

int main() {

    /*
    // Creating Objects
    */

    // initial nodes with initial probabilities:
    Node* one = new Node(1, 0.3);
    Node* two = new Node(2, 0.4);
    Node* three = new Node(3, 0.3);
    Node* four = new Node(4, 0.0);
    
    vector<Node*> nodes;    
    nodes.push_back(one);
    nodes.push_back(two);
    nodes.push_back(three);
    nodes.push_back(four);

    // emission sequence
    char seq[4] = {'A', 'B', 'D', 'C'};

    /*
    // Reading Files
    */

    string f1_str = "transition_matrix.csv";
    string f2_str = "emission_matrix.csv";
    
    vector<vector<string>> transition_data;
    vector<vector<string>> emission_data;

    vector<string> vos;
    string line, word;

    fstream f1 (f1_str, ios::in);
    fstream f2 (f2_str, ios::in);

    if(f1.is_open()){
        while(getline(f1, line)){
            vos.clear();
            stringstream str(line);
            while(getline(str, word, ','))
                vos.push_back(word);
            transition_data.push_back(vos);
        }
    }
    else
        cout << "Unable to open file: transition_matrix.csv" << endl;

    if(f2.is_open()){
        while(getline(f2, line)){
            vos.clear();
            stringstream str(line);
            while(getline(str, word, ','))
                vos.push_back(word);
            emission_data.push_back(vos);
        }
    }
    else
        cout << "Unable to open file: emission_matrix.csv" << endl;

    /*
    // Merging Data Into Node Objects
    */

    // merge transition data
    for(int i = 0; i < transition_data.size(); i++){
        for(int j = 0; j < transition_data[i].size(); j++){
            double d = stod(transition_data[i][j]);
            nodes[i]->setTransition(nodes[j], d);
        }
    }
 
    // merge emission data
    for(int i = 0; i < emission_data.size(); i++){
        for(int j = 0; j < emission_data[i].size(); j++){
            double d = stod(emission_data[i][j]);
            nodes[i]->setEmission(seq[j], d);
        }
    }
    
    /*
    // Graph Modeling
    */

    // Transition Matrix
    cout << "Transition Matrix: " << endl;
    cout << "      ";
    for(const auto& src: nodes){
        cout << src->getNum() << "      ";
    }
    cout << endl;
    for(const auto& src: nodes){
        cout << src->getNum() << "     ";
        for(const auto& tar: nodes){
            double d = src->getTransitionProb(tar);
            std::string str = std::to_string(d);
            while(str.back() == '0'){
                str.pop_back();
            }

            cout << str;
            spaces(7 - str.length());
        }
        cout << endl;
    }

    // Emission Matrix
    cout << "Emission Matrix: " << endl;
    cout << "      ";
    for(int i = 0; i < sizeof(seq); i++){
        cout << seq[i] << "      ";
    }
    cout << endl;

    for(const auto& src: nodes){
        cout << src->getNum() << "     ";
        for(int i = 0; i < sizeof(seq); i++){
            double d = src->getEmissionProb(seq[i]);
            std::string str = std::to_string(d);
            while(str.back() == '0'){
                str.pop_back();
            }

            cout << str;
            spaces(7 - str.length());
        }
        cout << endl;
    }
    cout << endl;

    /*
    // Finding Most Probable Path
    */

    FindMostProbablePath(nodes, seq, sizeof(seq));
}