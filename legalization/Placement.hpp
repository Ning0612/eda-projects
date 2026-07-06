#ifndef PLACEMENT_HPP
#define PLACEMENT_HPP

#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <regex>
#include <utility>
#include <vector>
#include <map>
#include <cctype>
#include <string>
#include "Component.hpp"
#include "CellRow.hpp"

class Placement{
    private:
    std::string inFileName;
    std::string outFileName;

    std::map<std::string, Component> components;
    std::map<double, std::pair<CellRow, std::vector<Component*>>> cellRows;
    

    int FixedCells = 0;

    double maximumDisplacement = 0;
    double totalDisplacement = 0;
    int componentsCount = 0;

    // ========== File I/O ==========
    void readNodesFile();
    void readPlFile();
    void readSclFile();

    void writeAuxFile();
    void writePlFile();
    
    void copyNodesFile();
    void copySclFile();
    void copyNetsFile();
    void copyWtsFile();
    // ==============================


    void alignComponents(int times);
    void placeInRows();
    public:
    Placement(char *inFileName, char *outFileName);
    void legalize(int times);
    
    void calculateBenchmark();

    void printComponents();
    void printBenchmark();
    
    void plotComponents();
    void plotCellRows();

    std::pair<double, double> getBenchmark() const;

    void outputAllFiles();
    bool detectLegalization();
};

#endif