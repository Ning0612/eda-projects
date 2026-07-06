#ifndef COMPONENT_H
#define COMPONENT_H

#include <iostream>
#include <fstream>
#include <string>
#include "Point.hpp"

class Component{
    private:
    std::string name;
    
    double width;
    double height;
    
    bool isFixed = false;

    Point origin_position_LD;
    std::string direction;

    public:
    Point legal_position_LD;

    Component();
    Component(std::string name,  Point origin_position_LD, std::string direction);
    std::string getName();

    void setSizes(double width, double height);
    std::pair<double, double> getSizes() const;
    double getWidth() const;
    double getHeight() const;

    bool getIsFixed() const;
    void setIsFixed(bool isFixed);

    Point getOriginPosition() const;
    void setOriginPosition(Point origin_position_LD);

    std::pair<Point, Point> getOriginBoundingBox() const;
    std::pair<Point, Point> getLegalBoundingBox() const;
    std::pair<Point, Point> getBoundingBoxAtPosition(const Point& position) const;

    std::ofstream& outPlFile(std::ofstream &out);
};

bool isOverlap(const Component& a, const Component& b);

#endif