#ifndef POINT_H
#define POINT_H


class point
{
public:
    point();

    void SetCoords(int x, int y);
    void SetOffsets(int oX,int oY);
    int GetX();
    int GetY();
private:
    int offsetX,offsetY,x,y,screenX,screenY;
};

#endif // POINT_H
