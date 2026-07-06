#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include "flute.h"
#include "ClockTree.hpp"


int main(int argc, char** argv)
{
    auto start = std::chrono::steady_clock::now();
    bool debugMode;

    if(argc == 4 && std::string(argv[3]) == "d" ){
        debugMode = true;
    }else if (argc != 3){
        std::cout << "Usage : " << argv[0] << " <input file> <output file>\n";
        return 0;
    }

    InputCTS cts = clockTreeFromFile(argv[1]);

    ClockTree fluteTree(cts);

    fluteTree.tryBest();
    fluteTree.outputCTSFile(argv[2]);

    if (debugMode){
        fluteTree.outputGraph("graph.plt");
        fluteTree.printInfo();
        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;
        std::cout << "Run Time : " << elapsed.count() *0.001 << " seconds" << std::endl;
    }
}