
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
        currentSource = destination;
    }
    return currentSource; // should be the location by the end
}

// part 1 result solver
long getLowestSeedLocation(vector<string> lines) {
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

// For each step, we have a set of source ranges.  Each source range will generate some number of destination ranges from the
// current map ranges, we iterate through this process until we've gone through each step in the map chain.
// At the end, we go through every single range in the final sourceRanges and find the absolute lowest value and return it
// This is extremely finicky since there are so many special cases (e.g. where a sourceRange start end partially overlaps a given
// map range, or starts in a map range and ends outside it, etc.) which is why this function is so stupidly complicated.
// There's no easy way to do this outside of a brute-force for(seedStart to seedEnd) approach with the part1 solution.
// This method is the "efficient" solution to the Part 2 problem, but it sacrifices legibility/simplicity and took ages to code.
long findLowestLocationFromSeedRange(vector<vector<vector<long>>*> mapChain, long seedStart, long seedEnd) {

    vector<pair<long, long>> sourceRanges;
    sourceRanges.push_back(make_pair(seedStart, seedEnd));

    for(int i = 0; i < mapChain.size(); i++) {
        vector<vector<long>> *currentMap = mapChain[i];
        //find all destination ranges from all source ranges
        vector<pair<long, long>> destinationRanges;
        for(int j = 0; j < sourceRanges.size(); j++) {
            pair<long, long> currentSourceRange = sourceRanges[j];
            long rangeStart = currentSourceRange.first;
            long rangeEnd = currentSourceRange.second;
            //checking if the currentSourceRange is entirely before the first mapping or after the last mapping
            if(rangeEnd < currentMap->at(0)[0] || rangeStart > currentMap->at(currentMap->size() - 1)[1]) {
                destinationRanges.push_back(make_pair(rangeStart, rangeEnd));
            }
            else {
                // go through each mapping range, find all destination ranges between rangeStart,rangeEnd
                for(int k = 0; k < currentMap->size(); k++) {
                    vector<long> mapRange = currentMap->at(k);
                    long mapRangeSourceStart = mapRange[0];
                    long mapRangeSourceEnd = mapRange[1];
                    long mapRangeDestinationStart = mapRange[2];
                    //if we're starting ahead of this range, then skip to the next range 
                    if(rangeStart > mapRangeSourceEnd) {
                        continue;
                    }
                    // default mappings before mapRangeSourceStart
                    if(rangeStart < mapRangeSourceStart) {
                        // this is true if it stops in the middle of a default block.  can skip the rest of the mapRanges for this range
                        if(rangeEnd < mapRangeSourceStart) {
                            destinationRanges.push_back(make_pair(rangeStart, rangeEnd));
                            break;
                        } else {
                            destinationRanges.push_back(make_pair(rangeStart, mapRangeSourceStart - 1));
                            rangeStart = mapRangeSourceStart; //now we move the rangestart up so we can start the in-maprange block next
                        }
                    }
                    //if start is in range (should be true now but let's just check to make sure)
                    if(rangeStart >= mapRangeSourceStart && rangeStart <= mapRangeSourceEnd) {
                        long rangeStartOffsetInMapRange = rangeStart - mapRangeSourceStart;
                        if(rangeEnd <= mapRangeSourceEnd) {
                            // both rangestart and rangeEnd are within this mapRange
                            long rangeEndOffsetInMapRange = rangeEnd - mapRangeSourceStart;
                            destinationRanges.push_back(make_pair(mapRangeDestinationStart + rangeStartOffsetInMapRange, 
                                                                  mapRangeDestinationStart + rangeEndOffsetInMapRange));
                            break; //we're done with this sourceRange
                        } else {
                            // rangeEnd is after the end of this mapRange
                            destinationRanges.push_back(make_pair(mapRangeDestinationStart + rangeStartOffsetInMapRange,
                                                                  mapRangeDestinationStart + (mapRangeSourceEnd - mapRangeSourceStart)));
                            rangeStart = mapRangeSourceEnd + 1;
                            // if there is a default block remaining after this add it at the end
                            if(k == currentMap->size() - 1) {
                                destinationRanges.push_back(make_pair(rangeStart, rangeEnd));
                                //break; // should be no need at this point
                            }
                        }
                    }
                }
            }
        }
        sourceRanges = destinationRanges;
    }

    //now sourceRanges is all the ranges of locations this seed range goes in.  Find the smallest.
    long smallestLocation = numeric_limits<long>::max();
    for(int i = 0; i < sourceRanges.size(); i++) {
        long location = sourceRanges[i].first; // we only care about the lowest value in the range (the start)
        if(location < smallestLocation) {
            smallestLocation = location;
        }
    }
    return smallestLocation;
}

// part 2 result solver
long getLowestSeedLocationPart2(vector<string> lines) {
    vector<vector<string>> wordLines = parseLinesIntoSpaceSeparatedWords(lines);
    vector<long> seeds = getSeedNumbers(wordLines[0]);
    vector<vector<vector<long>>*> mapChain = parseMapChain(wordLines);

    long smallestLocation = numeric_limits<long>::max();
    for(int i = 0; i < seeds.size(); i+=2) {
         //inclusive range, e.g. 1, 3 = seeds 1, 2, and 3
        long seedLocation = findLowestLocationFromSeedRange(mapChain, seeds[i], seeds[i] + seeds[i + 1] - 1);
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
    long result;
    if(PART_2_ENABLED) {
        result = getLowestSeedLocationPart2(lines);
    } else {
        result = getLowestSeedLocation(lines);
    }

    //print final result to console
    cout << "result=" << result << endl;
    return 0;
}