#include "drawlogic.h"

//Initialisations
vector<TriangleToDraw> *DrawLogic::triangles(nullptr);
 vector<StringToDraw> *DrawLogic::strings(nullptr);
 vector<Colors> DrawLogic::colors;
 vector<WindowsLayout> DrawLogic::myWindows;

 vector<TriangleToDraw> *DrawLogic::modalTriangles(nullptr);
 vector<StringToDraw> *DrawLogic::modalStrings(nullptr);


 int DrawLogic::currentNumber(0);
 int DrawLogic::currentStringNumber(0);
 int DrawLogic::currentWindowNumber(0);
 int DrawLogic::activeWindow(0);
 int DrawLogic::numberOfDeletedStrings(0);
 int DrawLogic::numberOfDeletedTriangles(0);
 int DrawLogic::stageForRectCreation(0);
 int DrawLogic::lastTriangleCreated(0);
 bool DrawLogic::hasDeletedStrings(false);
 bool DrawLogic::hasDeletedTriangles(false);
 int DrawLogic::currentModalTriangle(0);
 int DrawLogic::currentModalString(0);
 bool DrawLogic::modalWindowActive(false);

//Implementation
DrawLogic::DrawLogic()
{
}

DrawLogic::~DrawLogic(){
    for (auto w : myWindows){
        strings=w.wStrings;
        for (auto str : (*strings)) delete (str.s_String);
        delete w.wStrings;
        delete w.wTriangles;}
    if (modalTriangles!=nullptr) delete modalTriangles;
    if (modalStrings!=nullptr) {
        for (auto str:(*modalStrings)) delete str.s_String;
        delete modalStrings;
    }
}

void DrawLogic::WriteDebug(string message){
    string in_String="VR Tools : " +message+"\r\n";
    XPLMDebugString((char*)in_String.c_str());
}

void  DrawLogic::DefineColors(){
    modalTriangles=new vector<TriangleToDraw>;
    modalStrings=new vector<StringToDraw>;
    DefineColor(Clr_LightBlue,0.300f,0.500f,0.875f);
    DefineColor(Clr_SelectBlue,0.550f,0.750f,0.900f);
    DefineColor(Clr_PushedBlue,0.250f,0.400f,0.600f);
    DefineColor(Clr_Cyan,0.0f, 1.0f, 1.0f);
    DefineColor(Clr_White,0.950f,0.950f,0.950f);
    DefineColor(Clr_Black,0.020f,0.020f,0.020f);
    DefineColor(Clr_BlackInk,0.005f,0.005f,0.005f);
    DefineColor(Clr_PaperWhite,0.9922f,0.9608f,0.9098f);
    DefineColor(Clr_PaperDusk,0.9529f,0.7686f,0.4706f);
    DefineColor(Clr_Amber,1.0f,0.65f,0.0f);
    DefineColor(Clr_Red,1.0f,0.0f,0.0f);
    DefineColor(Clr_RedKey,1.0f,0.450f,0.450f);
    DefineColor(Clr_Green,0.1f,0.8f,0.1f);
    DefineColor(Clr_LighterGray,0.725f,0.725f,0.770f);
    DefineColor(Clr_LightGray,0.820f,0.820f,0.820f);
    DefineColor(Clr_Gray,0.375f, 0.375f, 0.368f);
    DefineColor(Clr_DarkGray,0.009f,0.009f,0.009f);
    DefineColor(Clr_Background,0.900f,0.900f,0.850f);
    DefineColor(Clr_BackgroundS,0.720f,0.720f,0.690f);
    DefineColor(Clr_BckgrdW,0.150f,0.180f,0.250f);
    DefineColor(Clr_InkSelect,0.9f,0.05f,0.4f);
}

void  DrawLogic::DefineColor(char in_Tab,float in_R, float in_G, float in_B){
    Colors toSet;
    toSet.color_ident=in_Tab;
    toSet.my_color[0]=in_R;
    toSet.my_color[1]=in_G;
    toSet.my_color[2]=in_B;
    colors.push_back(toSet);

}

int DrawLogic::AddRectangle(const rectangles &in_Rect,const char in_Color){
    triangle fromRect1,fromRect2;
    RectToTriangle(in_Rect,fromRect1,fromRect2);
    stageForRectCreation=1;
    int retVal=AddTriangle(fromRect1,in_Color);
    stageForRectCreation=2;
    AddTriangle(fromRect2,in_Color);
    stageForRectCreation=0;
    return retVal;
}

void DrawLogic::RectToTriangle(const rectangles &in_Rect, triangle &firstTriangle,triangle &secondTriangle) {
    firstTriangle[0]=in_Rect.GetLeft();
    firstTriangle[1]=in_Rect.GetTop();
    firstTriangle[2]=in_Rect.GetRight();
    firstTriangle[3]=in_Rect.GetTop();
    firstTriangle[4]=in_Rect.GetLeft();
    firstTriangle[5]=in_Rect.GetBottom();

    secondTriangle[0]=in_Rect.GetLeft();
    secondTriangle[1]=in_Rect.GetBottom();
    secondTriangle[2]=in_Rect.GetRight();
    secondTriangle[3]=in_Rect.GetTop();
    secondTriangle[4]=in_Rect.GetRight();
    secondTriangle[5]=in_Rect.GetBottom();

}

int DrawLogic::AddTriangle(const triangle &in_Symbol, const char in_Color){
 TriangleToDraw newT;
 int retVal=currentNumber;
 ulong vecSize=triangles->size();

 if (hasDeletedTriangles){
     for (ulong idx(0);idx<triangles->size();idx++){
         bool proceed(false);
         if ((*triangles)[idx].is_deleted){
            if (stageForRectCreation==0) {proceed=true;retVal=(*triangles)[idx].tr_identifier;}
            if (stageForRectCreation==1 && (*triangles)[idx<(vecSize-1)?idx+1:idx].is_deleted)
            {
                proceed=true;
                retVal=(*triangles)[idx].tr_identifier;
                lastTriangleCreated=retVal;
            }
            if (stageForRectCreation==2 && (*triangles)[idx].tr_identifier==lastTriangleCreated+1)
               {retVal=lastTriangleCreated++;proceed=true;}
            if (proceed){
                CopyTriangleFromTo(in_Symbol,idx);
                (*triangles)[idx].tr_Color_ident=in_Color;
                CopyColorFromTo(in_Color,(*triangles)[idx].tr_Color);
                (*triangles)[idx].is_deleted=false;
                numberOfDeletedTriangles--;
                hasDeletedTriangles=numberOfDeletedTriangles!=0;
                return retVal;}
         }
     }
 }
 currentNumber++;
 newT.tr_identifier=retVal;
 newT.tr_Color_ident=in_Color;
 CopyColorFromTo(in_Color,newT.tr_Color);
 newT.is_deleted=false;
 triangles->push_back(newT);
 CopyTriangleFromTo(in_Symbol,static_cast<ulong>(retVal));
 return retVal;
}

void DrawLogic::ChangeColorRect(const int in_Element,const char to_Color){
   ChangeColorTriangle(in_Element,to_Color);
   ChangeColorTriangle(in_Element+1,to_Color);
}

void DrawLogic::ChangeColorTriangle(const int in_Element,const char to_Color){
   ulong here=IndexOfTriangleElement(in_Element);
   if(here<(*triangles).size()){
       (*triangles)[here].tr_Color_ident=to_Color;
       CopyColorFromTo(to_Color,(*triangles)[here].tr_Color);
   }
}

void DrawLogic::ChangeColorString(const int in_Element,const char &to_Color){
   ulong here=IndexOfString(in_Element);
   if (here<(*strings).size()){
       (*strings)[here].s_Color_ident=to_Color;
       CopyColorFromTo(to_Color,(*strings)[here].s_Color);
   }
}

int DrawLogic::AddString(const string &in_String, const char in_Color,point where){
    char * fromString=ToC(in_String);
    StringToDraw thisString;
    int retVal=currentStringNumber; ulong siz=strings->size();
    if (hasDeletedStrings){
        for (ulong Idx(0);Idx<siz;Idx++)
        {   if ((*strings)[Idx].is_deleted)
            {
                retVal=(*strings)[Idx].s_identifier;
                (*strings)[Idx].s_String=fromString;
                (*strings)[Idx].s_Color_ident=in_Color;
                CopyColorFromTo(in_Color,(*strings)[Idx].s_Color);
                (*strings)[Idx].s_visible=true;
                (*strings)[Idx].s_location=where;
                (*strings)[Idx].is_deleted=false;
                numberOfDeletedStrings--;
                hasDeletedStrings=numberOfDeletedStrings!=0;
                return retVal;
            }
        }
    }
    else currentStringNumber++;
    thisString.s_identifier=retVal;
    thisString.s_String=fromString;//if doesn't work try strcpy_s
    thisString.s_Color_ident=in_Color;
    thisString.is_deleted=false;
    CopyColorFromTo(in_Color,thisString.s_Color);
    thisString.s_visible=true;
    thisString.s_location=where;
    strings->push_back(thisString);
    return retVal;
}

void DrawLogic::ChangeString(const int in_Element, const string &to_String){
    ulong here=IndexOfString(in_Element);
    if (here<(*strings).size()){
        ulong sz=to_String.size();sz++;
        char * fromString=new char [sz];
        strncpy_s(fromString,sz,to_String.c_str(),sz);
        (*strings)[here].s_String=fromString;
    }
}

void  DrawLogic::DeleteString(const int in_Element){
    ulong here=IndexOfString(in_Element);
    if (here<(*strings).size()){
        (*strings)[here].is_deleted=true;
        (*strings)[here].s_visible=false;
        hasDeletedStrings=true;
        numberOfDeletedStrings++;
    }
}

void DrawLogic::DeleteTriangle(const int in_Element){
    ulong here=IndexOfTriangleElement(in_Element);
    if(here<(*triangles).size()){
        (*triangles)[here].is_deleted=true;
        (*triangles)[here].tr_visible=false;
        hasDeletedTriangles=true;
        numberOfDeletedTriangles++;
    }
}

void DrawLogic::DeleteRect(const int in_Element){
    DeleteTriangle(in_Element);
    DeleteTriangle(in_Element+1);
    hasDeletedTriangles=true;
    numberOfDeletedTriangles+=2;
}

void DrawLogic::RelocateRect(const int in_Element,rectangles &in_Rect){
   ulong here=IndexOfTriangleElement(in_Element);
   if(here<(*triangles).size()){
       triangle t1,t2;
       //RectToTriangle(in_Rect,(*triangles)[here].tr_triangle,(*triangles)[here+1].tr_triangle);
       RectToTriangle(in_Rect,t1,t2);
       CopyTriangleFromTo(t1,here);
       CopyTriangleFromTo(t2,here+1);
   }
}

void DrawLogic::RelocateTriangle(const int in_Element,const triangle &in_Triangle){
   ulong here=IndexOfTriangleElement(in_Element);
   if(here<(*triangles).size()){
       CopyTriangleFromTo(in_Triangle,here);
   }
}

void DrawLogic::RelocateString(const int in_Element,const point &to_where){
    ulong here=IndexOfString(in_Element);
    if (here<(*strings).size()){
        (*strings)[here].s_location.myX=to_where.myX;
        (*strings)[here].s_location.myY=to_where.myY;
    }
}
void DrawLogic::SetVisibilityString(const int in_Element,const bool &is_Visible){
    ulong here=IndexOfString(in_Element);
    if (here<(*strings).size()){
        (*strings)[here].s_visible=is_Visible;
    }
}

void DrawLogic::SetVisibilityTriangle(const int in_Element,const bool &is_Visible){
    ulong here=IndexOfTriangleElement(in_Element);
    if(here<(*triangles).size()){
        (*triangles)[here].tr_visible=is_Visible;
    }
}

void DrawLogic::SetVisibilityRect(const int in_Element,const bool &is_Visible){
    SetVisibilityTriangle(in_Element,is_Visible);
    SetVisibilityTriangle(in_Element+1,is_Visible);
}

bool  DrawLogic::GetVisibilityTriangle(const int in_Element){
    ulong here=IndexOfTriangleElement(in_Element);
    if (here<(*triangles).size()) return (*triangles)[here].tr_visible;
}
bool  DrawLogic::GetVisibilityString(const int in_Element){
    ulong here=IndexOfString(in_Element);
    if (here<(*strings).size()){
        return (*strings)[here].s_visible;
    }
    return false;
}

char  DrawLogic::GetColorCodeTriangle(const int in_Element){
    ulong here=IndexOfTriangleElement(in_Element);
    if (here<(*triangles).size()){
        return (*triangles)[here].tr_Color_ident;
    }
    return 0;
}

char  DrawLogic::GetColorCodeString (const int in_Element){
    ulong here=IndexOfString(in_Element);
    if (here<(*strings).size()){
        return (*strings)[here].s_Color_ident;
    }
    return 0;
}

void DrawLogic::EraseRects(){
 triangles->clear();
 currentNumber=0;
}

void DrawLogic::EraseStrings(){
    strings->clear();
    currentStringNumber=0;
}

ulong DrawLogic::IndexOfTriangleElement(const int &in_Element){
    ulong uLimit=(*triangles).size();
    if ((in_Element)<0) return uLimit;
    ulong retVal=static_cast<ulong>(in_Element);
    if ((*triangles)[retVal<=uLimit?retVal:(uLimit-1)].tr_identifier!=in_Element)
    {
        if ((*triangles)[retVal].tr_identifier<in_Element){
            while((*triangles)[retVal].tr_identifier<in_Element&&retVal<(*triangles).size())
                retVal++;
            return retVal;
        }
        else{
            while((*triangles)[retVal].tr_identifier>in_Element&&retVal>0)
                retVal--;
            return retVal;
        }
    }
    return retVal;
}

ulong DrawLogic::IndexOfString(const int &in_Element){
    ulong uLimit=(*strings).size();
    if (in_Element<0) return (uLimit);
    ulong retVal=static_cast<ulong>(in_Element);
    if ((*strings)[retVal<=uLimit?retVal:(uLimit-1)].s_identifier!=in_Element)
    {
        if ((*strings)[retVal].s_identifier<in_Element){
            while((*strings)[retVal].s_identifier<in_Element&&retVal<uLimit)
                retVal++;
            return retVal;
        }
        else{
            while((*strings)[retVal].s_identifier>in_Element&&retVal>0)
                retVal--;
            return retVal;
        }
    }
    return retVal;
}

void DrawLogic::DrawElements(){
    XPLMSetGraphicsState(
        0 /* no fog */,
        0 /* 0 texture units */,
        0 /* no lighting */,
        0 /* no alpha testing */,
        0 /* do alpha blend */,
        1 /* do depth testing */,
        0 /* no depth writing */
        );

    //glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    char previousC=0;
    glBegin(GL_TRIANGLES);
    for (auto tr:*triangles){
        if ((!tr.is_deleted)&tr.tr_visible){
            if (tr.tr_Color_ident!=previousC)
            {
                previousC=tr.tr_Color_ident;
                glColor3fv(tr.tr_Color);
            }
            //glVertex2i(tr.tr_triangle[0],tr.tr_triangle[1]);
            //glVertex2i(tr.tr_triangle[2],tr.tr_triangle[3]);
            //glVertex2i(tr.tr_triangle[4],tr.tr_triangle[5]);
            glVertex2i(tr.v1x,tr.v1y);
            glVertex2i(tr.v2x,tr.v2y);
            glVertex2i(tr.v3x,tr.v3y);
        }
    }
    glEnd();

}

void  DrawLogic::DrawStrings(){
    for (auto st:*strings){
        if ((!st.is_deleted)&st.s_visible) {
           XPLMDrawString(st.s_Color,st.s_location.myX,st.s_location.myY,st.s_String,nullptr,xplmFont_Proportional);
        }
    }
}

int   DrawLogic::CreateMyWindow(){
    int retVal=currentWindowNumber;
    currentWindowNumber++;
    WindowsLayout wdw;
    wdw.windowNumber=retVal;
    wdw.wStrings=new (vector<StringToDraw>);
    wdw.wTriangles=new(vector<TriangleToDraw>);

    strings=wdw.wStrings;
    triangles=wdw.wTriangles;
    activeWindow=retVal;
    myWindows.push_back(wdw);

    currentNumber=0;
    currentStringNumber=0;
    numberOfDeletedStrings=0;
    hasDeletedTriangles=false;
    hasDeletedStrings=false;
    numberOfDeletedTriangles=0;
    return retVal;
}

int   DrawLogic::GetActiveWindowNumber(){
    return activeWindow;
}

ulong DrawLogic::IndexOfMyWindow(int wNumber){
    ulong retVal(0);
    while (myWindows[retVal].windowNumber!=wNumber&&retVal<myWindows.size())
        retVal++;
    return retVal;
}

void  DrawLogic::DeleteMyWindow(int wNumber){
    ulong uIdx=IndexOfMyWindow(wNumber);
    int idx=static_cast<int>(uIdx);
    if (uIdx<myWindows.size()){
        WindowsLayout wdw;
        wdw.windowNumber=myWindows[uIdx].windowNumber;
        wdw.wStrings=myWindows[uIdx].wStrings;
        wdw.wTriangles=myWindows[uIdx].wTriangles;

        myWindows[uIdx].wStrings=nullptr;
        myWindows[uIdx].wTriangles=nullptr;

        delete(wdw.wTriangles);
        delete(wdw.wStrings);
        wdw.wStrings=nullptr;
        wdw.wTriangles=nullptr;

        myWindows.erase(myWindows.begin()+idx);
    }
}

void  DrawLogic::HideMyWindow  (int wNumber){}

void  DrawLogic::ShowMyWindow  (int wNumber){
    if (activeWindow!=wNumber){
        ulong uIdx=IndexOfMyWindow(wNumber);
        if (uIdx<myWindows.size()){
            activeWindow=myWindows[uIdx].windowNumber;
            strings=myWindows[uIdx].wStrings;
            triangles=myWindows[uIdx].wTriangles;
            currentStringNumber=static_cast<int>(strings->size());
            currentNumber=static_cast<int>(triangles->size());
            hasDeletedStrings=false;
            numberOfDeletedStrings=0;
            for (auto st:(*strings)){
                if (st.is_deleted) {
                    hasDeletedStrings=true;
                    numberOfDeletedStrings++;
                }
            }
            hasDeletedTriangles=false;
            numberOfDeletedTriangles=0;
            for (auto tr:(*triangles)) {
                if (tr.is_deleted){
                    hasDeletedTriangles=true;
                    numberOfDeletedTriangles++;
                }
            }

        }
        else WriteDebug("inconsistency in internal window management (Draw Logic class)");
    }
}

void DrawLogic::CopyTriangleFromTo(const triangle &from, triangle &to){
    to[0]=from[0];
    to[1]=from[1];
    to[2]=from[2];
    to[3]=from[3];
    to[4]=from[4];
    to[5]=from[5];
}

void  DrawLogic::CopyTriangleFromTo(const triangle &from,ulong to_idx){
    ulong uidx=static_cast<ulong>(to_idx);
    (*triangles)[uidx].v1x=from[0];
    (*triangles)[uidx].v1y=from[1];
    (*triangles)[uidx].v2x=from[2];
    (*triangles)[uidx].v2y=from[3];
    (*triangles)[uidx].v3x=from[4];
    (*triangles)[uidx].v3y=from[5];
}

void DrawLogic::CopyColorFromTo(const char &from, color &to){
    ulong idx=static_cast<ulong>(from);
    to[0]=colors[idx].my_color[0];
    to[1]=colors[idx].my_color[1];
    to[2]=colors[idx].my_color[2];
}

triangle DrawLogic::GetTriangle(const int in_Element){
    ulong here=IndexOfTriangleElement(in_Element);
    triangle ret;
    ret[0]=(*triangles)[here].v1x;
    ret[1]=(*triangles)[here].v1y;
    ret[2]=(*triangles)[here].v2x;
    ret[3]=(*triangles)[here].v2y;
    ret[4]=(*triangles)[here].v3x;
    ret[5]=(*triangles)[here].v3y;
    return ret;
}

char* DrawLogic::ToC(const string &in_String){
    ulong sz=in_String.size();sz++;
    char * fromString=new char [sz];
    if (in_String=="") {
        *fromString='\0';
        return fromString;
    }
    strncpy_s(fromString,sz,in_String.c_str(),sz);
    return fromString;
}

void  DrawLogic::PrintMyStats(){
    WriteDebug("DrawLogic Stats");
    WriteDebug("window number "+std::to_string(activeWindow));
    WriteDebug("number of windows logged "+std::to_string(myWindows.size()));
    WriteDebug("next Window number "+std::to_string(currentWindowNumber));
    WriteDebug("number of triangles "+std::to_string(triangles->size()));
    WriteDebug("current Number "+std::to_string(currentNumber));
    WriteDebug("number of strings "+std::to_string(strings->size()));
    WriteDebug("current String Number "+std::to_string(currentStringNumber));
    WriteDebug("number of Deleted Triangles "+std::to_string(numberOfDeletedTriangles));
    WriteDebug("number of deleted strings "+std::to_string(numberOfDeletedStrings));
}

void DrawLogic::PrintModalTriangleCoords(int in_drawN){
    ulong uidx=static_cast<ulong>(in_drawN);
    if ((*modalTriangles)[uidx].tr_visible) WriteDebug("triangle N° "+std::to_string(in_drawN)+" is Visible");
    else {
        WriteDebug("triangle N° "+std::to_string(in_drawN)+" is inVisible");
    }
    WriteDebug("coords vertex 1 x y "+std::to_string((*modalTriangles)[uidx].v1x)+" "+std::to_string((*modalTriangles)[uidx].v1y));
    WriteDebug("coords vertex 2 x y "+std::to_string((*modalTriangles)[uidx].v2x)+" "+std::to_string((*modalTriangles)[uidx].v2y));
    WriteDebug("coords vertex 3 x y "+std::to_string((*modalTriangles)[uidx].v3x)+" "+std::to_string((*modalTriangles)[uidx].v3y));
    WriteDebug("its color code is  "+std::to_string((*modalTriangles)[uidx].tr_Color_ident));

}

void DrawLogic::ShowAllStrings(){
    for (auto str:(*strings)){
        WriteDebug("string N° "+std::to_string(str.s_identifier)+" color N° "+std::to_string(str.s_Color_ident)+(str.is_deleted?" deleted":" non deleted"));
        XPLMDebugString(str.s_String);WriteDebug("");
    }
}



void DrawLogic::EnableModalWindow(){
modalWindowActive=true;
if (modalTriangles==nullptr) modalTriangles=new vector<TriangleToDraw>;
if (modalStrings==nullptr) modalStrings=new vector<StringToDraw>;
}

bool  DrawLogic::IsModalWindowActive(){
    return modalWindowActive;
}

void DrawLogic::CopyModalTriangle(ulong idx,triangle in_tri){
    (*modalTriangles)[idx].v1x=in_tri[0];
    (*modalTriangles)[idx].v1y=in_tri[1];
    (*modalTriangles)[idx].v2x=in_tri[2];
    (*modalTriangles)[idx].v2y=in_tri[3];
    (*modalTriangles)[idx].v3x=in_tri[4];
    (*modalTriangles)[idx].v3y=in_tri[5];
}

int DrawLogic::AddModalRect(const rectangles &in_Rect,const char in_Color){
    triangle fromRect1,fromRect2;
    RectToTriangle(in_Rect,fromRect1,fromRect2);
    int retVal=AddModalTriangle(fromRect1,in_Color);
    AddModalTriangle(fromRect2,in_Color);
    return retVal;
}

int DrawLogic::AddModalTriangle(const triangle &in_Symbol, const char in_Color){
    TriangleToDraw newT;
    int retVal=currentModalTriangle;
    currentModalTriangle++;
    newT.tr_identifier=retVal;
    newT.tr_Color_ident=in_Color;
    newT.tr_visible=true;
    CopyColorFromTo(in_Color,newT.tr_Color);
    newT.is_deleted=false;
    newT.v1x=in_Symbol[0];
    newT.v1y=in_Symbol[1];
    newT.v2x=in_Symbol[2];
    newT.v2y=in_Symbol[3];
    newT.v3x=in_Symbol[4];
    newT.v3y=in_Symbol[5];
    modalTriangles->push_back(newT);
    CopyModalTriangle(static_cast<ulong>(retVal),in_Symbol);
    return retVal;
}

int DrawLogic::AddModalString(const string &in_String, const char in_Color,point where){
    char * fromString=ToC(in_String);
    StringToDraw thisString;
    int retVal=currentModalString;
    currentModalString++;
    thisString.s_identifier=retVal;
    thisString.s_String=fromString;//if doesn't work try strcpy_s
    thisString.s_Color_ident=in_Color;
    thisString.is_deleted=false;
    CopyColorFromTo(in_Color,thisString.s_Color);
    thisString.s_visible=true;
    thisString.s_location=where;
    modalStrings->push_back(thisString);
    return retVal;}

void DrawLogic::ChangeModalString(const int in_Element, const string &to_String){
    ulong here=static_cast<ulong>(in_Element);
    if (here<modalStrings->size()){
        ulong sz=to_String.size();sz++;
        char * fromString=new char [sz];
        strncpy_s(fromString,sz,to_String.c_str(),sz);
        (*modalStrings)[here].s_String=fromString;
    }
}
void DrawLogic::ChangeColorModalRect(const int in_Element,const char to_Color){
    ulong here=static_cast<ulong>(in_Element);
    if(here<(*modalTriangles).size()){
        (*modalTriangles)[here].tr_Color_ident=to_Color;
        CopyColorFromTo(to_Color,(*modalTriangles)[here].tr_Color);
        (*modalTriangles)[here+1].tr_Color_ident=to_Color;
        CopyColorFromTo(to_Color,(*modalTriangles)[here+1].tr_Color);
    }
}

void DrawLogic::ChangeColorModalString(const int in_Element,const char &to_Color){
    ulong here=static_cast<ulong>(in_Element);
    if (here<(*modalStrings).size()){
        (*modalStrings)[here].s_Color_ident=to_Color;
        CopyColorFromTo(to_Color,(*modalStrings)[here].s_Color);
    }
}

void DrawLogic::RelocateModalRect(const int in_Element, rectangles &in_Rect){
    ulong here=static_cast<ulong>(in_Element);
    if(here<(*modalTriangles).size()){
        triangle t1,t2;
        RectToTriangle(in_Rect,t1,t2);
        CopyModalTriangle(here,t1);
        CopyModalTriangle(here+1,t2);
    }
}
void  DrawLogic::SetVisibilityModalRect(const int in_Element,bool in_visibility){
    ulong here=static_cast<ulong>(in_Element);
    if(here<(*modalTriangles).size())
    {(*modalTriangles)[here].tr_visible=in_visibility;
     (*modalTriangles)[here+1].tr_visible=in_visibility;
    }

}

void  DrawLogic::SetVisibilityModalString(const int in_Element,bool in_visibility){
    ulong here=static_cast<ulong>(in_Element);
    if (here<(*modalStrings).size())(*modalStrings)[here].s_visible=in_visibility;

}

void DrawLogic::RelocateModalString(const int in_Element,const point &to_where){
    ulong here=static_cast<ulong>(in_Element);
    if (here<(*modalStrings).size()){
        (*modalStrings)[here].s_location.myX=to_where.myX;
        (*modalStrings)[here].s_location.myY=to_where.myY;
    }
}
void DrawLogic::DrawModalElements(){
    XPLMSetGraphicsState(
        0 /* no fog */,
        0 /* 0 texture units */,
        0 /* no lighting */,
        0 /* no alpha testing */,
        0 /* do alpha blend */,
        1 /* do depth testing */,
        0 /* no depth writing */
        );

    //glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    char previousC=0;
    glBegin(GL_TRIANGLES);
    for (auto tr:*modalTriangles){
        if (!tr.is_deleted&&tr.tr_visible){
            if (tr.tr_Color_ident!=previousC)
            {
                previousC=tr.tr_Color_ident;
                glColor3fv(colors[static_cast<ulong>(previousC)].my_color);
            }
            //glVertex2i(tr.tr_triangle[0],tr.tr_triangle[1]);
            //glVertex2i(tr.tr_triangle[2],tr.tr_triangle[3]);
            //glVertex2i(tr.tr_triangle[4],tr.tr_triangle[5]);
            glVertex2i(tr.v1x,tr.v1y);
            glVertex2i(tr.v2x,tr.v2y);
            glVertex2i(tr.v3x,tr.v3y);
        }
    }
    glEnd();

}
void DrawLogic::DrawModalStrings(){
    for (auto st:*modalStrings){
        if ((!st.is_deleted)&st.s_visible) {
            XPLMDrawString(colors[static_cast<ulong>(st.s_Color_ident)].my_color,st.s_location.myX,st.s_location.myY,st.s_String,nullptr,xplmFont_Proportional);
        }
    }
}
void DrawLogic::ResetModalWindow(){
    for (auto str:(*modalStrings)) delete str.s_String;
    modalStrings->clear();
    modalTriangles->clear();
    currentModalString=0;
    currentModalTriangle=0;
    modalWindowActive=false;
    rectangles rec;
    AddModalRect(rec,Clr_Black);
}
