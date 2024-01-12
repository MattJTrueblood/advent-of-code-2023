
/************************
 * Day 23: A long walk  *
 ************************/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <utility>
#include <set>
#include <queue>
#include <stack>
#include "../utils/util.h"

using namespace std;

static const bool PART_2_ENABLED = true;

enum tile_type {PATH=0, FOREST=1, RIGHT_SLOPE=2, DOWN_SLOPE=3, LEFT_SLOPE=4, UP_SLOPE=5};

class Tile {
public:
    int row;
    int col;
    char c;
    tile_type type;
    color myColor;
    bool isGraphNode = false;

    Tile() {}

    Tile(int row, int col, tile_type type, char c) {
        this->row = row;
        this->col = col;
        this->type = type;
        this->c = c;
        this->myColor = WHITE;
    }

    bool isSlope() {
        return (this->type != PATH && this->type != FOREST);
    }

    void markAsGraphNode() {
        this->myColor = RED;
        this->isGraphNode = true;
    }
};

class GraphNode {
public:
    Tile* tile;
    vector<GraphNode*> connections;
    vector<int> connectionDistances;

    GraphNode() {}

    GraphNode(Tile* tile) {
        this->tile = tile;
    }

    void addConnection(GraphNode* connection, int distance) {
        connections.push_back(connection);
        connectionDistances.push_back(distance);
    }
};

tile_type parseTileTypeFromChar(char c) {
    switch(c) {
    case '.':
        return PATH;
    case '#':
        return FOREST;
    case '>':
        return RIGHT_SLOPE;
    case '<':
        return LEFT_SLOPE;
    case '^':
        return UP_SLOPE;
    case 'v':
        return DOWN_SLOPE;
    }
    //this shouldn't happen
    cout << "bad character found: " << c << endl;
    return PATH; 
}

vector<vector<Tile*>> parseGrid(vector<string> lines) {
    vector<vector<Tile*>> grid;
    for(int i = 0; i < lines.size(); i++) {
        vector<Tile*> row;
        for(int j = 0; j < lines[i].size(); j++) {
            char c = lines[i][j];
            tile_type type = parseTileTypeFromChar(c);
            Tile* tile = new Tile(i, j, type, c);
            row.push_back(tile);
        }
        grid.push_back(row);
    }
    return grid;
}

void printGrid(vector<vector<Tile*>> grid) {
    for(int i = 0; i < grid.size(); i++) {
        for(int j = 0; j < grid[i].size(); j++) {
            char c = grid[i][j]->c;
            color tileColor = grid[i][j]->myColor;
            printColor(charToString(c), BLACK, tileColor);
        }
        cout << endl;
    }
}

bool isValidMove(Tile* from, Tile* to) {
    if(PART_2_ENABLED) {
        return true;
    }
    if(from->type == PATH) {
        if(to->type == PATH) {
            return true; // both are paths
        } else {
            switch(to->type) {
            case RIGHT_SLOPE:
                return (from->col != to->col + 1);
            case LEFT_SLOPE:
                return (from->col != to->col - 1);
            case UP_SLOPE:
                return (from->row != to->row - 1);
            case DOWN_SLOPE:
                return (from->row != to->row + 1);
            }
        }
    } else {
        switch(from->type) {
        case RIGHT_SLOPE:
            return (to->col == from->col + 1);
        case LEFT_SLOPE:
            return (to->col == from->col - 1);
        case UP_SLOPE:
            return (to->row == from->row - 1);
        case DOWN_SLOPE:
            return (to->row == from->row + 1);
        }
    }
    cout << "something went wrong in isValidMove." << endl;
    return false;
}

vector<GraphNode*> setupGraphNodesForGridIntersections(vector<vector<Tile*>> grid) {
    Tile* startTile = grid[0][1];
    Tile* endTile = grid[grid.size()-1][grid.size()-2];

    vector<GraphNode*> graph;
    graph.push_back(new GraphNode(startTile));
    graph.push_back(new GraphNode(endTile));

    for(int i = 0; i < grid.size(); i++) {
        for(int j = 0; j < grid[i].size(); j++) {
            vector<pair<int, int>> neighborCoords = findValidNeighborCoords(i, j, grid.size(), grid[0].size());
            int numSlopeNeighbors = 0;
            for(pair<int, int> currentCoords: neighborCoords) {
                if(grid[currentCoords.first][currentCoords.second]->isSlope()) {
                    numSlopeNeighbors++;
                }
            }
            if(numSlopeNeighbors > 1 && grid[i][j]->type == PATH) {
                graph.push_back(new GraphNode(grid[i][j]));
            }
        }
    }

    for(GraphNode* node : graph) {
        node->tile->markAsGraphNode();
    }

    return graph;
}

void findConnectedNodes(GraphNode* currentNode, vector<GraphNode*> graph, vector<vector<Tile*>> grid) {
    set<Tile*> visited;
    queue<pair<Tile*, int>> toVisit;
    toVisit.push(make_pair(currentNode->tile, 0));
    while(!toVisit.empty()) {
        pair<Tile*, int> currentTileAndDistance = toVisit.front();
        toVisit.pop();
        Tile* currentTile = currentTileAndDistance.first;
        int currentDistance = currentTileAndDistance.second;
        visited.insert(currentTile);

        if(currentTile->isGraphNode && currentTile != currentNode->tile) {
            //find the node with this tile
            for(GraphNode* possibleAdjacentNode : graph) {
                if(possibleAdjacentNode->tile == currentTile) {
                    currentNode->addConnection(possibleAdjacentNode, currentDistance);
                    break;
                }
            }
        } else {
            vector<pair<int, int>> neighborCoords = findValidNeighborCoords(currentTile->row, currentTile->col, grid.size(), grid[0].size());
            for(pair<int, int> currentCoords: neighborCoords) {
                Tile* neighborTile = grid[currentCoords.first][currentCoords.second];
                if(neighborTile->type != FOREST && visited.find(neighborTile) == visited.end() && isValidMove(currentTile, neighborTile)) {
                    toVisit.push(make_pair(neighborTile, currentDistance + 1));
                }
            }
        }
    }
}

void findAllGraphConnections(vector<GraphNode*> graph, vector<vector<Tile*>> grid) {
    for(GraphNode* node : graph) {
        findConnectedNodes(node, graph, grid);
    }
}

int findLongestPathRecursive(vector<pair<GraphNode*, int>> traverse, GraphNode* end) {
    pair<GraphNode*, int> latestStep = traverse.back();
    GraphNode* latestGraphNode = latestStep.first;
    int latestDistance = latestStep.second;

    if(latestGraphNode == end) {
        return latestDistance;
    }

    int bestDistance = -1; // default for paths that don't ever reach the end
    for(int i = 0; i < latestGraphNode->connections.size(); i++) {
        bool connectionNotAlreadyVisited = true;
        if(PART_2_ENABLED) {
            for(pair<GraphNode*, int> pathStep: traverse) {
                if(pathStep.first == latestGraphNode->connections[i]) {
                    connectionNotAlreadyVisited = false;
                    break;
                }
            }
        }
        if(connectionNotAlreadyVisited) {
            traverse.push_back(make_pair(latestGraphNode->connections[i], latestGraphNode->connectionDistances[i] + latestDistance));
            bestDistance = max(findLongestPathRecursive(traverse, end), bestDistance);
            traverse.pop_back();
        }
    }

    return bestDistance;
}


int findLongestPathFromStartToEnd(vector<GraphNode*> graph) {
    GraphNode* start = graph[0];
    GraphNode* end = graph[1];
    vector<pair<GraphNode*, int>> traverse;
    traverse.push_back(make_pair(start, 0));
    return findLongestPathRecursive(traverse, end);   
}

void deleteGrid(vector<vector<Tile*>> grid) {
    for(int i = 0; i < grid.size(); i++) {
        for(int j = 0; j < grid[i].size(); j++) {
            delete grid[i][j];
        }
    }
}

void deleteGraph(vector<GraphNode*> graph) {
    for(GraphNode* node: graph) {
        delete node;
    }
}

long long part1(vector<string> lines) {
    vector<vector<Tile*>> grid = parseGrid(lines);

    vector<GraphNode*> graph = setupGraphNodesForGridIntersections(grid);
    findAllGraphConnections(graph, grid);

    int result = findLongestPathFromStartToEnd(graph);

    deleteGrid(grid);
    deleteGraph(graph);
    return result;
}

long long part2(vector<string> lines) {
    return part1(lines); // easier to just check PART_2_ENABLED in the existing code (e.g. isValidMove)
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