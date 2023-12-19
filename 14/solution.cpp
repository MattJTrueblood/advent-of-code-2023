
/*************************************
 * Day 14: Parabolic Reflector Dish  *
 *************************************/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "../utils/util.h"
#include <unordered_map>

using namespace std;

enum rock_type { EMPTY=0, ROUND=1, SQUARE=2};

const static bool PART_2_ENABLED = true;

vector<vector<rock_type>> linesToRockGrid(vector<string> lines) {
    vector<vector<rock_type>> grid;
    for(int i = 0; i < lines.size(); i++) {
        vector<rock_type> gridRow;
        for(int j = 0; j < lines[0].size(); j++) {
            switch(lines[i][j]) {
            case '.':
                gridRow.push_back(EMPTY);
                break;
            case 'O':
                gridRow.push_back(ROUND);
                break;
            case '#':
                gridRow.push_back(SQUARE);
                break;
            default:
                break;
            }
        }
        grid.push_back(gridRow);
    }
    return grid;
}

vector<vector<rock_type>> rollGridNorth(vector<vector<rock_type>> grid) {
    for(int j = 0; j < grid[0].size(); j++) {
        int lastSquareStoneIndex = -1;
        int numRoundStonesInCurrentBlock = 0;
        for(int i = 0; i < grid.size(); i++) {
            rock_type currentTile = grid[i][j];
            if(currentTile == ROUND) {
                numRoundStonesInCurrentBlock++;
            }
            if(currentTile == SQUARE) {
                for(int k = lastSquareStoneIndex + 1; k < i; k++) {
                    if(numRoundStonesInCurrentBlock > 0) {
                        grid[k][j] = ROUND;
                        numRoundStonesInCurrentBlock--;
                    } else {
                        grid[k][j] = EMPTY;
                    }
                }
                lastSquareStoneIndex = i;
            }
            if(i == grid.size() - 1) {
                for(int k = lastSquareStoneIndex + 1; k <= i; k++) {
                    if(numRoundStonesInCurrentBlock > 0) {
                        grid[k][j] = ROUND;
                        numRoundStonesInCurrentBlock--;
                    } else {
                        grid[k][j] = EMPTY;
                    }
                }
            }
        }
    }
    return grid;
}

vector<vector<rock_type>> rollGridWest(vector<vector<rock_type>> grid) {
    for(int i = 0; i < grid.size(); i++) {
        int lastSquareStoneIndex = -1;
        int numRoundStonesInCurrentBlock = 0;
        for(int j = 0; j < grid[0].size(); j++) {
            rock_type currentTile = grid[i][j];
            if(currentTile == ROUND) {
                numRoundStonesInCurrentBlock++;
            }
            if(currentTile == SQUARE) {
                for(int k = lastSquareStoneIndex + 1; k < j; k++) {
                    if(numRoundStonesInCurrentBlock > 0) {
                        grid[i][k] = ROUND;
                        numRoundStonesInCurrentBlock--;
                    } else {
                        grid[i][k] = EMPTY;
                    }
                }
                lastSquareStoneIndex = j;
            }
            if(j == grid[0].size() - 1) {
                for(int k = lastSquareStoneIndex + 1; k <= j; k++) {
                    if(numRoundStonesInCurrentBlock > 0) {
                        grid[i][k] = ROUND;
                        numRoundStonesInCurrentBlock--;
                    } else {
                        grid[i][k] = EMPTY;
                    }
                }
            }
        }
    }
    return grid;
}

vector<vector<rock_type>> rollGridSouth(vector<vector<rock_type>> grid) {
    for(int j = 0; j < grid[0].size(); j++) {
        int lastSquareStoneIndex = grid.size();
        int numRoundStonesInCurrentBlock = 0;
        for(int i = grid.size() -1; i >= 0; i--) {
            rock_type currentTile = grid[i][j];
            if(currentTile == ROUND) {
                numRoundStonesInCurrentBlock++;
            }
            if(currentTile == SQUARE) {
                for(int k = lastSquareStoneIndex - 1; k > i; k--) {
                    if(numRoundStonesInCurrentBlock > 0) {
                        grid[k][j] = ROUND;
                        numRoundStonesInCurrentBlock--;
                    } else {
                        grid[k][j] = EMPTY;
                    }
                }
                lastSquareStoneIndex = i;
            }
            if(i == 0) {
                for(int k = lastSquareStoneIndex - 1; k >= i; k--) {
                    if(numRoundStonesInCurrentBlock > 0) {
                        grid[k][j] = ROUND;
                        numRoundStonesInCurrentBlock--;
                    } else {
                        grid[k][j] = EMPTY;
                    }
                }
            }
        }
    }
    return grid;
}

vector<vector<rock_type>> rollGridEast(vector<vector<rock_type>> grid) {
    for(int i = 0; i < grid.size(); i++) {
        int lastSquareStoneIndex = grid[0].size();
        int numRoundStonesInCurrentBlock = 0;
        for(int j = grid[0].size() - 1; j >= 0; j--) {
            rock_type currentTile = grid[i][j];
            if(currentTile == ROUND) {
                numRoundStonesInCurrentBlock++;
            }
            if(currentTile == SQUARE) {
                for(int k = lastSquareStoneIndex - 1; k > j; k--) {
                    if(numRoundStonesInCurrentBlock > 0) {
                        grid[i][k] = ROUND;
                        numRoundStonesInCurrentBlock--;
                    } else {
                        grid[i][k] = EMPTY;
                    }
                }
                lastSquareStoneIndex = j;
            }
            if(j == 0) {
                for(int k = lastSquareStoneIndex - 1; k >= j; k--) {
                    if(numRoundStonesInCurrentBlock > 0) {
                        grid[i][k] = ROUND;
                        numRoundStonesInCurrentBlock--;
                    } else {
                        grid[i][k] = EMPTY;
                    }
                }
            }
        }
    }
    return grid;
}

int calculateLoad(vector<vector<rock_type>> grid) {
    int load = 0;
    for(int i = 0; i < grid.size(); i++) {
        for(int j = 0; j < grid[0].size(); j++) {
            int loadPerRoundStone = grid.size() - i;
            if(grid[i][j] == ROUND) {
                load += loadPerRoundStone;
            }
        }
    }
    return load;
}

void printGrid(vector<vector<rock_type>> grid) {
    for(int i = 0; i < grid.size(); i++) {
        for(int j = 0; j < grid[0].size(); j++) {
            if(grid[i][j] == EMPTY) {
                cout << ".";
            } else if(grid[i][j] == ROUND) {
                printColor("O", RED, BLACK);
            } else {
                printColor("#", GREEN, BLACK);
            }
        }
        cout << endl;
    }
}

int getPart1Solution(vector<string> lines) {
    vector<vector<rock_type>> grid = linesToRockGrid(lines);
    printGrid(grid);
    cout << endl << endl;
    grid = rollGridNorth(grid);
    printGrid(grid);
    int totalLoad = calculateLoad(grid);
    return totalLoad;
}

 unordered_map<int, int> rollGridCycleAndFindRepeatingCycle(vector<vector<rock_type>> grid, int numInitialCycles, int numCycles) {
    // after enough cycles (e.g. 1000) it should be stuck in a repeating pattern.  From observation I've determined that this
    // pattern is 18 cycles long -- every 18 cycles it repeats the pattern.  Figure out this pattern and build
    // a hashmap with key = i % 18.  We can use this to calculate the load at any arbitrary n after this point with cycleMap[n % 18]

    unordered_map<int, int> cycleMap;
    for(int i = 1; i < numCycles + numInitialCycles + 1; i++) {
        grid = rollGridNorth(grid);
        grid = rollGridWest(grid);
        grid = rollGridSouth(grid);
        grid = rollGridEast(grid);
        int load = calculateLoad(grid);
        if(i >= numInitialCycles) {
            if(cycleMap[i % 18] != load) {
                cycleMap[i % 18] = load;
            }
        }
    }
    return cycleMap;
}

int getPart2Solution(vector<string> lines) {
    vector<vector<rock_type>> grid = linesToRockGrid(lines);
    unordered_map<int, int> cycleMap = rollGridCycleAndFindRepeatingCycle(grid, 1000, 50);
    return cycleMap[1000000000 % 18];
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
    int result;
    if(PART_2_ENABLED) {
        result = getPart2Solution(lines);
    } else {
        result = getPart1Solution(lines);
    }

    //print final result to console
    cout << "result=" << result << endl;
    return 0;
}