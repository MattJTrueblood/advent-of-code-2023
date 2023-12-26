
/**********************
 * Day N:  ~~~~~~~~~  *
 **********************/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <limits>
#include <queue>
#include "../utils/util.h"

using namespace std;

static const bool PART_2_ENABLED = false;

enum tile_type { ROCK=0, GARDEN=1, START=2 };

vector<vector<tile_type>> parseGrid(vector<string> lines, pair<int, int> &startLocation) {
    vector<vector<tile_type>> grid;
    for(int i = 0; i < lines.size(); i++) {
        vector<tile_type> row;
        for(int j = 0; j < lines[i].size(); j++) {
            if(lines[i][j] == '#') {
                row.push_back(ROCK);
            } else if(lines[i][j] == '.') {
                row.push_back(GARDEN);
            } else if(lines[i][j] == 'S') {
                row.push_back(START);
                startLocation.first = i;
                startLocation.second = j;
            }
        }
        grid.push_back(row);
    }
    return grid;
}

vector<vector<long long>> createDistanceGrid(int numRows, int numCols) {
    cout << numRows << "," << numCols << endl;
    long long maxValue = numeric_limits<long long>::max();
    vector<vector<long long>> distances(numRows, vector<long long>(numCols, maxValue));
    return distances;
}

vector<vector<bool>> createVisitedGrid(int numRows, int numCols) {
    vector<vector<bool>> visited(numRows, vector<bool>(numCols, false));
    return visited;
}

long long countCheckerboardTilesWithDistanceLessThan(vector<vector<long long>> distances, vector<vector<tile_type>> grid, int numSteps) {
    long long count = 0;
    for(int i = 0; i < distances.size(); i++) {
        for(int j = 0; j < distances.size(); j++) {
            if(((i % 2 != 0) == (j % 2 != 0)) && distances[i][j] <= numSteps) {
                if(grid[i][j] == START) {
                    printColor("S", BLUE, BLACK);
                } else {
                    printColor("O", GREEN, BLACK);
                }
                count++;
            } else {
                if(grid[i][j] == ROCK) {
                    printColor("#", WHITE, BLACK);
                } else if(grid[i][j] == GARDEN) {
                    printColor(".", RED, BLACK);
                }
            }
        }
        cout << endl;
    }
    return count;
}

long long part2(vector<string> lines) {
    // TODO
    return -1;
}

long long part1(vector<string> lines) {
    //initialize the grids
    pair<int, int> startLocation = {0, 0};
    vector<vector<tile_type>> grid = parseGrid(lines, startLocation);
    int numRows = grid.size();
    int numCols = grid[0].size();
    vector<vector<long long>> distances = createDistanceGrid(numRows, numCols);
    vector<vector<bool>> visited = createVisitedGrid(numRows, numCols);

    //initialize the search queue.  We're going to do DFS to get the minimum distances from the start for each tile.
    queue<pair<pair<int, int>, int>> toVisit;
    toVisit.push({startLocation, 0});
    visited[startLocation.first][startLocation.second] = true;

    //breadth first search
    while(!toVisit.empty()) {
        pair<pair<int, int>, int> current = toVisit.front();
        toVisit.pop();
        long long currentDistance = current.second;
        pair<int, int> currentCoords = current.first;

        vector<pair<int, int>> neighbors = findValidNeighborCoords(currentCoords.first, currentCoords.second, numRows, numCols);
        for(pair<int, int> neighbor : neighbors) {
            if(grid[neighbor.first][neighbor.second] == GARDEN && !visited[neighbor.first][neighbor.second]) {
                toVisit.push({neighbor, currentDistance + 1});
                visited[neighbor.first][neighbor.second] = true;
            }
        }

        distances[currentCoords.first][currentCoords.second] = currentDistance;
    }

    return countCheckerboardTilesWithDistanceLessThan(distances, grid, 64);
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
    long long result;
    if(PART_2_ENABLED) {
        result = part2(lines);
    } else {
        result = part1(lines);
    }

    //print final result to console
    cout << "result=" << result << endl;
    return 0;
}