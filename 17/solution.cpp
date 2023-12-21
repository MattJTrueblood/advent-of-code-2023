
/**********************
 * Day N:  ~~~~~~~~~  *
 **********************/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <limits.h>
#include <utility>
#include <stdlib.h>
#include <algorithm>
#include <queue>
#include <unordered_map>
#include <functional>
#include "../utils/util.h"

using namespace std;

static const bool PART_2_ENABLED = true;

class Tile {
public:
    int cost = 0;
    bool isPath = false; //used for drawing only

    // these variables probably represent state that needs to be tracked in some other place.
    int sameDirectionCount = 0;
    direction fromDirection;
    bool visited = false;
    int distanceFromStart = INT_MAX;
    Tile* previousTile;

    pair<int, int> coords;
    vector<Tile*> neighbors;

    Tile() {}

    Tile(int cost, pair<int, int> coords) {
        this->cost = cost;
        this->coords = coords;
    }
};

class TileVisitedState {
public:
    pair<int, int> coords;
    direction fromDirection;
    int sameDirectionCount;
    int distanceFromStart; // not counted for equality in the map
    int previousStateIndex;

    TileVisitedState() {}

    TileVisitedState(pair<int, int> coords, direction fromDirection, int sameDirectionCount, int distanceFromStart, int previousStateIndex) {
        this->coords = coords;
        this->fromDirection = fromDirection;
        this->sameDirectionCount = sameDirectionCount;
        this->distanceFromStart = distanceFromStart;
        this->previousStateIndex = previousStateIndex;
    }

    bool operator==(const TileVisitedState& other) const {
        return coords == other.coords
               && fromDirection == other.fromDirection
               && sameDirectionCount == other.sameDirectionCount;
    }

    struct Hash {
        size_t operator()(const TileVisitedState& state) const {
            return hashIntPair(state.coords) ^
                   hash<int>{}(static_cast<int>(state.fromDirection)) ^
                   hash<int>{}(state.sameDirectionCount);
        }

        // Hash function for pair<int, int>
        static size_t hashIntPair(const pair<int, int>& p) {
            size_t h1 = hash<int>{}(p.first);
            size_t h2 = hash<int>{}(p.second);
            return h1 ^ (h2 << 1);
        }
    };

    struct Compare {
        bool operator()(const TileVisitedState& lhs, const TileVisitedState& rhs) const {
            return lhs.distanceFromStart > rhs.distanceFromStart;
        }
    };
};

vector<vector<Tile*>> linesToTiles(vector<string> lines) {
    vector<vector<Tile*>> tiles;
    for(int i = 0; i < lines.size(); i++) {
        vector<Tile*> row;
        for(int j = 0; j < lines[0].size(); j++) {
            row.push_back(new Tile(charToInt(lines[i][j]), make_pair(i, j)));
        }
        tiles.push_back(row);
    }
    for(int i = 0; i < lines.size(); i++) {
        for(int j = 0; j < lines[0].size(); j++) {
            vector<pair<int, int>> neighborCoords = findValidNeighborCoords(i, j, lines.size(), lines[0].size());
            vector<Tile*> neighborTiles;
            for(int k = 0; k < neighborCoords.size(); k++) {
                pair<int, int> currentNeighborCoords = neighborCoords[k];
                neighborTiles.push_back(tiles[currentNeighborCoords.first][currentNeighborCoords.second]);
            }
            tiles[i][j]->neighbors = neighborTiles;
        }
    }
    return tiles;
}

void deleteTiles(vector<vector<Tile*>> tiles) {
    for(int i = 0; i < tiles.size(); i++) {
        for(int j = 0; j < tiles[0].size(); j++) {
            delete tiles[i][j];
        }
    }
}

vector<Tile*> getUnvisitedNeighbors(Tile* tile) {
    vector<Tile*> unvisitedNeighbors;
    for(int i = 0; i < tile->neighbors.size(); i++) {
        Tile* neighbor = tile->neighbors[i];
        if(!neighbor->visited) {
            unvisitedNeighbors.push_back(neighbor);
        }
    }
    return unvisitedNeighbors;
}

void printGrid(vector<vector<Tile*>> tiles) {
    for(int i = 0; i < tiles.size(); i++) {
        for(int j = 0; j < tiles[i].size(); j++) {
            string tileCost = to_string(tiles[i][j]->cost);
            if(tiles[i][j]->isPath) {
                printColor(tileCost, GREEN, BLACK);
            } else {
                printColor(tileCost, RED, BLACK);
            }
        }
        cout << endl;
    }
}

direction getDirectionOfMove(pair<int, int> fromCoords, pair<int, int> toCoords) {
    if(fromCoords.first > toCoords.first) {
        return UP_DIRECTION;
    }
    if(fromCoords.first < toCoords.first) {
        return DOWN_DIRECTION;
    }
    if(fromCoords.second > toCoords.second) {
        return LEFT_DIRECTION;
    }
    if(fromCoords.second < toCoords.second) {
        return RIGHT_DIRECTION;
    }
    return NONE_DIRECTION;
}

int findBestPathCostPart1(vector<string> lines) {
    vector<vector<Tile*>> tiles = linesToTiles(lines);

    Tile * startTile = tiles[0][0];
    Tile * endTile = tiles[tiles.size() -1][tiles[0].size() - 1];

    unordered_map<TileVisitedState, bool, TileVisitedState::Hash> visited;
    priority_queue<TileVisitedState, vector<TileVisitedState>, TileVisitedState::Compare> unvisited;

    TileVisitedState currentState = TileVisitedState(startTile->coords, NONE_DIRECTION, 0, 0, -1);
    unvisited.push(currentState);

    vector<TileVisitedState> allVisited;

    while(!unvisited.empty()) {

        currentState = unvisited.top();
        unvisited.pop();

        //check if we've already visited this state with a shorter distance before.  If so we can just skip and ignore this.
        if(visited[currentState]) {
            continue;
        }

        //let's add this to allVisited so we have a consistent memory address to use for prevState in all neighbors
        allVisited.push_back(currentState);
        int currentStateIndex = allVisited.size() - 1;

        if(currentState.coords == endTile->coords) {
            //we've found the end!  hooray!
            break;
        }

        // add neighbor states to the unvisited queue
        vector<Tile*> neighborTiles = tiles[currentState.coords.first][currentState.coords.second]->neighbors;
        for(int i = 0; i < neighborTiles.size(); i++) {
            //exclude the direction we just came from
            if(currentState.previousStateIndex != -1 && neighborTiles[i]->coords == allVisited[currentState.previousStateIndex].coords) {
                continue;
            }
            direction neighborDirection = getDirectionOfMove(currentState.coords, neighborTiles[i]->coords);
            int neighborSameDirectionCount = (neighborDirection == currentState.fromDirection) ? currentState.sameDirectionCount + 1 : 1;
            if(neighborSameDirectionCount > 3) {
                continue; // just don't include this neighbor state
            }
            int neighborDistance = currentState.distanceFromStart + neighborTiles[i]->cost;
            TileVisitedState neighborState = TileVisitedState(neighborTiles[i]->coords, neighborDirection, neighborSameDirectionCount,
                                                              neighborDistance, currentStateIndex);
            if (visited.find(neighborState) == visited.end()) {
                //not visited this state yet
                unvisited.push(neighborState);
            }
        }

        //set this as visited
        visited[currentState] = true;
    }

    TileVisitedState pathState = currentState; // should be at the end
    while(pathState.previousStateIndex != -1) {
        tiles[pathState.coords.first][pathState.coords.second]->isPath = true;
        pathState = allVisited[pathState.previousStateIndex];
    }

    printGrid(tiles);
    deleteTiles(tiles);

    return currentState.distanceFromStart;
}

int findBestPathCostPart2(vector<string> lines) {
    vector<vector<Tile*>> tiles = linesToTiles(lines);

    Tile * startTile = tiles[0][0];
    Tile * endTile = tiles[tiles.size() -1][tiles[0].size() - 1];

    unordered_map<TileVisitedState, bool, TileVisitedState::Hash> visited;
    priority_queue<TileVisitedState, vector<TileVisitedState>, TileVisitedState::Compare> unvisited;

    TileVisitedState currentState = TileVisitedState(startTile->coords, NONE_DIRECTION, 0, 0, -1);
    unvisited.push(currentState);

    vector<TileVisitedState> allVisited;

    while(!unvisited.empty()) {

        currentState = unvisited.top();
        unvisited.pop();

        //check if we've already visited this state with a shorter distance before.  If so we can just skip and ignore this.
        if(visited[currentState]) {
            continue;
        }

        //let's add this to allVisited so we have a consistent memory address to use for prevState in all neighbors
        allVisited.push_back(currentState);
        int currentStateIndex = allVisited.size() - 1;

        if(currentState.coords == endTile->coords) {
            //we've found the end!  hooray!
            break;
        }

        vector<Tile*> neighborTiles = tiles[currentState.coords.first][currentState.coords.second]->neighbors;
        for(int i = 0; i < neighborTiles.size(); i++) {
            //exclude the direction we just came from
            if(currentState.previousStateIndex != -1 && neighborTiles[i]->coords == allVisited[currentState.previousStateIndex].coords) {
                continue;
            }

            direction neighborDirection = getDirectionOfMove(currentState.coords, neighborTiles[i]->coords);
            int neighborSameDirectionCount = (neighborDirection == currentState.fromDirection) ? currentState.sameDirectionCount + 1 : 1;

            if(currentState.sameDirectionCount > 0 && currentState.sameDirectionCount < 4) {
                if(currentState.fromDirection != neighborDirection) {
                    continue;
                }
            }

            if(neighborSameDirectionCount > 10) {
                continue; // just don't include this neighbor state
            }
            int neighborDistance = currentState.distanceFromStart + neighborTiles[i]->cost;
            TileVisitedState neighborState = TileVisitedState(neighborTiles[i]->coords, neighborDirection, neighborSameDirectionCount,
                                                              neighborDistance, currentStateIndex);
            if (visited.find(neighborState) == visited.end()) {
                //not visited this state yet
                unvisited.push(neighborState);
            }
        }

        //set this as visited
        visited[currentState] = true;
    }

    TileVisitedState pathState = currentState; // should be at the end
    while(pathState.previousStateIndex != -1) {
        tiles[pathState.coords.first][pathState.coords.second]->isPath = true;
        pathState = allVisited[pathState.previousStateIndex];
    }

    printGrid(tiles);
    deleteTiles(tiles);

    return currentState.distanceFromStart;
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
        result = findBestPathCostPart2(lines);
    } else {
        result = findBestPathCostPart1(lines);
    }

    //print final result to console
    cout << "result=" << result << endl;
    return 0;
}