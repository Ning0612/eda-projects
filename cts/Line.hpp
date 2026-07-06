#ifndef LINE_HPP
#define LINE_HPP

#include "Point.hpp"
#include <vector>
#include <map>

class Line {
    public:
    Point p1, p2;

    Line();
    Line(Point p1, Point p2);
    Line(int p1x, int p1y, int p2x ,int p2y);

    bool operator<(const Line& other) const;
    bool operator==(const Line& other) const;
    bool operator!=(const Line& other) const;
    int distance() const;
};

bool isRectilinear(const Line &edge);

int shortestDistance(const std::vector<Line>&edges, const Point& src, const Point& dst);

int orientation(const Point &p, const Point &q, const Point &r);
bool onSegment(const Point &p, const Point &q, const Point &r);
bool isIntersect(const Line &l1, const Line &l2);
bool isLineInvalid(const Line &l1, const Line &l2);
bool isOverlap(const Line& l1, const Line& l2);
bool isLineOnSegment(const Line& l1, const Line& l2);
std::vector<Line> findLineOnSegment(const Line& l1, const Line& l2);

bool hasCrossingEdges(const std::vector<Line> edge);
bool isNewEdgeCrossing(const Line& new_line, const std::vector<Line>& edges);
bool isNewEdgeCrossing(const std::vector<Line>& new_lines, const std::vector<Line>& edges);
std::vector<Line> allCrossEdge(const std::vector<Line> edge);
std::map<Point, std::vector<Point>>distanceTable(std::vector<Point> points);
std::vector<Line> fixEdge(const std::vector<Line> edge);

#endif