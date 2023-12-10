
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

// can represent either a pipe or an empty space.  
class PipeTile {
public:
    bool isPipe = false;
    bool up = false;
    bool down = false;
    bool left = false;
    bool right = false;

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

vector<vector<PipeTile>> parsePipes(vector<string> lines) {
    vector<vector<PipeTile>> pipeTiles;
    for(int i = 0; i < lines.size(); i++) {
        vector<PipeTile> currentRow;
        for(int j = 0; j < lines[i].size(); j++) {
            currentRow.push_back(PipeTile(lines[i][j]));
        }
        pipeTiles.push_back(currentRow);
    }
    return pipeTiles;
}

int findFarthestDistanceFromStart(vector<string> lines) {
    pair<int, int> startCoords = findStartCoords(lines);
    vector<vector<PipeTile>> pipeTiles = parsePipes(lines);

    cout << "startcoords:" << startCoords.first << "," << startCoords.second << endl;
    cout << "max rows and cols: " << lines.size() << "," << lines[0].size() << endl;

    vector<pair<int, int>> startNeighbors = findValidNeighborCoords(
        startCoords.first, startCoords.second, lines.size(), lines[0].size());

    // find the two pipes that are connected to the start.  Kind of cumbersome.
    vector<pair<int, int>> startConnectingPipeCoords;
    vector<direction> startFromDirections;
    for(int i = 0; i < startNeighbors.size(); i++) {
        pair<int, int> coords = startNeighbors[i];
        PipeTile tile = pipeTiles[coords.first][coords.second];
        if(tile.isPipe) {
            // up neighbor
            if(coords.first < startCoords.first) {
                if(tile.down) {
                    startConnectingPipeCoords.push_back(coords);
                    startFromDirections.push_back(UP_DIRECTION);
                }
            }
            // down neighbor
            if(coords.first > startCoords.first) {
                if(tile.up) {
                    startConnectingPipeCoords.push_back(coords);
                    startFromDirections.push_back(DOWN_DIRECTION);
                }
            }
            //left neighbor
            if(coords.second < startCoords.second) {
                if(tile.right) {
                    startConnectingPipeCoords.push_back(coords);
                    startFromDirections.push_back(LEFT_DIRECTION);
                }
            }
            //right neighbor
            if(coords.second > startCoords.second) {
                if(tile.left) {
                    startConnectingPipeCoords.push_back(coords);
                    startFromDirections.push_back(RIGHT_DIRECTION);
                }
            }
        }
    }

    pair<int, int> walkCoordsOne = startConnectingPipeCoords[0];
    pair<int, int> walkCoordsTwo = startConnectingPipeCoords[1];
    direction walkDirectionOne = startFromDirections[0];
    direction walkDirectionTwo = startFromDirections[1];

    int distanceCount = 1;
    while(walkCoordsOne.first != walkCoordsTwo.first || walkCoordsOne.second != walkCoordsTwo.second) {
        PipeTile walkPipeOne = pipeTiles[walkCoordsOne.first][walkCoordsOne.second];
        PipeTile walkPipeTwo = pipeTiles[walkCoordsTwo.first][walkCoordsTwo.second];

        /*
        cout << "directions: " << walkDirectionOne << ", " << walkDirectionTwo << endl;
        cout << "coords: " << walkCoordsOne.first << "," << walkCoordsOne.second << "; "
            << walkCoordsTwo.first << "," << walkCoordsTwo.second << endl;
        */

        //get next coords and direction
        walkDirectionOne = walkPipeOne.getNextDir(walkDirectionOne);
        walkDirectionTwo = walkPipeTwo.getNextDir(walkDirectionTwo);

        walkCoordsOne = goInDirection(walkCoordsOne, walkDirectionOne);
        walkCoordsTwo = goInDirection(walkCoordsTwo, walkDirectionTwo);

        distanceCount++;
    }

    cout << "final coords: " << walkCoordsOne.first << "," << walkCoordsOne.second << "; "
            << walkCoordsTwo.first << "," << walkCoordsTwo.second << endl;

    return distanceCount;
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
    int result = findFarthestDistanceFromStart(lines);

    //print final result to console
    cout << "result=" << result << endl;
    return 0;
}