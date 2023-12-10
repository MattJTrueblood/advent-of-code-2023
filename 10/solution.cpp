
/**********************
 * Day 10:  Pipe Maze *
 **********************/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <utility>
#include "../utils/util.h"

using namespace std;

static const bool PART_2_ENABLED = true;

enum inside_status { OUTSIDE_LOOP=0, INSIDE_LOOP=1, UNKNOWN=2 };

// tile can represent either a pipe or an empty space.  
class PipeTile {
public:
    bool isPipe = false;
    bool up = false;
    bool down = false;
    bool left = false;
    bool right = false;

    bool loopPipe = false;
    inside_status insideOrOutside = UNKNOWN;

    char displayCharacter = '?';

    PipeTile() {}
    PipeTile(char c) {
        switch(c) {
        case '|':
            up = true; down = true; isPipe = true;
            break;
        case '-':
            left = true; right = true; isPipe = true;
            break;
        case 'L':
            up = true; right = true; isPipe = true;
            break;
        case 'J':
            up = true; left = true; isPipe = true;
            break;
        case '7':
            down = true; left = true; isPipe = true;
            break;
        case 'F':
            down = true; right = true; isPipe = true;
            break;
        default: // '.', 'S', etc.
            break;
        }
        displayCharacter = c;
    }

    direction getNextDir(direction dir) {
        if(dir == UP_DIRECTION) {
            if(up) return UP_DIRECTION;
            if(right) return RIGHT_DIRECTION;
            if(left) return LEFT_DIRECTION;
        }
        if(dir == DOWN_DIRECTION) {
            if(down) return DOWN_DIRECTION;
            if(right) return RIGHT_DIRECTION;
            if(left) return LEFT_DIRECTION;
        }
        if(dir == RIGHT_DIRECTION) {
            if(up) return UP_DIRECTION;
            if(down) return DOWN_DIRECTION;
            if(right) return RIGHT_DIRECTION;
        }
        if(dir == LEFT_DIRECTION) {
            if(up) return UP_DIRECTION;
            if(down) return DOWN_DIRECTION;
            if(left) return LEFT_DIRECTION;
        }
        return dir;
    }
};

// find row, column of the S tile
pair<int, int> findStartCoords(vector<string> lines) {
    for(int i = 0; i < lines.size(); i++) {
        for(int j = 0; j < lines[i].size(); j++) {
            if(lines[i][j] == 'S') {
                return make_pair(i, j);
            }
        }
    }
    return make_pair(-1, -1); // this should never happen
}

// read each char and convert it to a PipeTile in a grid
vector<vector<PipeTile*>> parsePipes(vector<string> lines) {
    vector<vector<PipeTile *>> pipeTiles;
    for(int i = 0; i < lines.size(); i++) {
        vector<PipeTile *> currentRow;
        for(int j = 0; j < lines[i].size(); j++) {
            PipeTile * pipeTile = new PipeTile(lines[i][j]);
            currentRow.push_back(pipeTile);
        }
        pipeTiles.push_back(currentRow);
    }
    return pipeTiles;
}

// free up all the heap-allocated PipeTile memory
void deletePipeTiles(vector<vector<PipeTile*>> pipeTiles) {
    for(int i = 0; i < pipeTiles.size(); i++) {
        for(int j = 0; j < pipeTiles[0].size(); j++) {
            delete pipeTiles[i][j];
        }
    }
    return;
}

// draw a colorful visualization of the grid
void drawPipes(vector<vector<PipeTile*>> pipeTiles) {
    for(int i = 0; i < pipeTiles.size(); i++) {
        for(int j = 0; j < pipeTiles[i].size(); j++) {
            PipeTile * tile = pipeTiles[i][j];
            if(tile->displayCharacter == 'S') {
                printColor(charToString(tile->displayCharacter), BLACK, YELLOW);
            } else if(tile->loopPipe) {
                printColor(charToString(tile->displayCharacter), YELLOW, BLACK);
            } else if(tile->insideOrOutside == INSIDE_LOOP) {
                printColor(charToString(tile->displayCharacter), WHITE, RED);
            } else if(tile->insideOrOutside == OUTSIDE_LOOP) {
                printColor(charToString(tile->displayCharacter), BLACK, CYAN);
            } else {
                cout << charToString(tile->displayCharacter);
            }
        }
        cout << endl;
    }
}

// set the tile to the left of your current walk direction as inside or outside
void setInsideTileStatusOfLoopTile(pair<int, int> walkCoords, vector<vector<PipeTile*>> pipeTiles,
                                   direction walkDirection, inside_status insideOrOutside) {
    if(walkDirection == UP_DIRECTION) {
        if(walkCoords.second > 0) {
            PipeTile* leftPipeTile = pipeTiles[walkCoords.first][walkCoords.second - 1];
            if(!leftPipeTile->loopPipe) {
                leftPipeTile->insideOrOutside = insideOrOutside;
            }
        }
    }
    if(walkDirection == DOWN_DIRECTION) {
        if(walkCoords.second < pipeTiles[0].size() - 1) {
            PipeTile* rightPipeTile = pipeTiles[walkCoords.first][walkCoords.second + 1];
            if(!rightPipeTile->loopPipe) {
                rightPipeTile->insideOrOutside = insideOrOutside;
            }
        }
    }
    if(walkDirection == RIGHT_DIRECTION) {
        if(walkCoords.first > 0) {
            PipeTile* upPipeTile = pipeTiles[walkCoords.first - 1][walkCoords.second];
            if(!upPipeTile->loopPipe) {
                upPipeTile->insideOrOutside = insideOrOutside;
            }
        }
    }
    if(walkDirection == LEFT_DIRECTION) {
        if(walkCoords.first < pipeTiles.size() - 1) {
            PipeTile* downPipeTile = pipeTiles[walkCoords.first + 1][walkCoords.second];
            if(!downPipeTile->loopPipe) {
                downPipeTile->insideOrOutside = insideOrOutside;
            }
        }
    }
}

// one iteration of fill-search on inside and outside tiles.  Returns the number of tiles you filled this iteration.  0 means it's done
int fillInsideOutsideNeighbors(vector<vector<PipeTile*>> pipeTiles) {
    int fillCount = 0;
    for(int i = 0; i < pipeTiles.size(); i++) {
        for(int j = 0; j < pipeTiles[i].size(); j++) {
            PipeTile * tile = pipeTiles[i][j];
            if(tile->insideOrOutside != UNKNOWN) {
                vector<pair<int, int>> neighborCoords = findValidNeighborCoords(i, j, pipeTiles.size(), pipeTiles[i].size());
                for(int k = 0; k < neighborCoords.size(); k++) {
                    PipeTile * neighbor =  pipeTiles[neighborCoords[k].first][neighborCoords[k].second];
                    if(!neighbor->loopPipe && neighbor->insideOrOutside == UNKNOWN) {
                        neighbor->insideOrOutside = tile->insideOrOutside;
                        fillCount++;
                    }
                }
            }
        }
    }
    return fillCount;
}

// find the number of tiles marked as inside on your grid
int countNumInsideTiles(vector<vector<PipeTile*>> pipeTiles) {
    int count = 0;
    for(int i = 0; i < pipeTiles.size(); i++) {
        for(int j = 0; j < pipeTiles[i].size(); j++) {
            PipeTile * tile = pipeTiles[i][j];
            if(!tile->loopPipe && tile->insideOrOutside == INSIDE_LOOP) {
                count++;
            }
        }
    }
    return count;
}

// both part 1 and part 2 are in here for simplicity.  Part 1 finds the farthest distance in the loop from the start.
// part 2 calculates the number of non-loop tiles completely enclosed by the loop.
int calculateResult(vector<string> lines) {
    pair<int, int> startCoords = findStartCoords(lines);
    vector<vector<PipeTile *>> pipeTiles = parsePipes(lines);

    cout << "startcoords:" << startCoords.first << "," << startCoords.second << endl;
    cout << "max rows and cols: " << lines.size() << "," << lines[0].size() << endl;

    vector<pair<int, int>> startNeighbors = findValidNeighborCoords(
        startCoords.first, startCoords.second, lines.size(), lines[0].size());

    // find the two pipes that are connected to the start and the directions they are traveling in.  Kind of cumbersome.
    vector<pair<int, int>> startConnectingPipeCoords;
    vector<direction> startFromDirections;
    for(int i = 0; i < startNeighbors.size(); i++) {
        pair<int, int> coords = startNeighbors[i];
        PipeTile* tile = pipeTiles[coords.first][coords.second];
        if(tile->isPipe) {
            // up neighbor
            if(coords.first < startCoords.first) {
                if(tile->down) {
                    startConnectingPipeCoords.push_back(coords);
                    startFromDirections.push_back(UP_DIRECTION);
                }
            }
            // down neighbor
            if(coords.first > startCoords.first) {
                if(tile->up) {
                    startConnectingPipeCoords.push_back(coords);
                    startFromDirections.push_back(DOWN_DIRECTION);
                }
            }
            //left neighbor
            if(coords.second < startCoords.second) {
                if(tile->right) {
                    startConnectingPipeCoords.push_back(coords);
                    startFromDirections.push_back(LEFT_DIRECTION);
                }
            }
            //right neighbor
            if(coords.second > startCoords.second) {
                if(tile->left) {
                    startConnectingPipeCoords.push_back(coords);
                    startFromDirections.push_back(RIGHT_DIRECTION);
                }
            }
        }
    }

    // setting up variables for the two-way walk
    pipeTiles[startCoords.first][startCoords.second]->loopPipe = true;
    pair<int, int> walkCoordsOne = startConnectingPipeCoords[0];
    pair<int, int> walkCoordsTwo = startConnectingPipeCoords[1];
    direction walkDirectionOne = startFromDirections[0];
    direction walkDirectionTwo = startFromDirections[1];
    int distanceCount = 1;

    //Now we're going to step through the loop in both directions until they converge
    while(walkCoordsOne.first != walkCoordsTwo.first || walkCoordsOne.second != walkCoordsTwo.second) {
        PipeTile* walkPipeOne = pipeTiles[walkCoordsOne.first][walkCoordsOne.second];
        PipeTile* walkPipeTwo = pipeTiles[walkCoordsTwo.first][walkCoordsTwo.second];

        walkPipeOne->loopPipe = true;
        walkPipeTwo->loopPipe = true;

        //get next coords and direction
        walkDirectionOne = walkPipeOne->getNextDir(walkDirectionOne);
        walkDirectionTwo = walkPipeTwo->getNextDir(walkDirectionTwo);

        walkCoordsOne = goInDirection(walkCoordsOne, walkDirectionOne);
        walkCoordsTwo = goInDirection(walkCoordsTwo, walkDirectionTwo);

        distanceCount++;
    }

    // fill in this last tile's loopPipe status
    PipeTile* finalPipe = pipeTiles[walkCoordsOne.first][walkCoordsOne.second];
    finalPipe->loopPipe = true;

    // part 1 is completed, we can simply return the distance count
    if(!PART_2_ENABLED) {
        deletePipeTiles(pipeTiles);
        return distanceCount;
    }
    
    /************************
    * PART 2 IMPLEMENTATION
    *************************/

    // We are going to run through the loop fully again (in one direction) and set all tiles to a given direction relative to the movement
    // direction to be "inside".  Then we'll loop in the opposite direction and reverse the logic so it's outside
    // think of running around a circular.  If you run around it one way, the inside is to your left always.  If you run it in reverse, the inside
    // is to your right always.  We essentially do this and set all the neighboring tiles directly on your "left" to beinside for one direction 
    // right for the other direction.   
    for(int i = 0; i < 2; i++) {
        pair<int, int> walkCoords = startConnectingPipeCoords[i];
        direction walkDirection = startFromDirections[i];
        inside_status insideOrOutside = (i == 0) ? OUTSIDE_LOOP : INSIDE_LOOP;

        while(walkCoords.first != startCoords.first || walkCoords.second != startCoords.second) {
            //for e.g. corners, we need to handle both the pre- and post- getNextDir directions which is why we call this function twice
            setInsideTileStatusOfLoopTile(walkCoords, pipeTiles, walkDirection, insideOrOutside);
            walkDirection = pipeTiles[walkCoords.first][walkCoords.second]->getNextDir(walkDirection);
            setInsideTileStatusOfLoopTile(walkCoords, pipeTiles, walkDirection, insideOrOutside);

            //now we update the coords
            walkCoords = goInDirection(walkCoords, walkDirection);
        }
    }

    //now consecutively spread inside or outside status to non-loop tile neighbors until there are no more unset neighbors.
    //we could do this more efficiently but this problem is hard enough and it's not too difficult anyways.
    while(fillInsideOutsideNeighbors(pipeTiles) > 0) {}

    //finally count the number of inside tiles in the grid.
    int insideTilesCount = countNumInsideTiles(pipeTiles);

    // we spent a lot of time on this so let's draw it all so we can admire it
    drawPipes(pipeTiles);

    // now clean up and return the part 2 result finally.
    deletePipeTiles(pipeTiles);
    return insideTilesCount;
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

    // now handle lines to generate the result.  Part 1 vs part 2 is handled inside this function.
    int result = calculateResult(lines);

    //print final result to console
    cout << "result=" << result << endl;
    return 0;
}