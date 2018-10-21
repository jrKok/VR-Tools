#include "button_vr.h"

button_VR::button_VR() : rectangles(),
    isSelected(false),
    isPressed(false),
    hasSymbol(false),
    posTextX(0),
    posTextY(0),
    offsetTextX(0),
    offsetTextY(0),
    numberPoints(0),
    buttonText(""),
    symbol(),
    myColor{0.300f,0.500f,0.875f},
    darkgray{0.150f,0.150f,0.150f},
    colorPressed{0.250f,0.400f,0.600f},
    colorSelected{0.550f,0.750f,0.900f},
    niceWhite{0.950f,0.950f,0.950f}


{
}
  void button_VR::recalculate(){
    //rectangles::recalculate();
      top=in_top-offsetY;
      bottom=in_top-offsetY-height;
      left=in_left+offsetX;
      right=in_left+offsetX+width;
    posTextX=in_left+offsetTextX;
    posTextY=in_top-offsetTextY+2;
    if (hasSymbol){
    for (int I(0);I<numberPoints;I++){
        symbol[I].recalculate(in_left,in_top);}
    }
}

void button_VR::addPoint(int oX, int oY){
    point pt;
    pt.offsetX=oX;
    pt.offsetY=oY;
    hasSymbol=true;
    symbol.push_back(pt);
    numberPoints++;
}

void button_VR::setText(const std::string &in_String){
    buttonText=in_String;
    //calculates Offsets for text, adjusts width of the rectangle if necessary
    //should occur after offsets of rectangle have been entered(I don't verify that until problems arise
    int tWidth=(int)XPLMMeasureString(xplmFont_Proportional,(char*)in_String.c_str(),in_String.length());
    int tHeight(0);
    XPLMGetFontDimensions(xplmFont_Proportional,NULL,&tHeight,NULL);
    offsetTextY=offsetY+(int)(height/2)+(int)(tHeight/2);
    if (tWidth>=(width-4)) width=tWidth+4;
    offsetTextX=offsetX+(int)(width/2)-(int)(tWidth/2);
}

void button_VR::resetMe(){
    symbol.clear();
    hasSymbol=false;
    isVisible=true;
    isSelected=false;
    buttonText="";
    numberPoints=0;
    top=0;left=0;bottom=0;right=0;in_top=0;in_left=0;height=0;width=0;offsetX=0;offsetY=0;
    posTextX=0;posTextY=0; offsetTextX=0; offsetTextY=0;
}

void button_VR::setColor(float custColor[]){
    myColor[0]=custColor[0];
    myColor[1]=custColor[1];
    myColor[2]=custColor[2];
}

void button_VR::drawButton(){
    if (isVisible){
        if (isSelected&&(!isPressed)){
           glColor3fv(colorSelected);
           glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
           glBegin(GL_TRIANGLE_FAN);
          {
            glVertex2i(left, top);
            glVertex2i(right, top);
            glVertex2i(right, bottom);
            glVertex2i(left, bottom);
          }
          glEnd();}

        if (isPressed){
            glColor3fv(colorPressed);
            glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
            glBegin(GL_TRIANGLE_FAN);
            {
             glVertex2i(left, top);
             glVertex2i(right, top);
             glVertex2i(right, bottom);
             glVertex2i(left, bottom);
            }
            glEnd();}
        if (!isPressed&&!isSelected){
            glColor3fv(myColor);
            glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
            glBegin(GL_TRIANGLE_FAN);
            {
             glVertex2i(left, top);
             glVertex2i(right, top);
             glVertex2i(right, bottom);
             glVertex2i(left, bottom);
            }
            glEnd();}
    if (buttonText!=""){
         XPLMDrawString(niceWhite,posTextX,posTextY,(char*)buttonText.c_str(),NULL,xplmFont_Proportional);
    }

    if (hasSymbol){
        glColor3fv(darkgray);
        glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
        glBegin(GL_TRIANGLES);
          {
            for (int pt(0);pt<numberPoints;pt++){
               glVertex2i(symbol[pt].myX, symbol[pt].myY);
            }
          }
        glEnd();
    }
    }
}

void button_VR::Press(){
    isPressed=true;
}

void button_VR::Release(){
    isPressed=false;
}

void button_VR::setSelect(bool selState){
    isSelected=selState;
}
