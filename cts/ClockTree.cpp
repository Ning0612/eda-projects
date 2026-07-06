#include "ClockTree.hpp"
#include "flute.h"
#include <set>
#include <map>
#include <algorithm>
#include <queue>
#include <limits>
#include <tuple>
#include <cmath>
#include <fstream>
#include <iostream>

#define INT_MAX std::numeric_limits<int>::max()
#define INT_MIN std::numeric_limits<int>::min()

InputCTS clockTreeFromFile(char* fileName) {
    InputCTS cts;
    std::ifstream inFile(fileName);
    cts.dotAmount = 0;
    cts.dimX = 0;
    cts.dimY = 0;
    cts.dst.clear();

    std::string s;
    while (cts.dotAmount == 0 || cts.dimX == 0 || cts.dimY == 0) {
        inFile >> s;
        if (s == ".p") {
            inFile >> cts.dotAmount;
        } else if (s == ".dimx") {
            inFile >> cts.dimX;
        } else if (s == ".dimy") {
            inFile >> cts.dimY;
        }
    }

    Point p;
    inFile >> p.x >> p.y;
    cts.src = p;

    for (int i = 1; i < cts.dotAmount; ++i) {
        inFile >> p.x >> p.y;
        cts.dst.push_back(p);
    }

    inFile.close();

    return cts;
}

ClockTree::ClockTree() {
    this->dimX = 0;
    this->dimY = 0;
    this->dotAmount = 0;
    this->flutewl = 0;
    this->maxSkew = INT_MIN;
    this->minSkew = INT_MAX;
    this->totalLength = 0;
    this->skewRatio = 0;
    this->wireRatio = 0;
    this->src = Point(0, 0);
    this->dst.clear();
    this->edge.clear();
}

ClockTree::ClockTree(InputCTS cts) {
    this->dimX = cts.dimX;
    this->dimY = cts.dimY;
    this->dotAmount = cts.dotAmount;
    this->flutewl = 0;
    this->maxSkew = INT_MIN;
    this->minSkew = INT_MAX;
    this->totalLength = 0;
    this->skewRatio = 0;
    this->wireRatio = 0;
    this->src = cts.src;
    this->dst = cts.dst;
    this->edge.clear();
}

void ClockTree::calculateBenchMark() {
    this->maxSkew = INT_MIN;
    this->minSkew = INT_MAX;

    for (const auto& point : this->dst) {
        int dist = shortestDistance(this->edge, this->src, point);

        this->minSkew = std::min(this->minSkew, dist);
        this->maxSkew = std::max(this->maxSkew, dist);
    }

    this->skewRatio = (double)this->maxSkew / (double)this->minSkew;

    this->totalLength = 0;
    for (const auto& line : this->edge) {
        this->totalLength += line.distance();
    }

    this->wireRatio = (double)this->totalLength / (double)this->flutewl;

    this->valid = !hasCrossingEdges(this->edge) && this->minSkew > 0;
}

void ClockTree::outputCTSFile(const char* fileName) {
    std::ofstream outFile(fileName);

    outFile << ".l " << this->edge.size() << '\n';
    outFile << ".dimx " << this->dimX << '\n';
    outFile << ".dimy " << this->dimY << '\n';

    for (const auto& e : this->edge) {
        outFile << e.p1.x << ' ' << e.p1.y << ' '
                << e.p2.x << ' ' << e.p2.y << '\n';
    }

    outFile << ".e";
    outFile.close();
}

void ClockTree::outputGraph(const std::string& fileName) {
    std::ofstream outFile(fileName);

    outFile << "set xrange [0:" << this->dimX << "]\n";
    outFile << "set yrange [0:" << this->dimY << "]\n";
    outFile << '\n';

    for (const auto& e : this->edge) {
        outFile << "set arrow from " << e.p1.x << ',' << e.p1.y << " to "
                << e.p2.x << ',' << e.p2.y << " nohead lw 0.1\n";
    }

    for (const auto& e : allCrossEdge(this->edge)) {
        outFile << "set arrow from " << e.p1.x << ',' << e.p1.y << " to "
                << e.p2.x << ',' << e.p2.y << " nohead lw 0.1 linecolor rgb \"red\"\n";
    }


    // outFile << "plot NaN\n";
    outFile << "plot '-' with points pt 7 ps 0.15 notitle, "
            << "'-' with points pt 5 ps 0.3 notitle\n";

    for (const auto& p : this->dst) {
        outFile << p.x << ' ' << p.y << '\n';
    }
    outFile << "e\n";

    outFile << this->src.x << ' ' << this->src.y << '\n';
    outFile << "e\n";

    outFile.close();
}

void ClockTree::tryBest(){
    this->buildFluteTree();

    this->buildHTreeBrute();
    this->calculateBenchMark();
    if(this->valid){
       return;
    }

    this->buildFluteTree();
    this->calculateBenchMark();
    if(this->valid){
       return;
    }

    this->buildBruteTree();
    this->calculateBenchMark();
}

void ClockTree::printInfo() {
    std::cout << "Min skew : " << this->minSkew << '\n'
              << "Max skew : " << this->maxSkew << '\n'
              << "Skew ratio : " << this->skewRatio << '\n'
              << "Flute length : " << this->flutewl << '\n'
              << "Total length : " << this->totalLength << '\n'
              << "Wire ratio : " << this->wireRatio << '\n'
              << (this->valid ? "Valid" : "Invalid") << '\n';
}

void ClockTree::buildFluteTree(){
    this->edge.clear();
    Flute::Tree flutetree;
    int flutewl;
    int *x, *y;

    x = new int[this->dotAmount];
    y = new int[this->dotAmount];

    x[0] = src.x;
    y[0] = src.y;

    for(int i = 0 ; i < this->dotAmount - 1; ++i){
        x[i + 1] = this->dst.at(i).x;
        y[i + 1] = this->dst.at(i).y;
    }

    Flute::FluteState *flute1 = Flute::flute_init(FLUTE_POWVFILE, FLUTE_PORTFILE);

    flutetree = Flute::flute(flute1, this->dotAmount, x, y, FLUTE_ACCURACY);

    for (int i = 0; i < 2 * this->dotAmount - 2; i++) {  
        int x1 = flutetree.branch[i].x;   
        int y1 = flutetree.branch[i].y; 
        int j = flutetree.branch[i].n;    
        int x2 = flutetree.branch[j].x;   
        int y2 = flutetree.branch[j].y;   

        if (i != j) {  
            if (x1 == x2 || y1 == y2) {
                this->edge.emplace_back(x1, y1, x2, y2);
            } else {
                Line l1(Point(x1,y1), Point(x2, y1));
                Line l2(Point(x2,y1), Point(x2, y2));

                if(isNewEdgeCrossing(l1, this->edge) || isNewEdgeCrossing(l2, this->edge)){
                    l1 = Line(Point(x1,y1), Point(x1, y2));
                    l2 = Line(Point(x1,y2), Point(x2, y2));
                }

                this->edge.push_back(l1);
                this->edge.push_back(l2);
            }
        }
    }

    this->flutewl = Flute::flute_wl(flute1, this->dotAmount, x, y, FLUTE_ACCURACY);
    this->edge = fixEdge(this->edge);
}

void ClockTree::buildBruteTree(){
    this->edge.clear();
    std::vector<Point> points(dst);
    std::map<int, std::map<int, Point>> pointTable;
    pointTable.clear();

    points.push_back(src);

    for(const auto & p : points){
        pointTable[p.y][p.x] = p;
        pointTable[p.y][src.x] = Point(src.x, p.y);
    }

    for(const auto& pT : pointTable){
        for (auto it = pT.second.begin(); it != pT.second.end(); ++it) {
            auto next_it = std::next(it);
            if (next_it != pT.second.end()) {
                this->edge.emplace_back(it->second, next_it->second);
            }
        }
    }

    for (auto it = pointTable.begin(); it != pointTable.end(); ++it) {
        auto next_it = std::next(it);
        if (next_it != pointTable.end()) {
            this->edge.emplace_back(src.x, it->first, src.x, next_it->first);
        }
    }
}

void ClockTree::buildHTreeBrute(){
    this->edge.clear();

    std::vector<Point> points(this->dst);
    points.push_back(this->src);

    int divY1 = this->dimX * 0.125;
    int divY2 = this->dimX * 0.375;
    int divY3 = this->dimX * 0.625;
    int divY4 = this->dimX * 0.875;

    int divXLU = findValidLineInRange(points, Point(divY1, this->dimY *0.7), Point(divY2, this->dimY *0.85), false);
    int divXRU = findValidLineInRange(points, Point(divY3, this->dimY *0.7), Point(divY4, this->dimY *0.85), false);
    int divXLD = findValidLineInRange(points, Point(divY1, this->dimY *0.15), Point(divY2, this->dimY *0.3), false);
    int divXRD = findValidLineInRange(points, Point(divY3, this->dimY *0.15), Point(divY4, this->dimY *0.3), false);
    int divYL = findValidLineInRange(points, Point(divY1, divXLD), Point(divY2, divXLU), true);
    int divYR = findValidLineInRange(points, Point(divY3, divXRD), Point(divY4, divXRU), true);
    int divXMM = findValidLineInRange(points, Point(divYL, this->dimY *0.35), Point(divYR, this->dimY *0.65), false);

    if(onSegment(Point(divY1, divXLU), this->src, Point(divY2, divXLU))){
        ++divXLU;
    }
    if(onSegment(Point(divY3, divXRU), this->src, Point(divY4, divXRU))){
        ++divXRU;
    }
    if(onSegment(Point(divY1, divXLD), this->src, Point(divY2, divXLD))){
        ++divXLD;
    }
    if(onSegment(Point(divY3, divXRD), this->src, Point(divY4, divXRD))){
        ++divXRD;
    }
    if(onSegment(Point(divYL, divXLU), this->src, Point(divYL, divXLD))){
        ++divYL;
    }
    if(onSegment(Point(divYR, divXRU), this->src, Point(divYR, divXRD))){
        ++divYR;
    }
    if(onSegment(Point(divYL, divXMM), this->src, Point(divYR, divXMM))){
        ++divXMM;
    }

    std::map<int, std::set<Point>> toConnectPoint;
    toConnectPoint[divY1].emplace(divY1, divXLU);
    toConnectPoint[divY1].emplace(divY1, divXLD);
    toConnectPoint[divY2].emplace(divY2, divXLU);
    toConnectPoint[divY2].emplace(divY2, divXLD);
    toConnectPoint[divY3].emplace(divY3, divXRU);
    toConnectPoint[divY3].emplace(divY3, divXRD);
    toConnectPoint[divY4].emplace(divY4, divXRU);
    toConnectPoint[divY4].emplace(divY4, divXRD);

    for(const auto& p : this->dst){
        int comX;

        if(divY1 <= p.x && p.x <= divY2){
            if(p.y == divXLD) {
                this->edge.emplace_back(p, Point(divYL, p.y));
                continue;
            } else if(p.y == divXLU) {
                this->edge.emplace_back(p, Point(divYL, p.y));
                continue;
            }
        }
        if(divY3 <= p.x && p.x <= divY4){
            if(p.y == divXRD) {
                this->edge.emplace_back(p, Point(divYR, p.y));
                continue;
            }else if(p.y == divXRU) {
                this->edge.emplace_back(p, Point(divYR, p.y));
                continue;
            }
        }
        
        if(divYL <= p.x && p.x <= divYR && p.y == divXMM) {
            this->edge.emplace_back(p, Point(this->dimX / 2, p.y));
            continue;
        }
        if(divXLD <= p.y && p.y <= divXLU && p.x == divYL) {
            this->edge.emplace_back(p, Point(p.x, divXMM));
            continue;
        }
        if(divXRD <= p.y && p.y <= divXRU && p.x == divYR) {
            this->edge.emplace_back(p, Point(p.x, divXMM));
            continue;
        }
        
        if(p.x < divYL){
            comX = divY1;
        }else if(p.x < this->dimX / 2){
            comX = divY2;
        }else if(p.x < divYR){
            comX = divY3;
        }else {
            comX = divY4;
        }

        if(onSegment(Point(comX, p.y),this->src ,p)){
            int newY = p.y + ((p.x > this->dimY / 2) ? -1 : 1);
            this->edge.emplace_back(p, Point(p.x, newY));
            this->edge.emplace_back(Point(p.x, newY), Point(comX, newY));
            continue;
        }else if(p.x != comX){
            this->edge.emplace_back(p, Point(comX, p.y));
        }

        toConnectPoint[comX].emplace(comX, p.y);
    }

    for(const auto& pS : toConnectPoint){
        for(auto it = pS.second.begin(); next(it) != pS.second.end(); ++it){
            auto iit = next(it);
            if(it->y >= divXMM && iit->y >= divXMM){
                this->edge.emplace_back(*it, *iit);
            }
            if(it->y < divXMM && iit->y < divXMM){
                this->edge.emplace_back(*it, *iit);
            }
        }
    }

    this->edge.emplace_back(divY1, divXLU, divY2, divXLU);
    this->edge.emplace_back(divY3, divXRU, divY4, divXRU);
    this->edge.emplace_back(divY1, divXLD, divY2, divXLD);
    this->edge.emplace_back(divY3, divXRD, divY4, divXRD);
    this->edge.emplace_back(divYL, divXLU, divYL, divXLD);
    this->edge.emplace_back(divYR, divXRU, divYR, divXRD);
    this->edge.emplace_back(divYL, divXMM, divYR, divXMM);

    std::vector<std::vector<bool>> grid(dimX, std::vector<bool>(dimY, false));

    for(const auto& e: this->edge){
        if(e.p1.x == e.p2.x){
            for(int i = std::min(e.p1.y, e.p2.y); i <= std::max(e.p1.y, e.p2.y); ++i){
                grid[e.p1.x][i] = true;
            }
        }else if(e.p1.y == e.p2.y){
            for(int i = std::min(e.p1.x, e.p2.x); i <= std::max(e.p1.x, e.p2.x); ++i){
                grid[i][e.p1.y] = true;
            }
        }
    }

    Point medDst(this->dimX / 2,divXMM);
    grid[medDst.x][medDst.y] = false;
    
    std::vector<Line> path = routing::convertPathToLines(routing::astar(grid, this->src, medDst));

    if (!path.empty()) {
        for(const auto & l : path){
            this->edge.push_back(l);
        }
    }

    this->edge = fixEdge(this->edge);
}

bool ClockTree::isValidDivLine(const std::set<int>& pointSet, int medPoint) {
    for (int i = -2; i <= 2; ++i) {
        if (pointSet.count(medPoint + i)) {
            return false;  
        }
    }
    return true;  
}

int ClockTree::findValidLineInRange(const std::vector<Point>& points, const Point& pLD, const Point& pRU, const bool &vertical){
    std::set<int> pSet;
    int med;
    int range;
    pSet.clear();

    for(const auto& p : points){
        if(pLD.x <= p.x && p.x <= pRU.x && pLD.y <= p.y && p.y <= pRU.y){
            if(vertical){
                pSet.insert(p.x);
            }else{
                pSet.insert(p.y);
            }
        }
    }

    if(vertical){
        med = (pLD.x + pRU.x) / 2;
        range = (pRU.x - pLD.x) / 2 - 2;
    }else{
        med = (pLD.y + pRU.y) / 2;
        range = (pRU.y - pLD.y) / 2 - 2;
    }

    int newMed = med;

    for(int i = range; i >= 0; --i){
        if(isValidDivLine(pSet, med + i)){
            newMed = med + i;
        }
        if(isValidDivLine(pSet, med - i)){
            newMed = med - i;
        }
    }

    return newMed;
}

namespace routing {
    const std::vector<Point> directions = {Point(0, 1), Point(0, -1), Point(1, 0), Point(-1, 0)};

    bool isValid(Point p, int rows, int cols) {
        return p.x >= 0 && p.x < rows && p.y >= 0 && p.y < cols;
    }

    int hF(const Point& p1, const Point& p2) {
        return std::abs(p1.x - p2.x) + std::abs(p1.y - p2.y); 
    }

    std::vector<Point> astar(const std::vector<std::vector<bool>>& grid, Point start, Point goal) {
        int rows = grid.size();
        int cols = grid[0].size();

        std::priority_queue<Node, std::vector<Node>, std::greater<Node>> openList; 
        std::set<Point> closedList; 

        openList.emplace(start, 0.0, hF(start, goal));

        while (!openList.empty()) {
            Node current = openList.top();
            openList.pop();

            if (current.point == goal) {
                std::vector<Point> path;
                Node* node = &current;
                while (node) {
                    path.emplace_back(node->point);
                    node = node->parent;
                }
                std::reverse(path.begin(), path.end());
                return path;
            }

            if (closedList.find(current.point) != closedList.end()) {
                continue;
            }

            closedList.insert(current.point);

            for (const auto& direction : directions) {
                Point newPoint = current.point + direction;

                if (isValid(newPoint, rows, cols) && !grid[newPoint.x][newPoint.y] && closedList.find(newPoint) == closedList.end()) {
                    double gNew = current.g + 1.0; 
                    double hNew = hF(newPoint, goal);

                    openList.emplace(newPoint, gNew, hNew, new Node(current));
                }
            }
        }

        return {};
    }

    std::vector<Line> convertPathToLines(const std::vector<Point>& path){
        Point p1 = path.front();
        Point p2;
        std::vector<Line> newEdge;

        for(auto it = path.begin(); next(it) != path.end(); ++it){
            if(p1.x == next(it)->x || p1.y == next(it)->y){
                p2 = *next(it);
            }else{
                newEdge.emplace_back(p1, p2);
                p1 = p2;
                p2 = *next(it);
            }
        }

        newEdge.emplace_back(p1, p2);
        return newEdge;
    }

}