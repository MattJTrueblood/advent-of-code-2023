
/************************************
 * Day 24:  Never Tell Me The Odds  *
 ************************************/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <utility>
#include <cmath>
#include "../utils/util.h"

using namespace std;

static const bool PART_2_ENABLED = false;

const double EPSILON = 1e-9;

class Hailstone {
public:
    long long x;
    long long y;
    long long z;
    long long dx;
    long long dy;
    long long dz;

    Hailstone() {}

    Hailstone(long long x, long long y, long long z, long long dx, long long dy, long long dz) {
        this->x = x;
        this->y = y;
        this->z = z;
        this->dx = dx;
        this->dy = dy;
        this->dz = dz;
    }
};

vector<Hailstone> parseLinesAsHailstones(vector<string> lines) {
    vector<vector<string>> wordLines = parseAllLines(lines);
    vector<Hailstone> hailstones;
    for(vector<string> words: wordLines) {
        long long x = stoll(words[0].substr(0, words[0].size() - 1));
        long long y = stoll(words[1].substr(0, words[1].size() - 1));
        long long z = stoll(words[2].substr(0, words[2].size() - 1));
        long long dx = stoll(words[4].substr(0, words[4].size() - 1));
        long long dy = stoll(words[5].substr(0, words[5].size() - 1));
        long long dz = stoll(words[6]);
        hailstones.push_back(Hailstone(x, y, z, dx, dy, dz));
    }
    return hailstones;
}

bool areParallel(Hailstone v1, Hailstone v2) {
    return fabs((double)v1.dx * v2.dy - (double)v1.dy * v2.dx) < EPSILON;
}

bool isForward(Hailstone v, pair<double, double> intersection) {
    double dx = intersection.first - v.x;
    double dy = intersection.second - v.y;

    return (dx * v.dx > -EPSILON) && (dy * v.dy > -EPSILON) && (dx*dx + dy*dy >= (double)v.dx*v.dx + (double)v.dy*v.dy);
}

pair<double, double> findIntersection(Hailstone v1, Hailstone v2) {
    double det = (double)v1.dx * v2.dy - (double)v1.dy * v2.dx;

    if (fabs(det) < EPSILON) {
        return {-1, -1}; // Indicate no intersection
    }

    double x = ((double)v1.dy * v2.dx * v1.x - (double)v1.dx * v2.dy * v2.x + (double)v1.dx * v2.dx * (v2.y - v1.y)) / -det;
    double y = ((double)v1.dy * v2.dy * (v2.x - v1.x) + v1.y * (double)v1.dx * v2.dy - v2.y * (double)v2.dx * v1.dy) / det;

    return {x, y};
}

long long findNumIntersectionsWithinBounds(vector<Hailstone> hailstones, long long min, long long max) {
    long long count = 0;
    for(size_t i = 0; i < hailstones.size(); i++) {
        for(size_t j = i + 1; j < hailstones.size(); j++) {
            if(!areParallel(hailstones[i], hailstones[j])) {
                pair<double, double> intersection = findIntersection(hailstones[i], hailstones[j]);
                if(intersection.first >= min && intersection.first <= max &&
                    intersection.second >= min && intersection.second <= max &&
                    isForward(hailstones[i], intersection) && isForward(hailstones[j], intersection)) {
                    count++;
                }
            }
        }
    }
    return count;
}

long long part2(vector<string> lines) {
    // TODO
    return -1;
}

long long part1(vector<string> lines) {
    vector<Hailstone> hailstones = parseLinesAsHailstones(lines);
    return findNumIntersectionsWithinBounds(hailstones, 200000000000000, 400000000000000);
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
        result = part2(lines);
    } else {
        result = part1(lines);
    }

    //print final result to console
    cout << "result=" << result << endl;
    return 0;
}