#ifndef GLOBALS_H
#define GLOBALS_H
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_TYPES_H
#include FT_OUTLINE_H
#include FT_RENDER_H
#include <rectangles.h>
#include <map>
#include <string>
#include <vector>
#include <array>
#include <VR_Tools_global.h>
#include <cmath>

/* class for defining global variables
 * to avoid initialisation nightmares
 */

using std::fabs;
using std::map;
using std::string;
using std::vector;
using vString=std::vector<string>;
using triangle=std::array<int,6>;


struct textureColor
{
   unsigned char red;
   unsigned char green;
   unsigned char blue;
   unsigned char alpha;
};

struct charrecord
{
    unsigned char bitmap[144];
    FT_ULong charcode;
    long yOffset;
    unsigned int width;
    unsigned int height;
    unsigned int advance;
};

struct color
{
    unsigned char red;
    unsigned char green;
    unsigned char blue;
    unsigned char alpha;
    float colorf[3];
};

struct fv3
{
  float colorfv[3];
};

struct TriangleToDraw
{
    char tr_Color_ident;
    int  v1x,v1y,v2x,v2y,v3x,v3y;
    bool tr_visible;
    bool is_deleted;

};

struct StringToDraw
{
    char   s_Color_ident;
    int    s_identifier;
    point  s_location;
    int    s_screenX;
    int    s_screenY;
    string s_String;
    float  s_Color[3];
    bool   s_visible;
    bool   is_deleted;
    char   s_BckGrdColor;
};


class globals
{
public:
    globals();

    void  DefineColors();
    void  DefineColor(char in_Tab,float in_R, float in_G, float in_B);
    static textureColor Copy(color in_color);

    static  map<char,color> colors;

};

#endif // GLOBALS_H
