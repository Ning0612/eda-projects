#ifndef SUBROW_HPP
#define SUBROW_HPP

enum RowType {COMPONENT, SPACING, FIXED, INVALID};

class SubRow
{
    private:
    double begin;
    double end;
    double width;
    RowType rowType;

    public:    
    SubRow();
    SubRow(double begin, double end, RowType rowType);

    double getBegin() const;
    double getWidth() const;
    double getEnd() const;
    RowType getType() const;

    void setBegin(double begin);
    void setEnd(double end);
    void setWidth(double width);
    void setType(RowType rowType);
};

#endif