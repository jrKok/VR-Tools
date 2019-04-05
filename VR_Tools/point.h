#ifndef POINT_H
#define POINT_H


class point
{
public:
    point();
    int offsetX,offsetY,t,l,myX,myY;
    void SetOffsets(int oX,int oY);
    void recalculate (int, int);
};

#endif // POINT_H
