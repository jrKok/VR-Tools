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
 * encoding of the gl_texture. The one dimensional character ensures contiguity of bytes in array, at least
 * it will be easier to check.
 *
 * Rendering will be done in modern openGL or direct GL if something is incompatible with modern
 *
 * Classes making windows for Drawlogic :
 * - ShowDir
 * - Layout
 * - subclass LayoutWithEdit
 * - manageModalWindows (see manageModalWindows for list of classes using that one)
 *
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
    #include <Psapi.h>
#endif

#if LIN
    #include <GL/glew.h>
    #include <cstring>
    #include <cstdint>
#else
    #include <glew.h>
    //#include <gl/GL.h>
#endif

class DrawLogic
{
public:
    DrawLogic();
    ~DrawLogic();
    //Helpers
    static void  WriteDebug(string message);
    static void  WriteDebug(string message,int i1);
    static void  WriteDebug(string message,int i1,int i2);
    static void  WriteDebug(string message,int i1,int i2,int i3);
    static void  WriteDebug(string message,int i1,int i2,int i3,int i4);

    //Initializers
       static bool  MakeGLContext();
 static DrawLogic*  GetCurrentPad();
              void  Initiate();
              void  ToUpperLevel();
              void  SetNewSize(int in_Width, int in_Height);
              void  SetWindowHandle(XPLMWindowID in_WH);
              void  CloseWindow();

    //Texture Functions
       void  FillAll(char in_Color);
static void  UpdateTexture();
       void  GenerateCurrentTexture(); //Draws all elements at once

       void  DrawBox(int in_l,int in_b,int in_r,int in_t,char in_color);
static void  PrintRectOnTexture(int in_l,int in_b,int in_r,int in_t,char in_color); //static function to call the next
       void  DrawRectOnTexture(int in_l,int in_b,int in_r,int in_t,char in_color);
       void  WipeRectangle(const rectangles * const in_rect);

static void  PrintString(const int in_Element);
static void  PrintStringOnLocalT(const int in_Element);
       void  DrawStringOnTexture(string in_String,char in_Color,char bckCol, point start_point, int fontSize=0);
       void  DrawStringOnLocalTexture(string in_String, char in_Color, point start_point, int fontSize=0);
       void  RelocateStrings();


    //Rectangle vector management
    ulong          GetNewRectangleNumber();
    static ulong   AddRectangle(rectangles *in_Rect);
    static void    AddAsFirstRectangle(rectangles *in_Rect);
    static void    UpdateRectangle(ulong tag_Rect);
    static void    ReleaseRectangle(ulong tag_Rect);
    static void    HideRectangle(ulong tag_Rect);
    static void    SwapRectangle(ulong tag_Rect1, ulong tag_Rect2);

    //string vector management
    static int   AddString(const string &in_String, const char in_Color,const char bckgrdcol, point where,int fontsize=0);
    void         EraseStrings();
    static ulong IndexOfString(const int &in_Element);
    static void  ChangeString(const int in_Element, const string &to_String);
    static void  ChangeFontSize(const int in_Element, int new_size);
    static void  ChangeColorString(const int in_Element,const char &to_Color);
    static void  ChangeBckGrdColorString (const int in_Element,const char &to_Color);
    static void  DeleteString(const int in_Element);
    static void  RelocateString(const int in_Element, point &to_where);
    static void  SetVisibilityString(const int in_Element,const bool &is_Visible);
    static bool  GetVisibilityString(const int in_Element);
    static char  GetColorCodeString (const int in_Element);

    //Cursor Management
    static void  SetCursorPosition(int x, int y);
    static void  RenderCursor();

    // Rendering the texture from the GPU to the screen
    static void  RenderContent();
    void         RenderElements();
    void         RenderElementsDirectGL();
    void         DrawStrings();
    void         SetId(string in_ID);
    string       GetId();
   static string GetSId();

   //Adjustements, Data coherence, synchronisations
    static char* ToC(const string &in_String);
    static void  FlushContent();
    static void  SetBackGroundColor(char in_Color);
    void UpdateDrawPad(int wW, int wH, int nleft, int nbottom, int nright, int ntop);//routine for updating texture to GPU, updating geometry

    //Debugging functions
           void  checkCompileErrors(unsigned int shader, std::string type);//to check for compiler errors
    static void  StringAssesment();
           void  PrintStringAssessment();
           void  PrintRectanglesAssessment();
    static void  PrintMyStats();
    static void  ShowAllStrings();
            int  GetWidth();
            int  GetHeight();
    static bool  VerifyPointer(ulong tag, rectangles *in_rect);
    static void  PrintCurrentIdent();
    static void  PrintMemoryStats();

private:
    static bool         verbose;
    static unsigned int shaderProgram,VAO,VBO,EBO;
    map<ulong,rectangles*> *rects;
    vector<StringToDraw> *strings;
    std::array<textureColor,MaxWWidth*MaxWHeight> textureZone;
    float vertexAttribs[16];
    char backGroundColor;
    int textNum;
    int currentRectNumber;
    int currentTriangleNumber;
    int currentStringNumber;
    int lastTriangleCreated;
    int screenL;
    int screenR;
    int screenT;
    int screenB;
    int numberOfDeletedStrings;
    bool hasDeletedStrings,hasDeletedRectangles,hasToUpdate;
    XPLMWindowID myWindow;
    static DrawLogic *myself;

    int    windowWidth,windowHeight;
    string ident;
    int    cursorX,cursorY;
    float  black[3];
    char   cursor[4];

};

#endif // DRAWLOGIC_H
