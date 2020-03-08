#include "drawmodallogic.h"
//a subclass of drawlogic with very few added and modded functionality

drawModalLogic::drawModalLogic():drawlogic
{

}

void drawModalLogic::EnableModalWindow(){
drawlogic::modalWindowActive=true;
if (modalTriangles==nullptr) modalTriangles=new  map<int,TriangleToDraw>;
if (modalStrings==nullptr) modalStrings=new vector<StringToDraw>;
}

bool  drawModalLogic::IsModalWindowActive(){
    return modalWindowActive;
}
