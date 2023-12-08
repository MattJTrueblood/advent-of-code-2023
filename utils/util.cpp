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
