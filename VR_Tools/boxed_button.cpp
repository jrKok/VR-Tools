#include "boxed_button.h"
#include "drawlogic.h"

Boxed_Button::Boxed_Button(string text, bool modal) : button_VR (text,modal),
    box_rect(true),
    box_clr(Clr_White)
{
    box_rect_number=box_rect.GetDrawNumber();
    box_rect.SetDebugName(text+" box");
}

void Boxed_Button::setVisibility(bool vis){
    if (vis!=isVisible){
        isVisible=vis;
        box_rect.SetVisibility(vis);
        DrawLogic::SetVisibilityString(stringNumber,vis);
        if (!isVisible)
           {DrawLogic::HideRectangle(box_rect_number);//should be enough to redraw background over the whole button
           }
        else {
            ReDrawButton();
        }
    }
}

void Boxed_Button::SetOrigin(int in_x, int in_y){
    box_rect.SetOrigin(in_x-1,in_y-1);
    box_rect.SetDimensions(width+2,height+2);
    if (in_y!=-1) bottom=in_y;
    if (in_x!=-1) left=in_x;
    right=left+width;
    top=bottom+height;
    if (box_rect_number>drawNumber){
        DrawLogic::SwapRectangle(box_rect_number,drawNumber);
        ulong swpi=box_rect_number;
        box_rect_number=drawNumber;
        box_rect.SetNewDrawNumber(box_rect_number);
        drawNumber=swpi;
    }
    LocateText();
}

void Boxed_Button::Shift(int dx,int dy){
    box_rect.Shift(dx,dy);
    rectangles::Shift(dx,dy);
    LocateText();
}

void Boxed_Button::ReDrawButton(){
    DrawLogic::UpdateRectangle(box_rect_number);
    DrawLogic::UpdateRectangle(drawNumber);
    DrawLogic::PrintString(stringNumber);
}

void Boxed_Button::SetBoxColor(char to_clr){
    box_rect.SetColor(to_clr);
    ReDrawButton();
}
