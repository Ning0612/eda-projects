#ifndef POINT_HPP
#define POINT_HPP

class Point {
    public:
    double x, y;

    Point();
    Point(double x, double y);

    bool operator<(const Point& other) const;
    bool operator>(const Point& other) const;
    bool operator==(const Point& other) const;
    bool operator!=(const Point& other) const;
    Point operator-(const Point& other) const;
    Point operator+(const Point& other) const;

    static double distance(const Point& a, const Point& b);
};

#endif