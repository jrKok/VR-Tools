#include "drawlogic.h"
DrawLogic *DrawLogic::myself(nullptr);
bool DrawLogic::FreeTypeStarted(false);
bool DrawLogic::FreeTypeError(false);
FT_Library DrawLogic::library(nullptr);
FT_Face    DrawLogic::face(nullptr);


//Implementation
DrawLogic::DrawLogic():
    rects(new  map<int,rectangles*>),
    strings(new vector<StringToDraw>),
    textureZone(),

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
    StartFreeType();

}


DrawLogic::~DrawLogic(){
    XPLMBindTexture2d(textNum, 0);
    GLuint t = textNum;
    glDeleteTextures(1, &t);
    if (rects!=nullptr) delete rects;
    rects=nullptr;

}

void DrawLogic::StartFreeType(){
    if (!FreeTypeStarted){
        FreeTypeStarted=true;
        int er=FT_Init_FreeType( &library );
        char sysPath[512];
        XPLMGetSystemPath(sysPath);
        string sysDir=sysPath;//conversion from char[]
        string fontname=sysDir+"Resources\\fonts\\DejaVuSans.ttf";
        if (er) {WriteDebug("In DrawLogic StartFreeType Init Free Type gave error "+std::to_string(er));FreeTypeError=true;}
        er= FT_New_Face( library,
                         fontname.c_str(),
                         0,
                         &face );
        if (er) {WriteDebug("In DrawLogic StartFreeType New Face returned error "+std::to_string(er));FreeTypeError=true;}
        //er= FT_Set_Char_Size(face,0,10*64,96,96);
        er= FT_Set_Pixel_Sizes(face,11,11);
    }
}

void DrawLogic::EndFreeType(){
    if (library!=nullptr){
            FT_Done_Face(face);
            FT_Done_Library(library);
            face=nullptr;
            library=nullptr;
}
}
void DrawLogic::WriteDebug(string message){
    string in_String="VR Tools : " +message+"\n";
    XPLMDebugString((char*)in_String.c_str());
}

void DrawLogic::Initiate(){
myself=this;
}

void DrawLogic::ToUpperLevel(){
    myself=this;
}

void DrawLogic::SetNewSize(int in_Width, int in_Height){
    /*if (textNum){
        GLuint tN= static_cast<GLuint>(textNum);
        glBindTexture(tN,0);
        glDeleteTextures(1,&tN);
    }*/
    windowWidth=in_Width;
    windowHeight=in_Height;
    screenR=screenL+windowWidth;
    screenT=screenB+windowHeight;
    XPLMGenerateTextureNumbers(&textNum, 1);
    XPLMBindTexture2d(textNum, 0);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,in_Width,in_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE,&textureZone);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //GenerateCurrentTexture();
}

void  DrawLogic::SetScreenPosition(int in_Top,int in_Left){
    screenT=in_Top;
    screenL=in_Left;
    screenB=screenT-myself->windowHeight;
    screenR=screenL+myself->windowWidth;
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
    textureColor rc;
    rc.red  =globals::colors[in_rect.GetColor()].red;
    rc.green=globals::colors[in_rect.GetColor()].green;
    rc.blue =globals::colors[in_rect.GetColor()].blue;
    rc.alpha=globals::colors[in_rect.GetColor()].alpha;
    int maxX=min(windowWidth,in_rect.GetRight());
    int maxY=min(in_rect.GetTop()*windowWidth,windowHeight*windowWidth);
    for (int line(in_rect.GetBottom()*windowWidth);line<=maxY;line+=windowWidth){
        int limX=line+maxX;
        for (int pos(line+in_rect.GetLeft());pos<=limX;pos++){
            textureZone[pos]=rc;
        }
    }
}

void DrawLogic::WipeRectangle(const rectangles &in_rect){
    textureColor bckgrdcolor;
    bckgrdcolor.red  =globals::colors[Clr_BckgrdW].red;
    bckgrdcolor.green=globals::colors[Clr_BckgrdW].green;
    bckgrdcolor.blue =globals::colors[Clr_BckgrdW].blue;
    bckgrdcolor.alpha=globals::colors[Clr_BckgrdW].alpha;
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
        //experimental Character draw

        if (!FreeTypeError){
            for (auto st:*strings){
                if ((!st.is_deleted)&st.s_visible) {
                    WriteDebug(st.s_String);
                    DrawStringOnTexture(st.s_String,st.s_Color_ident,st.s_location);
                }
            }
        }
    }
}

void DrawLogic::DrawStringOnTexture(string in_String, char in_Color, point start_point){

 ulong debut=static_cast<ulong>(start_point.GetY()*windowWidth);
 ulong xP=static_cast<ulong>(start_point.GetX());

 double bg[3],fg[3];
 FT_GlyphSlot glyph(face->glyph);
 bg[0]=textureZone[debut+xP].red/255.0;
 bg[1]=textureZone[debut+xP].green/255.0;
 bg[2]=textureZone[debut+xP].blue/255.0;

 fg[0]=globals::colors[in_Color].red/255.0;
 fg[1]=globals::colors[in_Color].green/255.0;
 fg[2]=globals::colors[in_Color].blue/255.0;

 unsigned char left(0),inc(0);
 int err(0);
 FT_ULong toPrint(0);
 char cSize(1);
 unsigned char construct[3];
 unsigned char it(0);

 for (ulong tg(0);tg<in_String.length();tg++){

     inc=static_cast<unsigned char>(in_String[tg]);
     if (inc>0x7F&&cSize==1){
         if (inc<=0xDF) {cSize=2;it=1;}
         else {cSize=3;it=2;}
         construct[it]=inc;
         it--;
         continue;
     } else{
         if (cSize>1){
             if (it){
                 construct[it]=inc;
                 it--;
                 continue;
             } else{
                 if (cSize==2){
                     //toPrint=((construct[1]&0x1F)*100+(inc<<2))>>2;
                     toPrint=construct[1]&0x1F;
                     toPrint=toPrint<<8;
                     inc=inc<<2;
                     WriteDebug("inc should be 168"+std::to_string(inc));
                     toPrint=toPrint+inc;
                     toPrint=toPrint>>2;
                     WriteDebug ("toPrint is" +std::to_string(toPrint));
                     WriteDebug("construct1 was "+std::to_string(construct[1]));
                 }
                 if (cSize==3){
                     toPrint=(construct[2]&0x0F);
                     toPrint=toPrint<<8;
                     unsigned char inter=(construct[1]<<2);
                     toPrint=toPrint+inter;
                     toPrint=toPrint<<4;
                     inc=inc&0x3F;
                     toPrint=toPrint+inc;
                 }
                 cSize=1;
             }

         } else{
             toPrint=inc;
         }
     }

         //go try to print
         if (toPrint==0x20){
             xP+=4;continue;
         }
         err=FT_Load_Char(face,toPrint,FT_LOAD_RENDER);
         if (!err){
             ulong lecteur=0;
             debut=static_cast<ulong>((start_point.GetY()+(glyph->metrics.horiBearingY>>6)-1)*windowWidth);
             for (ulong line=debut,ln=0;ln<glyph->bitmap.rows;ln++,line-=static_cast<ulong>(windowWidth)){
                 for (ulong px=line+xP,it(0);it<glyph->bitmap.width;it++,px++ ) {
                         double rap(glyph->bitmap.buffer[lecteur]/255.0);
                         textureZone[px].alpha=255;
                         textureZone[px].red  =static_cast<unsigned char>(255*(fg[0]*rap+bg[0]*(1-rap)));
                         textureZone[px].green=static_cast<unsigned char>(255*(fg[1]*rap+bg[1]*(1-rap)));
                         textureZone[px].blue =static_cast<unsigned char>(255*(fg[2]*rap+bg[2]*(1-rap)));

                       /*textureZone[px].red  =static_cast<unsigned char>(255*pow((1-rap)*pow(bg[0],2.4)+rap*pow(fg[0],2.4),1/2.4));
                         textureZone[px].green=static_cast<unsigned char>(255*pow((1-rap)*pow(bg[1],2.4)+rap*pow(fg[1],2.4),1/2.4));
                         textureZone[px].blue =static_cast<unsigned char>(255*pow((1-rap)*pow(bg[2],2.4)+rap*pow(fg[2],2.4),1/2.4));*/
                         lecteur++;
                     }
             }
             xP+=static_cast<ulong>((glyph->metrics.horiAdvance>>6));
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

int DrawLogic::AddString(const string &in_String, const char in_Color,point where){
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
    myself->DrawStringOnTexture((*(myself->strings))[here].s_String,(*(myself->strings))[here].s_Color_ident,(*(myself->strings))[here].s_location);
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

void DrawLogic::SetScreenOrigin(int in_left, int in_bottom){
    //update own coords
    if (myself->screenL==in_left&&in_bottom==myself->screenB) return;
    myself->screenB=in_bottom;
    myself->screenL=in_left;
    myself->screenR=in_left+myself->windowWidth;
    myself->screenT=in_bottom+myself->windowHeight;
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
