
/*****************************
 * Day 8:  Haunted Wasteland *
 *****************************/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <numeric>
#include "../utils/util.h"

using namespace std;

const static bool PART_2_ENABLED = true;

class MyNode {
public:
    string name;
    MyNode * left;
    MyNode * right;
    bool ZNode = false; // used in part 2

    MyNode() {};

    MyNode(string s) {
        name = s;
    }
};

int findNumStepsToEnd(vector<vector<string>> wordLines) {
    string leftRightLine = wordLines[0][0];

    map<string, MyNode*> nodes;

    // initialize each node in the map
    for(int i = 2; i < wordLines.size(); i++) {
        string firstWord = wordLines[i][0];
        MyNode * node = new MyNode(firstWord);
        nodes[firstWord] = node;
    }

    // now loop through again and set the relationships between nodes
    for(int i = 2; i < wordLines.size(); i++) {
        string firstWord = wordLines[i][0];
        string leftWord = wordLines[i][2].substr(1,3);
        string rightWord = wordLines[i][3].substr(0,3);

        nodes[firstWord]->left = nodes[leftWord];
        nodes[firstWord]->right = nodes[rightWord];
    }

    int count = 0;
    bool foundExit = false;
    MyNode * currentNode = nodes["AAA"];
    while(!foundExit) {
        for(int i = 0; i < leftRightLine.size(); i++) {
            char c = leftRightLine[i];
            if(c == 'L') {
                //cout << "traveling from " << currentNode->name << " to " << currentNode->left->name << endl;
                currentNode = currentNode->left;
            } else if(c == 'R') {
                //cout << "traveling from " << currentNode->name << " to " << currentNode->right->name << endl;
                currentNode = currentNode->right;
            }
            count++;
            if(currentNode->name == "ZZZ") {
                foundExit = true;
                break;
            }
        }
    }

    return count;
}

// left = false; right= true;
// used in part 2 to avoid repeated string comparisons
vector<bool> parseLeftRightString(string s) {
    vector<bool> leftRights;
    for(int i = 0; i < s.size(); i++) {
        if(s[i] == 'L') {
            leftRights.push_back(false);
        } else if (s[i] == 'R') {
            leftRights.push_back(true);
        }
    }
    return leftRights;
}

// part 2 solution
long long findNumGhostStepsToEnd(vector<vector<string>> wordLines) {
    string leftRightLine = wordLines[0][0];
    vector<bool> leftRights = parseLeftRightString(leftRightLine);

    map<string, MyNode*> nodes;
    vector<MyNode *> startNodes;

    // initialize each node in the map and add start nodes
    for(int i = 2; i < wordLines.size(); i++) {
        string firstWord = wordLines[i][0];
        MyNode * node = new MyNode(firstWord);
        nodes[firstWord] = node;
        if(firstWord[2] == 'Z') {
            node->ZNode = true;
        }
        if(firstWord[2] == 'A') {
            startNodes.push_back(node);
        }
    }

    // now loop through again and set the relationships between nodes
    for(int i = 2; i < wordLines.size(); i++) {
        string firstWord = wordLines[i][0];
        string leftWord = wordLines[i][2].substr(1,3);
        string rightWord = wordLines[i][3].substr(0,3);
        nodes[firstWord]->left = nodes[leftWord];
        nodes[firstWord]->right = nodes[rightWord];
    }

    // go from each start node to a Z node and find the distance it takes for each
    vector<int> distances;
    for(int k = 0; k < startNodes.size(); k++) {
        int count = 0;
        bool foundExit = false;
        MyNode * currentNode = startNodes[k];
        while(!foundExit) {
            for(int i = 0; i < leftRights.size(); i++) {
                if(!leftRights[i]) {
                    currentNode = currentNode->left;
                } else {
                    currentNode = currentNode->right;
                }
                count++;
                if(currentNode->ZNode) {
                    foundExit = true;
                    break;
                }
            }
        }
        distances.push_back(count);
        cout << startNodes[k]->name << " to " << currentNode->name << " takes " << count << " steps." << endl;
    }

    // Compute the least common multiple to get the count where they converge
    // have to convert everything to long longs, the result from my input can exceed 14 digits!
    return std::accumulate(distances.begin(), distances.end(), 1LL, 
            [](long long a, long long b) { return lcm(a, b); });
}

int main(int argc, char* argv[]) {
    //check arguments
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <filename>\n";
        return 1;
    }
    cout << "Filename: " << argv[1] << endl;

    //parse file
    vector<string> lines = parseFile(argv[1]);
    vector<vector<string>> wordLines = parseAllLines(lines);

    // now handle lines to generate the result
    long long result;
    if(PART_2_ENABLED) {
        result = findNumGhostStepsToEnd(wordLines);
    } else {
        result = (long long) findNumStepsToEnd(wordLines);
    }
    //print final result to console
    cout << "result=" << result << endl;
    return 0;
}