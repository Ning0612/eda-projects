#ifndef SCHEDULER_HPP
#define SCHEDULER_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <queue>

#include "Gate.hpp"


class Scheduler
{
    private:
    std::string blifFile;
    std::map<std::string, int> resourseConstraint;

    std::vector<std::string> inputNodes;
    std::vector<std::string> outputNodes;

    std::map<std::string, Gate> gates;
    std::map<std::string, std::pair<std::vector<Gate*>, std::vector<Gate*>>> adjList;

    std::vector<std::map<std::string, std::vector<Gate*>>> scheduleResult;
    std::map<int, std::map<std::string, std::vector<Gate*>>> ILPResult;

    std::map<std::string, int> asapSchedule;
    std::map<std::string, int> alapSchedule;

    const std::vector<std::string> gateTypes = {"AND", "OR", "NOT"};

    int listSschedulingLatency;
    int ILPLatency;

    void parseBlif();
    void buildGraph();
    void calculatePirority();
    void ASAP();
    void ALAP();

    public:
    Scheduler(std::string blifFile, int maxAnd, int maxOr, int maxNot);
    
    void printAllComponents();
    void printScheduleResult();
    void printILPResult();

    void listScheduling();
    void ILPScheduling();
    void outputResult();

    bool checkILPResult();
    bool checkListResult();
};

#endif