
/********************************************
 * Day 5:  If You Give A Seed A Fertilizer  *
 ********************************************/

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <utility>
#include <algorithm>
#include <limits>

using namespace std;

// set this to false to get part 1 results instead
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

// separates each lines into words (any character string separated by spaces) and returns a vector of vectors of those words
vector<vector<string>> parseLinesIntoSpaceSeparatedWords(vector<string> lines) {
    vector<vector<string>> wordLines;

    for(int i = 0; i < lines.size(); i++) {
        string line = lines[i];
        istringstream stringStream(line);

        vector<string> words;
        string word;
        while(getline(stringStream, word, ' ')) {
            words.push_back(word);
        }
        wordLines.push_back(words);
    }

    return wordLines;
}

vector<long> getSeedNumbers(vector<string> line) {
    vector<long> seeds;
    for(int i = 1; i < line.size(); i++) {
        seeds.push_back(stol(line[i]));
    }
    return seeds;
}

vector<vector<vector<long>>*> parseMapChain(vector<vector<string>> wordLines) {
    vector<vector<vector<long>>*> mapsChain;

    vector<vector<long>> * currentMap;
    for(int i = 1; i < wordLines.size(); i++) {
        vector<string> line = wordLines[i];
        if(line.size() == 0) { // empty line
            currentMap = new vector<vector<long>>();
            mapsChain.push_back(currentMap);
        }
        else if(isdigit(line[0][0])) { // number line
            long destination = stol(line[0]);
            long source = stol(line[1]);
            long rangeLength = stol(line[2]);

            long sourceEnd = source + rangeLength - 1;
            currentMap->push_back(vector<long>({source, sourceEnd, destination}));
        }
    }

    //sort each map in the chain in assending order
    for(int i = 0; i < mapsChain.size(); i++) {
        vector<vector<long>> * v = mapsChain[i];
        sort(v->begin(), v->end(), [](auto &left, auto &right) {
            return left[0] < right[0];
        });
    }

    return mapsChain;
}

void deleteMapChain(vector<vector<vector<long>>*> mapChain) {
    for(int i = 0; i < mapChain.size(); i++) {
        delete mapChain[i];
    }
}

long findSeedLocation(vector<vector<vector<long>>*> mapChain, long seedNumber) {
    long currentSource = seedNumber;
    for(int i = 0; i < mapChain.size(); i++) {
        //find the destination that maps this source and store it in source.
        long destination = currentSource; // default, if no range found in next step
        for(int j = 0; j < mapChain[i]->size(); j++) {
            vector<long> ranges = mapChain[i]->at(j);
            if(currentSource >= ranges[0] && currentSource <= ranges[1]) {
                destination = ranges[2] + (currentSource - ranges[0]);
                break;
            }
        }
        //cout << currentSource << " goes to " << destination << endl;
        currentSource = destination;
    }
    //cout << "seed " << seedNumber << " goes to " << currentSource << endl;
    return currentSource; // should be the location by the end
}

// part 1 result solver
int getLowestSeedLocation(vector<string> lines) {
    vector<vector<string>> wordLines = parseLinesIntoSpaceSeparatedWords(lines);
    vector<long> seeds = getSeedNumbers(wordLines[0]);
    vector<vector<vector<long>>*> mapChain = parseMapChain(wordLines);

    long smallestLocation = numeric_limits<long>::max();
    for(int i = 0; i < seeds.size(); i++) {
        long seedLocation = findSeedLocation(mapChain, seeds[i]);
        if(seedLocation < smallestLocation) {
            smallestLocation = seedLocation;
        }
    }

    deleteMapChain(mapChain);
    return smallestLocation;
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

    // now handle lines to generate the result
    int result;
    if(PART_2_ENABLED) {
        return 0; // TODO
    } else {
        result = getLowestSeedLocation(lines);
    }

    //print final result to console
    cout << "result=" << result << endl;
    return 0;
}