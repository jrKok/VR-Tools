#include "point.h"

point::point():
    offsetX(0),
    offsetY(0),
    x(0),
    y(0),
    screenX(0),
    screenY(0)
{

}

point::point(int in_x, int in_y):
    offsetX(0),
    offsetY(0),
    screenX(0),
    screenY(0)
{
    x=in_x;
    y=in_y;
}

void point::SetCoords(int in_x, int in_y){
    x=in_x;y=in_y;
}

void point::SetOffsets(int oX, int oY){
    offsetX=oX;
    offsetY=oY;
    screenX=x+oX;
    screenY=y+oY;
}

int point::GetX(){
    return x;
}

int point::GetY(){
    return y;
}
