
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
static const bool PART_2_ENABLED = false;

int getNumWinConditions(int time, int distance) {
    int winCount = 0;
    for(int i = 0; i <= time; i++) {
        int timeCharging = i;
        int timeTraveling = time - i;
        if(timeCharging * timeTraveling > distance) {
            cout << "wait " << timeCharging << ", travel " << timeTraveling << endl;
            winCount++;
        }
    }
    return winCount;
}

int getProductOfPossibleWinConditions(vector<vector<string>> wordsInLines) {
    vector<string> timesWords = wordsInLines[0];
    vector<string> distancesWords = wordsInLines[1];

    int product = 1;
    for(int i = 1; i < timesWords.size(); i++) {
        int currentTime = stoi(timesWords[i]);
        int currentDistance = stoi(distancesWords[i]);
        int wins = getNumWinConditions(currentTime, currentDistance);
        cout << "wins: " << wins << endl;
        product = product * wins;
    }
    return product;
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
    int result;
    if(PART_2_ENABLED) {
        result = -1; //todo
    } else {
        result = getProductOfPossibleWinConditions(wordsInLines);
    }

    //print final result to console
    cout << "result=" << result << endl;
    return 0;
}