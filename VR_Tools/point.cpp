#include "point.h"

point::point():
    offsetX(0),offsetY(0),t(0),l(0),myX(0),myY(0)
{

}
void point::recalculate(int in_left, int in_top){
    t=in_top;
    l=in_left;
    myX=l+offsetX;
    myY=t-offsetY;
}
