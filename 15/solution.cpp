
/**********************
 * Day N:  ~~~~~~~~~  *
 **********************/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include "../utils/util.h"

using namespace std;

static const bool PART_2_ENABLED = true;

// read a line and separate it into comma-delimited words.
vector<string> parseCommaSeparatedLine(string line) {
    istringstream stringStream(line);
    string word;
    vector<string> words;
    while(getline(stringStream, word, ',')) {
        words.push_back(word);
    }
    return words;
}

int getWordHash(string s) {
    int hash = 0;
    for(int i = 0; i < s.size(); i++) {
        char c = s[i];
        hash = hash + (int) c;
        hash = hash * 17;
        hash = hash % 256;
    }
    return hash;
}

string getWordLabel(string s) {
    for(int i = 0; i < s.size(); i++) {
        char c = s[i];
        if(c == '=' || c == '-') {
            return s.substr(0, i);
        }
    }
    return "???";
}

char getWordOperation(string s) {
    for(int i = 0; i < s.size(); i++) {
        char c = s[i];
        if(c == '=' || c == '-') {
            return c;
        }
    }
    return '?';
}

int getWordFocalLength(string s) {
    for(int i = 0; i < s.size(); i++) {
        char c = s[i];
        if(c == '=') {
            return charToInt(s[i+1]);
        }
    }
    return -1;
}

int getSumHash(vector<string> lines) {
    //should only be one line.
    string line = lines[0];
    vector<string> words = parseCommaSeparatedLine(line);
    int sumHash = 0;
    for(int i = 0; i < words.size(); i++) {
        sumHash += getWordHash(words[i]);
    }
    return sumHash;
}

void processWord(map<int, vector<string>*> &labelMap, map<int, vector<int>*> &focalLengthMap, string word) {
    string label = getWordLabel(word);
    char operation = getWordOperation(word);
    int labelHash = getWordHash(label);
    if(operation == '=') {
        int focalLength = getWordFocalLength(word);
        vector<string>* boxLabels = labelMap[labelHash];
        vector<int>* boxFocalLengths = focalLengthMap[labelHash];
        bool foundLabelInBox = false;
        //replace focal length if already exists
        for(int j = 0; j < boxLabels->size(); j++) {
            if(boxLabels->at(j) == label) {
                foundLabelInBox = true;
                boxFocalLengths->at(j) = focalLength;
            }
        }
        //else add the new element to the boxes
        if(!foundLabelInBox) {
            boxLabels->push_back(label);
            boxFocalLengths->push_back(focalLength);
        }
    } else if (operation == '-') {
        vector<string>* boxLabels = labelMap[labelHash];
        vector<int>* boxFocalLengths = focalLengthMap[labelHash];
        for(int j = 0; j < boxLabels->size(); j++) {
            if(boxLabels->at(j) == label) {
                boxLabels->erase(boxLabels->begin() + j);
                boxFocalLengths->erase(boxFocalLengths->begin() + j);
            }
        }
    }
}

int getFocusingPower(vector<string> lines) {
    //should only be one line.
    string line = lines[0];
    vector<string> words = parseCommaSeparatedLine(line);

    //create the hashmaps, allocating the memory
    map<int, vector<string>*> labelMap;
    map<int, vector<int>*> focalLengthMap;
    for(int i = 0; i < 256; i++) {
        labelMap[i] = new vector<string>();
        focalLengthMap[i] = new vector<int>();
    }

    // add or remove or update the words in the maps based on each word
    for(int i = 0; i < words.size(); i++) {
        processWord(labelMap, focalLengthMap, words[i]);
    }

    // now calculate the result
    int totalFocusingPower = 0;
    for(int i = 0; i < 256; i++) {
        vector<string>* labels = labelMap[i];
        vector<int>* focalLengths = focalLengthMap[i];
        for(int j = 0; j < labels->size(); j++) {
            totalFocusingPower += (i + 1) * (j + 1) * focalLengths->at(j);
        }

        //free the memory now that we're done with these vectors
        delete labelMap[i];
        delete focalLengthMap[i];
    }

    return totalFocusingPower;
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
        result = getFocusingPower(lines);
    } else {
        result = getSumHash(lines);
    }

    //print final result to console
    cout << "result=" << result << endl;
    return 0;
}