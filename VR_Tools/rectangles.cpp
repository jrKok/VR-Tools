#include "rectangles.h"
#include "drawlogic.h"

rectangles::rectangles(bool drawable):
    top(0),
    left(0),
    bottom(0),
    right(0),
    height(0),
    width(0),
    isVisible(true)
{
    if (drawable){
             drawNumber=DrawLogic::AddRectangle(this);
    }
    else drawNumber=-1;
}


rectangles::rectangles(int in_top, int in_left, int in_bottom, int in_right, char in_Color, bool in_visibility ) :

    top(in_top),
    left(in_left),
    bottom(in_bottom),
    right(in_right),
    height(0),
    width(0),
    my_currentColor(in_Color),
    isVisible(in_visibility)
{
    width=right-left;
    height=bottom-top;
    drawNumber=DrawLogic::AddRectangle(this);
}


rectangles::~rectangles(){

}

void rectangles::SetOrigin(int in_x, int in_y){
    if (in_y!=-1) bottom=in_y;
    if (in_x!=-1) left=in_x;
    right=left+width;
    top=bottom+height;
}


void rectangles::SetDimensions(int in_width, int in_height){
    width=in_width;
    height=in_height;
    right=left+width;
    top=bottom+height;
}

void rectangles::SetAngles(int in_left, int in_bottom, int in_right, int in_top){
    left=in_left;
    bottom=in_bottom;
    right=in_right;
    top=in_top;
    width=right-left;
    height=top-bottom;
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


char rectangles::GetColor() const{
    return my_currentColor;
}

int rectangles::GetId() const{
    return drawNumber;
}
void rectangles::WriteDebug(std::string message){
    std::string in_String="VR Tools : " +message+"\n";
    XPLMDebugString((char*)in_String.c_str());
}


bool rectangles::isHere(int x, int y){
    if ((x>=left)&&(x<=right)&&(y<=top)&&(y>=bottom)&&isVisible) return true;
    else return false;
}

void rectangles::resetMe(){
    top=0;left=0;bottom=0;right=0;height=0;width=0;
    setVisibility(false);
}

void rectangles::setColor(char in_Color){
    if (drawNumber>-1){
        my_currentColor=in_Color;
        DrawLogic::UpdateRectangle(drawNumber);
    }
}

void rectangles::setVisibility(bool in_Visibility){
    if (in_Visibility!=isVisible ){
        isVisible=in_Visibility;
        if (drawNumber>-1){
            if(isVisible) DrawLogic::UpdateRectangle(drawNumber);
            else DrawLogic::HideRectangle(drawNumber);
        }
    }
}

void rectangles::Shift(int dx, int dy){
    bottom+=dy;
    top+=dy;
    right+=dx;
    left+=dx;
}

bool rectangles::IsVisible()const {
    return isVisible;
}

void rectangles::PrintParameters(){
    WriteDebug("Rectangle : my Number is "+std::to_string(drawNumber));
}

void rectangles::UpdateMyTexture(){
    DrawLogic::UpdateRectangle(drawNumber);
}
