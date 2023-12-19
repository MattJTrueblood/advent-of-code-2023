
/******************************
 * Day 13: Point of Incidence *
 ******************************/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "../utils/util.h"

using namespace std;

static bool PART_2_ENABLED = true;

string getColumn(vector<string> rows, int columnIndex) {
    string column = "";
    for(int i = 0; i < rows.size(); i++) {
        column += rows[i][columnIndex];
    }
    return column;
}

int findVerticalReflection(vector<string> mirrorRows, int differentSummary) {
    int height = mirrorRows.size();
    for(int i = 0; i < height - 1; i++) {
        int upReflectionIndex = i;
        int downReflectionIndex = i + 1;
        bool isReflection = true;
        while(upReflectionIndex >= 0 && downReflectionIndex <= height - 1) {
            if(mirrorRows[upReflectionIndex] != mirrorRows[downReflectionIndex]) {
                isReflection = false;
                break;
            }
            upReflectionIndex--;
            downReflectionIndex++;
        }
        if(isReflection && (i + 1) * 100 != differentSummary) {
            return ((i + 1) * 100);
        }
    }
    return -1;
}

int findHorizontalReflection(vector<string> mirrorRows, int differentSummary) {
    int width = mirrorRows[0].size();
    for(int i = 0; i < width - 1; i++) {
        int leftReflectionIndex = i;
        int rightReflectionIndex = i + 1;
        bool isReflection = true;
        while(leftReflectionIndex >= 0 && rightReflectionIndex <= width - 1) {
            if(getColumn(mirrorRows, leftReflectionIndex) != getColumn(mirrorRows, rightReflectionIndex)) {
                isReflection = false;
                break;
            }
            leftReflectionIndex--;
            rightReflectionIndex++;
        }
        if(isReflection && (i+1) != differentSummary) {
            return ((i + 1));
        }
    }
    return -1;
}

int findSummary(vector<string> mirrorRows, int differentSummary) {
    int verticalSummary = findVerticalReflection(mirrorRows, differentSummary);
    if(verticalSummary != -1) {
        return verticalSummary;
    }

    int horizontalSummary = findHorizontalReflection(mirrorRows, differentSummary);
    if(horizontalSummary != -1) {
        return horizontalSummary;
    }

    // no reflection
    return -1;
}

int findSummaryWithSmudge(vector<string> mirrorRows, int summary) {
    //find the first summary that is different from summary.
    for(int i = 0; i < mirrorRows.size(); i++) {
        for(int j = 0; j < mirrorRows[0].size(); j++) {
            vector<string> mirrorRowsCopy = mirrorRows;
            mirrorRowsCopy[i][j] = (mirrorRowsCopy[i][j] == '#') ? '.' : '#';
            int smudgeIJSummary = findSummary(mirrorRowsCopy, summary);
            if(smudgeIJSummary != -1 && smudgeIJSummary != summary) {
                cout << smudgeIJSummary << "!=" << summary << endl;
                return smudgeIJSummary;
            }
        }
    }
    cout << "bad!" << endl;
    return -1;
}

int findReflectionSummaryNumber(vector<string> lines) {
    int sum = 0;
    vector<string> mirrorRows;
    for(int i = 0; i < lines.size(); i++) {
        if(lines[i].size() == 0) {
            int summary = findSummary(mirrorRows, -1);
            cout << "summary = " << summary << endl;
            if(PART_2_ENABLED) {
                summary = findSummaryWithSmudge(mirrorRows, summary);
            }
            mirrorRows.clear();
            sum += summary;
        } else {
            mirrorRows.push_back(lines[i]);
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
    int result = findReflectionSummaryNumber(lines);

    //print final result to console
    cout << "result=" << result << endl;
    return 0;
}