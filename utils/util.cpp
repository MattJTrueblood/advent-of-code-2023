#include "util.h"

// print a string (without newline) with a specified color
void printColor(const string& text, color foreground, color background) {
    cout << "\033[" << foreground + 30 << ";" << background + 40 << "m" << text << "\033[0m\e[K";
}

// print a string (with newline) with a specified color
void printlnColor(const string& text, color foreground, color background) {
    printColor(text + '\n', foreground, background);
}

// read each line of a file as a string as return them inside a vector.
vector<string> parseFile(char* filename) {
    ifstream file(filename);
    string line;
    vector<string> lines;
    while (getline(file, line)) {
        lines.push_back(line);
    }
    return lines;
}

// read a line and separate it into space-delimited words.  Multiple spaces between words are handled properly.
vector<string> parseLine(string line) {
    istringstream stringStream(line);
    string word;
    vector<string> words;
    while(getline(stringStream, word, ' ')) {
        if(word.size() > 0) { // handles multiple spaces
             words.push_back(word);
        }
    }
    return words;
}

// calls parseLine for every line in a lines vector.
vector<vector<string>> parseAllLines(vector<string> lines) {
    vector<vector<string>> wordsInLines;
    for(int i = 0; i < lines.size(); i++) {
        wordsInLines.push_back(parseLine(lines[i]));
    }
    return wordsInLines;
}

// converts a number character (e.g. '4') to the int that it represents
int charToInt(char c) {
    return int(c) - 48;
}

// greatest common denominator
long long gcd(long long a, long long b)
{
    for (;;)
    {
        if (a == 0) return b;
        b %= a;
        if (b == 0) return a;
        a %= b;
    }
}

// least common multiple
long long lcm(long long a, long long b)
{
    long long temp = gcd(a, b);
    return temp ? ((a / temp) * b) : 0;
}

// least common multiple of a vector
long long lcm(const vector<long long>& nums) {
    long long result = 1;
    for (long long num : nums) {
        result = lcm(result, num);
    }
    return result;
}

// finds the row, col of neighbors above, below, left and right a given coord that are in range.
vector<pair<int, int>> findValidNeighborCoords(int row, int col, int numRows, int numCols) {
    vector<pair<int, int>> neighborCoords;
    if(row > 0) {
        neighborCoords.push_back(make_pair(row - 1, col));
    }
    if(row < numRows - 1) {
        neighborCoords.push_back(make_pair(row + 1, col));
    }
    if(col > 0) {
        neighborCoords.push_back(make_pair(row, col - 1));
    }
    if(col < numCols - 1){
        neighborCoords.push_back(make_pair(row, col + 1));
    }
    return neighborCoords;
}

pair<int, int> goInDirection(pair<int, int> coords, direction dir) {
    if(dir == UP_DIRECTION) {
        return make_pair(coords.first - 1, coords.second);
    }
    if(dir == DOWN_DIRECTION) {
        return make_pair(coords.first + 1, coords.second);
    }
    if(dir == LEFT_DIRECTION) {
        return make_pair(coords.first, coords.second - 1);
    }
    if(dir == RIGHT_DIRECTION) {
        return make_pair(coords.first, coords.second + 1);
    }

    //should never happen
    return coords;
}

pair<int, int> goInDirectionDistance(pair<int, int> coords, direction dir, int distance) {
    if(dir == UP_DIRECTION) {
        return make_pair(coords.first - distance, coords.second);
    }
    if(dir == DOWN_DIRECTION) {
        return make_pair(coords.first + distance, coords.second);
    }
    if(dir == LEFT_DIRECTION) {
        return make_pair(coords.first, coords.second - distance);
    }
    if(dir == RIGHT_DIRECTION) {
        return make_pair(coords.first, coords.second + distance);
    }

    //should never happen
    return coords;
}

string charToString(char c) {
    string s(1, c);
    return s;
}

size_t IntPairHash::operator()(const pair<int, int>& p) const {
    return hash<int>()(p.first) ^ (hash<int>()(p.second) << 1);
}