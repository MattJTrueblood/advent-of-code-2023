
/********************
 * Day 19: Aplenty  *
 ********************/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include "../utils/util.h"

using namespace std;

static const bool PART_2_ENABLED = true;

class Part {
public:
    int x;
    int m;
    int a;
    int s;

    Part() {}
    
    Part(string ratings){
        parseAllRatingsString(ratings);
    }

    void parseAllRatingsString(string ratings) {
        ratings = ratings.substr(1, ratings.length() - 2);

        istringstream myStringStream(ratings);
        string rating;

        while (getline(myStringStream, rating, ',')) {
            parseRating(rating);
        }
    }

    void parseRating(string rating) {
        char category = rating[0];
        int value = stoi(rating.substr(2));

        if(category == 'x') x = value;
        if(category == 'm') m = value;
        if(category == 'a') a = value;
        if(category == 's') s = value;
    }

    int sum() {
        return x + m + a + s;
    }
};

class PartRanges {
public:
    pair<int, int> xRange;
    pair<int, int> mRange;
    pair<int, int> aRange;
    pair<int, int> sRange;

    // sometimes we just want to mark a range as invalid.  We can set this to true for that.
    bool invalidRange = false; 

    PartRanges() {};

    long long calculateCombinations() {
        long long possibleX = ((xRange.second - xRange.first) + 1);
        long long possibleM = ((mRange.second - mRange.first) + 1);
        long long possibleA = ((aRange.second - aRange.first) + 1);
        long long possibleS = ((sRange.second - sRange.first) + 1);
        return (possibleX * possibleM * possibleA * possibleS);
    }

    bool isValid() {
        return (xRange.second >= xRange.first
            && mRange.second >= mRange.first
            && aRange.second >= aRange.first
            && sRange.second >= sRange.first);
    }
};

enum condition_type { GREATER_THAN = 0, LESS_THAN = 1, NO_CONDITION = 2 };

class Rule {
public:
    condition_type condition;
    char condition_part;
    int condition_val;
    string destinationName;

    Rule() {}

    Rule(string ruleString) {
        parseRuleString(ruleString);
    }

    void parseRuleString(string ruleString) {
        int colonIndex = ruleString.find(':');
        if (colonIndex == string::npos) {
            condition = NO_CONDITION;
            destinationName = ruleString;
        } else {
            string left = ruleString.substr(0, colonIndex);
            string right = ruleString.substr(colonIndex + 1);

            int conditionIndex = left.find('<');
            if (conditionIndex == string::npos) {
                conditionIndex = left.find('>');
            }

            string leftCondition = left.substr(0, conditionIndex);
            string rightCondition = left.substr(conditionIndex + 1);

            destinationName = right;
            condition = (ruleString[conditionIndex] == '<') ? LESS_THAN : GREATER_THAN;
            condition_part = leftCondition[0];
            condition_val = stoi(rightCondition);
        }
    }

    bool checkRule(Part part) {
        if(condition == NO_CONDITION) {
            return true;
        }

        int partValue;
        if(condition_part == 'x') partValue = part.x;
        if(condition_part == 'm') partValue = part.m;
        if(condition_part == 'a') partValue = part.a;
        if(condition_part == 's') partValue = part.s;

        if(condition == LESS_THAN) {
            return (partValue < condition_val);
        }
        //else GREATER_THAN condition
        return (partValue > condition_val);
    }

    //returns two new PartRanges.  The first is the ranges with this rule.  The second is the ranges without this rule.
    // e.g. ranges x[1,4000]... and rule: x<2000 will return ranges: first = (x[1, 1999]...), second = (x(2000, 4000]...))
    pair<PartRanges, PartRanges> checkRuleWithRanges(PartRanges ranges) {

        //this code is tricky to remove duplication for.  Let's keep it verbose for now but be explicit about the rules and edge cases.
        PartRanges inclusive = ranges;
        PartRanges exclusive = ranges;

        // only one scenario for NO_CONDITION:  simply return the inclusive range as the whole range and exclusive range as invalid.
        if(condition == NO_CONDITION) {
            exclusive.invalidRange = true;
        }

        // three scenarios for LESS THAN:
        //   1) value is less than min input range.  e.g. rule x < 1000, range=x[2000, 3000].  should return {invalid, x[2000,3000]}.
        //   2) value is within range.  e.g. rule x < 2500, range = x[2000, 3000].  should return {x[2000, 2499], x[2500, 3000]}.
        //   3) value is greater than max input range.  e.g. rule x < 3500, range=[2000, 3000].  should return {x[2000, 3000], invalid}.

        if(condition == LESS_THAN) {
            if(condition_part == 'x') {
                if(condition_val <= ranges.xRange.first) {
                    inclusive.invalidRange = true;
                } else if(condition_val > ranges.xRange.first && condition_val <= ranges.xRange.second) {
                    inclusive.xRange = make_pair(ranges.xRange.first, condition_val - 1);
                    exclusive.xRange = make_pair(condition_val, ranges.xRange.second);
                } else { //if condition_val > ranges.xRange.second 
                    exclusive.invalidRange = true;
                }
            }
            if(condition_part == 'm') {
                if(condition_val <= ranges.mRange.first) {
                    inclusive.invalidRange = true;
                } else if(condition_val > ranges.mRange.first && condition_val <= ranges.mRange.second) {
                    inclusive.mRange = make_pair(ranges.mRange.first, condition_val - 1);
                    exclusive.mRange = make_pair(condition_val, ranges.mRange.second);
                } else { //if condition_val > ranges.mRange.second 
                    exclusive.invalidRange = true;
                }
            }
            if(condition_part == 'a') {
                if(condition_val <= ranges.aRange.first) {
                    inclusive.invalidRange = true;
                } else if(condition_val > ranges.aRange.first && condition_val <= ranges.aRange.second) {
                    inclusive.aRange = make_pair(ranges.aRange.first, condition_val - 1);
                    exclusive.aRange = make_pair(condition_val, ranges.aRange.second);
                } else { //if condition_val > ranges.aRange.second 
                    exclusive.invalidRange = true;
                }
            }
            if(condition_part == 's') {
                if(condition_val <= ranges.sRange.first) {
                    inclusive.invalidRange = true;
                } else if(condition_val > ranges.sRange.first && condition_val <= ranges.sRange.second) {
                    inclusive.sRange = make_pair(ranges.sRange.first, condition_val - 1);
                    exclusive.sRange = make_pair(condition_val, ranges.sRange.second);
                } else { //if condition_val > ranges.sRange.second 
                    exclusive.invalidRange = true;
                }
            }
        }

        // three scenarios for GREATER THAN:
        //   1) value is greater than min input range.  e.g. rule x > 3500, range=x[2000, 3000].  should return {invalid, x[2000,3000]}.
        //   2) value is within range.  e.g. rule x > 2500, range = x[2000, 3000].  should return {x[2501, 3000], x[2000, 2500]}.
        //   3) value is less than max input range.  e.g. rule x > 1500, range=x[2000, 3000].  should return {x[2000, 3000], invalid}

        if(condition == GREATER_THAN) {
            if(condition_part == 'x') {
                if(condition_val >= ranges.xRange.second) {
                    inclusive.invalidRange = true;
                } else if(condition_val >= ranges.xRange.first && condition_val < ranges.xRange.second) {
                    inclusive.xRange = make_pair(condition_val + 1, ranges.xRange.second);
                    exclusive.xRange = make_pair(ranges.xRange.first, condition_val);
                } else { //if condition_val < ranges.xRange.second 
                    exclusive.invalidRange = true;
                }
            }
            if(condition_part == 'm') {
                if(condition_val >= ranges.mRange.second) {
                    inclusive.invalidRange = true;
                } else if(condition_val >= ranges.mRange.first && condition_val < ranges.mRange.second) {
                    inclusive.mRange = make_pair(condition_val + 1, ranges.mRange.second);
                    exclusive.mRange = make_pair(ranges.mRange.first, condition_val);
                } else { //if condition_val < ranges.mRange.second 
                    exclusive.invalidRange = true;
                }
            }
            if(condition_part == 'a') {
                if(condition_val >= ranges.aRange.second) {
                    inclusive.invalidRange = true;
                } else if(condition_val >= ranges.aRange.first && condition_val < ranges.aRange.second) {
                    inclusive.aRange = make_pair(condition_val + 1, ranges.aRange.second);
                    exclusive.aRange = make_pair(ranges.aRange.first, condition_val);
                } else { //if condition_val < ranges.aRange.second 
                    exclusive.invalidRange = true;
                }
            }
            if(condition_part == 's') {
                if(condition_val >= ranges.sRange.second) {
                    inclusive.invalidRange = true;
                } else if(condition_val >= ranges.sRange.first && condition_val < ranges.sRange.second) {
                    inclusive.sRange = make_pair(condition_val + 1, ranges.sRange.second);
                    exclusive.sRange = make_pair(ranges.sRange.first, condition_val);
                } else { //if condition_val < ranges.sRange.second 
                    exclusive.invalidRange = true;
                }
            }
        }

        return make_pair(inclusive, exclusive);
    }
};

class Workflow {
public:
    string name;
    vector<Rule> rules;

    Workflow() {}

    Workflow(string line) {
        parseWorkflowLine(line);
    }

    void parseWorkflowLine(string line) {
        int curlyStartIndex = line.find('{');
        name = line.substr(0, curlyStartIndex);
        string allRules = line.substr(curlyStartIndex + 1, line.length() - curlyStartIndex - 2);

        istringstream myStringStream(allRules);
        string ruleString;
        vector<string> allRuleStrings;

        while (getline(myStringStream, ruleString, ',')) {
            allRuleStrings.push_back(ruleString);
        }

        for(int i = 0; i < allRuleStrings.size(); i++) {
            Rule rule = Rule(allRuleStrings[i]);
            rules.push_back(rule);
        }
    }
};

string findNextWorkflow(Workflow workflow, Part part) {
    for(int i = 0; i < workflow.rules.size(); i++) {
        Rule rule = workflow.rules[i];
        if(rule.checkRule(part)) {
            return rule.destinationName;
        }
    }
    cout << "something went wrong, no rule found???" << endl;
    return "R";
}

bool partIsAccepted(unordered_map<string, Workflow> &workflowsMap, Part part) {
    string workflowName = "in";
    while(workflowName != "R" && workflowName != "A") {
        Workflow currentWorkflow = workflowsMap[workflowName];
        workflowName = findNextWorkflow(currentWorkflow, part);
    }
    return (workflowName == "A");
}

long long getSumAcceptedRatingNumbers(vector<string> lines) {
    unordered_map<string, Workflow> workflowsMap;
    int blankLineIndex = -1;
    for(int i = 0; i < lines.size(); i++) {
        if(lines[i].size() <= 2) { 
            // empty or mostly empty line, accounting for whitespace characters or newlines
            blankLineIndex = i;
            break;
        }
        Workflow workflow = Workflow(lines[i]);
        workflowsMap[workflow.name] = workflow;
    }

    long long sum = 0;
    for(int i = blankLineIndex + 1; i < lines.size(); i++) {
        Part part = Part(lines[i]);
        if(partIsAccepted(workflowsMap, part)) {
            sum += part.sum();
        }
    }

    return sum;
}

// walk through each rule finding the combinations for this range of numbers for each rule.  Recursively traverse through workflows until they
// resolve to A (in which case we find the combinations for that range) or R (in which case we ignore it), keeping track of the ranges that
// work for that particular rule (and those that don't and thus move on to the next rule; inclusive vs exclusive is what I call that).
long long findAcceptedInRangeRecursive(unordered_map<string, Workflow> &workflowsMap, string currentWorkflowName, PartRanges currentRanges) {
    Workflow currentWorkflow = workflowsMap[currentWorkflowName];
    long long totalAccepted = 0;
    for(Rule rule : currentWorkflow.rules) {
        pair<PartRanges, PartRanges> inclusiveExclusiveRanges = rule.checkRuleWithRanges(currentRanges);
        PartRanges inclusive = inclusiveExclusiveRanges.first;
        PartRanges exclusive = inclusiveExclusiveRanges.second;

        if(!inclusive.invalidRange && inclusive.isValid()) {
            if(rule.destinationName == "A") {
                totalAccepted += inclusive.calculateCombinations();
            } else if(rule.destinationName != "R") { //if R we can basically just ignore it.
                totalAccepted += findAcceptedInRangeRecursive(workflowsMap, rule.destinationName, inclusive);
            }
        }
        if(exclusive.invalidRange || !exclusive.isValid()) {
            // we can ignore all future rules in this workflow since the remaining ranges are known to be invalid
            break; 
        }
        currentRanges = exclusive;
    }

    return totalAccepted;
}

long long getAcceptedCombinationsStart(unordered_map<string, Workflow> &workflowsMap) {
    PartRanges startRanges = PartRanges();
    startRanges.xRange = make_pair(1, 4000);
    startRanges.mRange = make_pair(1, 4000);
    startRanges.aRange = make_pair(1, 4000);
    startRanges.sRange = make_pair(1, 4000);
    string startWorkflowName = "in";

    return findAcceptedInRangeRecursive(workflowsMap, startWorkflowName, startRanges);
}

//part 2 solution solver
long long getNumAcceptedRatingCombinations(vector<string> lines) {
    //parse the workflows into a map just like before.
    unordered_map<string, Workflow> workflowsMap;
    int blankLineIndex = -1;
    for(int i = 0; i < lines.size(); i++) {
        if(lines[i].size() <= 2) { 
            blankLineIndex = i;
            break;
        }
        Workflow workflow = Workflow(lines[i]);
        workflowsMap[workflow.name] = workflow;
    }

    return getAcceptedCombinationsStart(workflowsMap);
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
    long long result;
    if(PART_2_ENABLED) {
        result = getNumAcceptedRatingCombinations(lines);
    } else {
        result = getSumAcceptedRatingNumbers(lines);
    }

    //print final result to console
    cout << "result=" << result << endl;
    return 0;
}