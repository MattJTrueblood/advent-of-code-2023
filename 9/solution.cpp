
/*******************************
 * Day 9:  Mirage Maintenance  *
 *******************************/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "../utils/util.h"

using namespace std;

static const bool PART_2_ENABLED = true;

// parse each word in the sequence as an int and return the resulting sequence
vector<int> parseSequence(vector<string> wordLine) {
    vector<int> sequence;
    for(int i = 0; i < wordLine.size(); i++) {
        sequence.push_back(stoi(wordLine[i]));
    }
    return sequence;
}

// generate the next sequence in a set by calculating the differences between each adjacent pair
vector<int> generateNextSequence(vector<int> previousSequence) {
    vector<int> nextSequence;
    for(int i = 0; i < previousSequence.size() - 1; i++) {
        nextSequence.push_back(previousSequence[i + 1] - previousSequence[i]);
    }
    return nextSequence;
}

// return true if all values in a sequence are zero, else return false
bool isAllZeroSequence(vector<int> sequence) {
    for(int i = 0; i < sequence.size(); i++) {
        if(sequence[i] != 0) {
            return false;
        }
    }
    return true;
}

// generate the list of diff sequences from a given input line and return it
vector<vector<int>> parseAndGenerateSequences(vector<string> wordLine) {
    //parse initial sequence
    vector<vector<int>> sequences;
    sequences.push_back(parseSequence(wordLine));

    //generate additional sequences until you reach the all-zero sequence.
    while(true) {
        vector<int> nextSequence = generateNextSequence(sequences.back());
        sequences.push_back(nextSequence);
        if(isAllZeroSequence(nextSequence)) {
            break;
        }
    }
    return sequences;
}

// calculate the next value in the original sequence using the full diff sequence list
long long extrapolateNextValue(vector<vector<int>> sequences) {
    int extrapolatedValue = 0;
    for(int i = sequences.size() - 2; i >= 0; i--) { // start at second-last sequence
        vector<int> currentSequence = sequences[i];
        extrapolatedValue = currentSequence.back() + extrapolatedValue;
    }
    return extrapolatedValue;
}

// calculate the previous value in the original sequence using the full diff sequence list
long long extrapolatePrevValue(vector<vector<int>> sequences) {
    int extrapolatedValue = 0;
    for(int i = sequences.size() - 2; i >= 0; i--) { // start at second-last sequence
        vector<int> currentSequence = sequences[i];
        extrapolatedValue = currentSequence[0] - extrapolatedValue;
    }
    return extrapolatedValue;
}

// find all the extrapolated values (next for part 1, previous for part 2) and return their sum
long long getSumExtrapolatedValues(vector<vector<string>> wordLines) {
    long long sum = 0;
    for(int i = 0; i < wordLines.size(); i++) {
        vector<vector<int>> sequences = parseAndGenerateSequences(wordLines[i]);
        if(PART_2_ENABLED) {
            sum += extrapolatePrevValue(sequences);
        } else {
            sum += extrapolateNextValue(sequences);
        }
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
    long long result = getSumExtrapolatedValues(wordLines);

    //print final result to console
    cout << "result=" << result << endl;
    return 0;
}