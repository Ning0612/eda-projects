#include "CellRow.hpp"

CellRow::CellRow()
{
    this->isHorizontal = false;
    this->coordinate = 0;
    this->height = 0;
    this->siteWidth = 0;
    this->siteSpacing = 0;
    this->siteorient = "";
    this->sitesymmetry = "";
}

CellRow::CellRow(bool isHorizontal, double coordinate, double height, double siteWidth, double siteSpacing, std::string siteorient, std::string sitesymmetry, std::vector<std::pair<double, int>> subRowsInput)
{
    this->isHorizontal = isHorizontal;

    this->coordinate = coordinate;
    this->height = height;
    this->siteWidth = siteWidth;
    this->siteSpacing = siteSpacing;

    this->siteorient = siteorient;
    this->sitesymmetry = sitesymmetry;

    this->subRowsInput = subRowsInput;

    for (auto &subRowsIn : subRowsInput)
    {
        if(this->siteWidth == this->siteSpacing){
            this->subRows.emplace(subRowsIn.first, SubRow(subRowsIn.first, subRowsIn.first + subRowsIn.second * this->siteWidth, RowType::COMPONENT));
        }else{

            for(int i = 0; i < subRowsIn.second; i++){
                this->subRows.emplace(subRowsIn.first + i * this->siteSpacing, 
                    SubRow(subRowsIn.first + i * this->siteSpacing,
                            subRowsIn.first + i * this->siteSpacing + this->siteWidth, 
                            RowType::SPACING));
                this->subRows.emplace(subRowsIn.first + i * this->siteSpacing + this->siteWidth, 
                    SubRow(subRowsIn.first + i * this->siteSpacing + this->siteWidth,
                            subRowsIn.first + (i + 1) * this->siteSpacing,
                            RowType::INVALID));
            }
        }
    }

    this->totalSpace = 0;
    for(const auto& subRow : this->subRows){
        this->totalSpace += subRow.second.getWidth();
    }

    double x_min = std::numeric_limits<double>::max(), y_min = std::numeric_limits<double>::max();
    double x_max = std::numeric_limits<double>::min(), y_max = std::numeric_limits<double>::min();
    
    for(const auto& subRow : this->subRows){
        x_min = std::min(x_min, subRow.second.getBegin());
        x_max = std::max(x_max, subRow.second.getEnd());
        y_min = std::min(y_min, subRow.second.getBegin());
        y_max = std::max(y_max, subRow.second.getEnd());
    }

    this->boundingBox = std::make_pair(Point(x_min, y_min), Point(x_max, y_max));
}

bool CellRow::getIsHorizontal() const
{
    return this->isHorizontal;
}

double CellRow::getHeight() const
{
    return this->height;
}

double CellRow::getCoordinate() const
{
    return this->coordinate;
}

double CellRow::getTotalSpace() const
{
    return this->totalSpace;
}

bool CellRow::isValidSpacing(const std::pair<Point, Point>& boundingBox) const 
{
    double positionA, positionB;

    if (this->isHorizontal) {
        if (boundingBox.first.y != this->coordinate || boundingBox.first.y + this->height != boundingBox.second.y) {
            return false;
        }

        positionA = std::min(boundingBox.first.x, boundingBox.second.x);
        positionB = std::max(boundingBox.first.x, boundingBox.second.x);
    } else {
        if (boundingBox.first.x != this->coordinate || boundingBox.first.x + this->height != boundingBox.second.x) {
            return false;
        }

        positionA = std::min(boundingBox.first.y, boundingBox.second.y);
        positionB = std::max(boundingBox.first.y, boundingBox.second.y);
    }

    auto it = this->subRows.lower_bound(positionA); // 找到 >= target 的第一個鍵
    if (it == this->subRows.begin()) {
        return false; // 沒有 < target 的鍵
    }
    auto target = *(--it);

    if(target.second.getEnd() < positionB || target.second.getType() != RowType::SPACING){
        return false;
    }

    return true;
}

bool CellRow::addComponent(const std::pair<Point, Point>& boundingBox)
{
    if (!isValidSpacing(boundingBox)) {
        return false;
    }

    double positionA, positionB;

    if (this->isHorizontal) {
        positionA = std::min(boundingBox.first.x, boundingBox.second.x);
        positionB = std::max(boundingBox.first.x, boundingBox.second.x);
    } else {
        positionA = std::min(boundingBox.first.y, boundingBox.second.y);
        positionB = std::max(boundingBox.first.y, boundingBox.second.y);
    }

    auto it = this->subRows.lower_bound(positionA); // 找到 >= target 的第一個鍵
    if (it == this->subRows.begin()) {
        return false; // 沒有 < target 的鍵
    }
    auto target = *(--it);

    this->subRows.emplace(positionB, SubRow(positionB, target.second.getEnd(), RowType::SPACING));
    this->subRows.emplace(positionA, SubRow(positionA, positionB, RowType::COMPONENT));
    this->subRows[target.first].setEnd(positionA);


    return true;
}

bool CellRow::removeComponent(const std::pair<Point, Point>& boundingBox)
{
    double positionA, positionB;

    if (this->isHorizontal) {
        positionA = std::min(boundingBox.first.x, boundingBox.second.x);
        positionB = std::max(boundingBox.first.x, boundingBox.second.x);
    } else {
        positionA = std::min(boundingBox.first.y, boundingBox.second.y);
        positionB = std::max(boundingBox.first.y, boundingBox.second.y);
    }


    auto itComp = this->subRows.find(positionA);
    if(itComp == this->subRows.end() || itComp->second.getType() != RowType::COMPONENT){
        return false;
    }

    auto mid = itComp;
    auto left = --itComp;
    auto right = std::next(mid);

    double lowerBound = mid->second.getBegin();
    double upperBound = mid->second.getEnd();

    if(left != this->subRows.begin() && left->second.getType() == RowType::SPACING){
        lowerBound = left->second.getBegin();
        this->subRows.erase(left);
    }
    if(right != this->subRows.end() && right->second.getType() == RowType::SPACING){
        upperBound = right->second.getEnd();
        this->subRows.erase(right);
    }
    this->subRows.erase(mid);

    this->subRows.emplace(lowerBound, SubRow(lowerBound, upperBound, RowType::SPACING));

    return true;
}

std::map<double, SubRow> CellRow::getSubRows() const
{
    return this->subRows;
}

std::pair<Point, Point> CellRow::getBoundingBox() const
{
    return this->boundingBox;
}