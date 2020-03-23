#include "fontman.h"
#include "drawlogic.h"
bool fontMan::FreeTypeStarted(false);
bool fontMan::FreeTypeError(false);
FT_Library fontMan::library(nullptr);
FT_Face    fontMan::face(nullptr);
map<int,charrecord> *fontMan::chars(new map<int,charrecord>);

fontMan::fontMan()
{

}

void fontMan::Initialise(){
    if (!FreeTypeStarted){
        FreeTypeStarted=true;
        int er=FT_Init_FreeType( &library );
        char sysPath[512];
        XPLMGetSystemPath(sysPath);
        string sysDir=sysPath;//conversion from char[]
        string fontname=sysDir+"Resources\\fonts\\DejaVuSans.ttf";
        if (er) {DrawLogic::WriteDebug("In DrawLogic StartFreeType Init Free Type gave error "+std::to_string(er));FreeTypeError=true;}
        er= FT_New_Face( library,
                         fontname.c_str(),
                         0,
                         &face );
        if (er) {DrawLogic::WriteDebug("In DrawLogic StartFreeType New Face returned error "+std::to_string(er));FreeTypeError=true;}
        //er= FT_Set_Char_Size(face,0,10*64,96,96);
        er= FT_Set_Pixel_Sizes(face,11,11);
        unsigned char pass[3];
        //ASCII codes
        for (unsigned char c(0);c<=0x7F;c++){

            pass[0]=c;
            CharCodeToMap(1,pass);
        }
        //UTF_8 2 byte codes
        for (unsigned char c1(0xC2);c1<=0xDF;c1++){
            for (unsigned char c(0x80);c<=0xBF;c++){
                pass[1]=c1;
                pass[0]=c;
                CharCodeToMap(2,pass);
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
                    CharCodeToMap(3,pass);
                }
            }
        }
    }
}

void fontMan::CharCodeToMap(char cSize, unsigned char in_code[]){
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

    int err=FT_Load_Char(face,charCode,FT_LOAD_RENDER);
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
        LeftShift(newChar);
        newChar.yOffset=(glyph->metrics.horiBearingY>>6)-1;
        newChar.advance=(glyph->metrics.horiAdvance>>6);
    }
    else newChar.bitmap[0]=0;
    chars->insert(std::make_pair(charTag,newChar));

}

charrecord fontMan::GetCharFromMap(int in_UTF, int &out_width, int &out_height, int &out_offset,int &out_advance){
    charrecord rec;
    if ((*chars).find(in_UTF)==(*chars).end())
        DrawLogic::WriteDebug("fontMan GetCharFromMap : didn't find "+std::to_string(in_UTF));
    else {
        out_height=0;
        rec.height=0;

    }
    rec=(*chars)[in_UTF];
    out_width=static_cast<int>(rec.width);
    out_height=static_cast<int>(rec.height);
    out_advance=static_cast<int>(rec.advance);
    out_offset=static_cast<int>(rec.yOffset);
    return rec;
}

void fontMan::LeftShift(charrecord &toworkon){

    bool leftColnotBlack(false);
    unsigned int allPix=toworkon.width*toworkon.height;
    for (unsigned int col(0);col<allPix;col+=toworkon.width){
        leftColnotBlack=leftColnotBlack|toworkon.bitmap[col];
    }

    if (leftColnotBlack) return;
    else
    {
        unsigned char temp[144];
        for (int idx(0);idx<144;idx++) temp[idx]=toworkon.bitmap[idx];
        for (unsigned int row(0);row<toworkon.height;row++){
            for (unsigned int pix(row*toworkon.width);pix<(toworkon.width-1);pix++){
                toworkon.bitmap[pix]=temp[pix+1];
            }
            toworkon.bitmap[row*toworkon.width+toworkon.width-1]=0;
        }
    }
    return;
}

int fontMan::MeasureString(const string &in_String){

   int length(0);
   unsigned char inc(0);
   char cSize(1),it(0);
   int charInt(0),height(0),width(0),advance(0),offset(0);
   if (in_String.length()==0) return length;

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
       //decode character
       if (charInt==0x20){
           length+=4;continue;
       }
       if (charInt==9){
           length+=12;continue;
       }
       fontMan::GetCharFromMap(charInt,width,height,offset,advance);
       if (height){
           length+=height+advance;
       }
   }
   return length;
}

void fontMan::GetPositions(const string &in_String,vInt &out_pos){
    out_pos.clear();
    int length(0);
    out_pos.push_back(length);
    unsigned char inc(0);
    char cSize(1),it(0);
    int charInt(0),height(0),width(0),advance(0),offset(0);
    if (in_String.length()==0) return;

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
        //decode character
        if (charInt==0x20){
            length+=4;
            out_pos.push_back(length);
            continue;

        }
        if (charInt==9){
            length+=12;
            out_pos.push_back(length);
            continue;
        }
        fontMan::GetCharFromMap(charInt,width,height,offset,advance);
        if (height){
            length+=advance;
            out_pos.push_back(length);
        }
    }
    return;

}

void fontMan::EndFreeType(){
    if (library!=nullptr){
            FT_Done_Face(face);
            FT_Done_Library(library);
            face=nullptr;
            library=nullptr;
}
}
