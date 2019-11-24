#ifndef DRAWLOGIC_H
#define DRAWLOGIC_H

#include "rectangles.h"
#include "VR_Tools_global.h"
#include "XPLMDisplay.h"
#include "XPLMGraphics.h"
#include "XPLMDataAccess.h"
#include "XPLMPlugin.h"
#include "XPLMProcessing.h"
#include "XPLMUtilities.h"
#include "XPLMMenus.h"
#include "point.h"
#include <memory>
#include <string>
#include <array>

#if IBM
    #include <windows.h>
#endif

#if LIN
    #include <GL/gl.h>
    #include <cstring>
    #include <cstdint>
#else
    #include <GL/gl.h>
#endif

using std::string;
using std::vector;
using  color=float[3];
using triangle=std::array<int,6>;
using rectangle=std::array<int,4>;//tlrb


struct TriangleToDraw
{
    char tr_Color_ident;
    int tr_identifier;
    //triangle tr_triangle;
    int v1x,v1y,v2x,v2y,v3x,v3y;
    color tr_Color;
    bool tr_visible;
    bool is_deleted;

};

struct StringToDraw
{
    char s_Color_ident;
    int s_identifier;
    point s_location;
    char* s_String;
    color s_Color;
    bool s_visible;
    bool is_deleted;
};

struct Colors
{
    char color_ident;
    color my_color;
};

struct WindowsLayout
{
    int windowNumber;
    vector<TriangleToDraw> *wTriangles;
    vector<StringToDraw> *wStrings;
};


class DrawLogic
{
public:
    DrawLogic();
    ~DrawLogic();
    static void  DefineColors();
    static void  WriteDebug(string message);
    static void  DefineColor(char in_Tab,float in_R, float in_G, float in_B);
    static int   AddRectangle(const rectangles &in_Rect,const char in_Color);
    static void  RectToTriangle(const rectangles &in_Rect, triangle &firstTriangle, triangle &secondTriangle);
    static int   AddTriangle(const triangle &in_Symbol, const char in_Color);
    static void  ChangeColorRect(const int in_Element,const char to_Color);
    static void  ChangeColorTriangle(const int in_Element,const char to_Color);
    static int   AddString(const string &in_String, const char in_Color,point where);
    static void  ChangeString(const int in_Element, const string &to_String);
    static void  ChangeColorString(const int in_Element,const char &to_Color);
    static void  DeleteString(const int in_Element);
    static void  DeleteTriangle (const int in_Element);
    static void  DeleteRect(const int in_Element);
    static void  RelocateRect(const int in_Element, rectangles &in_Rect);
    static void  RelocateTriangle(const int in_Element,const triangle &in_Triangle);
    static void  RelocateString(const int in_Element,const point &to_where);
    static void  SetVisibilityString(const int in_Element,const bool &is_Visible);
    static void  SetVisibilityTriangle(const int in_Element,const bool &is_Visible);
    static void  SetVisibilityRect(const int in_Element,const bool &is_Visible);
    static bool  GetVisibilityString(const int in_Element);
    static bool  GetVisibilityTriangle(const int in_Element);
    static char GetColorCodeTriangle(const int in_Element);
    static char  GetColorCodeString (const int in_Element);
    static triangle GetTriangle(const int in_Element);
    static void  EraseRects();
    static void  EraseStrings();
    static ulong IndexOfTriangleElement(const int &in_Element);
    static ulong IndexOfString(const int &in_Element);
    static void  DrawElements();
    static void  DrawStrings();
    static int   CreateMyWindow ();
    static int   GetActiveWindowNumber();
    static ulong IndexOfMyWindow(int wNumber);
    static void  DeleteMyWindow (int wNumber);
    static void  HideMyWindow   (int wNumber);
    static void  ShowMyWindow   (int wNumber);
    static void  CopyTriangleFromTo(const triangle &from, triangle &to);
    static void  CopyTriangleFromTo(const triangle &from, ulong to_idx);
    static void  CopyColorFromTo(const char &from, color &to);
    static char* ToC(const string &in_String);
    static void  PrintMyStats();
    static void  ShowAllStrings();
    static void  CopyModalTriangle(ulong idx,triangle in_tri);
    static void  PrintModalTriangleCoords(int in_drawN);

static void  EnableModalWindow();
static bool  IsModalWindowActive();
static int   AddModalRect(const rectangles &in_Rect,const char in_Color);
static int   AddModalTriangle(const triangle &in_Symbol, const char in_Color);
static int   AddModalString(const string &in_String, const char in_Color,point where);
static void  ChangeModalString(const int in_Element, const string &to_String);
static void  ChangeColorModalRect(const int in_Element,const char to_Color);
static void  ChangeColorModalString(const int in_Element,const char &to_Color);
static void  RelocateModalRect(const int in_Element, rectangles &in_Rect);
static void  RelocateModalString(const int in_Element,const point &to_where);
static void  SetVisibilityModalRect(const int in_Element,bool in_visibility);
static void  SetVisibilityModalString(const int in_Element,bool in_visibility);
static void  DrawModalElements();
static void  DrawModalStrings();
static void  ResetModalWindow();



private:
    static vector<TriangleToDraw> *triangles;
    static vector<StringToDraw> *strings;
    static vector<Colors> colors;
    static vector<WindowsLayout> myWindows;
    static int currentNumber,currentStringNumber,currentWindowNumber,activeWindow,numberOfDeletedStrings,numberOfDeletedTriangles,stageForRectCreation,lastTriangleCreated;
    static bool hasDeletedStrings,hasDeletedTriangles;
    static vector<TriangleToDraw> *modalTriangles;
    static vector<StringToDraw> *modalStrings;
    static int currentModalTriangle,currentModalString;
    static bool modalWindowActive;

};

#endif // DRAWLOGIC_H
