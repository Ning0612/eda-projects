#include "SubRow.hpp"

SubRow::SubRow()
{
}

SubRow::SubRow(double begin, double end, RowType rowType)
{
    this->begin = begin;
    this->end = end;
    this->width = this->end - this->begin;
    this->rowType = rowType;
}

double SubRow::getBegin() const
{
    return this->begin;
}

double SubRow::getEnd() const
{
    return this->end;
}

double SubRow::getWidth() const
{
    return this->width;
}

RowType SubRow::getType() const
{
    return this->rowType;
}

void SubRow::setBegin(double begin)
{
    this->begin = begin;
    this->width = this->end - this->begin;
}

void SubRow::setEnd(double end)
{
    this->end = end;
    this->width = this->end - this->begin;
}

void SubRow::setWidth(double width)
{
    this->width = width;
    this->end = this->begin + this->width;
}

void SubRow::setType(RowType rowType)
{
    this->rowType = rowType;
}

