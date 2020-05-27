#include "background.h"
#include "fontman.h"

Background::Background() :
    width(325),
    height(467),
    valid(false)
{

}

void Background::MakeBackGround(DrawLogic * layoutsDrawPad,XPLMWindowID twd,char ink_color){
    DrawLogic::WriteDebug("background beginning construction");
    thisW=twd;
    drawp=layoutsDrawPad;
    std::fstream tFile;
    instructs.clear();
    tFile.open("Resources/plugins/VR_Tools/resources/kneeboard.txt");
    if (tFile.is_open()){
        string in_line;
        while (std::getline(tFile,in_line)) instructs.push_back(in_line);
        if (instructs.size()>=66) valid=true;
    }
    drawp->ToUpperLevel();
    drawp->SetNewSize(width,height);
    boxes.clear();
    lines.clear();
    if (valid){

        for (int I(0);I<16;I++){
            rectangles nr(false);
            nr.SetColor(ink_color);
            boxes.push_back(nr);
        }
        for (ulong I(0);I<66;I++){
            textshow tl;
            tl.color=ink_color;
            tl.pos.SetCoords(11,11);
            tl.fontSize=0;
            tl.text=instructs[I];
            lines.push_back(tl);
        }

        //Pilot Report box Max width 320, Max heidht 467
        boxes[0].SetAngles( 3, 5,95, 152);
        boxes[6].SetAngles(boxes[0].GetLeft()+2,boxes[0].GetTop()-17,boxes[0].GetRight()-2,boxes[0].GetTop()-2);
        MakeLeftList(47,10,boxes[6].GetLeft()+2,boxes[6].GetBottom()-14);
        CenterText(lines[45],boxes[6]);
        //Comm Freqs
        boxes[1].SetAngles(100, 5,321,112);
        boxes[7].SetAngles(boxes[1].GetLeft()+2,boxes[1].GetTop()-17,boxes[1].GetRight()-2,boxes[1].GetTop()-2);
        CenterText(lines[12],boxes[7]);
        MakeCenteredList(14,6,boxes[7].GetBottom()-18,boxes[7].GetLeft(),boxes[7].GetRight());
        //Transponder
        boxes[2].SetAngles( 3, boxes[0].GetTop()+20,143,boxes[0].GetTop()+121);
        boxes[8].SetAngles(boxes[2].GetLeft()+2,boxes[2].GetTop()-17,boxes[2].GetRight()-2,boxes[2].GetTop()-2);
        CenterText(lines[39],boxes[8]);
        MakeLeftList(41,5,boxes[8].GetLeft()+2,boxes[8].GetBottom()-18);
        //Cruising Altitudes
        boxes[3].SetAngles(146,boxes[1].GetTop()+5,321,boxes[1].GetTop()+184);
        boxes[9].SetAngles(boxes[3].GetLeft()+2,boxes[3].GetTop()-17,boxes[3].GetRight()-2,boxes[3].GetTop()-2);
        CenterText(lines[19],boxes[9]);
        boxes[12].SetOrigin(boxes[3].GetLeft(),boxes[3].GetBottom()+52);boxes[12].SetDimensions(boxes[3].GetWidth()/2,12*7+1);
        boxes[13].SetOrigin(boxes[12].GetRight()-1,boxes[12].GetBottom());boxes[13].SetDimensions(boxes[12].GetWidth()+2,boxes[12].GetHeight());
        MakeLeftList(21,2,boxes[3].GetLeft()+1,boxes[9].GetBottom()-11);
        MakeCenteredList(23,6,boxes[12].GetTop()-16,boxes[12].GetLeft(),boxes[12].GetRight());
        MakeCenteredList(30,6,boxes[13].GetTop()-16,boxes[13].GetLeft(),boxes[13].GetRight());
        MakeLeftList(36,4,boxes[12].GetLeft()+1,boxes[13].GetBottom()-12);
        //Rules of Thumb
        boxes[4].SetAngles( 3,boxes[2].GetTop()+25,157,boxes[2].GetTop()+183);
        boxes[10].SetAngles(boxes[4].GetLeft()+2,boxes[4].GetTop()-31,boxes[4].GetRight()-2,boxes[4].GetTop()-16);
        CenterText(lines[0],boxes[10]);
        MakeCenteredList(2,1,boxes[4].GetTop()-13,boxes[4].GetLeft(),boxes[4].GetRight());
        MakeRightList(3,3,boxes[10].GetBottom()-12,boxes[4].GetRight());
        MakeLeftList(6,3,boxes[10].GetLeft(),boxes[10].GetBottom()-4*12);
        MakeCenteredList(9,4,boxes[10].GetBottom()-7*12,boxes[10].GetLeft(),boxes[10].GetRight());

        //Postion Report
        boxes[5].SetAngles(163,boxes[3].GetTop()+13,320,boxes[3].GetTop()+160);
        boxes[11].SetAngles(boxes[5].GetLeft()+2,boxes[5].GetTop()-17,boxes[5].GetRight()-2,boxes[5].GetTop()-2);
        CenterText(lines[56],boxes[11]);
        MakeLeftList(58,9,boxes[11].GetLeft()+2,boxes[11].GetBottom()-18);
        //Titles boxes
        //secondary boxes

    }
}

void Background::GenerateMyTexture(){
        drawp->FillAll(Clr_DarkGray);
        for (ulong I(0);I<14;I++){
            drawp->DrawBox(boxes[I].GetLeft(),boxes[I].GetBottom(),boxes[I].GetRight(),boxes[I].GetTop(),boxes[I].GetColor());
        }
        for (auto st:lines){
            drawp->DrawStringOnLocalTexture(st.text,st.color,st.pos,st.fontSize);
        }
}

void Background::CenterText(textshow &in_line, rectangles posRect){
    int sz=fontMan::MeasureString(in_line.text);
    in_line.pos.SetCoords(posRect.GetLeft()+posRect.GetWidth()/2-sz/2,posRect.GetBottom()+3);
}

void Background::MakeLeftList(ulong begin_line, ulong number_of_lines, int pos_up_x, int pos_up_y){
    for (ulong indx(begin_line-1),I(0);I<number_of_lines;I++,indx++){
        lines[indx].pos.SetCoords(pos_up_x,pos_up_y-static_cast<int>(I*12));
    }
}

void Background::MakeRightList(ulong begin_line, ulong number_of_lines, int start_y, int lim_right){
    for (ulong indx(begin_line-1),I(0);I<number_of_lines;I++,indx++){
        string tx=lines[indx].text;
        int sz=fontMan::MeasureString(tx);
        lines[indx].pos.SetCoords(lim_right-2-sz,start_y-static_cast<int>(I*12));
    }
}

void Background::MakeCenteredList(ulong begin_line, ulong number_of_lines, int start_y, int lim_left, int lim_right){
    for (ulong indx(begin_line-1),I(0);I<number_of_lines;I++,indx++){
        string tx=lines[indx].text;
        int sz=fontMan::MeasureString(tx);
        lines[indx].pos.SetCoords(((lim_right-lim_left)/2-sz/2+lim_left),start_y-static_cast<int>(I*12));
    }
}

void Background::LoadTexture(){
    int t(0),b(0),r(0),l(0),w(0),h(0);
    XPLMGetWindowGeometry(thisW,&l,&t,&r,&b);
    if (XPLMWindowIsInVR(thisW)){
        XPLMGetWindowGeometryVR(thisW,&w,&h);
    }
    else {
       w=r-l;
       h=t-b;
    }
    drawp->UpdateDrawPad(w,h,l,b,r,t);
}

void Background::KeepSize(){
    int t(0),b(0),r(0),l(0),w(0),h(0);
    XPLMGetWindowGeometry(thisW,&l,&t,&r,&b);
    if (XPLMWindowIsInVR(thisW)){
        XPLMGetWindowGeometryVR(thisW,&w,&h);
        if (w!=width||h!=height) XPLMSetWindowGeometryVR(thisW,width,height);
    }
    else {
       w=r-l;
       h=t-b;
       if (w!=width||h!=height) {
           XPLMSetWindowGeometry(thisW,l,b+height,l+width,b);
           XPLMGetWindowGeometry(thisW,&l,&t,&r,&b);
           w=r-l;
           h=t-b;
       }
    }
    drawp->UpdateDrawPad(w,h,l,b,r,t);
}
