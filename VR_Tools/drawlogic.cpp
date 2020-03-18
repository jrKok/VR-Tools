#include "drawlogic.h"
#include "fontman.h"

DrawLogic *DrawLogic::myself(nullptr);



//Implementation
DrawLogic::DrawLogic():
    rects(new  map<int,rectangles*>),
    strings(new vector<StringToDraw>),
    textureZone(),
     backGroundColor(Clr_BckgrdW),
     textNum(0),
     currentRectNumber(0),
     currentTriangleNumber(0),
     currentStringNumber(0),
     lastTriangleCreated(0),
     lastRectangleCreated(0),
     screenL(0),
     screenR(0),
     screenT(0),
     screenB(0),
     numberOfDeletedStrings(0),
     hasDeletedStrings(false),
     isModal(false),
    windowWidth(0),
    windowHeight(0),
    currentXWPos(0),
    currentYWPos(0),
    ident("DrawPad")
{
    myself=this;

}


DrawLogic::~DrawLogic(){
    XPLMBindTexture2d(textNum, 0);
    GLuint t = textNum;
    glDeleteTextures(1, &t);
    if (rects!=nullptr) delete rects;
    rects=nullptr;

}

void DrawLogic::SetBackGroundColor(char in_Color){
    myself->backGroundColor=in_Color;
}

void DrawLogic::WriteDebug(string message){
    string in_String="VR Tools : " +message+"\n";
    XPLMDebugString((char*)in_String.c_str());
}

void DrawLogic::Initiate(){
    myself=this;
ulong sze(MaxWWidth*MaxWHeight);
long adress(reinterpret_cast<long>(&textureZone[0])),newAdress(0);
for (ulong atg(1);atg<sze;atg++){
    newAdress=reinterpret_cast<long>(&textureZone[atg]);
  if (newAdress-adress==4){
      adress=newAdress;
  }
  else{
      WriteDebug("DrawLogic : textureZone is not contiguous at iteration "+std::to_string(atg));
      break;
  }
}
}

void DrawLogic::ToUpperLevel(){
    myself=this;
}

void DrawLogic::SetNewSize(int in_Width, int in_Height){
    windowWidth=in_Width;
    windowHeight=in_Height;
    if ((windowWidth*windowHeight)>(MaxWWidth*MaxWHeight)) WriteDebug("DrawLogic Set New Size : Texture size exceeds maximum allowance");
    else
    {
        screenR=screenL+windowWidth;
        screenT=screenB+windowHeight;
        XPLMGenerateTextureNumbers(&textNum, 1);
        XPLMBindTexture2d(textNum, 0);
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,in_Width,in_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE,&textureZone);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }
    //GenerateCurrentTexture();
}

void DrawLogic::DiagnosisOfRects(){
    WriteDebug("diagnosis of rectangles for window"+ident);
    for (const auto &rct: (*rects)){
        WriteDebug("coords of Rect nb : "+std::to_string((*rct.second).GetId())+" - lt bt rg top :"
                   +std::to_string((*rct.second).GetLeft())+" "
                   +std::to_string((*rct.second).GetBottom())+" "
                   +std::to_string((*rct.second).GetRight())+" "
                   +std::to_string((*rct.second).GetTop())+" ");
    }
}

void DrawLogic::FillAll(char in_Color){
    textureColor allCol;
    allCol.red  =globals::colors[in_Color].red;
    allCol.green=globals::colors[in_Color].green;
    allCol.blue =globals::colors[in_Color].blue;
    allCol.alpha=globals::colors[in_Color].alpha;

    for (auto &pixel : myself->textureZone){
        pixel=allCol;
    }

}

void DrawLogic::DrawRectOnTexture(const rectangles &in_rect){
    if (windowWidth&&windowHeight)
    {
        int r(in_rect.GetRight()),l(in_rect.GetLeft());
        int t(in_rect.GetTop()),b(in_rect.GetBottom());
        if (r<0|t<0) return;
        if (l<0) l=0;
        if (b<0) b=0;
        if (t>windowHeight) t=windowHeight;
        if (r>windowWidth) r=windowWidth;
        textureColor rc;
        rc.red  =globals::colors[in_rect.GetColor()].red;
        rc.green=globals::colors[in_rect.GetColor()].green;
        rc.blue =globals::colors[in_rect.GetColor()].blue;
        rc.alpha=globals::colors[in_rect.GetColor()].alpha;
        for (int line(b), idx(b*windowWidth);line<=t;line++,idx+=windowWidth){
            for (int pos(idx+l),wd(l);wd<=r;pos++,wd++){
                textureZone[pos]=rc;
            }
        }
    }
}

void DrawLogic::WipeRectangle(const rectangles &in_rect){
    textureColor bckgrdcolor;
    bckgrdcolor.red  =globals::colors[backGroundColor].red;
    bckgrdcolor.green=globals::colors[backGroundColor].green;
    bckgrdcolor.blue =globals::colors[backGroundColor].blue;
    bckgrdcolor.alpha=255;
    int maxX=min(windowWidth,in_rect.GetRight());
    int maxY=min(in_rect.GetTop()*windowWidth,windowHeight*windowWidth);
    for (int line(in_rect.GetBottom()*windowWidth);line<=maxY;line+=windowWidth){
        int limX=line+maxX;
        for (int pos(line+in_rect.GetLeft());pos<=limX;pos++){
            textureZone[pos]=bckgrdcolor;
        }
    }
}

void DrawLogic::GenerateCurrentTexture(){

    if (windowWidth&&windowHeight)
    {
         FillAll(Clr_BckgrdW);
        for (const auto &rct: (*rects))
        {   rectangles todr=*rct.second;
            if (rct.second->IsVisible()) DrawRectOnTexture(*rct.second);
        }


        if (!fontMan::FreeTypeError){
            for (auto st:*strings){
                if ((!st.is_deleted)&st.s_visible) {
                    DrawStringOnTexture(st.s_String,st.s_Color_ident,st.s_BckGrdColor,st.s_location);
                }
            }
        }
    }
}

void DrawLogic::DrawStringOnTexture(string in_String, char in_Color,char bckCol, point start_point){
    if (start_point.GetX()>0&&start_point.GetY()>0){
        ulong debut=static_cast<ulong>(start_point.GetY()*windowWidth);
        ulong xP=static_cast<ulong>(start_point.GetX());

        double bg[3],fg[3];
        bg[0]=globals::colors[bckCol].red/255.0;
        bg[1]=globals::colors[bckCol].green/255.0;
        bg[2]=globals::colors[bckCol].blue/255.0;

        fg[0]=globals::colors[in_Color].red/255.0;
        fg[1]=globals::colors[in_Color].green/255.0;
        fg[2]=globals::colors[in_Color].blue/255.0;

        unsigned char inc(0);
        char cSize(1),it(0);
        charrecord cBitmap;
        int charInt(0),height(0),width(0),advance(0),offset(0);
        if (in_String.length()==0) return;
        for (ulong tg(0);tg<in_String.length();tg++){

            inc=static_cast<unsigned char>(in_String[tg]);
            if (inc>0x7F&&cSize==1){
                if (inc<=0xDF) {
                    cSize=2;it=0;charInt=inc*0x100;}
                else
                {cSize=3;it=1;charInt=inc*0x10000;}
                continue;
            } else{
                if (cSize>1){
                    if (it){
                        charInt+=inc*0x100;
                        it--;
                        continue;
                    } else{
                        charInt+=inc;
                        cSize=1;
                    }

                } else{
                    charInt=inc;
                }
            }

            //go try to print
            if (charInt==0x20){
                xP+=4;continue;
            }
            cBitmap=fontMan::GetCharFromMap(charInt,width,height,offset,advance);
            if (height&&((xP+advance)<=windowWidth)&&(start_point.GetY()+offset+height)<=windowHeight){
                ulong lecteur=0;
                debut=static_cast<ulong>((start_point.GetY()+offset)*windowWidth);
                for (ulong line=debut,ln=0;ln<height;ln++,line-=static_cast<ulong>(windowWidth)){
                    for (ulong px=line+xP,it(0);it<width;it++,px++ ) {
                        double rap(cBitmap.bitmap[lecteur]/255.0);
                        textureZone[px].alpha=255;
                        textureZone[px].red  =static_cast<unsigned char>(255*(fg[0]*rap+bg[0]*(1-rap)));
                        textureZone[px].green=static_cast<unsigned char>(255*(fg[1]*rap+bg[1]*(1-rap)));
                        textureZone[px].blue =static_cast<unsigned char>(255*(fg[2]*rap+bg[2]*(1-rap)));

                        lecteur++;
                    }
                }
                xP+=static_cast<ulong>((advance));
            }
        }
    }
}

void DrawLogic::UpdateTexture(){
    myself->GenerateCurrentTexture();
}

int DrawLogic::AddRectangle(rectangles *in_Rect){
    myself->lastRectangleCreated++;
    (*(myself->rects))[myself->lastRectangleCreated]=in_Rect;
    return myself->lastRectangleCreated;
}

void DrawLogic::UpdateRectangle(int tag_Rect){
    if (myself->windowWidth&&myself->windowHeight)
    {
        rectangles rectToUpdate=*(*(myself->rects))[tag_Rect];
        if ((*(myself->rects))[tag_Rect]->IsVisible()) myself->DrawRectOnTexture(rectToUpdate);
    }
}

void DrawLogic::ReleaseRectangle(int tag_Rect){
    (*(myself->rects))[tag_Rect]=nullptr;
    (*(myself->rects)).erase(tag_Rect);
}

void DrawLogic::HideRectangle(int tag_Rect){
    if (myself->windowWidth&&myself->windowHeight){
        rectangles rectToUpdate=*(*(myself->rects))[tag_Rect];
        myself->WipeRectangle(rectToUpdate);
    }
}

void DrawLogic::ChangeColorString(const int in_Element,const char &to_Color){
   ulong here=IndexOfString(in_Element);
   if (here<(*(myself->strings)).size()){
       (*(myself->strings))[here].s_Color_ident=to_Color;
       (*(myself->strings))[here].s_Color[0]=globals::colors[to_Color].colorf[0];
       (*(myself->strings))[here].s_Color[1]=globals::colors[to_Color].colorf[1];
       (*(myself->strings))[here].s_Color[2]=globals::colors[to_Color].colorf[2];
   }
}

void  DrawLogic::ChangeBckGrdColorString (const int in_Element,const char &to_Color){
    ulong here=IndexOfString(in_Element);
    if (here<(*(myself->strings)).size()){
        (*(myself->strings))[here].s_BckGrdColor=to_Color;
    }
}

int DrawLogic::AddString(const string &in_String, const char in_Color, const char bckgrdcol, point where){
    //char * fromString=myself->ToC(in_String);
    StringToDraw thisString;
    int retVal=myself->currentStringNumber; ulong siz=myself->strings->size();
    if (myself->hasDeletedStrings){
        for (ulong Idx(0);Idx<siz;Idx++)
        {   if ((*(myself->strings))[Idx].is_deleted)
            {
                retVal=(*(myself->strings))[Idx].s_identifier;
                (*(myself->strings))[Idx].s_String=in_String;
                (*(myself->strings))[Idx].s_Color_ident=in_Color;
                (*(myself->strings))[Idx].s_Color[0]=globals::colors[in_Color].colorf[0];
                (*(myself->strings))[Idx].s_Color[1]=globals::colors[in_Color].colorf[1];
                (*(myself->strings))[Idx].s_Color[2]=globals::colors[in_Color].colorf[2];
                (*(myself->strings))[Idx].s_visible=true;
                (*(myself->strings))[Idx].s_location=where;
                (*(myself->strings))[Idx].is_deleted=false;
                (*(myself->strings))[Idx].s_BckGrdColor=bckgrdcol;
                myself->numberOfDeletedStrings--;
                myself->hasDeletedStrings=myself->numberOfDeletedStrings!=0;
                return retVal;
            }
        }
    }
    else myself->currentStringNumber++;
    thisString.s_identifier=retVal;
    thisString.s_String=in_String;//if doesn't work try strcpy_s
    thisString.s_Color_ident=in_Color;
    thisString.is_deleted=false;
    thisString.s_Color[0]=globals::colors[in_Color].colorf[0];
    thisString.s_Color[1]=globals::colors[in_Color].colorf[1];
    thisString.s_Color[2]=globals::colors[in_Color].colorf[2];
    thisString.s_visible=true;
    thisString.s_location=where;
    myself->strings->push_back(thisString);
    return retVal;
}

void DrawLogic::ChangeString(const int in_Element, const string &to_String){
    ulong here=IndexOfString(in_Element);
    (*(myself->strings))[here].s_String=to_String;
    /*if (here<(*(myself->strings)).size()){
        ulong sz=to_String.size();sz++;
        char * fromString=new char [sz];
         #if IBM
        strncpy_s(fromString,sz,to_String.c_str(),sz);
         #endif
         #if LIN
        strcpy(fromString,to_String.c_str());
         #endif
        (*(myself->strings))[here].s_String=fromString;
    }*/
}

void  DrawLogic::DeleteString(const int in_Element){
    ulong here=IndexOfString(in_Element);
    if (here<(*(myself->strings)).size()){
        (*(myself->strings))[here].is_deleted=true;
        (*(myself->strings))[here].s_visible=false;
        myself->hasDeletedStrings=true;
        myself->numberOfDeletedStrings++;
    }
}

void DrawLogic::RelocateString(const int in_Element, point &to_where){
    ulong here=IndexOfString(in_Element);
    if (here<(*(myself->strings)).size()){
        (*(myself->strings))[here].s_location.SetCoords(to_where.GetX(),to_where.GetY());
        (*(myself->strings))[here].s_screenX=(*(myself->strings))[here].s_location.GetX()+myself->screenL;
        (*(myself->strings))[here].s_screenY=(*(myself->strings))[here].s_location.GetY()+myself->screenB;
    }
}
void DrawLogic::SetVisibilityString(const int in_Element,const bool &is_Visible){
    ulong here=IndexOfString(in_Element);
    if (here<(*(myself->strings)).size()){
        (*(myself->strings))[here].s_visible=is_Visible;
    }
}

bool  DrawLogic::GetVisibilityString(const int in_Element){
    ulong here=IndexOfString(in_Element);
    if (here<(*(myself->strings)).size()){
        return (*(myself->strings))[here].s_visible;
    }
    return false;
}

char  DrawLogic::GetColorCodeString (const int in_Element){
    ulong here=IndexOfString(in_Element);
    if (here<(*(myself->strings)).size()){
        return (*(myself->strings))[here].s_Color_ident;
    }
    return 0;
}

void DrawLogic::EraseStrings(){
    strings->clear();
    currentStringNumber=0;
}

ulong DrawLogic::IndexOfString(const int &in_Element){
    ulong uLimit=(*(myself->strings)).size();
    if (in_Element<0) return (uLimit);
    ulong retVal=static_cast<ulong>(in_Element);
    if ((*(myself->strings))[retVal<=uLimit?retVal:(uLimit-1)].s_identifier!=in_Element)
    {
        if ((*(myself->strings))[retVal].s_identifier<in_Element){
            while((*(myself->strings))[retVal].s_identifier<in_Element&&retVal<uLimit)
                retVal++;
            return retVal;
        }
        else{
            while((*(myself->strings))[retVal].s_identifier>in_Element&&retVal>0)
                retVal--;
            return retVal;
        }
    }
    return retVal;
}

void DrawLogic::PrintString(const int in_Element){
    ulong here=IndexOfString(in_Element);
    myself->DrawStringOnTexture((*(myself->strings))[here].s_String,
                                (*(myself->strings))[here].s_Color_ident,
                                (*(myself->strings))[here].s_BckGrdColor,
                                (*(myself->strings))[here].s_location);
}

void DrawLogic::DrawContent(){
    myself->DrawElements();
    //myself->DrawStrings();
}

void DrawLogic::DrawElements(){
    XPLMBindTexture2d(textNum, 0);

    glTexSubImage2D(GL_TEXTURE_2D,
                    0,  // mipmap level
                    0,  // x-offset
                    0,  // y-offset
                    myself->windowWidth,
                    myself->windowHeight,
                    GL_RGBA,           // color of data we are seding
                    GL_UNSIGNED_BYTE,  // encoding of data we are sending
                    &myself->textureZone);
    XPLMSetGraphicsState(
        0,   // No fog, equivalent to glDisable(GL_FOG);
        1,   // One texture, equivalent to glEnable(GL_TEXTURE_2D);
        0,   // No lighting, equivalent to glDisable(GL_LIGHT0);
        0,   // No alpha testing, e.g glDisable(GL_ALPHA_TEST);
        1,   // Use alpha blending, e.g. glEnable(GL_BLEND);
        0,   // No depth read, e.g. glDisable(GL_DEPTH_TEST);
        0);  // No depth write, e.g. glDepthMask(GL_FALSE);

    glColor3f(1, 1, 1);
    glBegin(GL_QUADS);
        glTexCoord2f(0, 0);
        glVertex2i(screenL, screenB);
        glTexCoord2f(0, 1);
        glVertex2i(screenL, screenT);
        glTexCoord2f(1, 1);
        glVertex2i(screenR, screenT);
        glTexCoord2f(1, 0);
        glVertex2i(screenR, screenB);
    glEnd();
}

void  DrawLogic::DrawStrings(){
    /*for (auto st:*strings){
        if ((!st.is_deleted)&st.s_visible) {
           XPLMDrawString(st.s_Color,st.s_screenX,st.s_screenY,st.s_String,nullptr,xplmFont_Proportional);
        }
    }*/
}

char* DrawLogic::ToC(const string &in_String){
    ulong sz=in_String.size();sz++;
    char * fromString=new char [sz];
    if (in_String=="") {
        *fromString='\0';
        return fromString;
    }
#if IBM
strncpy_s(fromString,sz,in_String.c_str(),sz);
#endif
#if LIN
strcpy(fromString,in_String.c_str());
#endif

    return fromString;
}

void  DrawLogic::PrintMyStats(){
    myself->WriteDebug("DrawLogic Stats");
    myself->WriteDebug("number of strings "+std::to_string(myself->strings->size()));
    myself->WriteDebug("current String Number "+std::to_string(myself->currentStringNumber));
    myself->WriteDebug("number of deleted strings "+std::to_string(myself->numberOfDeletedStrings));
}

void DrawLogic::ShowAllStrings(){
    for (auto str:(*myself->strings)){
        myself->WriteDebug("string N° "+std::to_string(str.s_identifier)+" color N° "+std::to_string(str.s_Color_ident)+(str.is_deleted?" deleted":" non deleted"));
        myself->WriteDebug(str.s_String);
    }
}

void DrawLogic::EnableModalWindow(){
    myself->isModal=true;
}

bool DrawLogic::IsModalWindow(){
    return myself->isModal;
}

void DrawLogic::SetScreenOrigin(int in_left, int in_bottom, int in_right, int in_top){
    //update own coords
    if (myself->screenL==in_left&&in_bottom==myself->screenB) return;
    myself->screenB=in_bottom;
    myself->screenL=in_left;
    myself->screenR=in_right;
    myself->screenT=in_top;
    if (in_right-in_left!=myself->windowWidth) WriteDebug("inconsistency in WindowWidth "+std::to_string(in_right-in_left)+" "+std::to_string(myself->windowWidth));
    if (in_top-in_bottom!=myself->windowHeight) WriteDebug("inconsistency in WindowHeight"+std::to_string(in_top-in_bottom)+" "+std::to_string(myself->windowHeight));
    //compute screen coords for strings

    for (auto &strg: (*myself->strings)){
        strg.s_screenX=strg.s_location.GetX()+myself->screenL;
        strg.s_screenY=strg.s_location.GetY()+myself->screenB;
    }
}

void DrawLogic::FlushContent(){
    myself->rects->clear();
    myself->strings->clear();
    myself->numberOfDeletedStrings=0;
    myself->lastTriangleCreated=0;
    myself->lastRectangleCreated=0;
    myself->hasDeletedStrings=false;
    myself->currentRectNumber=0;
    myself->currentStringNumber=0;
    myself->currentTriangleNumber=0;
}

void DrawLogic::SetId(string in_ID){
    ident=in_ID;
}

string DrawLogic::GetId(){
    return ident;
}
