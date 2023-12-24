
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

int getSumAcceptedRatingNumbers(vector<string> lines) {
    unordered_map<string, Workflow> workflowsMap;
    int blankLineIndex = -1;
    for(int i = 0; i < lines.size(); i++) {
        if(lines[i].size() <= 2) { 
            // empty or mostly empty line, accounting for whitespace characters or newlines
            cout << "done with workflows" << endl;
            blankLineIndex = i;
            break;
        }
        cout << "parsing " << lines[i];
        Workflow workflow = Workflow(lines[i]);
        cout << " as name " << workflow.name << endl;
        workflowsMap[workflow.name] = workflow;
    }

    int sum = 0;
    for(int i = blankLineIndex + 1; i < lines.size(); i++) {
        Part part = Part(lines[i]);
        cout << "part:" << part.x << "," << part.m << "," << part.a << "," << part.s << endl;
        if(partIsAccepted(workflowsMap, part)) {
            sum += part.sum();
        }
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

    // now handle lines to generate the result
    int result = getSumAcceptedRatingNumbers(lines);

    //print final result to console
    cout << "result=" << result << endl;
    return 0;
}