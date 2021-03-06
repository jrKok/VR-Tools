#include "fontman.h"
#include "drawlogic.h"
bool fontMan::FreeTypeStarted(false);
bool fontMan::FreeTypeError(false);
FT_Library fontMan::library(nullptr);
FT_Face    fontMan::face(nullptr);
map<int,charrecord> *fontMan::chars (new map<int,charrecord>);
map<int,charrecord> *fontMan::bigger_chars (new map<int,charrecord>);

fontMan::fontMan() :
    count(0)
{

}

fontMan::~fontMan(){
    if (chars) delete chars;
    if (bigger_chars) delete bigger_chars;
    chars=nullptr;
    bigger_chars=nullptr;
}

void fontMan::Initialise(){
    if (!FreeTypeStarted){
        FreeTypeStarted=true;
        int er=FT_Init_FreeType( &library );
        char sysPath[512];
        XPLMGetSystemPath(sysPath);
        string sysDir=sysPath;//conversion from char[]
        string fontname=sysDir+"Resources/fonts/DejaVuSans.ttf";
        if (er) {DrawLogic::WriteDebug("In DrawLogic StartFreeType Init Free Type gave error ",er);FreeTypeError=true;}
        er= FT_New_Face( library,
                         fontname.c_str(),
                         0,
                         &face );
        if (er) {DrawLogic::WriteDebug("In DrawLogic StartFreeType New Face returned error ",er);FreeTypeError=true;}
        BuildFontMap(11,chars);
        BuildFontMap(14,bigger_chars);

    }
}

void fontMan::BuildFontMap(unsigned int size, map<int, charrecord> * to_map){

    int er= FT_Set_Pixel_Sizes(face,size,size);
    if (er) {DrawLogic::WriteDebug("In DrawLogic StartFreeType Set Pixel Sizes returned error for size : ",er,static_cast<int>(size));FreeTypeError=true;}
    unsigned char pass[3];
    //ASCII codes
    for (unsigned char c(0);c<=0x7F;c++){

        pass[0]=c;
        CharCodeToMap(1,pass,to_map);
    }
    //UTF_8 2 byte codes
    for (unsigned char c1(0xC2);c1<=0xDF;c1++){
        for (unsigned char c(0x80);c<=0xBF;c++){
            pass[1]=c1;
            pass[0]=c;
            CharCodeToMap(2,pass,to_map);
        }
    }
    //UTF-8 3 byte codes
    for (unsigned char c2(0xE0);c2<=0xEF;c2++){
        for (unsigned char c1(0x80);c1<=0xBF;c1++){
            for (unsigned char c(0x80);c<=0xBF;c++)
            {
                pass[2]=c2;
                pass[1]=c1;
                pass[0]=c;
                CharCodeToMap(3,pass,to_map);
            }
        }
    }

}

void fontMan::CharCodeToMap(char cSize, unsigned char in_code[], map<int,charrecord> * to_map){
    FT_ULong charCode(0);
    int charTag(0);
    if (cSize==1) {charCode=in_code[0];charTag=in_code[0];}
    if (cSize==2){
        charTag=in_code[0]+in_code[1]*0x100;
        unsigned char inc=in_code[0];
        charCode=in_code[1]&0x1F;
        charCode=charCode<<8;
        inc=inc<<2;
        charCode=charCode+inc;
        charCode=charCode>>2;
    }
    if (cSize==3){
        charTag=in_code[0]+in_code[1]*0x100+in_code[2]*0x10000;
        unsigned char inc=in_code[0];
        charCode=(in_code[2]&0x0F);
        charCode=charCode<<8;
        unsigned char inter=(in_code[1]<<2);
        charCode=charCode+inter;
        charCode=charCode<<4;
        inc=inc&0x3F;
        charCode=charCode+inc;
    }

    int err=FT_Load_Char(face,charCode,FT_LOAD_RENDER|FT_LOAD_FORCE_AUTOHINT);
    charrecord newChar;
    newChar.height=0;
    newChar.charcode=charCode;
    if (!err&&(charCode>0x20)){
        FT_GlyphSlot glyph(face->glyph);
        newChar.height=glyph->bitmap.rows;
        newChar.width=glyph->bitmap.width;
        ulong size=newChar.height*newChar.width;
        if (size>144)
        { size=144;
        }
        for (ulong tg(0);tg<size;tg++) newChar.bitmap[tg]=glyph->bitmap.buffer[tg];
        newChar.yOffset=(glyph->metrics.horiBearingY>>6)-1;
        newChar.advance=(glyph->metrics.horiAdvance>>6);
    }
    else newChar.bitmap[0]=0;
    to_map->insert(std::make_pair(charTag,newChar));

}

charrecord fontMan::GetCharFromMap(int in_UTF, int &out_width, int &out_height, int &out_offset, int &out_advance, int fontSize){
    map<int,charrecord> * charMap_toSearch;
    if (fontSize==0) {
        charMap_toSearch=chars;
    }
    else{
        charMap_toSearch=bigger_chars;
    }
    charrecord rec;
    rec.height=0;
     rec.width=0;
    out_height=0;
     out_width=0;
    if ((*charMap_toSearch).find(in_UTF)==(*charMap_toSearch).end())
    {
        //DrawLogic::WriteDebug("fontMan GetCharFromMap : didn't find "+std::to_string(in_UTF));
        return rec;
    }
    rec=(*charMap_toSearch)[in_UTF];
     if (in_UTF==0x20) {out_width=4 ;rec.width=4;}
     if (in_UTF==0x09) {out_width=12;rec.width=12;}
      out_width=static_cast<int>(rec.width);
     out_height=static_cast<int>(rec.height);
    out_advance=static_cast<int>(rec.advance);
     out_offset=static_cast<int>(rec.yOffset);
    return rec;
}

int fontMan::MeasureString(const string &in_String, int fontSize){

   int length(0);
   int height(0),width(0),advance(0),offset(0);
   if (in_String.length()==0) return length;
   vInt codes;
   StringToCode(in_String,codes);

   for (auto charInt:codes){


       //decode character
       if (charInt==0x20){
           length+=4;continue;
       }
       if (charInt==9){
           length=NextTab(length);
           continue;
       }
       fontMan::GetCharFromMap(charInt,width,height,offset,advance,fontSize);
       if (height){
           length+=advance;
       }
   }
   return length;
}

int fontMan::GetFontSize(int in_S){
    if (in_S==0) return 11;
    else {
        return 13;
    }
}

void fontMan::GetPositions(const string &in_String, vInt &out_pos, int fontSize){
    out_pos.clear();
    int length(0);
    out_pos.push_back(length);

    int height(0),width(0),advance(0),offset(0);
    if (in_String.length()==0) return;
    vInt codes;
    StringToCode(in_String,codes);
    for (auto charInt:codes){
        //decode character
        if (charInt==0x20){
            length+=4;
            out_pos.push_back(length);
            continue;

        }
        if (charInt==9){
            length=NextTab(length);
            out_pos.push_back(length);
            continue;
        }
        fontMan::GetCharFromMap(charInt,width,height,offset,advance,fontSize);
        if (width){
            length+=advance;
            out_pos.push_back(length);
        }
    }
    string debug("");
    for (auto ps:out_pos){
        debug+=std::to_string(ps)+" ";
    }
    return;

}

int fontMan::GetNumberOfUTFCharsInString(const string &in_String){
    vInt codes;
    StringToCode(in_String,codes);
    return static_cast<int>(codes.size());
}

void fontMan::EndFreeType(){
    if (library!=nullptr){
        FT_Done_Face(face);
        FT_Done_Library(library);
        face=nullptr;
        library=nullptr;
        if (chars) delete chars;
        if (bigger_chars) delete bigger_chars;
        chars=nullptr;
        bigger_chars=nullptr;

}
}

void fontMan::StringToCode(const string &in_String, vInt &out_codes){
    //makes a vector of ints, one int per UTF8 character
    out_codes.clear();

    int charInt(0);
    unsigned char inc(0);
    char cSize(1),it(0);
    for (ulong tg(0);tg<in_String.length();tg++){

        inc=static_cast<unsigned char>(in_String[tg]);
        if (inc>0x7F&&cSize==1){
            if (inc<=0xDF) {
                cSize=2;it=0;charInt=inc*0x100;}
            else
            {cSize=3;it=1;charInt=inc*0x10000;}
            continue;
        } else{
            if (cSize>1){
                if (it){
                    charInt+=inc*0x100;
                    it--;
                    continue;
                } else{
                    charInt+=inc;
                    cSize=1;
                }

            } else{
                charInt=inc;
            }
        }
        if ((charInt==0x09)|(charInt>=0x20)){
            out_codes.push_back(charInt);
        }
    }
}

int fontMan::NextTab(int const in_pos){
    int ext=in_pos+4;//if next tab position is within a space reach, go to next
    int leftTab=ext/12;
    return (leftTab+1)*12;
}
