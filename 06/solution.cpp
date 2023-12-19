
/**********************
 * Day 6: Wait For It *
 **********************/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "../utils/util.h"

using namespace std;

// set this to false to get part 1 results instead
static const bool PART_2_ENABLED = true;

//loops through all possible hold/travel times and computes how many possible win conditions there are
//This is surprisingly fast, times under 1,000,000,000 are totally manageable.
//It's possible to do this more efficiently with quadratric formula or efficient searching, but it's honestly unnecessary.
long getNumWinConditions(long time, long distance) {
    long winCount = 0;
    for(long i = 0; i <= time; i++) {
        long timeCharging = i;
        long timeTraveling = time - i;
        if(timeCharging * timeTraveling > distance) {
            winCount++;
        }
    }
    return winCount;
}

// Part 1 solution
int getProductOfPossibleWinConditions(vector<vector<string>> wordsInLines) {
    vector<string> timesWords = wordsInLines[0];
    vector<string> distancesWords = wordsInLines[1];

    int product = 1;
    for(int i = 1; i < timesWords.size(); i++) {
        int currentTime = stoi(timesWords[i]);
        int currentDistance = stoi(distancesWords[i]);
        int wins = getNumWinConditions(currentTime, currentDistance);
        product = product * wins;
    }
    return product;
}

// Part 2 solution
long getPossibleWinConditionsPart2(vector<vector<string>> wordsInLines) {
    // combine all the times and distance number strings to get one big time and one big distance
    vector<string> timesWords = wordsInLines[0];
    vector<string> distancesWords = wordsInLines[1];
    string combinedTimes = "";
    string combinedDistances = "";
    for(int i = 1; i < timesWords.size(); i++) {
        combinedTimes = combinedTimes + timesWords[i];
        combinedDistances = combinedDistances + distancesWords[i];
    }
    //parse the string, we need to use long because it is pretty huge
    long combinedTime = stol(combinedTimes);
    long combinedDistance = stol(combinedDistances);

    //Now simply run getNumWinConditions normally on it.
    return getNumWinConditions(combinedTime, combinedDistance);
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
    vector<vector<string>> wordsInLines = parseAllLines(lines);

    // now handle lines to generate the result
    long result;
    if(PART_2_ENABLED) {
        result = getPossibleWinConditionsPart2(wordsInLines);
    } else {
        result = getProductOfPossibleWinConditions(wordsInLines);
    }

    //print final result to console
    cout << "result=" << result << endl;
    return 0;
}