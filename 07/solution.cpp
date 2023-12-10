
/**********************
 * Day 7:  ~~~~~~~~~  *
 **********************/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <utility>
#include "../utils/util.h"

using namespace std;

// set this to false to get part 1 results instead
static const bool PART_2_ENABLED = true;

enum camel_card {A, K, Q, J, T, NINE, EIGHT, SEVEN, SIX, FIVE, FOUR, THREE, TWO, JOKER };
enum hand_type {FIVE_OF_A_KIND, FOUR_OF_A_KIND, FULL_HOUSE, THREE_OF_A_KIND, TWO_PAIR, ONE_PAIR, HIGH_CARD };

class Hand {
public:
    string baseString;
    vector<camel_card> cards;
    hand_type handType;
    int bet;

    Hand(string cardString, string betString) {
        baseString = cardString;
        parseHand(cardString);
        parseBet(betString);
        if(PART_2_ENABLED) {
            computeHandTypeWithJoker(cards);
        } else {
            computeHandType(cards);
        }
    }

    void parseHand(string s) {
        for(int i = 0; i < s.size(); i++) {
            camel_card card;
            if(isdigit(s[i])) {
                int num = charToInt(s[i]);
                card = camel_card(12 - (num - 2)); // the numbers 2 to 9 are in descending order in range (5 to 12) 
            }
            else {
                switch(s[i]) {
                case 'A':
                    card = A; break;
                case 'K':
                    card = K; break;
                case 'Q':
                    card = Q; break;
                case 'J':
                    card = PART_2_ENABLED ? JOKER : J; break;
                case 'T':
                    card = T; break;
                }
            }
            cards.push_back(card);
        }
    }

    void parseBet(string s) {
        bet = stoi(s);
    }

    void computeHandType(vector<camel_card> hand) {
        // get a sorted-by-descending-order list of card frequencies in this hand
        map<int, int> cardOccurrences;
        for (auto i : hand) {
            cardOccurrences[i]++;
        }
        vector<int> sortedOccurrenceValues;
        for (auto [element, count] : cardOccurrences) {
            sortedOccurrenceValues.push_back(count);
        }
        sort(sortedOccurrenceValues.begin(), sortedOccurrenceValues.end(), greater<int>()); // descending

        // we only need to check the two highest counts to get the hand type 
        computeHandTypeFromHighestTwoCardCounts(sortedOccurrenceValues[0], sortedOccurrenceValues[1]);
    }

    void computeHandTypeFromHighestTwoCardCounts(int firstCount, int secondCount) {
        if(firstCount == 5) {
            handType = FIVE_OF_A_KIND;
        } else if (firstCount == 4) {
            handType = FOUR_OF_A_KIND;
        } else if (firstCount == 3) {
            if(secondCount == 2) {
                handType = FULL_HOUSE;
            } else {
                handType = THREE_OF_A_KIND;
            }
        } else if (firstCount == 2) {
            if(secondCount == 2) {
                handType = TWO_PAIR;
            } else {
                handType = ONE_PAIR;
            }
        } else {
            handType = HIGH_CARD;
        }
    }

    void computeHandTypeWithJoker(vector<camel_card> hand) {
        // get a sorted-by-descending-order list of card value/frequencies in this hand
        map<int, int> cardOccurrences;
        for (auto i : hand) {
            cardOccurrences[i]++;
        }
        vector<pair<int, int>> sortedOccurrenceKeyValues;
        int numJokers = 0;
        for (auto [element, count] : cardOccurrences) {
            // if it's 5 jokers, 5-of-a-kind should already count
            // if it's 1 to 4 jokers, instead of adding their occurrences, track them separately and add them to the highest count later.
            if(element == JOKER && count != 5) {
                numJokers = count; 
            } else {
               sortedOccurrenceKeyValues.push_back({element, count});
            }
        }
        sort(sortedOccurrenceKeyValues.begin(), sortedOccurrenceKeyValues.end(), [](pair<int, int> left, pair<int, int> right) {
            return left.second > right.second; // sort by count descending
        });

        //adding the numJokers to the highest count should handle the joker behavior properly.
        computeHandTypeFromHighestTwoCardCounts(sortedOccurrenceKeyValues[0].second + numJokers, sortedOccurrenceKeyValues[1].second);
    }

    static bool compareHands(Hand left, Hand right) {
        //return left.bet < right.bet;  //sort from lowest to highest bet
        if(left.handType != right.handType) {
            return left.handType > right.handType;
        } else {
            for(int i = 0; i < left.cards.size(); i++) {
                if(left.cards[i] != right.cards[i]) {
                    return left.cards[i] > right.cards[i];
                }
            }
            cout << "WE HAVE A PROBLEM HOUSTON" << endl;
            return false;
        }
    }
};

int computeTotalWinnings(vector<vector<string>> words) {
    // convert words to a better format
    vector<int> bets;
    vector<Hand> hands;
    for(int i = 0; i < words.size(); i++) {
        hands.push_back(Hand(words[i][0], words[i][1]));
    }

    sort(hands.begin(), hands.end(), Hand::compareHands);

    //calculate rank and use it to find total winnnings
    int totalWinnings = 0;
    for(int i = 0; i < hands.size(); i++) {
        totalWinnings += (i + 1) * hands[i].bet;
    }

    return totalWinnings;
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
    vector<vector<string>> words = parseAllLines(lines);

    // now handle lines to generate the result
    // usually we call a different function for PART_2_ENABLED but I tried something different here.  Instead, we can minimized
    //   code duplication by just handling some of the methods of Hand differently
    int result = computeTotalWinnings(words);

    //print final result to console
    cout << "result=" << result << endl;
    return 0;
}