#ifndef ClockTree_H
#define ClockTree_H

#include <vector>
#include <set>
#include <string>
#include <map>
#include "Line.hpp"

struct InputCTS {
    int dimX;
    int dimY;
    int dotAmount;
    Point src;
    std::vector<Point> dst;
};

class ClockTree {
private:
    int dimX;
    int dimY;
    int dotAmount;

    int maxDepth;

    int maxSkew;
    int minSkew;
    double skewRatio;

    int totalLength;
    int flutewl;
    double wireRatio;

    bool valid = true;

    Point src;
    std::vector<Point> dst;
    std::vector<Line> edge;

    void buildHTreeBrute();
    void buildFluteTree();
    void buildBruteTree(); 


    std::vector<Line> bruteTree(const Point& src, const std::vector<Point>& dst);
    bool isValidDivLine(const std::set<int>& pointSet, int medPoint);
    int findValidLineInRange(const std::vector<Point>& points, const Point& pLD, const Point& pRU, const bool &vertical);

public:
    ClockTree();
    ClockTree(InputCTS cts);

    void outputCTSFile(const char* fileName);
    void outputGraph(const std::string& fileName);
    void printInfo();
    void calculateBenchMark();
    void tryBest();
};

InputCTS clockTreeFromFile(char* fileName);

namespace routing{
struct Node {
    Point point; 
    double g, h; 
    Node* parent; 

    Node(Point point, double g, double h, Node* parent = nullptr) : point(point), g(g), h(h), parent(parent) {}

    double f() const {
        return g + h; 
    }

    bool operator>(const Node& other) const {
        return f() > other.f(); 
    }
};

std::vector<Point> astar(const std::vector<std::vector<bool>>& grid, Point start, Point goal);
std::vector<Line> convertPathToLines(const std::vector<Point>& path);

}

#endif 