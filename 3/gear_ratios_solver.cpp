
/***********************
 * Day 1:  Trebuchet?! *
 ***********************/

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

static const bool PART_2_ENABLED = true;

static const int MAX_RED_CUBES = 12;
static const int MAX_GREEN_CUBES = 13;
static const int MAX_BLUE_CUBES = 14;

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

//return the id of the game at this line if it's impossible, otherwise return 0
int getGameIdIfImpossible(string &str) {
    // add a termination character to simplify the loop
    str = str + ";";

    // setup string stream for parsing this line
    istringstream stringStream(str);
    string word;

    getline(stringStream, word, ' '); // skip the first line, "Game"
    getline(stringStream, word, ' '); // now `word` is the game id
    string gameIdStr = word.substr(0, word.size() - 1); // strip the colon from the end
    int gameId = stoi(gameIdStr); // parse as integer

    string countStr;
    string color;
    int count;

    // count and color will always come in pairs from now on
    while(getline(stringStream, countStr,' ')) {
        getline(stringStream, color, ' ');

        // parse color and count properly 
        color = color.substr(0, color.size() - 1);
        count = stoi(countStr);

        //now check against predefined limits
        if(color == "blue" && count > MAX_BLUE_CUBES) {
            return 0;
        }
        if(color == "green" && count > MAX_GREEN_CUBES) {
            return 0;
        }
        if(color == "red" && count > MAX_RED_CUBES) {
            return 0;
        }
    }

    // this game is possible
    return gameId;
}

// return the minimum red, green, and blue counts of the game multiplied together
int getPowerOfGame(string &str) {
    // add a termination character to simplify the loop
    str = str + ";";

    // setup string stream for parsing this line
    istringstream stringStream(str);
    string word;

    getline(stringStream, word, ' '); // skip the first line, "Game"
    getline(stringStream, word, ' '); // now `word` is the game id

    string countStr;
    string color;
    int count;

    int minRed = 0;
    int minBlue = 0;
    int minGreen = 0;

    // count and color will always come in pairs from now on
    while(getline(stringStream, countStr,' ')) {
        getline(stringStream, color, ' ');

        // parse color and count properly 
        color = color.substr(0, color.size() - 1);
        count = stoi(countStr);

        //now check against predefined limits
        if(color == "blue" && count > minBlue) {
            minBlue = count;
        }
        if(color == "green" && count > minGreen) {
            minGreen = count;
        }
        if(color == "red" && count > minRed) {
            minRed = count;
        }
    }

    return minRed * minBlue * minGreen;
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

    // compute sum of possible game ids
    int sum = 0;
    for(int i = 0; i < lines.size(); i++) {
        if(PART_2_ENABLED) {
            sum += getPowerOfGame(lines[i]);
        } else {
            sum += getGameIdIfImpossible(lines[i]);
        }
    }

    //print final result
    cout << "sum=" << sum << endl;

    return 0;
}