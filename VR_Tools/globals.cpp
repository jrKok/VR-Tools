#include "globals.h"

map<char,color>  globals::colors;

globals::globals()
{

}

void  globals::DefineColors(){

    DefineColor(Clr_LightBlue,0.270f,0.510f,0.885f);
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
    DefineColor(Clr_TextSelectBlue,0.600f,0.960f,1.0f);
}

void  globals::DefineColor(char in_Tab,float in_R, float in_G, float in_B){
    colors[in_Tab].colorf[0]=in_R;
    colors[in_Tab].colorf[1]=in_G;
    colors[in_Tab].colorf[2]=in_B;

    colors[in_Tab].red  =static_cast<unsigned char>(in_R*255);
    colors[in_Tab].green=static_cast<unsigned char>(in_G*255);
    colors[in_Tab].blue =static_cast<unsigned char>(in_B*255);
    colors[in_Tab].alpha=static_cast<unsigned char>(255);
}

textureColor globals::Copy(color in_color){
    textureColor ret;
    ret.red=in_color.red;
    ret.blue=in_color.blue;
    ret.alpha=static_cast<unsigned char>(255);
    return ret;

}
