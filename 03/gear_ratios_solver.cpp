
/***********************
 * Day 1:  Trebuchet?! *
 ***********************/

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

static const bool PART_2_ENABLED = true;

// converts a filename to a string vector of all the lines in the file
vector<string> parseFile(char* filename) {
    ifstream file(filename);
    string line;
    vector<string> lines = vector<string>();
    while (getline(file, line)) {
        lines.push_back(line);
    }
    return lines;
}

//converts lines into a 2d grid of chars (probably not needed?  Just to be safe)
vector<vector<char>> generateGrid(vector<string> &lines) {
    vector<vector<char>> grid;
    for(int i = 0; i < lines.size(); i++) {
        vector<char> gridLine;
        for(int j = 0; j < lines[i].size(); j++) {
            gridLine.push_back(lines[i][j]);
        }
        grid.push_back(gridLine);
    }
    return grid;
}

// converts a char vector to the equivalent integer
int charVectorToInt(vector<char> vec) {
    int num = 0;
    int mod = 1;
    for(int i = 0; i < vec.size(); i++) {
        char c = vec[(vec.size() - 1) - i];
        int digit = c - '0';
        num += (digit * mod);
        mod = mod * 10;
    }
    return num;
}

// true if c is any character except 0-9 or period.
bool isSymbol(char c) {
    return !(isdigit(c) || c == '.');
}

// checks if it's a gear character (checking for 2 adjacent numbers is handled elsewhere)
bool isGear(char c) {
    return c == '*';
}

// find all in bounds neighbors of grid[i][k]
// return true if any of them are symbols, else false
bool hasAdjacentSymbol(vector<vector<char>> grid, int i, int j) {
    vector<char> charsToCheck;

    //check bounds
    bool rightIsValid = j < grid[0].size() - 1;
    bool upIsValid = i > 0;
    bool leftIsValid = j > 0;
    bool downIsValid = i < grid.size() - 1;

    //add all in-bounds neighbors
    if(upIsValid) {
        charsToCheck.push_back(grid[i-1][j]);
        if(leftIsValid) {
            charsToCheck.push_back(grid[i-1][j-1]);
        }
        if(rightIsValid) {
            charsToCheck.push_back(grid[i-1][j+1]);
        }
    }
    if(leftIsValid) {
        charsToCheck.push_back(grid[i][j-1]);
    }
    if(rightIsValid) {
        charsToCheck.push_back(grid[i][j+1]);
    }
    if(downIsValid) {
        charsToCheck.push_back(grid[i+1][j]);
        if(leftIsValid) {
            charsToCheck.push_back(grid[i+1][j-1]);
        }
        if(rightIsValid) {
            charsToCheck.push_back(grid[i+1][j+1]);
        }
    }

    // return true if any of those chars are symbols
    for(int k=0; k < charsToCheck.size(); k++) {
        if(isSymbol(charsToCheck[k])) {
            return true;
        }
    }

    // no adjacent symbols
    return false;
}

// goes through the grid to find all part numbers (numbers with adjacent symbols)
// convert the part numbers to numbers and sum them up.  Return the sum.
int getSumOfAllPartNumbers(vector<vector<char>> grid) {

    vector<char> currentNumberVector;
    bool currentlyReadingNumber = false;
    bool currentNumberIsPartNumber = false;
    int sum = 0;

    for(int i= 0; i < grid.size(); i++) {
        for(int j = 0; j < grid[0].size(); j++) {
            char c = grid[i][j];
            //cout << c;
            if(isdigit(c)) {
                currentNumberVector.push_back(c);
                currentlyReadingNumber = true;
                if(!currentNumberIsPartNumber) {
                    if(hasAdjacentSymbol(grid, i, j)) {
                        currentNumberIsPartNumber = true;
                    }
                }
            } else {
                if(currentlyReadingNumber) {
                    int currentNumber = charVectorToInt(currentNumberVector);
                    if(currentNumberIsPartNumber) {
                        int currentNumber = charVectorToInt(currentNumberVector);
                        //cout << currentNumber << " is a part number" << endl;
                        sum += currentNumber;
                    } else {
                        int currentNumber = charVectorToInt(currentNumberVector);
                        //cout << currentNumber << " is NOT a part number" << endl;
                    }
                    currentNumberVector.clear();
                    currentlyReadingNumber = false;
                    currentNumberIsPartNumber = false;
                }
            }
        }
    }
    return sum;
}

//uses a part num grid and i,j coordinates to find the gear ratio for a gear at that tile (product of adjacent numbers)
int calculateGearRatio(vector<vector<int>> grid, int i, int j) {
    //check bounds
    bool rightIsValid = j < grid[0].size() - 1;
    bool upIsValid = i > 0;
    bool leftIsValid = j > 0;
    bool downIsValid = i < grid.size() - 1;

    bool foundGap = false;
    bool lookingForNumOne = true;

    int numOne = -1;
    int numTwo = -1;

    int neighbors[] = { grid[i-1][j-1], grid[i-1][j], grid[i-1][j+1],
                        grid[i][j-1],                 grid[i][j+1],
                        grid[i+1][j-1], grid[i+1][j], grid[i+1][j+1] };

    // make sure you get separate numbers.  This is surprisingly annoying to do and requires checking gaps and line breaks and such
    // in the rare possible case that the numOne and numTwo are the equal but separate numbers on the grid.  This is a consequence of
    // the hacky way we're doing this where the same number is repeated in every tile it's located in in the grid here, so we need to 
    // be able to distinguish between true duplicates and false duplicates.  
    for(int k = 0; k < 8; k++) {
        if(lookingForNumOne) {
            if(neighbors[k] != 0) {
                numOne = neighbors[k];
                lookingForNumOne = false;
            }
        }
        else if(!lookingForNumOne) {
            if(!foundGap) {
                if(k == 3 || k == 4 || k == 5) {
                    //natural gap (e.g. neighbor 3, to the left, is always a different number from neighbor 2, to the up/right)
                    foundGap = true;
                }
                if(neighbors[k] == 0) {
                    // normal gap
                    foundGap = true;
                }
            }
            if(foundGap && neighbors[k] != 0) {
                //std::cout << "num two set to " << neighbors[k] << endl;;
                numTwo = neighbors[k];
                break;
            }
        }
    }

    //if only one neighbor, it's not a real gear.
    if(numOne == -1 || numTwo == -1) {
        return 0;
    }

    return numOne * numTwo;
}

// find all gear ratio sums
int getSumOfAllGearRatios(vector<vector<char>> grid) {

    vector<char> currentNumberVector;
    bool currentlyReadingNumber = false;
    bool currentNumberIsPartNumber = false;
    int currentNumberDigitCount = 0;

    // this vector is used to track part nums at [i][j]
    // kinda hacky way to make this work with my existing part 1 implementation
    // 0 = no digit.  >0 = digit (same for each tile the digit is in).  -1 = gear
    vector<vector<int>> partNumGrid;

    for(int i= 0; i < grid.size(); i++) {
        vector<int> partNumGridLine;
        for(int j = 0; j < grid[0].size(); j++) {
            char c = grid[i][j];
            int currentPartGridValue = 0; // no part set here yet.
            if(isdigit(c)) {
                currentNumberDigitCount++;
                currentNumberVector.push_back(c);
                currentlyReadingNumber = true;
                if(!currentNumberIsPartNumber) {
                    if(hasAdjacentSymbol(grid, i, j)) {
                        currentNumberIsPartNumber = true;
                    }
                }
            } else {
                if(currentlyReadingNumber) {
                    int currentNumber = charVectorToInt(currentNumberVector);
                    if(currentNumberIsPartNumber) {
                        int currentNumber = charVectorToInt(currentNumberVector);

                        //set all previous partNumGrid tiles this number is in to currentNumber
                        if(j != 0) { //normal case, update the previous values in this line 
                            for(int k = 0; k < currentNumberDigitCount; k++) {
                                partNumGridLine[partNumGridLine.size() - 1 - k] = currentNumber;
                            }
                        } else { //special case, this is the first column, we need to set the value from the end of the previous line
                            for(int k = 0; k < currentNumberDigitCount; k++) {
                                partNumGrid[partNumGrid.size() - 1][partNumGrid[0].size() - 1 - k] = currentNumber;
                            }    
                        }
                    }
                    currentNumberVector.clear();
                    currentlyReadingNumber = false;
                    currentNumberIsPartNumber = false;
                    currentNumberDigitCount = 0;
                }
                if(isGear(c)) {
                    currentPartGridValue = -1;
                }
            }
            partNumGridLine.push_back(currentPartGridValue);
        }
        partNumGrid.push_back(partNumGridLine);
    }

    //now go through the partnum grid and sum the gear ratios for each gear you find there
    int gearRatioSum = 0;
    for(int i= 0; i < partNumGrid.size(); i++) {
        for(int j = 0; j < partNumGrid[0].size(); j++) {
            int n = partNumGrid[i][j];
            if(n == -1) {
                // gear tile
                gearRatioSum += calculateGearRatio(partNumGrid, i, j);
            }
        }
    }

    return gearRatioSum;
}

int main(int argc, char* argv[]) {
    // check usage correctness
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <filename>\n";
        return 1;
    }
    cout << "Filename: " << argv[1] << endl;

    // parse file
    vector<string> lines = parseFile(argv[1]);

    // calculate result
    vector<vector<char>> grid = generateGrid(lines);
    int sum;
    if(PART_2_ENABLED) {
        sum = getSumOfAllGearRatios(grid);
    } else {
        sum = getSumOfAllPartNumbers(grid);
    }   

    // print final result
    cout << "sum=" << sum << endl;

    return 0;
}