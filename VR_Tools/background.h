#ifndef BACKGROUND_H
#define BACKGROUND_H
#include "globals.h"
#include "drawlogic.h"
#include "point.h"
#include <fstream>
#if IBM
#include <filesystem>
#endif
#if LIN
#include <experimental/filesystem>
#endif

struct textshow{
    string text;
    point pos;
    char color;
    int fontSize;
};

class Background
{
public:
    Background();
    void MakeBackGround(DrawLogic * layoutsDrawPad, XPLMWindowID twd, char ink_color);
    void GenerateMyTexture();
    void LoadTexture();
    void KeepSize();
    void CenterText(textshow &in_line,rectangles posRect);
    void MakeRightList(ulong begin_line, ulong number_of_lines, int start_y, int lim_right);
    void MakeCenteredList(ulong begin_line, ulong number_of_lines, int start_y, int lim_left, int lim_right);
    void MakeLeftList(ulong begin_line, ulong number_of_lines,int pos_up_x,int pos_up_y);
private:
    XPLMWindowID thisW;
    DrawLogic * drawp;
    int width,height;
    bool valid;

    vector<string> instructs;
    vector<rectangles> boxes ;
    vector<textshow> lines;
};

#endif // BACKGROUND_H
