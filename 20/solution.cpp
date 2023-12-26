
/**********************
 * Day N:  ~~~~~~~~~  *
 **********************/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include "../utils/util.h"

using namespace std;

static const bool PART_2_ENABLED = true;

enum emitter_type { FLIP_FLOP = 0, CONJUNCTION = 1, BROADCAST = 2 };
enum pulse_type { LOW_PULSE = 0, HIGH_PULSE = 1 };

class Module {
public:
    emitter_type type;
    string name;
    vector<Module*> destinations;
    unordered_map<Module*, pulse_type> mostRecentReceived;
    bool flipflopOn = false;

     // for part 2 cycle detection.  Only relevant for subterminal nodes, ignore otherwise.
    long long lastCycleNumPresses = 0;
    long long lastCycleLength = 0;

    Module() {}

    Module(emitter_type type, string name) {
        this->type = type;
        this->name = name;
    }

    void flipTheFlipFlop() {
        flipflopOn = !flipflopOn;
    }

    pulse_type getConjunctionPulse() {
        for(const pair<Module*, pulse_type>& pair : mostRecentReceived) {
            if(pair.second != HIGH_PULSE) {
                return HIGH_PULSE;
            }
        }
        return LOW_PULSE;
    }

    // special function for part 2.  We only call this for the inputs to the penultimate node &mg -> rx.  This should help to find
    // a pattern in the low pulse received cycles of those modules.
    void updateCycle(long long numButtonPresses) {
        lastCycleLength = numButtonPresses - lastCycleNumPresses;
        lastCycleNumPresses = numButtonPresses;
        cout << "cycle length for module " << name << " : " << lastCycleLength << " with offset "<< numButtonPresses << endl;
    }
};

class ModuleQueueItem {
public:
    Module* fromModule;
    Module* currentModule;
    pulse_type pulse;

    ModuleQueueItem() {}

    ModuleQueueItem(Module* fromModule, Module* currentModule, pulse_type pulse) {
        this->fromModule = fromModule;
        this->currentModule = currentModule;
        this->pulse = pulse;
    }
};

emitter_type parseType(string firstWord) {
    char typeChar = firstWord[0];
    if(typeChar == '%') {
        return FLIP_FLOP;
    } else if(typeChar == '&') {
        return CONJUNCTION;
    }
    return BROADCAST;
}

string parseName(string firstWord) {
    return firstWord.substr(1);
}

unordered_map<string, Module*> parseModules(vector<vector<string>> wordLines) {
    //create all the modules
    unordered_map<string, Module*> moduleMap;
    for(vector<string> wordLine : wordLines) {
        emitter_type type = parseType(wordLine[0]);        
        string name = parseName(wordLine[0]);
        moduleMap[name] = new Module(type, name);
    }

    //set up the connections with the references in this map
    for(vector<string> wordLine : wordLines) {
        string name = parseName(wordLine[0]);
        Module* module = moduleMap[name];
        for(int i = 2; i < wordLine.size(); i++) {
            string destName = wordLine[i];
            if(destName.back() == ',') {
                destName.pop_back();
            }
            Module* destModule = moduleMap[destName];
            module->destinations.push_back(destModule);
            if(destModule != nullptr && destModule->type == CONJUNCTION) {
                destModule->mostRecentReceived[module] = LOW_PULSE;
            }
        }
    }

    return moduleMap;
}

void incrementPulseCounts(long long &numLowPulsesSent, long long &numHighPulsesSent, pulse_type pulse) {
    if(pulse == HIGH_PULSE) {
        numHighPulsesSent++;
    } else {
        numLowPulsesSent++;
    }
}

void pressButtonOnce(long long &numLowPulsesSent, long long &numHighPulsesSent, Module* broadcaster) {
    queue<ModuleQueueItem> moduleQueue;

    //manually add the button press signal
    ModuleQueueItem firstQueueItem = ModuleQueueItem(nullptr, broadcaster, LOW_PULSE);
    incrementPulseCounts(numLowPulsesSent, numHighPulsesSent, LOW_PULSE);
    moduleQueue.push(firstQueueItem);

    while(!moduleQueue.empty()) {
        ModuleQueueItem current = moduleQueue.front();
        moduleQueue.pop();
        if(current.currentModule != nullptr) {
            if(current.currentModule->type == BROADCAST) {
                for(Module* dest : current.currentModule->destinations) {
                    ModuleQueueItem destQueueItem = ModuleQueueItem(current.currentModule, dest, current.pulse);
                    incrementPulseCounts(numLowPulsesSent, numHighPulsesSent, destQueueItem.pulse);
                    moduleQueue.push(destQueueItem);
                }
            } else if(current.currentModule->type == FLIP_FLOP) {
                if(current.pulse == LOW_PULSE) {
                    pulse_type nextPulse = current.currentModule->flipflopOn ? LOW_PULSE : HIGH_PULSE;
                    for(Module* dest : current.currentModule->destinations) {  
                        ModuleQueueItem destQueueItem = ModuleQueueItem(current.currentModule, dest, nextPulse);
                        incrementPulseCounts(numLowPulsesSent, numHighPulsesSent, destQueueItem.pulse);
                        moduleQueue.push(destQueueItem);
                    }
                    current.currentModule->flipTheFlipFlop();
                }
            } else if(current.currentModule->type == CONJUNCTION) {
                current.currentModule->mostRecentReceived[current.fromModule] = current.pulse;
                pulse_type nextPulse = current.currentModule->getConjunctionPulse();
                for(Module* dest : current.currentModule->destinations) {
                    ModuleQueueItem destQueueItem = ModuleQueueItem(current.currentModule, dest, nextPulse);
                    incrementPulseCounts(numLowPulsesSent, numHighPulsesSent, destQueueItem.pulse);
                    moduleQueue.push(destQueueItem);
                }
            }
        }
    }
}

long long getProductOfPulsesAfterButtonPresses(unordered_map<string, Module*> moduleMap, int numPresses) {
    long long numLowPulsesSent = 0; //includes initial button pulse
    long long numHighPulsesSent = 0;
    for(int i = 0; i < numPresses; i++) {
       pressButtonOnce(numLowPulsesSent, numHighPulsesSent, moduleMap["broadcaster"]);
    }

    return numLowPulsesSent * numHighPulsesSent;
}

bool gotRxLowPulseFromButtonPress(unordered_map<string, Module*> moduleMap, long long numButtonPresses) {
    queue<ModuleQueueItem> moduleQueue;

    //manually add the button press signal
    ModuleQueueItem firstQueueItem = ModuleQueueItem(nullptr, moduleMap["broadcaster"], LOW_PULSE);
    moduleQueue.push(firstQueueItem);

    while(!moduleQueue.empty()) {
        ModuleQueueItem current = moduleQueue.front();
        moduleQueue.pop();
        if(current.currentModule != nullptr) {
            // If it's a reasonably sized loop before it reaches this, we can just return true.
            if(current.currentModule->name == "rx" && current.pulse == LOW_PULSE) {
                return true;
            }
            // Otherwise we need to do cycle detection.  jg, jm, rh, and hf all must receive LOW at the same time for rx to get LOW.
            // Here we can just update the module's internally tracked cycle length whenever we receive a low pulse.
            if((current.currentModule->name == "jg" || current.currentModule->name == "rh" 
                || current.currentModule->name == "jm" || current.currentModule->name == "hf") && current.pulse == LOW_PULSE) {
                current.currentModule->updateCycle(numButtonPresses);
            }
            if(current.currentModule->type == BROADCAST) {
                for(Module* dest : current.currentModule->destinations) {
                    ModuleQueueItem destQueueItem = ModuleQueueItem(current.currentModule, dest, current.pulse);
                    moduleQueue.push(destQueueItem);
                }
            } else if(current.currentModule->type == FLIP_FLOP) {
                if(current.pulse == LOW_PULSE) {
                    pulse_type nextPulse = current.currentModule->flipflopOn ? LOW_PULSE : HIGH_PULSE;
                    for(Module* dest : current.currentModule->destinations) {  
                        ModuleQueueItem destQueueItem = ModuleQueueItem(current.currentModule, dest, nextPulse);
                        moduleQueue.push(destQueueItem);
                    }
                    current.currentModule->flipTheFlipFlop();
                }
            } else if(current.currentModule->type == CONJUNCTION) {
                current.currentModule->mostRecentReceived[current.fromModule] = current.pulse;
                pulse_type nextPulse = current.currentModule->getConjunctionPulse();
                for(Module* dest : current.currentModule->destinations) {
                    ModuleQueueItem destQueueItem = ModuleQueueItem(current.currentModule, dest, nextPulse);
                    moduleQueue.push(destQueueItem);
                }
            }
        }
    }

    return false;
}

long long getNumButtonPressesUntilRxLowPulse(unordered_map<string, Module*> moduleMap) {
    long long numButtonPresses = 0;
    while(true) {
        numButtonPresses++;
        if(gotRxLowPulseFromButtonPress(moduleMap, numButtonPresses)) {
            break;
        }
        if(numButtonPresses % 10000 == 0) {
            cout << numButtonPresses << " presses so far." << endl;
            // we know that jg, rh, jm, and hf -> mg -> rx.  For mg to fire low, all four of them need to be high.
            // we track the cycle length of these four modules (the number of presses between the last LOW pulse they receive).
            // to find the convergence we just use lcm from our utils library (adapted for vectors of more than one number)
            if( moduleMap["jg"]->lastCycleLength != 0 &&
                moduleMap["rh"]->lastCycleLength != 0 &&
                moduleMap["jm"]->lastCycleLength != 0 &&
                moduleMap["hf"]->lastCycleLength != 0 ) {

                vector<long long> cycleLengths;
                cycleLengths.push_back(moduleMap["jg"]->lastCycleLength);
                cycleLengths.push_back(moduleMap["rh"]->lastCycleLength);
                cycleLengths.push_back(moduleMap["jm"]->lastCycleLength);
                cycleLengths.push_back(moduleMap["hf"]->lastCycleLength);

                numButtonPresses = lcm(cycleLengths);
                cout << " found cycles.  LCM = " << numButtonPresses << endl;
                break;
            }
        }
    }
    return numButtonPresses;
}

long long part2(vector<string> lines) {
    vector<vector<string>> wordLines = parseAllLines(lines);
    unordered_map<string, Module*> moduleMap = parseModules(wordLines);
    return getNumButtonPressesUntilRxLowPulse(moduleMap);
}

long long part1(vector<string> lines) {
    vector<vector<string>> wordLines = parseAllLines(lines);
    unordered_map<string, Module*> moduleMap = parseModules(wordLines);
    return getProductOfPulsesAfterButtonPresses(moduleMap, 1000);
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