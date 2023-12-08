#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

using std::string;
using std::vector;
using std::cout;
using std::ifstream;
using std::istringstream;

// Enumeration for color
enum color { BLACK = 0, RED = 1, GREEN = 2, YELLOW = 3, BLUE = 4, MAGENTA = 5, CYAN = 6, WHITE = 7, DEFAULT = 9 };

// Function declarations
void printColor(const string& text, color foreground, color background);
void printlnColor(const string& text, color foreground, color background);
vector<string> parseFile(char* filename);
vector<string> parseLine(string line);
vector<vector<string>> parseAllLines(vector<string>);
int charToInt(char c);
long long gcd(long long a, long long b);
long long lcm(long long a, long long b);

#endif // UTIL_H
