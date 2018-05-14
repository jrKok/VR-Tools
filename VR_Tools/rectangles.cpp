#include "rectangles.h"

rectangles::rectangles() :
    top(0),left(0),bottom(0),right(0),in_top(0),in_left(0),height(0),width(0),offsetX(0),offsetY(0),
    isVisible(true)

{

}
void rectangles::recalculate(){
    top=in_top-offsetY;
    bottom=in_top-offsetY-height;
    left=in_left+offsetX;
    right=in_left+offsetX+width;

}

void rectangles::recalculate(int lt, int tp){
    in_top=tp;in_left=lt;
    recalculate();
}

bool rectangles::isHere(int x, int y){
    if ((x>=left)&&(x<=right)&&(y<=top)&&(y>=bottom)&&isVisible) return true;
    else return false;

}

void rectangles::resetMe(){
    top=0;left=0;bottom=0;right=0;in_top=0;in_left=0;height=0;width=0;offsetX=0;offsetY=0;
}
