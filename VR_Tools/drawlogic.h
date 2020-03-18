#ifndef DRAWLOGIC_H
#define DRAWLOGIC_H

/*
 * New Draw Logic :
 * all rectangles (buttons, backgrounds etc) are going to be rendered
 * procedurally in a generated texture. The texture will then be rendered as such
 * in X Plane's custom window.
 *
 * For starters, strings are still rendered by X Plane, but if the price is too heavy
 * I will make rendering strings on the texture possible as well, at first for fixed elements ie the
 * buttons, then eventually the text also (will be more difficult to do)
 *
 * There be far less computations for relocating elements when the window is dragged. However resizing
 * will require a whole redraw of the texture. Some work will have to be done to recode the whole window
 * geometry.
 *
 * Detection in rectangles has also to be rebuilt. Use 2 sets of coordinates in the rectangle class :
 *  -coordinates in the texture
 *  -coordinates on screen
 * Thus mouse click detection is done with screen coordinates only.
 *
 * The texture itself is a one dimensional array of ints. It contains 4*maxWidth*maxHeight chars for rgba
 * encoding of the gl_texture.
 */



#include "XPLMDisplay.h"
#include "XPLMGraphics.h"
#include "XPLMDataAccess.h"
#include "XPLMPlugin.h"
#include "XPLMProcessing.h"
#include "XPLMUtilities.h"
#include "XPLMMenus.h"
#include "point.h"
#include <memory>
#include <globals.h>

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

class DrawLogic
{
public:
    DrawLogic();
    ~DrawLogic();
    static void  WriteDebug(string message);
    void  Initiate();
    void  ToUpperLevel();
    void  SetNewSize(int in_Width, int in_Height);
    void  FillAll(char in_Color);
    void  DrawRectOnTexture(const rectangles &in_rect);
    void  WipeRectangle(const rectangles &in_rect);
    void  GenerateCurrentTexture();
    void  DrawStringOnTexture(string in_String,char in_Color,char bckCol, point start_point);
    void  RelocateStrings();
    static int   AddRectangle(rectangles *in_Rect);
    static void  UpdateRectangle(int tag_Rect);
    static void  ReleaseRectangle(int tag_Rect);
    static void  HideRectangle(int tag_Rect);
    static void  UpdateTexture();
    static int   AddString(const string &in_String, const char in_Color,const char bckgrdcol, point where);
    static void  ChangeString(const int in_Element, const string &to_String);
    static void  ChangeColorString(const int in_Element,const char &to_Color);
    static void  ChangeBckGrdColorString (const int in_Element,const char &to_Color);
    static void  DeleteString(const int in_Element);
    static void  RelocateString(const int in_Element, point &to_where);
    static void  SetVisibilityString(const int in_Element,const bool &is_Visible);
    static bool  GetVisibilityString(const int in_Element);
    static char  GetColorCodeString (const int in_Element);
    static void  PrintString(const int in_Element);
    static void  EnableModalWindow();
    static bool  IsModalWindow();
    static void  DrawContent();
    static void  FlushContent();
    static void  SetScreenOrigin(int in_left,int in_bottom,int in_right,int in_top);
    static void  SetBackGroundColor(char in_Color);

    void  DiagnosisOfRects();
    void  EraseStrings();
    static ulong IndexOfString(const int &in_Element);
    void  DrawElements();
    void  DrawStrings();
    void  SetId(string in_ID);
    string GetId();
    static char* ToC(const string &in_String);
    static void  PrintMyStats();
    static void  ShowAllStrings();

private:
    map<int,rectangles*> *rects;
    vector<StringToDraw> *strings;
    std::array<textureColor,MaxWWidth*MaxWHeight> textureZone;
    char backGroundColor;
    int textNum;
    int currentRectNumber;
    int currentTriangleNumber;
    int currentStringNumber;
    int lastTriangleCreated;
    int lastRectangleCreated;
    int screenL;
    int screenR;
    int screenT;
    int screenB;
    int numberOfDeletedStrings;
    bool hasDeletedStrings;
    bool isModal;
    static DrawLogic *myself;

    int    windowWidth,windowHeight;
    int    currentXWPos,currentYWPos;
    string ident;

};

#endif // DRAWLOGIC_H
