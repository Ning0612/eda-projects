#include <iostream>
#include <limits>
#include <chrono>
#include "Placement.hpp"

int main(int argc, char **argv){
    auto start = std::chrono::steady_clock::now();
    if(argc != 3 && argc != 4){
        std::cerr << "Usage: " << argv[0] << " <input_file> <output_file>" << std::endl;
        return 1;
    }

    char *inFileName = argv[1];
    char *outFileName = argv[2];
    
    Placement placement(inFileName, outFileName);

    double MinMaxDisplacement = std::numeric_limits<double>::max();
    double MinTotalDisplacement = std::numeric_limits<double>::max();

    int bestProcessTimes = 0;
    double bestScore = std::numeric_limits<double>::max();
    for(int i = 0 ; i < 30 ; ++i){
        Placement tempPlacement(placement);
        tempPlacement.legalize(i);
        tempPlacement.calculateBenchmark();
        std::pair<double, double> benchmark = tempPlacement.getBenchmark();
        double score = benchmark.first + benchmark.second;

        if(score < bestScore){
            bestScore = score;
            bestProcessTimes = i;
        }

        if(i - bestProcessTimes > 10){
            break;
        }

        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;
        if(elapsed.count() * 0.001 > 3){
            break;
        }
    }

    placement.legalize(bestProcessTimes);
    placement.calculateBenchmark();
    placement.printBenchmark();
    placement.outputAllFiles();
    
    if(argc == 4 && std::string(argv[3]) == "plot"){
        placement.plotComponents();
        placement.plotCellRows();
        placement.detectLegalization();
        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;
        std::cout << "Run Time : " << elapsed.count() *0.001 << " seconds" << std::endl;
        std::cout << "Best process times: " << bestProcessTimes << std::endl;
    }
}