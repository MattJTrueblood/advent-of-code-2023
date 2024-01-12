
/***********************
 * Day 22:  Sand Slabs *
 ***********************/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include "../utils/util.h"

using namespace std;

static const bool PART_2_ENABLED = false;

class Coords {
public:
    int x;
    int y;
    int z;

    Coords() {}

    Coords(int x, int y, int z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }
};

class Block {
public:
    vector<Block*> supports;
    vector<Block*> supportedBy;
    vector<Coords> coords;
    int minZ;
    int maxZ;
    int id;

    Block() {}

    Block(vector<Coords> coords, int id) {
        this->coords = coords;
        this->id = id;
        this->setZBounds();
    }

    void setZBounds() {
        minZ = coords[0].z;
        maxZ = coords[0].z;
        for(int i = 0; i < coords.size(); i++) {
            if(coords[i].z > maxZ) {
                maxZ = coords[i].z;
            }
            if(coords[i].z < minZ) {
                minZ = coords[i].z;
            }
        }
    }

    bool overlapsInXYDimension(Block* otherBlock) {
        for(Coords otherCoords: otherBlock->coords) {
            for(Coords myCoords: this->coords) {
                if(otherCoords.x == myCoords.x && otherCoords.y == myCoords.y) {
                    return true;
                }
            }
        }
        return false;
    }

    bool canDescendPast(Block * otherBlock) {
        for(Coords otherCoords: otherBlock->coords) {
            for(Coords myCoords: this->coords) {
                if(otherCoords.x == myCoords.x && otherCoords.y == myCoords.y && otherCoords.z == myCoords.z - 1) {
                    return false;
                }
            }
        }
        return true;
    }

    void descendByOne() {
        for(int i = 0; i < this->coords.size(); i++) {
            this->coords[i].z--;
        }
        setZBounds();
    }
};

vector<Coords> coordsBetween(Coords start, Coords end) {
    vector<Coords> coords;

    if(start.x != end.x) {
        for(int i = start.x; i <= end.x; i++) {
            coords.push_back(Coords(i, start.y, start.z));
        }
    } else if(start.y != end.y) {
        for(int i = start.y; i <= end.y; i++) {
            coords.push_back(Coords(start.x, i, start.z));
        }
    } else if(start.z != end.z) {
        for(int i = start.z; i <= end.z; i++) {
            coords.push_back(Coords(start.x, start.y, i));
        }
    } else {
        coords.push_back(start);
    }

    return coords;
}

Block* parseLineAsBlock(string line, int i) {
    //parse all 6 numbers in the line
    replace(line.begin(), line.end(), '~', ',');
    vector<int> numbers;
    stringstream ss(line);
    string token;
    while (getline(ss, token, ',')) {
        numbers.push_back(stoi(token));
    }

    //find all coords in this block
    Coords coordsStart(numbers[0], numbers[1], numbers[2]);
    Coords coordsEnd(numbers[3], numbers[4], numbers[5]);
    vector<Coords> coords = coordsBetween(coordsStart, coordsEnd);

    //create the block
    Block* block = new Block(coords, i);
    return block;
}

vector<Block*> parseBlocks(vector<string> lines) {
    vector<Block*> blocks;
    for(int i = 0; i < lines.size(); i++) {
        blocks.push_back(parseLineAsBlock(lines[i], i));
    }
    return blocks;
}

void deleteBlocks(vector<Block*> blocks) {
    for(int i = 0; i < blocks.size(); i++) {
        delete blocks[i];
    }
}

//essentially just nudge each block down 1 (checking for collisions) until none of the blocks can descend further.
//this is very inefficent and bad and there are easier ways but this is the least-error-prone way I found.
//if I cared more I could easily optimize this but it's not really necessary, it only takes a few seconds for part 1 and part 2.
void settleAllBlocks(vector<Block*> blocks) {
    bool finishedSettlingBlocks = false;

    int totalNumIterations = 0;
    while(!finishedSettlingBlocks) {

        int numDescendingBlocksThisLoop = 0;
        finishedSettlingBlocks = true; // will set to false if any of the blocks can descend this loop.

        for(Block* currentBlock : blocks) {
            bool currentBlockCanDescend = true;
            if(currentBlock->minZ == 1) {
                currentBlockCanDescend = false; // currentBlock is blocked by the floor
            } else {
                for(Block* otherBlock : blocks) {
                    if(otherBlock->id != currentBlock->id) {
                        if(!currentBlock->canDescendPast(otherBlock)) {
                            currentBlockCanDescend = false; //currentBlock is blocked by the other block.
                            break; // no need to check the rest of the loop.
                        }
                    }
                }
            }
            if(currentBlockCanDescend) {
                finishedSettlingBlocks = false;
                numDescendingBlocksThisLoop++;
                //move this block down by one.
                currentBlock->descendByOne();
            }
        }
        totalNumIterations++;
        if(totalNumIterations % 10 == 0) {
            cout << "settling blocks: " << totalNumIterations << " iterations so far. ";
            cout << numDescendingBlocksThisLoop << " blocks descended last iteration." << endl;
        }
    }

    cout << "settling took " << totalNumIterations << " iterations." << endl;
}

// each block can be supported by and support multiple other blocks.  This should set up all the relationships properly
void setupBlockDependencies(vector<Block*> blocks) {
    for(Block* currentBlock : blocks) {
        for(Block* otherBlock: blocks) {
            if(otherBlock->id != currentBlock->id) {
                if(!currentBlock->canDescendPast(otherBlock)) {
                    currentBlock->supportedBy.push_back(otherBlock);
                    otherBlock->supports.push_back(currentBlock);
                }
            }
        }
    }
}

bool checkIfCanBeRemoved(Block* blockToRemove) {
    for(Block * blockThisOneSupports: blockToRemove->supports) {
        if(blockThisOneSupports->supportedBy.size() == 1) {
            return false;
        }
    }
    return true;
}

void printGraph(vector<Block*> blocks) {
    for(Block* block: blocks) {
        for(Block* s: block->supports) {
            cout << "block " << block->id << " supports " << s->id << endl;
        }
        for(Block* sb: block->supportedBy) {
            cout << "block " << block->id << " is supportedBy " << sb->id << endl;
        }
    }
}

int getNumBlocksThatCanBeRemoved(vector<Block*> blocks) {
    int canBeRemoved = 0;
    for(Block* block: blocks) {
        if(checkIfCanBeRemoved(block)) {
            canBeRemoved++;
        }
    }
    return canBeRemoved;
}

long long part2(vector<string> lines) {

    return -1;
}

long long part1(vector<string> lines) {
    vector<Block*> blocks = parseBlocks(lines);

    //sort
    sort(blocks.begin(), blocks.end(), [](Block* a, Block* b) {
        return a->minZ < b->minZ;
    });

    settleAllBlocks(blocks);
    setupBlockDependencies(blocks);
    //printGraph(blocks);

    int result = getNumBlocksThatCanBeRemoved(blocks);

    deleteBlocks(blocks);
    return result;
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