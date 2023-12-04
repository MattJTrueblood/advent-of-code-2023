
/**********************
 * Day N:  ~~~~~~~~~  *
 **********************/

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_set>

using namespace std;

// set this to false to get part 1 results instead
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

int calculateScore(string line) {
    istringstream stringStream(line);
    string word;

    //skip first 2 words (the "Card   X: " label) with arbitrary number of spaces between word 1 and word 2
    getline(stringStream, word, ' ');
    while(getline(stringStream, word, ' ')) {
        if(word.size() > 0) { 
            break;
        }
    }

    //collect winning numbers in a set
    unordered_set<int> winningNumbers;
    while(getline(stringStream, word, ' ')) {
        if(word.size() == 0) { // multiple spaces
            continue;
        }
        if(word == "|") {
            break;
        }
        winningNumbers.insert(stoi(word));
    }

    // calculate score using remaining numbers
    int score = 0;
    while(getline(stringStream, word, ' ')) {
        if(word.size() == 0) { // multiple spaces
            continue;
        }
        if(winningNumbers.count(stoi(word))) {
            score = score == 0 ? 1 : score * 2;
        }
    }
    return score;
}

int calculateTotalScore(vector<string> lines) {
    int sum = 0;
    for(int i = 0; i < lines.size(); i++) {
        string line = lines[i];
        sum += calculateScore(line);
    }
    return sum;
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

    int result = calculateTotalScore(lines);

    //print final result to console
    cout << "result=" << result << endl;
    return 0;
}