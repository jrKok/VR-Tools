#include "rectangles.h"
#include "drawlogic.h"

rectangles::rectangles(bool drawable,bool ismodal):
    top(0),left(0),bottom(0),right(0),in_top(0),in_left(0),height(0),width(0),offsetX(0),offsetY(0),
    isVisible(true),isModal(ismodal)
{
    if (drawable){
        if (ismodal)
            drawNumber=DrawLogic::AddModalRect(*this,Clr_Black);
        else {
             drawNumber=DrawLogic::AddRectangle(*this,Clr_Black);
        }
    }
    else drawNumber=-1;
}


rectangles::rectangles(int in_top, int in_left, int in_bottom, int in_right, char in_Color, bool in_visibility ) :
    in_top(0),in_left(0),height(0),width(0),offsetX(0),offsetY(0)

{
    top=in_top;
    left=in_left;
    bottom=in_bottom;
    right=in_right;
    drawNumber=DrawLogic::AddRectangle(*this,in_Color);
    isVisible=in_visibility;
    DrawLogic::SetVisibilityRect(drawNumber,in_visibility);
}


rectangles::~rectangles(){}

void rectangles::SetOrigin(int in_x,int in_y){
    in_top=in_y;
    in_left=in_x;
    recalculate();
}

void rectangles::SetOffsets(int ox, int oy){
    offsetX=ox;
    offsetY=oy;
    rectangles::recalculate();
}

void rectangles::SetDimensions(int in_width,int in_height){
    width=in_width;
    height=in_height;
    recalculate();
}

void rectangles::SetAngles(int in_left,int in_top, int in_right,int in_bottom){
    left=in_left;
    bottom=in_bottom;
    right=in_right;
    top=in_top;
    width=right-left;
    height=top-bottom;
    recalculate();
}

int  rectangles::GetTop() const {
    return top;
}
int  rectangles::GetBottom()const{
    return bottom;
}
int  rectangles::GetLeft()const{
    return left;
}
int  rectangles::GetRight()const{
    return right;
}
int  rectangles::GetWidth()const{
    return width;
}
int  rectangles::GetHeight()const{
    return height;
}
int  rectangles::GetOffsetX()const{
    return offsetX;
}
int  rectangles::GetOffsetY()const{
    return offsetY;
}

void rectangles::WriteDebug(std::string message){
    std::string in_String="VR Tools : " +message+"\n";
    XPLMDebugString((char*)in_String.c_str());
}

void rectangles::recalculate(){
    top=in_top-offsetY;
    bottom=in_top-offsetY-height;
    left=in_left+offsetX;
    right=in_left+offsetX+width;
    if (drawNumber>-1){
        if (isModal){
            DrawLogic::RelocateModalRect(drawNumber,*this);
        }
        else {
            DrawLogic::RelocateRect(drawNumber,*this);
        }
    }

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
    setVisibility(false);
}

void rectangles::setColor(char in_Color){
    if (drawNumber>-1){
        if (isModal)
           {DrawLogic::ChangeColorModalRect(drawNumber,in_Color);}
        else {
            DrawLogic::ChangeColorRect(drawNumber,in_Color);
        }
    }

}

void rectangles::setVisibility(bool in_Visibility){
    if (drawNumber>-1){
        isVisible=in_Visibility;
        if (isModal)
            DrawLogic::SetVisibilityModalRect(drawNumber,in_Visibility);
        else {
            DrawLogic::SetVisibilityRect(drawNumber,in_Visibility);
        }
    }
}

void rectangles::SetAsModalRect0(){

    if (isModal){
        DrawLogic::SetVisibilityModalRect(drawNumber,false);
        drawNumber=0;
        DrawLogic::RelocateModalRect(drawNumber,*this);
    }
}

void rectangles::PrintParameters(){
    WriteDebug("Rectangle : my Number is "+std::to_string(drawNumber));
    WriteDebug("visibility tr 1 "+std::to_string(DrawLogic::GetVisibilityTriangle(drawNumber)));
    WriteDebug("visibility tr 2 "+std::to_string(DrawLogic::GetVisibilityTriangle(drawNumber+1)));
    WriteDebug("color of tri1 "+std::to_string((DrawLogic::GetColorCodeTriangle(drawNumber))));
    WriteDebug("color of tri2 "+std::to_string((DrawLogic::GetColorCodeTriangle(drawNumber+1))));

}

void rectangles::PrintModalParams(){
    DrawLogic::PrintModalTriangleCoords(drawNumber);
    DrawLogic::PrintModalTriangleCoords(drawNumber+1);
}
