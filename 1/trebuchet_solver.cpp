
/***********************
 * Day 1:  Trebuchet?! *
 ***********************/

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

// this enables checking for digit strings as well as digit characters (for part 2).
static const bool PART_2_ENABLED = true; 

// all possible digit strings to search for (as well as backwards versions)
static const string DIGIT_STRINGS[] = {"one","two","three","four","five",
                                    "six", "seven", "eight", "nine"};
static const string DIGIT_STRINGS_REVERSED[] = {"eno", "owt", "eerht", "ruof", "evif",
                                                 "xis", "neves", "thgie", "enin"};

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

//find the leftmost digit character/string and return its integer representation
int getFirstDigitOrDigitString(string const &str) {
    //buffer for substring safety
    string buffer("____");
    string bufferedStr = str + buffer;

    //now loop through the string searching for digits
    for(int i = 0; i < str.size(); i++) {
        char c = str[i];
        if(isdigit(c)) {
            return c - '0'; //convert to int
        } else {
            // check for digit string.  TODO:  this can be made more efficient
            for(int j = 0; j < 9; j++) {
                string d = DIGIT_STRINGS[j];
                if(bufferedStr.substr(i, d.size()) == d) {
                    return j + 1;
                }
            }
        }
    }
    return -1;
}

//find the rightmost digit character/string and return its integer representation
int getLastDigitOrDigitString(string const &str) {
    //reverse the string first
    string reversedStr = str;
    reverse(reversedStr.begin(), reversedStr.end());

    //buffer for substring safety
    string buffer("____");
    string bufferedReversedStr = reversedStr + buffer;

    //now loop through the reversed string looking for digits
    for(int i = 0; i < reversedStr.size(); i++) {
        char c = reversedStr[i];
        if(isdigit(c)) {
            return c - '0'; //convert to int
        } else {
            // check for digit string.  TODO:  this can be made more efficient
            for(int j = 0; j < 9; j++) {
                string d = DIGIT_STRINGS_REVERSED[j];
                if(bufferedReversedStr.substr(i, d.size()) == d) {
                    return j + 1;
                }
            }
        }
    }
    return -1;
}

//find the leftmost digit character in the string and return it as an integer
int getFirstDigit(string const &str) {
    for(int i = 0; i < str.size(); i++) {
        char c = str[i];
        if(isdigit(c)) {
            return c - '0'; //convert to int
        }
    }
    return -1;
}

//find the rightmost digit character in the string and return it as an integer
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
    if(PART_2_ENABLED) {
        int first = getFirstDigitOrDigitString(str);
        int last = getLastDigitOrDigitString(str);
        return (first * 10) + last;
    } else {
        int first = getFirstDigit(str);
        int last = getLastDigit(str);
        return (first * 10) + last;
    }
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