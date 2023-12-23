
/***************************
 * Day 18: Lavaduct Lagoon *
 ***************************/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <set>
#include "../utils/util.h"

using namespace std;

static const bool PART_2_ENABLED = true;

class MyTile {
public:
    pair<int, int> coords;
    string rgb;

    MyTile() {}

    MyTile(pair<int, int> coords, string rgb) {
        this->coords = coords;
        this->rgb = rgb;
    }
};


direction parseDirString(string dirString) {
    if(dirString == "U") {
        return UP_DIRECTION;
    }
    if(dirString == "R") {
        return RIGHT_DIRECTION;
    }
    if(dirString == "L") {
        return LEFT_DIRECTION;
    }
    if(dirString == "D") {
        return DOWN_DIRECTION;
    }
    return NONE_DIRECTION;
}

void printTiles(unordered_map<pair<int, int>, MyTile, IntPairHash> &tiles, pair<int, int> minCoords, pair<int, int> maxCoords) {
    ofstream outFile("output.txt"); // Open a file stream

    //go through each row from the min to the max
    for(int i = minCoords.first; i <= maxCoords.first; i++) {
        vector<pair<int, int>> currentRowFillCoords;
        for(int j = minCoords.second; j <= maxCoords.second; j++) {
            pair<int, int> coordsIJ = make_pair(i, j);
            if(tiles.find(coordsIJ) != tiles.end()) {
                outFile << '#'; // Tile here, print '#'
            } else {
                outFile << '.'; // No tile here, print '.'
            }
        }
        outFile << '\n'; // Newline after each row
    }

    outFile.close(); // Close the file stream
}

void fillInsideTiles(unordered_map<pair<int, int>, MyTile, IntPairHash> &tiles, pair<int, int> minCoords, pair<int, int> maxCoords) {

    //let's start with a seed.  We looked at the output and found this inside tile.  Makes everything so much easier!
    pair<int, int> seed = make_pair(minCoords.first + 30, minCoords.second + 30);

    vector<pair<int, int>> toFill;
    toFill.push_back(seed);

    while(!toFill.empty()) {
        pair<int, int> currentTile = toFill.back();
        toFill.pop_back();

        tiles[currentTile] = MyTile(currentTile, "(inside)");

        vector<pair<int, int>> neighborCandidates;

        neighborCandidates.push_back(make_pair(currentTile.first - 1, currentTile.second));
        neighborCandidates.push_back(make_pair(currentTile.first + 1, currentTile.second));
        neighborCandidates.push_back(make_pair(currentTile.first, currentTile.second - 1));
        neighborCandidates.push_back(make_pair(currentTile.first, currentTile.second + 1));

        for(int i = 0; i < neighborCandidates.size(); i++) {
            if(tiles.find(neighborCandidates[i]) == tiles.end()) {
                toFill.push_back(neighborCandidates[i]);
            }
        }
    }
}

long long getLagoonCapacity(vector<string> lines) {
    vector<vector<string>> wordLines = parseAllLines(lines);

    unordered_map<pair<int, int>, MyTile, IntPairHash> tiles;

    pair<int, int> currentCoords = make_pair(0, 0);
    tiles[currentCoords] = MyTile(currentCoords, "???");

    pair<int,int> maxCoords = make_pair(0,0);
    pair<int,int> minCoords = make_pair(0,0);

    for(int i = 0; i < wordLines.size(); i++) {
        string dirString = wordLines[i][0];
        int distance = stoi(wordLines[i][1]);
        string currentColor = wordLines[i][2];
        
        direction dir = parseDirString(dirString);

        for(int k = 0; k < distance; k++) {
            currentCoords = goInDirection(currentCoords, dir);
            tiles[currentCoords] = MyTile(currentCoords, currentColor);
        }

        //update min and max coords
        if(currentCoords.first > maxCoords.first) {
            maxCoords.first = currentCoords.first;
        }
        if(currentCoords.second > maxCoords.second) {
            maxCoords.second = currentCoords.second;
        }
        if(currentCoords.first < minCoords.first) {
            minCoords.first = currentCoords.first;
        }
        if(currentCoords.second < minCoords.second) {
            minCoords.second = currentCoords.second;
        }
    }

    fillInsideTiles(tiles, minCoords, maxCoords);
    printTiles(tiles, minCoords, maxCoords);

    return (long long) tiles.size();
}

direction parseDirHexChar(char c) {
    if(c == '0') {
        return RIGHT_DIRECTION;
    }
    if(c == '1') {
        return DOWN_DIRECTION;
    }
    if(c == '2') {
        return LEFT_DIRECTION;
    }
    if(c == '3') {
        return UP_DIRECTION;
    }
    return NONE_DIRECTION;
}

//part 2 implementation with crazy distances and hex string parsing.
long long getLagoonCapacityHard(vector<string> lines) {

    vector<vector<string>> wordLines = parseAllLines(lines);
    vector<pair<int, direction>> moves;

    for(int i = 0; i < wordLines.size(); i++) {
        //parse the hex string into distance and direction
        string hexString = wordLines[i][2];
        string cleanedHexString = hexString.substr(2, hexString.size() - 3);
        int distance = stoi(cleanedHexString.substr(0, 5), nullptr, 16);
        direction dir = parseDirHexChar(cleanedHexString[5]);
        moves.push_back(make_pair(distance, dir));
    }

    /*
        Here's an overview of our method for this.

        total tiles = edge tiles + fill tiles
        fill tiles = sum(fill tiles in each row)
        sum(fill tiles in each row) = sum(fill tiles in rows with corners) + sum(fill tiles in rows without corners)

        So, we need to find:
            * total edge tiles
            * sum of fill tiles in rows with corner tiles
            * sum of fill tiles in rows without corner tiles

        To do this, first we walk through each step to get all the vertices of the path.
        We mark the rows with corners in this step as well as the upper/lower bounds.

        The trick we use is that we don't need to calculate every single non-corner row separately.  Instead, we can assume
        that any block of rows between corner rows has equal fill tiles for each row within that block.  We can calculate the
        fill of a single row and multiply it by the height of that block to get the fill tiles for the whole range.  This turns an
        O(num rows) problem (num rows in the input.txt is around 10,000,000) to an O(num vertices) problem (in the input.txt, around 200).

        So, we mark the "wall row ranges" (rows without corners, i.e. rows that only pass through walls).

        Now, to find the num fill tiles for a given row, we basically raytrace from left to right and mark where the bounds of the
        fill blocks in that row begin and end.  This is finnicky and requires a lot of edge cases and corner cases.

        The logic for the wall row ranges is simpler because we don't have to handle the corner rows.

        Then we can simply loop through each fill block start/stop range and add the difference to the total fill tiles(or the difference times
        the wall row block height, in the wall rows case).
    */

    long long totalEdgeDistance = 0;
    vector<pair<int, int>> vertices;
    pair<int, int> currentPosition = make_pair(0, 0);
    pair<int, int> maxCoords = make_pair(0,0);
    pair<int, int> minCoords = make_pair(0,0);

    set<int> cornerRows; // rows that contain corners
    vector<pair<int, int>> wallRowRanges; // start/end ranges of rows that contain no corners

    for(int i = 0; i < moves.size(); i++) {
        pair<int, direction> move = moves[i];
        totalEdgeDistance += move.first;

        cornerRows.insert(currentPosition.first);

        currentPosition = goInDirectionDistance(currentPosition, move.second, move.first);
        vertices.push_back(currentPosition);

        //update min and max coords
        if(currentPosition.first > maxCoords.first) {
            maxCoords.first = currentPosition.first;
        }
        if(currentPosition.second > maxCoords.second) {
            maxCoords.second = currentPosition.second;
        }
        if(currentPosition.first < minCoords.first) {
            minCoords.first = currentPosition.first;
        }
        if(currentPosition.second < minCoords.second) {
            minCoords.second = currentPosition.second;
        }
    }

    //get non-corner row ranges
    bool isFirstCornerRow = true;
    int prevCornerRow = -1;
    for(int cornerRow : cornerRows) {
        if(isFirstCornerRow) {
            isFirstCornerRow = false;
        }
        else {
            if(cornerRow - prevCornerRow > 1) {
               wallRowRanges.push_back(make_pair(prevCornerRow + 1, cornerRow - 1));
            }
        }
        prevCornerRow = cornerRow;
    }

    cout << "corner rows: " << cornerRows.size() << endl;
    cout << "wall row ranges: " << wallRowRanges.size() << endl;

    cout << "total edge tile count: " << totalEdgeDistance << endl;

    long long totalFillCount = 0;
    
    //first find the fills for all the corner rows
    for(int currentRow: cornerRows) {
        vector<int> bounds;
        long long thisRowFillCount = 0;
        currentPosition = make_pair(0, 0); // reset for this block
        for(int i = 0; i < moves.size(); i++) {
            pair<int, direction> move = moves[i];
            pair<int, direction> prevMove = (i == 0) ? moves[moves.size() - 1] : moves[i-1];

            int currentMoveDir = move.second;
            int prevMoveDir = prevMove.second;
            int currentMoveDistance = move.first;
            int prevMoveDistance = prevMove.first;

            if(currentMoveDir == RIGHT_DIRECTION && currentPosition.first == currentRow) {
                if(prevMoveDir == DOWN_DIRECTION) {
                    bounds.push_back(currentPosition.second - 1); // special corner case
                }
            }

            if(currentMoveDir == LEFT_DIRECTION && currentPosition.first == currentRow) {
                if(prevMoveDir == UP_DIRECTION) {
                    bounds.push_back(currentPosition.second + 1); // special corner case 
                }
            }

            if(currentMoveDir == UP_DIRECTION) {
                if(currentRow < currentPosition.first && currentRow > currentPosition.first - currentMoveDistance) {
                    bounds.push_back(currentPosition.second + 1); // left wall
                }
                if(currentRow == currentPosition.first && prevMoveDir == RIGHT_DIRECTION) {
                    bounds.push_back(currentPosition.second + 1); // corner case 
                }
            }

            if(currentMoveDir == DOWN_DIRECTION) {
                if(currentRow > currentPosition.first && currentRow < currentPosition.first + currentMoveDistance) {
                    bounds.push_back(currentPosition.second - 1); // special right wall
                }
                if(currentRow == currentPosition.first && prevMoveDir == LEFT_DIRECTION) {
                    bounds.push_back(currentPosition.second - 1); // special corner case
                }
            }

            currentPosition = goInDirectionDistance(currentPosition, move.second, move.first);
        }

        sort(bounds.begin(), bounds.end());
        for(int i = 0; i < bounds.size(); i+=2) {
            int start = bounds[i];
            int end = bounds[i+1];
            thisRowFillCount += ((end - start) + 1);
        }

        if(bounds.size() % 2 != 0) {
            cout << "something terribly wrong happened with the bounds!" << bounds.size() << ", row=" << currentRow << endl;
            return -1;
        }
        
        totalFillCount += thisRowFillCount;
    }

    cout << "fill count after corner rows:" << totalFillCount << endl;

    //now find the fills for all the non-corner row blocks.   We can skip all the corner cases here.
    for(pair<int, int> wallRowRange : wallRowRanges) {
        int currentRow = wallRowRange.first; //all rows in this range should give the same bounds, let's just use first here since it's easy
        int currentBlockHeight = ((wallRowRange.second - wallRowRange.first) + 1);
        vector<int> bounds;
        long long currentRangeFillCount = 0;
        currentPosition = make_pair(0, 0);
        for(int i = 0; i < moves.size(); i++) {
            pair<int, direction> move = moves[i];\
            int currentMoveDir = move.second;
            int currentMoveDistance = move.first;

            if(currentMoveDir == UP_DIRECTION && currentRow < currentPosition.first && currentRow > currentPosition.first - currentMoveDistance) {
                bounds.push_back(currentPosition.second + 1); // left wall
            }

            if(currentMoveDir == DOWN_DIRECTION && currentRow > currentPosition.first && currentRow < currentPosition.first + currentMoveDistance) {
                bounds.push_back(currentPosition.second - 1); // special right wall
            }

            currentPosition = goInDirectionDistance(currentPosition, move.second, move.first);
        }

        sort(bounds.begin(), bounds.end());
        for(int i = 0; i < bounds.size(); i+=2) {
            long long start = bounds[i];
            long long end = bounds[i+1];
            long long thisBoundsWidth = ((end - start) + 1);
            currentRangeFillCount += (thisBoundsWidth * currentBlockHeight);
        }

        if(bounds.size() % 2 != 0) {
            cout << "something terribly wrong happened with the bounds!" << bounds.size() << ", row=" << currentRow << endl;
            return -1;
        }
        
        totalFillCount += currentRangeFillCount;
    }

    cout << "fill count after wall ranges:" << totalFillCount << endl;

    return totalEdgeDistance + totalFillCount;
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
        result = getLagoonCapacityHard(lines);
    } else {
        result = getLagoonCapacity(lines);
    }

    //print final result to console
    cout << "result=" << result << endl;
    return 0;
}