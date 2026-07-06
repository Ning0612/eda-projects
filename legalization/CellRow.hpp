#ifndef CELLROW_HPP
#define CELLROW_HPP
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <limits>
#include "Point.hpp"
#include "SubRow.hpp"

class CellRow{
    private:
    bool isHorizontal;

    double coordinate;
    double height;
    double siteWidth;
    double siteSpacing;
    
    std::string siteorient;
    std::string sitesymmetry;

    double totalSpace;

    std::vector<std::pair<double, int>> subRowsInput;
    std::map<double, SubRow> subRows;

    std::pair<Point, Point> boundingBox;

    public:
    CellRow();
    CellRow(bool isHorizontal, double coordinate, double height, double siteWidth, double siteSpacing, std::string siteorient, std::string sitesymmetry, std::vector<std::pair<double, int>> subRows);

    bool getIsHorizontal() const;
    double getHeight() const;
    double getTotalSpace() const;

    double getCoordinate() const;
    std::map<double, SubRow> getSubRows() const;
    bool isValidSpacing(const std::pair<Point, Point>& boundingBox) const;
    bool addComponent(const std::pair<Point, Point>& boundingBox);
    bool removeComponent(const std::pair<Point, Point>& boundingBox);
    std::pair<Point, Point> getBoundingBox() const;
};

#endif