#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <queue>
#include <algorithm>
#include <stdexcept>

#include "Scheduler.hpp"

// 主函式
int main(int argc, char *argv[]) {
    if(argc != 6) {
        std::cout << "Usage: " << argv[0] << " -h/-e <blif_file> <max_and> <max_or> <max_not>" << std::endl;
        return 1;
    }

    std::string mode = argv[1];
    if(mode != "-h" && mode != "-e") {
        std::cout << argv[1] << std::endl;
        std::cout << "Usage: " << argv[0] << " -h/-e <blif_file> <max_and> <max_or> <max_not>" << std::endl;
        return 1;
    }

    Scheduler scheduler(argv[2], std::stoi(argv[3]), std::stoi(argv[4]), std::stoi(argv[5]));
    if(mode == "-h") {
        scheduler.listScheduling();
        scheduler.printScheduleResult();
    } else if(mode == "-e") {
        scheduler.listScheduling();
        try {
            scheduler.ILPScheduling();
        } catch (const std::runtime_error& e) {
            std::cerr << e.what() << std::endl;
            return 1;
        }
        scheduler.printILPResult();

        if(!scheduler.checkILPResult()) {
            std::cout << "ILP Result Incorrect" << std::endl;
        }

        if(!scheduler.checkListResult()) {
            std::cout << "List Result Incorrect" << std::endl;
        }
    }
}
