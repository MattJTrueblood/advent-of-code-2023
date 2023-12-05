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
