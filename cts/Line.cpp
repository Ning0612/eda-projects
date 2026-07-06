#include "Line.hpp"
#include <tuple>
#include <map>
#include <set>
#include <vector>
#include <limits>
#include <queue>
#include <iostream>
#include <algorithm>

Line::Line() : p1(Point()), p2(Point()) {}
Line::Line(Point p1, Point p2) : p1(p1), p2(p2) {}
Line::Line(int p1x, int p1y, int p2x ,int p2y) : p1(Point(p1x, p1y)), p2(Point(p2x, p2y)) {}

bool Line::operator<(const Line& other) const {
    return std::tie(p1, p2) < std::tie(other.p1, other.p2);
}

bool Line::operator==(const Line& other) const {
    return (p1 == other.p1 && p2 == other.p2) || (p1 == other.p2 && p2 == other.p1);
}

bool Line::operator!=(const Line& other) const {
    return !(*this == other);
}

int Line::distance() const{
    return calculateDistance(this->p1, this->p2);
}

bool isRectilinear(const Line &edge) {
    return edge.p1.x == edge.p2.x || edge.p1.y == edge.p2.y;
}

int shortestDistance(const std::vector<Line>&edges, const Point& src, const Point& dst) {
    std::map<Point, std::vector<std::pair<Point, int>>> adjList;

    for (const auto& e : edges) {
        Point p1 = e.p1;
        Point p2 = e.p2;
        int dist = calculateDistance(p1, p2);
        adjList[p1].push_back({p2, dist});
        adjList[p2].push_back({p1, dist});
    }

    struct CompareDist {
        bool operator()(const std::pair<int, Point>& a, const std::pair<int, Point>& b) const {
            return a.first > b.first;
        }
    };

    std::priority_queue<std::pair<int, Point>, std::vector<std::pair<int, Point>>, CompareDist> pq;
    std::map<Point, int> distances;
    std::set<Point> visited;

    for (auto& p : adjList) {
        distances[p.first] = std::numeric_limits<int>::max();
    }

    pq.push({0, src});
    distances[src] = 0;

    while (!pq.empty()) {
        Point current = pq.top().second;
        int currentDist = pq.top().first;
        pq.pop();

        if (visited.count(current)) continue;

        visited.insert(current);

        for (const auto& neighbor : adjList[current]) {
            Point next = neighbor.first;
            int weight = neighbor.second;

            int newDist = currentDist + weight;
            if (newDist < distances[next]) {
                distances[next] = newDist;
                pq.push({newDist, next});
            }
        }
    }

    return distances[dst] == std::numeric_limits<int>::max() ? -1 : distances[dst];
}

int orientation(const Point &p, const Point &q, const Point &r) {
    int val = (q.y - p.y) * (r.x - q.x) - 
              (q.x - p.x) * (r.y - q.y);

    if (val == 0)
        return 0;  
    return (val > 0) ? 1 : 2;  
}

bool onSegment(const Point &p, const Point &q, const Point &r) {
    if (p.x == q.x && q.x == r.x){
        return std::min(p.y, r.y) < q.y  && q.y < std::max(p.y, r.y);
    }
    if(p.y == q.y && q.y == r.y){
        return std::min(p.x, r.x) < q.x  && q.x < std::max(p.x, r.x);
    }
    return false;
}

bool isIntersect(const Line &l1, const Line &l2) {
    Point p1(l1.p1), p2(l1.p2), q1(l2.p1), q2(l2.p2);

    int o1 = orientation(p1, p2, q1);
    int o2 = orientation(p1, p2, q2);
    int o3 = orientation(q1, q2, p1);
    int o4 = orientation(q1, q2, p2);

    if (o1 != o2 && o3 != o4)
        return true;

    return false;
}

bool isLineInvalid(const Line &l1, const Line &l2) {
    Point p1(l1.p1), p2(l1.p2), q1(l2.p1), q2(l2.p2);
    if (!isIntersect(l1, l2))
        return false;

    if ((p1.x == q1.x && p1.y == q1.y) ||
        (p1.x == q2.x && p1.y == q2.y) ||
        (p2.x == q1.x && p2.y == q1.y) ||
        (p2.x == q2.x && p2.y == q2.y))
        return false;

    return true;
}

bool isOverlap(const Line& l1, const Line& l2) {
    if (l1.p1.x == l1.p2.x && l2.p1.x == l2.p2.x && l1.p1.x == l2.p1.x) {
        int l1_min_y = std::min(l1.p1.y, l1.p2.y);
        int l1_max_y = std::max(l1.p1.y, l1.p2.y);
        int l2_min_y = std::min(l2.p1.y, l2.p2.y);
        int l2_max_y = std::max(l2.p1.y, l2.p2.y);

        if (!(l1_max_y <= l2_min_y || l2_max_y <= l1_min_y)) {
            return true;
        }
    }

    if (l1.p1.y == l1.p2.y && l2.p1.y == l2.p2.y && l1.p1.y == l2.p1.y) {
        int l1_min_x = std::min(l1.p1.x, l1.p2.x);
        int l1_max_x = std::max(l1.p1.x, l1.p2.x);
        int l2_min_x = std::min(l2.p1.x, l2.p2.x);
        int l2_max_x = std::max(l2.p1.x, l2.p2.x);

       if (!(l1_max_x <= l2_min_x || l2_max_x <= l1_min_x)) {
            return true;
        }
    }

    return false;
}

bool isLineOnSegment(const Line& l1, const Line& l2){
    Point p1(l1.p1), p2(l1.p2), q1(l2.p1), q2(l2.p2);
    int o1 = orientation(p1, p2, q1);
    int o2 = orientation(p1, p2, q2);
    int o3 = orientation(q1, q2, p1);
    int o4 = orientation(q1, q2, p2);

    if (o1 == 0 && onSegment(p1, q1, p2)) return true;
    if (o2 == 0 && onSegment(p1, q2, p2)) return true;
    if (o3 == 0 && onSegment(q1, p1, q2)) return true;
    if (o4 == 0 && onSegment(q1, p2, q2)) return true;

    return false;
}

std::vector<Line> findLineOnSegment(const Line& l1, const Line& l2){
    Point p1(l1.p1), p2(l1.p2), q1(l2.p1), q2(l2.p2);
    std::vector<Line> lines;
    int o1 = orientation(p1, p2, q1);
    int o2 = orientation(p1, p2, q2);
    int o3 = orientation(q1, q2, p1);
    int o4 = orientation(q1, q2, p2);

    if (o1 == 0 && onSegment(p1, q1, p2)){
        lines.push_back(Line(p1, q1));
        lines.push_back(Line(q1, p2));
        lines.push_back(Line(q1, q2));
    } 
    if (o2 == 0 && onSegment(p1, q2, p2)) {
        lines.push_back(Line(p1, q2));
        lines.push_back(Line(q2, p2));
        lines.push_back(Line(q2, q1));
    }
    if (o3 == 0 && onSegment(q1, p1, q2)){
        lines.push_back(Line(q1, p1));
        lines.push_back(Line(p1, q2));
        lines.push_back(Line(p1, p2));
    }
    if (o4 == 0 && onSegment(q1, p2, q2)){
        lines.push_back(Line(q1, p2));
        lines.push_back(Line(p2, q2));
        lines.push_back(Line(p2, p1));
    }

    return lines;
}

bool hasCrossingEdges(const std::vector<Line> edge) {
    for (auto it1 = edge.begin(); it1 != edge.end(); ++it1) {
        for (auto it2 = it1 + 1; it2 != edge.end(); ++it2) {
            if (isLineInvalid(*it1, *it2) || isOverlap(*it1, *it2))
                // std::cout << it1->p1.x << ' ' << it1->p1.y << ' ' << it2->p2.x << ' ' << it2->p2.y << '\n';
                return true;
        }
    }
    return false;
}

bool isNewEdgeCrossing(const Line& new_line, const std::vector<Line>& edges) {
    for (const auto& edge : edges) {
        if(edge == new_line){
            continue;
        }

        if (isLineInvalid(new_line, edge)|| isOverlap(new_line, edge)) {
            return true;
        }
    }
    return false;
}

bool isNewEdgeCrossing(const std::vector<Line>& new_lines, const std::vector<Line>& edges) {
    for (const auto& line : new_lines) {
        if(isNewEdgeCrossing(line, edges)){
            return true;
        }
    }
    return false;
}

std::vector<Line> allCrossEdge(const std::vector<Line> edges){
    std::set<Line> crossingEdges;

    for (auto it1 = edges.begin(); it1 != edges.end(); ++it1) {
        for (auto it2 = it1 + 1; it2 != edges.end(); ++it2) {
            if (isLineInvalid(*it1, *it2) || isOverlap(*it1, *it2)){
                crossingEdges.insert(*it1);
                crossingEdges.insert(*it2);
            }
        }
    }
    
    return std::vector<Line>(crossingEdges.begin(), crossingEdges.end());
}

std::map<Point, std::vector<Point>> distanceTable(std::vector<Point> points){
    std::map<Point, std::vector<Point>> table;
    table.clear();

    for(const auto& p : points){
        std::vector<Point> pList;
        pList.clear();

        for(const auto& q : points){
            if(p != q){
                pList.push_back(q);
            }
        }

        std::sort(pList.begin(), pList.end(), [&](Point p1, Point p2){
            int distance1 = calculateDistance(p, p1);
            int distance2 = calculateDistance(p, p2);
            return distance1 < distance2;
        });

        table[p] = pList;
    }

    return table;
}

std::vector<Line> fixEdge(const std::vector<Line> edge){
    std::set<Line> newEdge(edge.begin(), edge.end());
    int k = 0;


    while(true){
        std::vector<Line> newEdgeList(newEdge.begin(), newEdge.end());
        std::set<std::pair<Line, Line>> overlapEdge;
        std::set<std::pair<Line, Line>> onSegmentsEdge;

        overlapEdge.clear();
        onSegmentsEdge.clear();
        bool hasNoInValidEdge = true;

        for (auto it1 = newEdgeList.begin(); it1 != newEdgeList.end(); ++it1) {
            for (auto it2 = it1 + 1; it2 != newEdgeList.end(); ++it2) {
                if (isOverlap(*it1, *it2)){
                    overlapEdge.insert(std::make_pair(*it1, *it2));
                    hasNoInValidEdge = false;
                }
                if (isLineOnSegment(*it1, *it2)){
                    onSegmentsEdge.insert(std::make_pair(*it1, *it2));
                    hasNoInValidEdge = false;
                }
            }
        }

        if(hasNoInValidEdge){
            return newEdgeList;
        }


        for (const auto& pL : overlapEdge){
            newEdge.erase(pL.first);
            newEdge.erase(pL.second);

            std::vector<Point> allPoints;
            allPoints.push_back(pL.first.p1);
            allPoints.push_back(pL.first.p2);
            allPoints.push_back(pL.second.p1);
            allPoints.push_back(pL.second.p2);
            
            std::sort(allPoints.begin(), allPoints.end());

            for(auto it = allPoints.begin(); next(it) != allPoints.end(); ++it){
                if(*it != *next(it)){
                    newEdge.insert(Line(*it, *next(it)));
                }
            }
        }
        
        for(const auto& pL : onSegmentsEdge){
            newEdge.erase(pL.first);
            newEdge.erase(pL.second);

            std::vector<Line> lines = findLineOnSegment(pL.first, pL.second);

            for(const auto& l : lines){
                newEdge.insert(l);
            }
        }
    }
}