
/***********************
 * Day 1:  Trebuchet?! *
 ***********************/

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

static const bool PART_2_ENABLED = false;

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
    int sum = getSumOfAllPartNumbers(grid);

    // print final result
    cout << "sum=" << sum << endl;

    return 0;
}