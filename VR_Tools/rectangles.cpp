#include "rectangles.h"
#include "drawlogic.h"

rectangles::rectangles(bool drawable):
    top(0),
    left(0),
    bottom(0),
    right(0),
    height(0),
    width(0),
    my_currentColor(Clr_Black),
    isDrawable(drawable),
    isVisible(true),
    isModal(false),
    dbgName("")
{
    if (drawable){
        drawNumber =DrawLogic::AddRectangle(this);
    }
}

rectangles::rectangles(string name,bool drawable):
    top(0),
    left(0),
    bottom(0),
    right(0),
    height(0),
    width(0),
    my_currentColor(Clr_Black),
    isDrawable(drawable),
    isVisible(true),
    isModal(false),
    dbgName(name)
{
    if (drawable){
             drawNumber =DrawLogic::AddRectangle(this);
    }
    else drawNumber=0;
}

rectangles::rectangles(int in_top, int in_left, int in_bottom, int in_right, char in_Color, bool in_visibility ) :

    top(in_top),
    left(in_left),
    bottom(in_bottom),
    right(in_right),
    height(0),
    width(0),
    my_currentColor(in_Color),
    isDrawable(true),
    isVisible(in_visibility),
    dbgName("")
{
    width=right-left;
    height=bottom-top;
    drawNumber=DrawLogic::AddRectangle(this);

}


rectangles::~rectangles(){
    if (isDrawable) {DrawLogic::ReleaseRectangle(drawNumber);}
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

ulong rectangles::GetId() const{
    return drawNumber;
}

void rectangles::WriteDebug(std::string message){
    std::string in_String="VR Tools : " +message+"\n";
    XPLMDebugString((char*)in_String.c_str());
}


bool rectangles::IsHere(int x, int y){
    if ((x>=left)&&(x<=right)&&(y<=top)&&(y>=bottom)&&isVisible) return true;
    else return false;
}

void rectangles::ResetMe(){
    top=0;left=0;bottom=0;right=0;height=0;width=0;
    SetVisibility(false);
}

void rectangles::SetColor(char in_Color){
    my_currentColor=in_Color;
    if (isDrawable){
        DrawLogic::UpdateRectangle(drawNumber);
    }
}

void rectangles::SetVisibility(bool in_Visibility){
    if ((in_Visibility!=isVisible)&isDrawable ){
        isVisible=in_Visibility;
        if(isVisible) DrawLogic::UpdateRectangle(drawNumber);
        else DrawLogic::HideRectangle(drawNumber);

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

void rectangles::SetDebugName(string in_string){
    dbgName=in_string;
}

string rectangles::GetDebugName(){
    return dbgName;
}

void rectangles::SetNewDrawNumber(ulong in_Nb){
    drawNumber=in_Nb;
    isDrawable=true;
}

ulong rectangles::GetDrawNumber(){
    return drawNumber;
}

void rectangles::UnAttach(){
    isDrawable=false;
}
