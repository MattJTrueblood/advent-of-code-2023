
/************************
 * Day 4: Scratchcards  *
 ************************/

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

using namespace std;

// set this to false to get part 1 results instead
static const bool PART_2_ENABLED = true;

// this tracks the number of copies of each card you have right now.
unordered_map<int, int> copiesPerCard;

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

//calculates the score for a line in part 1 (2^(winning numbers - 1))
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

// updates the copiesPerCard map for the subsequent cards based on numCopies
// and how many winning numbers are in this card
void addCopiesForCardLine(string line, int cardNum, int numCopies) {
    istringstream stringStream(line);
    string word;

    //skip first 2 words (the "Card   X: " label) with arbitrary number of spaces between word 1 and word 2
    getline(stringStream, word, ' ');
    while(getline(stringStream, word, ' ')) {
        if(word.size() > 0) { 
            break; // we could technically get cardNum here instead but this is less hassle
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

    // find winning numbers that match and update copiesPerCard accordingly if they do
    int numWinningCards = 0;
    while(getline(stringStream, word, ' ')) {
        if(word.size() == 0) { // multiple spaces
            continue;
        }
        if(winningNumbers.count(stoi(word))) {
            numWinningCards++;

            // add 1 copy of the next card for each copy of this card
            if(copiesPerCard.count(cardNum + numWinningCards)) {
                copiesPerCard[cardNum + numWinningCards] = copiesPerCard[cardNum + numWinningCards] + numCopies;
            } else {
                copiesPerCard.emplace(cardNum + numWinningCards, numCopies);
            }
        }
    }
    
    return;
}

//part 1 calculation
int calculateTotalScore(vector<string> lines) {
    int sum = 0;
    for(int i = 0; i < lines.size(); i++) {
        string line = lines[i];
        sum += calculateScore(line);
    }
    return sum;
}

//part 2 calculation
int calculateTotalCards(vector<string> lines) {
    int total = 0;
    for(int i = 0; i < lines.size(); i++) {
        string line = lines[i];
        int cardNum = i+1;

        //get numcopies for this card
        if(copiesPerCard.count(cardNum)) {
            copiesPerCard[cardNum] = copiesPerCard[cardNum] + 1;
        } else {
            copiesPerCard.emplace(cardNum, 1);
        }
        int copies = copiesPerCard.at(cardNum);
        total += copies;

        //now iterate copy count of the next (# winning num) cards based on winning nums from this card
        addCopiesForCardLine(line, cardNum, copies);
    }
    return total;
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

    int result;
    if(PART_2_ENABLED) {
        result = calculateTotalCards(lines);
    }
    else {
        result = calculateTotalScore(lines);
    }

    //print final result to console
    cout << "result=" << result << endl;
    return 0;
}