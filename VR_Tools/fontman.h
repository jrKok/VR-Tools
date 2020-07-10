#ifndef FONTMAN_H
#define FONTMAN_H
/*
 * This class stores bitmaps for the first FFFF UTF8 characters (1, 2, 3, byte encoded UTF8 characters).
 * UTF8 is the base text class for X Plane.
 * The bitmaps are generated by freetype and pretreated to center or left-adjust the maps generated and store all
 * the relevant parameters for final rendering.
 *
 * The final rendering is done by the class "drawlogic" which is informed on which texture to render the string.
 * Since the char bitmaps are gray-scaled, a simple algorithm is used for final rendering (without gamma correction
 * until now, but gamma-correction is already coded but has more overhead and isn't well optimized yet as it
 * also uses alpha parameters).
 *
 * This class can also convert strings to a vector of U-Codes speeding up the rendering of those strings
 * Those vectors also enable quick measuring of the rendered length of a string, the number of rendered characters.
 *
 */

#include "globals.h"
#include <ft2build.h>
#include <memory>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_TYPES_H
#include FT_OUTLINE_H
#include FT_RENDER_H

struct charrecord
{
    unsigned char bitmap[144];
    FT_ULong charcode;
    long yOffset;
    unsigned int width;
    unsigned int height;
    unsigned int advance;
};

class fontMan
{
public:
    fontMan();
    ~fontMan();

    void          Initialise ();
    void          BuildFontMap(unsigned int size, map<int,charrecord> * to_map);
    void          EndFreeType();
    void          CharCodeToMap (char cSize,unsigned char in_code[], map<int,charrecord> * to_map);
static charrecord GetCharFromMap(int in_UTF, int &out_width, int &out_height, int &out_offset, int &out_advance,int fontSize=0);
static int        MeasureString (const string &in_string, int fontSize=0);
static void       GetPositions  (const string &in_string,vInt &out_pos,int fontSize=0);
static void       StringToCode  (const string &in_String, vInt &out_codes);
static int        GetNumberOfUTFCharsInString(const string &in_string);
static int        GetFontSize(int in_S);
static int        NextTab(const int in_pos);


static bool       FreeTypeStarted;
static bool       FreeTypeError;

private:
static map<int,charrecord> *chars;
static map<int,charrecord> *bigger_chars;
    static FT_Library library;
    static FT_Face    face;
           int      count;

};

#endif // FONTMAN_H
