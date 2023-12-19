
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

enum tile_type { EMPTY=0, BACKSLASH_MIRROR=1, FORWARDSLASH_MIRROR=2, VERTICAL_SPLITTER=3, HORIZONTAL_SPLITTER=4 };

class Tile {
public:
    tile_type tile;
    bool energized = false;
    bool visitedGoingDown = false;
    bool visitedGoingRight = false;
    bool visitedGoingUp = false;
    bool visitedGoingLeft = false;
    pair<int, int> coords;

    Tile() {}

    Tile(tile_type tile, pair<int, int> coords) {
        this->tile = tile;
        this->coords = coords;
    }
};

vector<vector<Tile*>> linesToTiles(vector<string> lines) {
    vector<vector<Tile*>> tileGrid;
    for(int i = 0; i < lines.size(); i++) {
        vector<Tile*> gridRow;
        for(int j = 0; j < lines[0].size(); j++) {
            pair<int, int> coords = make_pair(i, j);
            switch(lines[i][j]) {
            case '.':
                gridRow.push_back(new Tile(EMPTY, coords));
                break;
            case '\\': //escape backslash
                gridRow.push_back(new Tile(BACKSLASH_MIRROR, coords));
                break;
            case '/':
                gridRow.push_back(new Tile(FORWARDSLASH_MIRROR, coords));
                break;
            case '|':
                gridRow.push_back(new Tile(VERTICAL_SPLITTER, coords));
                break;
            case '-':
                gridRow.push_back(new Tile(HORIZONTAL_SPLITTER, coords));
                break;
            default:
                break;
            }
        }
        tileGrid.push_back(gridRow);
    }
    return tileGrid;
}

void deleteTileGrid(vector<vector<Tile*>> grid) {
    for(int i = 0; i < grid.size(); i++) {
        for(int j = 0; j < grid[0].size(); j++) {
            delete grid[i][j];
        }
    }
}

void resetTileGrid(vector<vector<Tile*>> grid) {
    for(int i = 0; i < grid.size(); i++) {
        for(int j = 0; j < grid[0].size(); j++) {
            grid[i][j]->energized = false;
            grid[i][j]->visitedGoingUp = false;
            grid[i][j]->visitedGoingDown = false;
            grid[i][j]->visitedGoingLeft = false;
            grid[i][j]->visitedGoingRight = false;
        }
    }
}

bool inBounds(pair<int, int> coords, int rowMax, int colMax) {
    return coords.first >= 0 && coords.first < rowMax && coords.second >= 0 && coords.second < colMax;
}

void lightGrid(vector<vector<Tile*>> tileGrid, pair<int, int> coords, direction dir) {
    if(!inBounds(coords, tileGrid.size(), tileGrid[0].size())) {
        return;
    }
    Tile * currentTile = tileGrid[coords.first][coords.second];

    if(currentTile->energized) {
        if( (currentTile->visitedGoingUp && dir == UP_DIRECTION) ||
            (currentTile->visitedGoingDown && dir == DOWN_DIRECTION) ||
            (currentTile->visitedGoingLeft && dir == LEFT_DIRECTION) ||
            (currentTile->visitedGoingRight && dir == RIGHT_DIRECTION)) {
            return; //we've already gone down this loop before
        }
    }

    switch(dir) {
    case UP_DIRECTION:
        currentTile->visitedGoingUp = true;
        break;
    case DOWN_DIRECTION:
        currentTile->visitedGoingDown = true;
        break;
    case RIGHT_DIRECTION:
        currentTile->visitedGoingRight = true;
        break;
    case LEFT_DIRECTION:
        currentTile->visitedGoingLeft = true;
        break;
    default:
        break;
    }

    currentTile->energized = true;

    switch(currentTile->tile) {
    case(EMPTY):
        lightGrid(tileGrid, goInDirection(coords, dir), dir);
        break;
    case(BACKSLASH_MIRROR):
        if(dir == RIGHT_DIRECTION) lightGrid(tileGrid, goInDirection(coords, DOWN_DIRECTION), DOWN_DIRECTION);
        if(dir == UP_DIRECTION) lightGrid(tileGrid, goInDirection(coords, LEFT_DIRECTION), LEFT_DIRECTION);
        if(dir == LEFT_DIRECTION) lightGrid(tileGrid, goInDirection(coords, UP_DIRECTION), UP_DIRECTION);
        if(dir == DOWN_DIRECTION) lightGrid(tileGrid, goInDirection(coords, RIGHT_DIRECTION), RIGHT_DIRECTION);
        break;
    case(FORWARDSLASH_MIRROR):
        if(dir == RIGHT_DIRECTION) lightGrid(tileGrid, goInDirection(coords, UP_DIRECTION), UP_DIRECTION);
        if(dir == UP_DIRECTION) lightGrid(tileGrid, goInDirection(coords, RIGHT_DIRECTION), RIGHT_DIRECTION);
        if(dir == LEFT_DIRECTION) lightGrid(tileGrid, goInDirection(coords, DOWN_DIRECTION), DOWN_DIRECTION);
        if(dir == DOWN_DIRECTION) lightGrid(tileGrid, goInDirection(coords, LEFT_DIRECTION), LEFT_DIRECTION);
        break;
    case(VERTICAL_SPLITTER):
        if(dir == UP_DIRECTION || dir == DOWN_DIRECTION) {
            lightGrid(tileGrid, goInDirection(coords, dir), dir);
        } else {
            lightGrid(tileGrid, goInDirection(coords, UP_DIRECTION), UP_DIRECTION);
            lightGrid(tileGrid, goInDirection(coords, DOWN_DIRECTION), DOWN_DIRECTION);
        }
        break;
    case(HORIZONTAL_SPLITTER):
        if(dir == RIGHT_DIRECTION || dir == LEFT_DIRECTION) {
            lightGrid(tileGrid, goInDirection(coords, dir), dir);
        } else {
            lightGrid(tileGrid, goInDirection(coords, LEFT_DIRECTION), LEFT_DIRECTION);
            lightGrid(tileGrid, goInDirection(coords, RIGHT_DIRECTION), RIGHT_DIRECTION);
        }
        break;
    default:
        break;
    }
}

int countEnergizedTiles(vector<vector<Tile*>> grid) {
    int count = 0;
    for(int i = 0; i < grid.size(); i++) {
        for(int j = 0; j < grid[0].size(); j++) {
            if(grid[i][j]->energized == true) {
                count++;
            }
        }
    }
    return count;
}

int getNumEnergizedTiles(vector<string> lines) {
    vector<vector<Tile*>> tileGrid = linesToTiles(lines);   

    lightGrid(tileGrid, make_pair(0, 0), RIGHT_DIRECTION);
    int count = countEnergizedTiles(tileGrid);

    deleteTileGrid(tileGrid);
    return count;
}

int getNumEnergizedTilesBestConfiguration(vector<string> lines) {
    vector<vector<Tile*>> tileGrid = linesToTiles(lines); 

    int bestCount = 0;

    for(int i = 0; i < tileGrid.size(); i++) {
        lightGrid(tileGrid, make_pair(i, 0), RIGHT_DIRECTION);
        int count = countEnergizedTiles(tileGrid);
        bestCount = bestCount < count ? count : bestCount;
        resetTileGrid(tileGrid);

        lightGrid(tileGrid, make_pair(i, tileGrid[0].size() - 1), LEFT_DIRECTION);
        count = countEnergizedTiles(tileGrid);
        bestCount = bestCount < count ? count : bestCount;
        resetTileGrid(tileGrid);
    }
    for(int i = 0; i < tileGrid[0].size(); i++) {
        lightGrid(tileGrid, make_pair(0, i), DOWN_DIRECTION);
        int count = countEnergizedTiles(tileGrid);
        bestCount = bestCount < count ? count : bestCount;
        resetTileGrid(tileGrid);

        lightGrid(tileGrid, make_pair(tileGrid.size() - 1, i), UP_DIRECTION);
        count = countEnergizedTiles(tileGrid);
        bestCount = bestCount < count ? count : bestCount;
        resetTileGrid(tileGrid);
    }

    deleteTileGrid(tileGrid);
    return bestCount;
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
        result = getNumEnergizedTilesBestConfiguration(lines);
    } else {
        result = getNumEnergizedTiles(lines);
    }
    

    //print final result to console
    cout << "result=" << result << endl;
    return 0;
}