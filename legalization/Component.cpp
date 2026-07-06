#include "Component.hpp"

Component::Component()
{
    this->name = "";
    this->width = 0;
    this->height = 0;
    this->isFixed = false;
    this->origin_position_LD = Point();
    this->direction = "";
}

Component::Component(std::string name, Point origin_position_LD, std::string direction)
{
    this->name = name;
    this->origin_position_LD = origin_position_LD;
    this->legal_position_LD = origin_position_LD;
    this->isFixed = false;
    this->direction = direction;
}

std::string Component::getName()
{
    return name;
}

void Component::setSizes(double width, double height)
{
    this->width = width;
    this->height = height;
}

std::pair<double, double> Component::getSizes() const
{
    return std::make_pair(width, height);
}

double Component::getWidth() const
{
    return this->width;
}

double Component::getHeight() const
{
    return this->height;
}

bool Component::getIsFixed() const
{
    return isFixed;
}

void Component::setIsFixed(bool isFixed) 
{
    this->isFixed = isFixed;
}

Point Component::getOriginPosition() const
{
    return origin_position_LD;
}

void Component::setOriginPosition(Point origin_position_LD)
{
    this->origin_position_LD = origin_position_LD;
}

std::pair<Point, Point> Component::getOriginBoundingBox() const
{
    return std::make_pair(this->origin_position_LD, this->origin_position_LD + Point(this->width, this->height));
}

std::pair<Point, Point> Component::getLegalBoundingBox() const
{
    return std::make_pair(this->legal_position_LD, this->legal_position_LD + Point(this->width, this->height));
}

std::pair<Point, Point> Component::getBoundingBoxAtPosition(const Point& position) const
{
    return std::make_pair(position, position + Point(this->width, this->height));
}

std::ofstream& Component::outPlFile(std::ofstream &out)
{
    out << this->name << "\t" << this->legal_position_LD.x << "\t" << this->legal_position_LD.y << " : " << this->direction << '\n';
    return out;
}

bool isOverlap(const Component& a, const Component& b)
{
    auto a_bbox = a.getLegalBoundingBox();
    auto b_bbox = b.getLegalBoundingBox();

    // 取得各自的邊界
    double a_left = a_bbox.first.x;
    double a_right = a_bbox.second.x;
    double a_top = a_bbox.second.y;
    double a_bottom = a_bbox.first.y;

    double b_left = b_bbox.first.x;
    double b_right = b_bbox.second.x;
    double b_top = b_bbox.second.y;
    double b_bottom = b_bbox.first.y;

    // 判斷是否在水平或垂直方向上沒有重疊
    if (a_right <= b_left || b_right <= a_left) return false;  // 水平方向沒有重疊
    if (a_bottom >= b_top || b_bottom >= a_top) return false;  // 垂直方向沒有重疊

    // 兩個矩形在水平方向和垂直方向上都有重疊
    return true;
}
