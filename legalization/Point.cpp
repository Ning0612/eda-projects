#include "Point.hpp"
#include <tuple>
#include <cmath>

Point::Point() : x(0), y(0) {}
Point::Point(double x, double y) : x(x), y(y) {}

bool Point::operator<(const Point& other) const {
    return std::tie(x, y) < std::tie(other.x, other.y);
}

bool Point::operator>(const Point& other) const {
    return std::tie(x, y) > std::tie(other.x, other.y);
}

bool Point::operator==(const Point& other) const {
    return x == other.x && y == other.y;
}

bool Point::operator!=(const Point& other) const {
    return !(*this == other);
}

Point Point::operator-(const Point& other) const {
    return Point(x - other.x, y - other.y);
}

Point Point::operator+(const Point& other) const {
    return Point(x + other.x, y + other.y);
}

double Point::distance(const Point& a, const Point& b) {
    return std::abs(a.x - b.x) + std::abs(a.y - b.y);
}