#ifndef FONTMAN_H
#define FONTMAN_H

#include "globals.h"

class fontMan
{
public:
    fontMan();
       void       Initialise ();
       void       EndFreeType();
       void       CharCodeToMap (char cSize,unsigned char in_code[]);
       void       LeftShift     (charrecord &toworkon);
static charrecord GetCharFromMap(int in_UTF, int &out_width, int &out_height, int &out_offset, int &out_advance);
static int        MeasureString (const string &in_string);
static void       GetPositions  (const string &in_string,vInt &out_pos);

static bool       FreeTypeStarted;
static bool       FreeTypeError;

private:
    static map<int,charrecord> *chars;
    static FT_Library library;
    static FT_Face    face;

};

#endif // FONTMAN_H
