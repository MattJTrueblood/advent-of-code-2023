
/**********************
 * Day N:  ~~~~~~~~~  *
 **********************/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <utility>
#include "../utils/util.h"

using namespace std;

static const bool PART_2_ENABLED = true;
static const int EXPANSION_DISTANCE = 1000000;

// adds an extra row or column between empty rows.  Marks it with '+' characters
vector<string> expandSpace(vector<string> lines) {
    vector<string> expandedLines;

    //first expand rows
    for(int i = 0; i < lines.size(); i++) {
        bool emptyRow = true;
        for(int j = 0; j < lines[0].size(); j++) {
            if(lines[i][j] == '#') {
                emptyRow = false;
                break;
            }
        }
        expandedLines.push_back(lines[i]);
        if(emptyRow) {
            expandedLines.push_back(string(lines[i].size(), '+'));
        }
    }

    //now expand columns.  Go backwards to prevent new columns from messing up the j coordinate
    for(int j = lines[0].size() - 1; j >= 0; j--) {
        bool emptyCol = true;
        for(int i = 0; i < lines.size(); i++) {
            if(lines[i][j] == '#') {
                emptyCol = false;
                break;
            }
        }
        if(emptyCol) {
            for(int i = 0; i < expandedLines.size(); i++) {
                expandedLines[i].insert(j, charToString('+')); // mark expanded space with '+'
            }
        }
    }

    return expandedLines;
}

// find the basic coordinates of each '#' tile in the grid
vector<pair<int, int>> getGalaxyCoords(vector<string> lines) {
    vector<pair<int, int>> galaxyCoords;
    for(int i = 0; i < lines.size(); i++) {
        for(int j = 0; j < lines[0].size(); j++) {
            if(lines[i][j] == '#') {
                galaxyCoords.push_back(make_pair(i, j));
            }
        }
    }
    return galaxyCoords;
}

// use '+' tiles to calculate expansion with EXPANSION_DISTANCE expansion rate instead of 2.
vector<pair<int, int>> getMuchOlderGalaxyCoords(vector<string> lines) {
    vector<pair<int, int>> galaxyCoords;
    int rowMod = 0;
    for(int i = 0; i < lines.size(); i++) {
        if(lines[i][0] == '+') {
            rowMod += EXPANSION_DISTANCE - 2; // -2 because we have the original . row and the new + row
            continue;
        }
        int colMod = 0;
        for(int j = 0; j < lines[0].size(); j++) {
            if(lines[i][j] == '+') {
                colMod += EXPANSION_DISTANCE - 2; // -2 for same reason as rowMod
            }
            else if(lines[i][j] == '#') {
                galaxyCoords.push_back(make_pair(i + rowMod, j + colMod));
            }
        }
    }
    return galaxyCoords;
}

// find all the distances between each pair of galaxies
vector<int> getGalaxyDistances(vector<pair<int, int>> galaxyCoords) {
    vector<int> distances;
    for(int i = 0; i < galaxyCoords.size() - 1; i++) {
        for(int j = i + 1; j < galaxyCoords.size(); j++) {
            int rowDiff = abs(galaxyCoords[i].first - galaxyCoords[j].first);
            int colDiff = abs(galaxyCoords[i].second - galaxyCoords[j].second);
            distances.push_back(rowDiff + colDiff);
        }
    }
    return distances;
}

// sum all the distances
long getSumGalaxyDistances(vector<int> distances) {
    long sum = 0;
    for(int i = 0; i < distances.size(); i++) {
        sum += distances[i];
    }
    return sum;
}

// part 1 solution with original getGalaxyCoords
long getSumOfShortestPaths(vector<string> lines) {
    vector<string> expandedLines = expandSpace(lines);

    vector<pair<int, int>> galaxyCoords = getGalaxyCoords(expandedLines);
    vector<int> galaxyDistances = getGalaxyDistances(galaxyCoords);

    return getSumGalaxyDistances(galaxyDistances);
}

// part 2 solution using new getMuchOlderGalaxyCoords
long getSumOfShortestPathsPart2(vector<string> lines) {
    vector<string> expandedLines = expandSpace(lines);

    vector<pair<int, int>> galaxyCoords = getMuchOlderGalaxyCoords(expandedLines);
    vector<int> galaxyDistances = getGalaxyDistances(galaxyCoords);

    return getSumGalaxyDistances(galaxyDistances);
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

    // now handle lines to generate the result
    long result;
    if(PART_2_ENABLED) {
        result = getSumOfShortestPathsPart2(lines);
    } else{
        result = getSumOfShortestPaths(lines);
    }

    //print final result to console
    cout << "result=" << result << endl;
    return 0;
}