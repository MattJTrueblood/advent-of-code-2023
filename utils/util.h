#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <utility>

using std::string;
using std::vector;
using std::cout;
using std::ifstream;
using std::istringstream;
using std::pair;
using std::make_pair;
using std::endl;

// Useful enums
enum color { BLACK = 0, RED = 1, GREEN = 2, YELLOW = 3, BLUE = 4, MAGENTA = 5, CYAN = 6, WHITE = 7, DEFAULT = 9 };
enum direction { UP_DIRECTION = 0, RIGHT_DIRECTION = 1, DOWN_DIRECTION = 2, LEFT_DIRECTION = 3, NONE_DIRECTION = 4 };

// Function declarations
void printColor(const string& text, color foreground, color background);
void printlnColor(const string& text, color foreground, color background);
vector<string> parseFile(char* filename);
vector<string> parseLine(string line);
vector<vector<string>> parseAllLines(vector<string>);
int charToInt(char c);
long long gcd(long long a, long long b);
long long lcm(long long a, long long b);
vector<pair<int, int>> findValidNeighborCoords(int row, int col, int numRows, int numCols);
pair<int, int> goInDirection(pair<int, int> coords, direction dir);
string charToString(char c);

#endif // UTIL_H
