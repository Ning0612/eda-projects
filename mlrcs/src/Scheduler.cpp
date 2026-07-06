#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <queue>

#include "gurobi_c++.h"
#include "Scheduler.hpp"

Scheduler::Scheduler(std::string blifFile, int maxAnd, int maxOr, int maxNot)
{
    this->blifFile = blifFile;
    this->resourseConstraint["AND"] = maxAnd;
    this->resourseConstraint["OR"] = maxOr;
    this->resourseConstraint["NOT"] = maxNot;
    this->parseBlif();
    this->buildGraph();
    this->calculatePirority();
}

void Scheduler::parseBlif()
{
    std::ifstream file(this->blifFile);
    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open BLIF file: " + this->blifFile);
    }

    this->gates.clear();

    std::string line;
    while (std::getline(file, line))
    {
        std::string subLine = line;
        if(subLine.find("\\") != std::string::npos){
            line = "";
            while (subLine.find("\\") != std::string::npos)
            {
                line+= subLine.substr(0, subLine.find("\\"));
                std::getline(file, subLine);
            }
            line += subLine;
        }

        if (line.find(".model") != std::string::npos)
        {
            continue;
        }

        if(line.find(".inputs") != std::string::npos){
            size_t pos = 0;
            std::string token;
            std::vector<std::string> tokens;

            while ((pos = line.find(' ')) != std::string::npos)
            {
                token = line.substr(0, pos);
                token.erase(std::remove_if(token.begin(), token.end(), ::isspace), token.end());
                tokens.push_back(token);
                line.erase(0, pos + 1);
            }

            token = line;
            token.erase(std::remove_if(token.begin(), token.end(), ::isspace), token.end());
            tokens.push_back(token);

            this->inputNodes.assign(tokens.begin() + 1, tokens.end());
        }

        if(line.find(".outputs") != std::string::npos){
            size_t pos = 0;
            std::string token;
            std::vector<std::string> tokens;

            while ((pos = line.find(' ')) != std::string::npos)
            {
                token = line.substr(0, pos);
                token.erase(std::remove_if(token.begin(), token.end(), ::isspace), token.end());
                tokens.push_back(token);
                line.erase(0, pos + 1);
            }

            token = line;
            token.erase(std::remove_if(token.begin(), token.end(), ::isspace), token.end());
            tokens.push_back(token);

            this->outputNodes.assign(tokens.begin() + 1, tokens.end());
        }

        if (line.find(".names") == 0)
        {
            std::vector<std::string> tokens;
            std::string token;
            size_t pos = 0;

            while ((pos = line.find(' ')) != std::string::npos)
            {
                token = line.substr(0, pos);
                token.erase(std::remove_if(token.begin(), token.end(), ::isspace), token.end());
                tokens.push_back(token);
                line.erase(0, pos + 1);
            }

            token = line;
            token.erase(std::remove_if(token.begin(), token.end(), ::isspace), token.end());
            tokens.push_back(token);

            std::string outputName = tokens.back();
            std::vector<std::string> inputNames(tokens.begin() + 1, tokens.end() - 1);

            std::getline(file, line);
            std::string gateType;

            if(inputNames.size() == 1)
            {
                gateType = "NOT";
            }
            else
            {
                gateType = (line.find("-") != std::string::npos) ? "OR" : "AND";
            }

            this->gates.emplace(outputName, Gate(outputName, inputNames, gateType));
        }
    }

    file.close();
}

void Scheduler::buildGraph() {
    for(auto &node : this->inputNodes){
        this->gates.emplace(node, Gate(node, {}, "INPUT"));
    }
    this->gates.emplace("__END__", Gate("__END__", outputNodes, "__END__"));

    for(auto &gate : this->gates){
        for(auto &input : gate.second.getInputNames()){
            this->adjList[input].second.push_back(&gate.second);
            this->adjList[gate.second.getOutputName()].first.push_back(&this->gates[input]);
        }
    }

    for(const auto &output : this->outputNodes){
        this->adjList["__END__"].first.push_back(&this->gates[output]);
        this->adjList[output].second.push_back(&this->gates["__END__"]);
    }
}

void Scheduler::calculatePirority(){
    std::queue<Gate*> q;

    for(auto &node : this->outputNodes){
        this->gates[node].setPirority(0);
        q.push(&this->gates[node]);
    }

    while(!q.empty()){
        Gate* gate = q.front();
        q.pop();

        for(auto &input : gate->getInputNames()){
            Gate* inputGate = &this->gates[input];
            if(inputGate->getPirority() <= gate->getPirority()){
                inputGate->setPirority(gate->getPirority() + 1);
                q.push(inputGate);
            }
        }
    }
}

void Scheduler::listScheduling(){
    std::map<std::string, std::map<Gate*, std::set<Gate*>>> adjListCopy;
    for (auto &type : this->gateTypes)
    {
        adjListCopy[type].clear();
    }

    this->scheduleResult.clear();

    for(auto &node : this->adjList){
        if(node.second.first.size() != 0){
            std::set<Gate*> temp;

            for(auto &gate : node.second.first){
                if (gate->getGateType() != "INPUT")
                {
                    temp.emplace(gate);
                }
            }
            adjListCopy[this->gates[node.first].getGateType()][&this->gates[node.first]] = temp;
        }
    }

    while(adjListCopy["AND"].size() != 0 || adjListCopy["OR"].size() != 0 || adjListCopy["NOT"].size() != 0){
        std::map<std::string, std::vector<Gate*>> temp;

        for(auto &type : this->gateTypes){
            temp[type].clear();
            std::vector<Gate*> order;

            for(auto &gate : adjListCopy[type]){
                order.push_back(gate.first);
            }
            std::sort(order.begin(), order.end(), [](Gate* a, Gate* b) {
                return a->getPirority() > b->getPirority(); 
            });

            for(auto &gate : order){

                if(adjListCopy[type][gate].size() == 0 && temp[type].size() < this->resourseConstraint[type]){
                    temp[type].push_back(gate);
                }

                if(temp[type].size() >= this->resourseConstraint[type]){
                    break;
                }
            }
        }

        for(auto &gate : temp){
            for(auto &gateType : gate.second){
                if(adjListCopy[gateType->getGateType()].find(gateType) != adjListCopy[gateType->getGateType()].end()){
                    adjListCopy[gateType->getGateType()].erase(gateType);
                }

                for(auto &node : adjListCopy){
                    for(auto &gate : node.second){
                        if(gate.second.find(gateType) != gate.second.end()){
                            gate.second.erase(gateType);
                        }
                    }
                }
            }
        }

        this->scheduleResult.push_back(temp);
    }

    this->listSschedulingLatency = this->scheduleResult.size() + 1;
}

void Scheduler::ASAP() {
    std::map<std::string, int> startTimes;
    std::map<std::string, int> inDegree;
    std::queue<std::string> readyQueue;

    std::map<std::string, std::pair<std::vector<Gate *>, std::vector<Gate *>>> tempAdjList;

    for(auto &node : this->adjList){
        if (this->gates[node.first].getGateType() != "INPUT")
        {
            for(auto &gate : node.second.first){
                if(gate->getGateType() != "INPUT"){
                    tempAdjList[node.first].first.push_back(gate);
                }
            }

            tempAdjList[node.first].second = node.second.second;
        }
    }

    for (const auto& node : tempAdjList) {
        const std::string& nodeName = node.first;
        inDegree[nodeName] = tempAdjList[nodeName].first.size(); 
        startTimes[nodeName] = 1; 
        if (inDegree[nodeName] == 0) {
            readyQueue.push(nodeName); 
        }
    }

    while (!readyQueue.empty()) {
        std::string current = readyQueue.front();
        readyQueue.pop();

        for (const auto successor : tempAdjList[current].second) {
            inDegree[successor->getOutputName()]--;
            startTimes[successor->getOutputName()] = std::max(startTimes[successor->getOutputName()], startTimes[current] + 1); // 更新開始時間
            if (inDegree[successor->getOutputName()] == 0) {
                readyQueue.push(successor->getOutputName()); 
            }
        }
    }

    this->asapSchedule = startTimes;
}

void Scheduler::ALAP(){
    std::map<std::string, int> endTimes; 
    std::map<std::string, int> outDegree; 
    std::queue<std::string> readyQueue; 

    std::map<std::string, std::pair<std::vector<Gate *>, std::vector<Gate *>>> tempAdjList;

    for(auto &node : this->adjList){
        if (this->gates[node.first].getGateType() != "INPUT")
        {
            for(auto &gate : node.second.first){
                if(gate->getGateType() != "INPUT"){
                    tempAdjList[node.first].first.push_back(gate);
                }
            }

            tempAdjList[node.first].second = node.second.second;
        }
    }

    for (const auto& node : tempAdjList) {
        const std::string& nodeName = node.first;
        outDegree[nodeName] = tempAdjList[nodeName].second.size(); 
        endTimes[nodeName] = this->listSschedulingLatency; 
        if (outDegree[nodeName] == 0) {
            readyQueue.push(nodeName);
        }
    }

    while (!readyQueue.empty()) {
        std::string current = readyQueue.front();
        readyQueue.pop();

        for (const auto predecessor : tempAdjList[current].first) {
            outDegree[predecessor->getOutputName()]--; 
            endTimes[predecessor->getOutputName()] = std::min(endTimes[predecessor->getOutputName()], endTimes[current] - 1); // 更新最晚開始時間
            if (outDegree[predecessor->getOutputName()] == 0) {
                readyQueue.push(predecessor->getOutputName()); 
            }
        }
    }

    this->alapSchedule = endTimes;
}

void Scheduler::ILPScheduling() {
    this->ASAP();
    this->ALAP();

    std::map<std::string, std::pair<std::vector<Gate *>, std::vector<Gate *>>> tempAdjList;

    for(auto &node : this->adjList){
        if (this->gates[node.first].getGateType() != "INPUT")
        {
            for(auto &gate : node.second.first){
                if(gate->getGateType() != "INPUT"){
                    tempAdjList[node.first].first.push_back(gate);
                }
            }

            tempAdjList[node.first].second = node.second.second;
        }
    }

    try {
        GRBEnv env = GRBEnv(true);
        env.set("LogFile", "gurobi_ilp.log");
        env.start();

        GRBModel model = GRBModel(env);
        // model.set(GRB_IntParam_OutputFlag, 0);

        std::map<std::string, std::map<int, GRBVar>> x;
        for (const auto& gate : this->gates) {
            if (gate.second.getGateType() == "INPUT") {
                continue;
            }

            const std::string& gateName = gate.first;
            const int asap = this->asapSchedule[gateName];
            const int alap = this->alapSchedule[gateName];

            for (int l = asap; l <= alap; ++l) {
                x[gateName].emplace(l, model.addVar(0.0, 1.0, 0.0, GRB_BINARY, "x_" + gateName + "_" + std::to_string(l)));
            }
        }

        GRBLinExpr objective = 0;
        for (int l = this->asapSchedule["__END__"]; l <= this->alapSchedule["__END__"]; ++l){
            objective += l * x["__END__"][l];
        }
        model.setObjective(objective, GRB_MINIMIZE);

        for (const auto& gate : this->gates) {
            if (gate.second.getGateType() == "INPUT") {
                continue;
            }

            const std::string& gateName = gate.first;
            GRBLinExpr sum = 0;
            for (auto& var : x[gateName]) {
                sum += var.second;
            }
            model.addConstr(sum == 1, "unique_start_" + gateName);
        }

        for (const auto& gate : this->gates) {
            if (gate.second.getGateType() == "INPUT") {
                continue;
            }

            const std::string& gateName = gate.first;

            for (const auto& inputName : gate.second.getInputNames()) {
                if (x.find(inputName) != x.end() && x.find(gateName) != x.end()) {
                    GRBLinExpr startTime = 0;
                    
                    for (int l = this->asapSchedule[inputName]; l <= this->alapSchedule[inputName]; ++l){
                        startTime -= l * x[inputName][l];
                    }

                    for (int l = this->asapSchedule[gateName]; l <= this->alapSchedule[gateName]; ++l){
                        startTime += l * x[gateName][l];
                    }

                    model.addConstr(startTime >= 1, "dependency_" + inputName + "_to_" + gateName);
                }
            }
        }

        for (int l = 0; l < this->listSschedulingLatency; ++l) {
            for (const auto& type : this->resourseConstraint) {
                GRBLinExpr resourceUsage = 0;
                bool flag = false;

                for (const auto& gate : this->gates) {
                    if (gate.second.getGateType() == "INPUT") {
                        continue;
                    }

                    if (gate.second.getGateType() == type.first) {
                        if (x[gate.first].find(l) != x[gate.first].end()) {
                            resourceUsage += x[gate.first][l];
                            flag = true;
                        }
                    }
                }

                if(flag){
                    model.addConstr(resourceUsage <= type.second, "resource_" + type.first + "_time_" + std::to_string(l));
                }
            }
        }

        model.update();
        // model.write("model.lp");
        model.optimize();

        if (model.get(GRB_IntAttr_Status) == GRB_OPTIMAL) {
            this->ILPLatency = model.get(GRB_DoubleAttr_ObjVal) - 1;

            for (const auto& gate : this->gates) {
                if(gate.second.getGateType() == "INPUT" ){
                    continue;
                }

                for (int l = this->asapSchedule[gate.first]; l <= this->alapSchedule[gate.first]; ++l){
                    if (x[gate.first][l].get(GRB_DoubleAttr_X) > 0.5) {
                        this->ILPResult[l][this->gates[gate.first].getGateType()].push_back(&this->gates[gate.first]);
                        break;
                    }
                }
            }
        } else {
            std::cout << "No optimal solution found." << std::endl;
        }
    } catch (GRBException &e) {
        std::cerr << "Error code = " << e.getErrorCode() << std::endl;
        std::cerr << e.getMessage() << std::endl;
    } catch (...) {
        std::cerr << "Exception during optimization" << std::endl;
    }
}

void Scheduler::printAllComponents()
{
    std::cout << "Inputs: ";
    for(auto &node : this->inputNodes){
        std::cout << node << ' ' ;
    }
    std::cout << std::endl;

    std::cout << "Outputs: ";
    for(auto &node : this->outputNodes){
        std::cout << node << ' ' ;
    }
    std::cout << std::endl;

    std::cout << "Gates: " << std::endl;
    for(auto &gate : this->gates){
        std::cout << gate.second << std::endl;
    }

    for(auto &node : this->adjList){
        for(auto &gate : node.second.first){
            std::cout << gate->getOutputName() << ' ';
        }
        std::cout << "-> "<< node.first << " -> ";
        for(auto &gate : node.second.second){
            std::cout << gate->getOutputName() << ' ';
        }
        std::cout << std::endl;
    }

    
    std::cout << "ASAP Schedule" << std::endl;
    for(int i = 1 ; i <= this->listSschedulingLatency; ++i){
        std::cout << i << ": ";
        for(auto &node : this->asapSchedule){
            if(node.second == i){
                std::cout << node.first << ' ';
            }
        }
        std::cout << std::endl;
    }


    std::cout << "ALAP Schedule" << std::endl;
    for(int i = 1 ; i <= this->listSschedulingLatency; ++i){
        std::cout << i << ": ";
        for(auto &node : this->alapSchedule){
            if(node.second == i){
                std::cout << node.first << ' ';
            }
        }
        std::cout << std::endl;
    }
}

void Scheduler::printScheduleResult(){
    std::cout << "Heuristic Scheduling Result" << std::endl;
    int pass = 1;
    for(auto &schedule : this->scheduleResult){
        std::cout << pass++ << ": " ;
        for(auto &type : gateTypes){
            std::cout << "{" ;
            for(int i = 0; i < schedule[type].size(); i++){
                std::cout << schedule[type][i]->getOutputName();
                if(i != schedule[type].size() - 1){
                    std::cout << " ";
                }
            }
            std::cout << "} ";
        }
        std::cout << std::endl;
    }

    std::cout << "LATENCY: " << this->scheduleResult.size() << std::endl;
    std::cout << "END" << std::endl;
}

void Scheduler::printILPResult(){
    std::cout << "ILP-based Scheduling Result" << std::endl;

    for(int pass = 1; pass <= this->ILPLatency; ++pass){
        std::cout << pass << ": " ;
        for(auto &type : gateTypes){
            std::cout << "{" ;
            for(int i = 0; i < this->ILPResult[pass][type].size(); i++){
                std::cout << this->ILPResult[pass][type][i]->getOutputName();
                if(i != this->ILPResult[pass][type].size() - 1){
                    std::cout << " ";
                }
            }
            std::cout << "} ";
        }
        std::cout << std::endl;
    }

    std::cout << "LATENCY: " << this->ILPLatency << std::endl;
    std::cout << "END" << std::endl;
}

void Scheduler::outputResult(){
    std::string filename(this->blifFile.begin(), this->blifFile.end() - 5);
    filename += "_result.txt";
    
    std::ofstream out(filename);

    out << "ASAP Schedule" << std::endl;
    for(int i = 1 ; i <= this->listSschedulingLatency; ++i){
        out << i << ": ";
        for(auto &node : this->asapSchedule){
            if(node.second == i){
                out << node.first << ' ';
            }
        }
        out << std::endl;
    }

    out << "\nALAP Schedule" << std::endl;
    for(int i = 1 ; i <= this->listSschedulingLatency; ++i){
        out << i << ": ";
        for(auto &node : this->alapSchedule){
            if(node.second == i){
                out << node.first << ' ';
            }
        }
        out << std::endl;
    }

    out << "\nHeuristic Scheduling Result" << std::endl;
    int pass = 1;
    for(auto &schedule : this->scheduleResult){
        out << pass++ << ": " ;
        for(auto &type : gateTypes){
            out << "{" ;
            for(int i = 0; i < schedule[type].size(); i++){
                out << schedule[type][i]->getOutputName();
                if(i != schedule[type].size() - 1){
                    out << " ";
                }
            }
            out << "} ";
        }
        out << std::endl;
    }

    out << "LATENCY: " << this->scheduleResult.size() << std::endl;
    out << "END" << std::endl;

    out << "\nILP-based Scheduling Result" << std::endl;
    for(int pass = 1; pass <= this->ILPLatency; ++pass){
        out << pass << ": " ;
        for(auto &type : gateTypes){
            out << "{" ;
            for(int i = 0; i < this->ILPResult[pass][type].size(); i++){
                out << this->ILPResult[pass][type][i]->getOutputName();
                if(i != this->ILPResult[pass][type].size() - 1){
                    out << " ";
                }
            }
            out << "} ";
        }
        out << std::endl;
    }

    out << "LATENCY: " << this->ILPLatency << std::endl;
    out << "END" << std::endl;
}

bool Scheduler::checkILPResult(){
    std::set<std::string> allGate;

    for(auto const &gate: this->gates){
        if(gate.second.getGateType() == "AND" || gate.second.getGateType() == "OR" || gate.second.getGateType() == "NOT"){
            allGate.emplace(gate.first);
        }
    }

    for (auto &pass : this->ILPResult)
    {
        for (auto &type : this->gateTypes)
        {
            if (pass.second[type].size() > this->resourseConstraint[type])
            {
                return false;
            }
        }
    }

    for (auto &pass : this->ILPResult)
    {
        for (auto &type : this->gateTypes)
        {
            for (auto &gate : pass.second[type])
            {
                allGate.erase(gate->getOutputName());

                for (auto &input : gate->getInputNames())
                {
                    if(this->gates[input].getGateType() == "INPUT"){
                        continue;
                    }

                    int p = -1;

                    for (auto &predecessor : this->ILPResult)
                    {
                        for (auto &predecessorGate : predecessor.second[this->gates[input].getGateType()])
                        {
                            if (predecessorGate->getOutputName() == input)
                            {
                                p = predecessor.first;
                                break;
                            }
                        }

                        if (p != -1)
                        {
                            break;
                        }
                    }

                    if (p == -1 || p >= pass.first)
                    {
                        return false;
                    }
                }
            }
        }
    }

    if(!allGate.empty()){
        return false;
    }

    return true;
}

bool Scheduler::checkListResult(){
    std::set<std::string> allGate;

    for(auto const &gate: this->gates){
        if(gate.second.getGateType() == "AND" || gate.second.getGateType() == "OR" || gate.second.getGateType() == "NOT"){
            allGate.emplace(gate.first);
        }
    }

    for (auto &pass : this->scheduleResult)
    {
        for (auto &type : this->gateTypes)
        {
            if (pass[type].size() > this->resourseConstraint[type])
            {
                std ::cout << "Resourse\n";
                return false;
            }   
        }
    }

    for(int i = 0 ; i < this->scheduleResult.size(); ++i){
        for(auto &type : this->gateTypes){
            for(auto &gate : this->scheduleResult[i][type]){
                allGate.erase(gate->getOutputName());
                
                for(auto &input : gate->getInputNames()){
                    if(this->gates[input].getGateType() == "INPUT"){
                        continue;
                    }

                    int p = -1;
                    for(int j = 0 ; j < i; ++j){
                        for(auto &predecessor : this->scheduleResult[j][this->gates[input].getGateType()]){
                            if(predecessor->getOutputName() == input){
                                p = j;
                                break;
                            }
                        }
                        if(p != -1){
                            break;
                        }
                    }

                    if(p == -1 || p >= i){
                        return false;
                    }
                }
            }
        }
    }

    if(!allGate.empty()){
        return false;
    }

    return true;
}