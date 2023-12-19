
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

enum spring_condition { OPERATIONAL_SPRING=1, DAMAGED_SPRING=2, UNKNOWN_SPRING=3 };

static bool PART_2_ENABLED = true;

// this is used as a key for the memoization table.  vec1 is conditions, vec2 is contiguous.  
class MyParameters {
public:
    std::vector<int> vec1;
    std::vector<int> vec2;

    MyParameters(const std::vector<int>& v1, const std::vector<int>& v2) : vec1(v1), vec2(v2) {}

    bool operator==(const MyParameters& other) const {
        return vec1 == other.vec1 && vec2 == other.vec2;
    }

    struct Hash {
        std::size_t operator()(const MyParameters& params) const {
            std::size_t hash = 0;
            std::size_t prime = 31; // A prime number used in the hash combination

            for (int i : params.vec1) {
                hash = hash * prime + std::hash<int>()(i);
            }
            for (int i : params.vec2) {
                hash = hash * prime + std::hash<int>()(i);
            }

            return hash;
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
    //todo:  use memorize to cache results for, and retrieve cached results of, this function

    vector<int> conditions = params.vec1;
    vector<int> contiguous = params.vec2;
    long long possibleArrangements = 0;

    // walk through conditions/contiguous backwards, slowly reducing the list, until you encounter unknown springs.
    // call this recursive function on the two possibilities (unknown is damaged or undamaged)
    while(conditions.size() > 0) {
        int currentCondition = conditions.back();
        if(currentCondition == (int) DAMAGED_SPRING) {
            int currentContiguous = contiguous.back();
            if(currentContiguous > 1) {
                contiguous.back() = currentContiguous - 1;
            } else {
                contiguous.pop_back();
            }
        } else if(currentCondition == (int) UNKNOWN_SPRING) {
            vector<int> conditionsIfDamaged = conditions;
            conditionsIfDamaged.back() = (int) DAMAGED_SPRING;
            MyParameters paramsIfDamaged = MyParameters(conditionsIfDamaged, contiguous);
            possibleArrangements += getPossibleArrangementsRecursiveWithDP(memorize, paramsIfDamaged);

            vector<int> conditionsIfOperational = conditions;
            conditionsIfOperational.back() = (int) OPERATIONAL_SPRING;
            MyParameters paramsIfOperational = MyParameters(conditionsIfOperational, contiguous);
            possibleArrangements += getPossibleArrangementsRecursiveWithDP(memorize, paramsIfOperational);
        } //else if operational we don't need to do anything
        conditions.pop_back();
    }

    return possibleArrangements;
}

long long getPossibleArrangementsRecursive(vector<int> conditions, vector<int> contiguousDamaged) {
    unordered_map<MyParameters, long long, MyParameters::Hash>* memorize = new unordered_map<MyParameters, long long, MyParameters::Hash>();
    // reversing the lists at the start makes this more efficient (can effecively pop from front using pop_back)
    reverse(conditions.begin(), conditions.end());
    reverse(contiguousDamaged.begin(), contiguousDamaged.end());
    MyParameters params = MyParameters(conditions, contiguousDamaged);
    long long result = getPossibleArrangementsRecursiveWithDP(memorize, params);
    delete memorize;
    return result;
}

// our original solution won't work for very large strings.  we'll need to use some sort of DP/recursion instead.
long long getSumPossibleArrangementCountsUnfolded(vector<vector<string>> wordLines) {
    int sum = 0;
    for(int i = 0; i < wordLines.size(); i++) {
        //parse the data
        //string unfoldedConditions = unfoldConditionsList(wordLines[i][0]);
        //vector<int> conditions = parseConditionsList(unfoldedConditions);
        //string unfoldedContiguousDamaged = unfoldContiguousDamagedList(wordLines[i][1]);
        //vector<int> contiguousDamaged = parseContiguousDamagedList(unfoldedContiguousDamaged);

        //call the recursive function this time.
        //long long possibleArrangements = getPossibleArrangementsRecursive(conditions, contiguousDamaged);
        long long possibleArrangements = getPossibleArrangementsRecursive(
                parseConditionsList(wordLines[i][0]),
                parseContiguousDamagedList(wordLines[i][1])
        );
        cout << wordLines[i][0] << ": " << possibleArrangements << endl;
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