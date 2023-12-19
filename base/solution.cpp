
/**********************
 * Day N:  ~~~~~~~~~  *
 **********************/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "../utils/util.h"

using namespace std;

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

    //print final result to console
    cout << "result=" << result << endl;
    return 0;
}