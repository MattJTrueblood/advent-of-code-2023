
/***********************
 * Day 1:  Trebuchet?! *
 ***********************/

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

// converts a filename argument to a string vector of all the lines in the file
vector<string> parseFile(char* filename) {
    ifstream file(filename);
    string line;
    vector<string> lines = vector<string>();
    while (getline(file, line)) {
        lines.push_back(line);
    }
    return lines;
}

//find the leftmost digit character in the string and return it (or -1 if not found)
int getFirstDigit(string const &str) {
    for(int i = 0; i < str.size(); i++) {
        char c = str[i];
        if(isdigit(c)) {
            return c - '0'; //convert to int
        }
    }
    return -1;
}

//find the rightmost digit character in the string and return it (or -1 if not found)
int getLastDigit(string const &str) {
    for(int i = str.size() - 1; i >= 0; i--) {
        char c = str[i];
        if(isdigit(c)) {
            return c - '0'; //convert to int
        }
    }
    return -1;
}

// finds the calibration value hidden in a string
int computeCalibrationValue(string const &str) {
    int first = getFirstDigit(str);
    int last = getLastDigit(str);
    return (first * 10) + last;
}

int main(int argc, char* argv[]) {
    //check usage correctness
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <filename>\n";
        return 1;
    }
    cout << "Filename: " << argv[1] << endl;

    //parse file
    vector<string> lines = parseFile(argv[1]);

    // compute calibration values for each line, then sum them all
    int sum = 0;
    for(int i = 0; i < lines.size(); i++) {
        int v = computeCalibrationValue(lines[i]);
        sum += v;
    }

    //print final result to console
    cout << "sum=" << sum << endl;

    return 0;
}