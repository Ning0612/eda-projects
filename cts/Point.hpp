#ifndef POINT_HPP
#define POINT_HPP

class Point {
    public:
    int x, y;

    Point();
    Point(int x, int y);

    bool operator<(const Point& other) const;
    bool operator>(const Point& other) const;
    bool operator==(const Point& other) const;
    bool operator!=(const Point& other) const;
    Point operator-(const Point& other) const;
    Point operator+(const Point& other) const;
};

int calculateDistance(const Point& a, const Point& b) ;
Point medianPoint(const Point& a, const Point& b);

#endif