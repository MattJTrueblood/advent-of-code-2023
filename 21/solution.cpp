
/*************************
 * Day 21:  Step Counter *
 *************************/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <limits>
#include <queue>
#include <tuple>
#include <cmath>
#include "../utils/util.h"

using namespace std;

static const bool PART_2_ENABLED = true;

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

// Create a grid potentially larger than the size of lines for part 2, which requires us to extend the grid in all 4 directions.
vector<vector<tile_type>> parseGridExpanded(vector<string> lines, pair<int, int> &startLocation, int steps) {

    int numRepetitionsNeeded = (((steps - 65) / 131) * 2) + 1;
    int center = (int) (numRepetitionsNeeded / 2);

    vector<vector<tile_type>> grid;
    int gridSize = lines.size() * numRepetitionsNeeded;
    for(int i = 0; i < gridSize; i++) {
        vector<tile_type> row;
        for(int j = 0; j < gridSize; j++) {
            char c = lines[i % 131][j % 131];
            if(c == '#') {
                row.push_back(ROCK);
            } else if(c == '.') {
                row.push_back(GARDEN);
            } else if(c == 'S') {
                if((i / 131) == center && (j / 131) == center) {
                    row.push_back(START);
                    startLocation.first = i;
                    startLocation.second = j;
                } else {
                    row.push_back(GARDEN);
                }
            }
        }
        grid.push_back(row);
    }
    return grid;
}

vector<vector<long long>> createDistanceGrid(int numRows, int numCols) {
    long long maxValue = numeric_limits<long long>::max();
    vector<vector<long long>> distances(numRows, vector<long long>(numCols, maxValue));
    return distances;
}

vector<vector<bool>> createVisitedGrid(int numRows, int numCols) {
    vector<vector<bool>> visited(numRows, vector<bool>(numCols, false));
    return visited;
}

void pathfindGetAllDistancesInGrid(vector<vector<long long>> &distances, vector<vector<tile_type>> &grid, vector<vector<bool>> &visited,
        pair<int, int> startLocation, long long startDistance) {

    //initialize the search queue.  We're going to do DFS to get the distances from the start for each tile in the grid.
    queue<pair<pair<int, int>, long long>> toVisit;
    toVisit.push({startLocation, startDistance});
    visited[startLocation.first][startLocation.second] = true;

    //breadth first search
    while(!toVisit.empty()) {
        pair<pair<int, int>, int> current = toVisit.front();
        toVisit.pop();
        long long currentDistance = current.second;
        pair<int, int> currentCoords = current.first;

        vector<pair<int, int>> neighbors = findValidNeighborCoords(currentCoords.first, currentCoords.second, grid.size(), grid[0].size());
        for(pair<int, int> neighbor : neighbors) {
            if(grid[neighbor.first][neighbor.second] == GARDEN && !visited[neighbor.first][neighbor.second]) {
                toVisit.push({neighbor, currentDistance + 1});
                visited[neighbor.first][neighbor.second] = true;
            }
        }

        distances[currentCoords.first][currentCoords.second] = currentDistance;
    }
}

long long countCheckerboardTilesWithDistanceLessThan(vector<vector<long long>> distances, vector<vector<tile_type>> grid, int numSteps, bool evenGrid) {
    long long count = 0;
    for(int i = 0; i < distances.size(); i++) {
        for(int j = 0; j < distances.size(); j++) {

            tile_type t = grid[i][j];
            string s = "#";
            if(t == GARDEN) s = ".";
            if(t == START) s = "S";

            if(distances[i][j] <= numSteps) {
                if (((i % 2 != 0) == (j % 2 != 0)) == evenGrid) {
                    //printColor(s, GREEN, BLACK);
                    count++;
                } else {
                    //printColor(s, RED, BLACK);
                }
            } else {
                //printColor(s, WHITE, BLACK);
            }
        }
        //cout << endl;
    }
    return count;
}

long long getCheckerboardTilesInExpandedGrid(int steps, vector<string> lines) {
    //basically the same as part 1 solution but we use parseGridExpanded to handle the "infinite" grid for >65 steps.
    //only feasible for small numbers of steps! This is used for calibrating the quadratic function for part 2.
    pair<int, int> startLocation = {0, 0};
    vector<vector<tile_type>> grid = parseGridExpanded(lines, startLocation, steps);
    vector<vector<long long>> distances = createDistanceGrid(grid.size(), grid[0].size());
    vector<vector<bool>> visited = createVisitedGrid(grid.size(), grid[0].size());

    //run the pathfinding to get the distances for all the tiles in the grid
    pathfindGetAllDistancesInGrid(distances, grid, visited, startLocation, 0);

    //now count the tiles on the correct checkerboard rank with distance <= steps.
    return countCheckerboardTilesWithDistanceLessThan(distances, grid, steps, steps % 2 == 0);
}

// I didn't write this function myself, it's implementing elsewhere and simply adapted here for my usage.
// Assuming the points p1, p2, and p3 are in a quadratic function, find a, b, c for equation ax^2 + bx + c that satisfies all 3 points
tuple<long double, long double, long double> getQuadraticParameters(pair<int, int> p1, pair<int, int> p2, pair<int, int> p3) {
    long double x1 = p1.first, y1 = p1.second;
    long double x2 = p2.first, y2 = p2.second;
    long double x3 = p3.first, y3 = p3.second;

    // Coefficients of the linear system
    long double a11 = x1 * x1, a12 = x1, a13 = 1, b1 = y1;
    long double a21 = x2 * x2, a22 = x2, a23 = 1, b2 = y2;
    long double a31 = x3 * x3, a32 = x3, a33 = 1, b3 = y3;

    // Solving the linear system using Cramer's Rule
    long double det = a11 * (a22 * a33 - a32 * a23) - a12 * (a21 * a33 - a31 * a23) + a13 * (a21 * a32 - a31 * a22);
    long double det_a = b1 * (a22 * a33 - a32 * a23) - a12 * (b2 * a33 - b3 * a23) + a13 * (b2 * a32 - b3 * a22);
    long double det_b = a11 * (b2 * a33 - b3 * a23) - b1 * (a21 * a33 - a31 * a23) + a13 * (a21 * b3 - a31 * b2);
    long double det_c = a11 * (a22 * b3 - a32 * b2) - a12 * (a21 * b3 - a31 * b2) + b1 * (a21 * a32 - a31 * a22);

    // Calculate a, b, c
    long double a = det_a / det;
    long double b = det_b / det;
    long double c = det_c / det;

    return make_tuple(a, b, c);
}

// Function to solve the quadratic equation given a, b, c, and x
long double solveQuadratic(long double a, long double b, long double c, long long x) {
    cout << "a=" << a << ", b=" << b << ", c=" << c << endl;
    return (long double) ((a * x * x) + (b * x) + c);
}

long long part2(vector<string> lines) {
    //let's find 3 points to for the quadratic model.
    vector<pair<int, int>> points;
    for(int i = 0; i < 3; i++) {
        int steps = 65 + (131 * i); // 26501365, the part 2 target value, is in this set
        int result = getCheckerboardTilesInExpandedGrid(steps, lines);
        points.push_back({steps, result});
        cout << "result for steps=" << steps << ": " << result << endl;
    }

    //now we can just find a, b, and c and then solve the quadratic equation.
    long long numStepsActual = 26501365;
    tuple<long double, long double, long double> parameters = getQuadraticParameters(points[0], points[1], points[2]);
    long double result = solveQuadratic(get<0>(parameters), get<1>(parameters), get<2>(parameters), numStepsActual);

    //actual result is likely off because of floating point precision.  Since we use long double it should be close enough to round.
    cout << "result before rounding: " << fixed << result << endl;
    return llround(result);
}

long long part1(vector<string> lines) {
    //initialize the grids
    pair<int, int> startLocation = {0, 0};
    vector<vector<tile_type>> grid = parseGrid(lines, startLocation);
    vector<vector<long long>> distances = createDistanceGrid(grid.size(), grid[0].size());
    vector<vector<bool>> visited = createVisitedGrid(grid.size(), grid[0].size());

    //run the pathfinding to get the distances for all the tiles in the grid
    pathfindGetAllDistancesInGrid(distances, grid, visited, startLocation, 0);

    //now count the tiles on the correct checkerboard rank with distance <= 64.
    return countCheckerboardTilesWithDistanceLessThan(distances, grid, 64, true);
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