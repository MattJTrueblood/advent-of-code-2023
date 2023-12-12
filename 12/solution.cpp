
/**********************
 * Day N:  ~~~~~~~~~  *
 **********************/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include "../utils/util.h"

using namespace std;

enum spring_condition { OPERATIONAL_SPRING=1, DAMAGED_SPRING=2, UNKNOWN_SPRING=3 };

vector<spring_condition> parseConditionsList(string s) {
    vector<spring_condition> conditions;
    for(int i = 0; i < s.size(); i++) {
        switch(s[i]) {
        case '#':
            conditions.push_back(DAMAGED_SPRING);
            break;
        case '.':
            conditions.push_back(OPERATIONAL_SPRING);
            break;
        case '?':
            conditions.push_back(UNKNOWN_SPRING);
            break;
        default:
            break;
        }
    }
    return conditions;
}

vector<int> parseContiguousDamagedList(string s) {
    vector<int> contiguous;
    stringstream stringstream(s);
    string word;
    while(getline(stringstream, word, ',')) {
        contiguous.push_back(stoi(word));
    }
    return contiguous;
}

int getNumUnknowns(vector<spring_condition> conditions) {
    int numUnknowns = 0;
    for(int i = 0; i < conditions.size(); i++) {
        if(conditions[i] == UNKNOWN_SPRING) {
            numUnknowns++;
        }
    }
    return numUnknowns;
}

bool checkIfPossible(vector<spring_condition> possibleConditions, vector<int> contiguousDamaged) {
    int currentContiguousIndex = 0;
    bool insideDamagedBlock = false;
    int currentBlockDamagedCount = 0;
    for(int i = 0; i < possibleConditions.size(); i++) {
        if(possibleConditions[i] == DAMAGED_SPRING) {
            insideDamagedBlock = true;
            currentBlockDamagedCount++;
        } else {
            if(insideDamagedBlock) {
                if(currentContiguousIndex == contiguousDamaged.size()) {
                    return false;
                }
                int expectedContiguous = contiguousDamaged[currentContiguousIndex];
                if(expectedContiguous != currentBlockDamagedCount) {
                    return false;
                }
                //now reset for the next block
                insideDamagedBlock = false;
                currentBlockDamagedCount = 0;
                currentContiguousIndex++;
            }
        }
    }
    return currentContiguousIndex == contiguousDamaged.size();
}

int getPossibleArrangements(vector<spring_condition> conditions, vector<int> contiguousDamaged) {
    int numUnknowns = getNumUnknowns(conditions);
    int numArrangementsToCheck = 1 << numUnknowns; // 2^(numUnknowns)

    int numPossibleArrangements = 0;
    for(int i = 0; i < numArrangementsToCheck; i++) {
        vector<spring_condition> possibleConditions;
        
        // Get the i-th possible arrangement.  The trick is, we use a bit mask on i itself.
        // for example, if numUnknowns =4, i could be 0b00, 0b01, 0b10, or 0b11. If we treat 0 as damaged and 1 as operational,
        // we can simply read the bits of i to get the i-ith possible arrangement of unknowns.
        int bitMask = 0b1;
        for(int j = 0; j < conditions.size(); j++) {
            if(conditions[j] == UNKNOWN_SPRING) {
                possibleConditions.push_back(bitMask & i ? DAMAGED_SPRING : OPERATIONAL_SPRING);
                bitMask = bitMask << 1;
            } else {
                possibleConditions.push_back(conditions[j]);
            }
        }

        //buffer with one extra operational spring
        possibleConditions.push_back(OPERATIONAL_SPRING);

        if(checkIfPossible(possibleConditions, contiguousDamaged)) {
            /*
            cout << "possible: ";
            for(int j = 0; j < possibleConditions.size(); j++) {
                if(possibleConditions[j] == DAMAGED_SPRING) cout << "#";
                if(possibleConditions[j] == OPERATIONAL_SPRING) cout << ".";
            }
            cout << endl;
            */
            numPossibleArrangements++;
        }
    }

    return numPossibleArrangements;
}

int getSumPossibleArrangementCounts(vector<vector<string>> wordLines) {
    int sum = 0;
    for(int i = 0; i < wordLines.size(); i++) {
        vector<spring_condition> conditions = parseConditionsList(wordLines[i][0]);
        vector<int> contiguousDamaged = parseContiguousDamagedList(wordLines[i][1]);
        int possibleArrangements = getPossibleArrangements(conditions, contiguousDamaged);
        //cout << possibleArrangements << endl;
        sum += possibleArrangements; 
    }
    return sum;
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
    int result = getSumPossibleArrangementCounts(wordLines);

    //print final result to console
    cout << "result=" << result << endl;
    return 0;
}