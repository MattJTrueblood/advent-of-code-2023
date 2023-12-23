
/**********************
 * Day N:  ~~~~~~~~~  *
 **********************/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <utility>
#include <algorithm>
#include "../utils/util.h"

using namespace std;

static bool PART_2_ENABLED = true;

// all three possible conditions for springs in the conditions list
enum spring_condition { OPERATIONAL_SPRING=1, DAMAGED_SPRING=2, UNKNOWN_SPRING=3 };

class MyParameters {
public:
    vector<int> conditions;
    vector<int> contiguous;
    int currentBlockDamagedCount;

    MyParameters(vector<int> conditions, vector<int> contiguous, int currentBlockDamagedCount) {
        this->conditions = conditions;
        this->contiguous = contiguous;
        this->currentBlockDamagedCount = currentBlockDamagedCount;
    }

    bool operator==(const MyParameters &other) const {
        return conditions == other.conditions && contiguous == other.contiguous && currentBlockDamagedCount == other.currentBlockDamagedCount;
    }

    struct Hash {
        size_t operator()(const MyParameters& params) const {
            size_t h = 0;
            for (int i : params.conditions) {
                h ^= hash<int>{}(i) + 0x9e3779b9 + (h << 6) + (h >> 2);
            }
            for (int i : params.contiguous) {
                h ^= hash<int>{}(i) + 0x9e3779b9 + (h << 6) + (h >> 2);
            }
            h ^= hash<int>{}(params.currentBlockDamagedCount) + 0x9e3779b9 + (h << 6) + (h >> 2);
            return h;
        }
    };
};

vector<int> parseConditionsList(string s) {
    vector<int> conditions;
    for(int i = 0; i < s.size(); i++) {
        switch(s[i]) {
        case '#':
            conditions.push_back((int) DAMAGED_SPRING);
            break;
        case '.':
            conditions.push_back((int) OPERATIONAL_SPRING);
            break;
        case '?':
            conditions.push_back((int) UNKNOWN_SPRING);
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

int getNumUnknowns(vector<int> conditions) {
    int numUnknowns = 0;
    for(int i = 0; i < conditions.size(); i++) {
        if(conditions[i] == (int) UNKNOWN_SPRING) {
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

int getPossibleArrangements(vector<int> conditions, vector<int> contiguousDamaged) {
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
            if(conditions[j] == (int) UNKNOWN_SPRING) {
                possibleConditions.push_back(bitMask & i ? DAMAGED_SPRING : OPERATIONAL_SPRING);
                bitMask = bitMask << 1;
            } else {
                possibleConditions.push_back((spring_condition) conditions[j]);
            }
        }

        //buffer with one extra operational spring
        possibleConditions.push_back(OPERATIONAL_SPRING);

        if(checkIfPossible(possibleConditions, contiguousDamaged)) {
            numPossibleArrangements++;
        }
    }

    return numPossibleArrangements;
}

int getSumPossibleArrangementCounts(vector<vector<string>> wordLines) {
    int sum = 0;
    for(int i = 0; i < wordLines.size(); i++) {
        vector<int> conditions = parseConditionsList(wordLines[i][0]);
        vector<int> contiguousDamaged = parseContiguousDamagedList(wordLines[i][1]);
        int possibleArrangements = getPossibleArrangements(conditions, contiguousDamaged);
        cout << wordLines[i][0] << ": " << possibleArrangements << endl;
        sum += possibleArrangements; 
    }
    return sum;
}

string unfoldConditionsList(string s) {
    return s + "?" + s + "?" + s + "?" + s + "?" + s;
}

string unfoldContiguousDamagedList(string s) {
    return s + "," + s + "," + s + "," + s + "," + s;
}

long long getPossibleArrangementsRecursiveWithDP(unordered_map<MyParameters, long long, MyParameters::Hash>* memorize, MyParameters params) {

    //check memorization map to see if we need to compute this
    if (memorize->find(params) != memorize->end()) {
        return (*memorize)[params];
    }

    vector<int> conditions = params.conditions;
    vector<int> contiguous = params.contiguous;

    int numDamagedInCurrentBlock = params.currentBlockDamagedCount;

    //walk through known conditions from the front until you find the first unknown term.
    while(!conditions.empty() && conditions.front() != UNKNOWN_SPRING) {
        if(conditions.front() == OPERATIONAL_SPRING) {
            if(numDamagedInCurrentBlock != 0) { //end of a damaged block.
                if(contiguous.empty()) {
                    return 0; // not possible, should have at least one in contiguous.
                }
                if(contiguous.front() != numDamagedInCurrentBlock) {
                    return 0; //this arrangement is not possible.
                }
                contiguous.erase(contiguous.begin()); // pop front
                numDamagedInCurrentBlock = 0;

            }
            conditions.erase(conditions.begin()); // pop front
            continue;
        }
        if(conditions.front() == DAMAGED_SPRING) {
            numDamagedInCurrentBlock++; // start block
            conditions.erase(conditions.begin());
        }
    }

    //check if we've finished the list
    if(conditions.empty()) {
        //finish any spare block we didn't f inish before.
        if(numDamagedInCurrentBlock != 0) {
            if(contiguous.empty()) {
                return 0; // not possible
            }
            if(contiguous.front() != numDamagedInCurrentBlock) {
                return 0;
            }
            contiguous.erase(contiguous.begin()); // pop front
        }

        return contiguous.empty() ? 1 : 0; // if both are empty then this is a possible arrangement.  else it's not possible
    }

    //now we know the conditions.front() is unknown.  This is the complicated part.
    if(conditions.front() == UNKNOWN_SPRING) {
        vector<int> conditionsIfUnknownIsDamaged = conditions;
        conditionsIfUnknownIsDamaged[0] = DAMAGED_SPRING;
        MyParameters unknownIsDamagedParams = MyParameters(conditionsIfUnknownIsDamaged, contiguous, numDamagedInCurrentBlock);

        vector<int> conditionsIfUnknownIsOperational = conditions;
        conditionsIfUnknownIsOperational[0] = OPERATIONAL_SPRING;
        MyParameters unknownIsOperationalParams = MyParameters(conditionsIfUnknownIsOperational, contiguous, numDamagedInCurrentBlock);

        long long unknownIsDamagedResult = getPossibleArrangementsRecursiveWithDP(memorize, unknownIsDamagedParams);
        long long unknownIsOperationalResult = getPossibleArrangementsRecursiveWithDP(memorize, unknownIsOperationalParams);

        (*memorize)[unknownIsDamagedParams] = unknownIsDamagedResult;
        (*memorize)[unknownIsOperationalParams] = unknownIsOperationalResult;

        return unknownIsDamagedResult + unknownIsOperationalResult;
    }

    cout << "something went wrong, we should have triggered at least one of the conditions above." << endl;
    return -1; // error result
}

long long getPossibleArrangementsRecursive(vector<int> conditions, vector<int> contiguousDamaged) {
    unordered_map<MyParameters, long long, MyParameters::Hash> memorize;
    // Start with 0 remaining in the current block
    MyParameters startParams = MyParameters(conditions, contiguousDamaged, 0);
    long long result = getPossibleArrangementsRecursiveWithDP(&memorize, startParams);
    return result;
}


// our original solution won't work for very large strings.  we'll need to use some sort of DP/recursion instead.
long long getSumPossibleArrangementCountsUnfolded(vector<vector<string>> wordLines) {
    long long sum = 0;
    for(int i = 0; i < wordLines.size(); i++) {
        //parse the data
        string unfoldedConditions = unfoldConditionsList(wordLines[i][0]);
        vector<int> conditions = parseConditionsList(unfoldedConditions);
        string unfoldedContiguousDamaged = unfoldContiguousDamagedList(wordLines[i][1]);
        vector<int> contiguousDamaged = parseContiguousDamagedList(unfoldedContiguousDamaged);

        vector<int> originalConditions = parseConditionsList(wordLines[i][0]);
        vector<int> originalContiguousDamaged = parseContiguousDamagedList(wordLines[i][1]);

        //call the recursive function this time.
        long long possibleArrangements = getPossibleArrangementsRecursive(conditions, contiguousDamaged);
        cout << wordLines[i][0] << "; " << wordLines[i][1] << ":  " << possibleArrangements << endl;
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

    long long result;

    // now handle lines to generate the result
    if(PART_2_ENABLED) {
        result = getSumPossibleArrangementCountsUnfolded(wordLines);
    } else {
        result = getSumPossibleArrangementCounts(wordLines);
    }
    //print final result to console
    cout << "result=" << result << endl;
    return 0;
}