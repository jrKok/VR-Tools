                                                                                                                  #include "drawlogic.h"
#include "fontman.h"

//Static variables initialisations
bool         DrawLogic::verbose(false);
DrawLogic   *DrawLogic::myself(nullptr);
unsigned int DrawLogic::shaderProgram(0);
unsigned int DrawLogic::VAO(0);
unsigned int DrawLogic::VBO(0);
unsigned int DrawLogic::EBO(0);

//Implementation

DrawLogic::DrawLogic():
    rects(new map<ulong,rectangles*>),
    strings(new vector<StringToDraw>),
    textureZone(),
     backGroundColor(Clr_BckgrdW),
     currentRectNumber(0),
     currentTriangleNumber(0),
     currentStringNumber(0),
     lastTriangleCreated(0),
     screenL(0),
     screenR(0),
     screenT(0),
     screenB(0),
     numberOfDeletedStrings(0),
     hasDeletedStrings(false),
     hasDeletedRectangles(false),
     hasToUpdate(true),
     myWindow(nullptr),
     windowWidth(0),
     windowHeight(0),
     ident("DrawPad"),
     black{0.0,0.0,0.0},
     cursor{'\xE2','\x88','\xA3','\0'}

{
    myself=this;
    XPLMGenerateTextureNumbers(&textNum, 1);

}

DrawLogic::~DrawLogic(){
    XPLMBindTexture2d(textNum, 0);
    GLuint t = static_cast<GLuint>(textNum);
    glDeleteTextures(1, &t);
    for (auto &rct:(*rects)) rct.second->UnAttach();
    if (rects!=nullptr) delete rects;
    if (strings) delete strings;
    rects=nullptr;
    strings=nullptr;
    if (VAO){
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }
}

DrawLogic* DrawLogic::GetCurrentPad(){
    return myself;
}

void DrawLogic::SetBackGroundColor(char in_Color){
    myself->backGroundColor=in_Color;
}

void DrawLogic::WriteDebug(string message){
    if (verbose){
        string in_String="VR Tools : " +message+"\n";
        XPLMDebugString(ToC(in_String));
    }
}

void DrawLogic::WriteDebug(string message,int i1){
    WriteDebug(message+std::to_string(i1));
}
void DrawLogic::WriteDebug(string message,int i1,int i2){
    WriteDebug(message+std::to_string(i1)+" "+std::to_string(i2));
}
void DrawLogic::WriteDebug(string message,int i1,int i2,int i3){
    WriteDebug(message+std::to_string(i1)+" "+std::to_string(i2)+" "+std::to_string(i3));
}
void DrawLogic::WriteDebug(string message,int i1,int i2,int i3,int i4){
    WriteDebug(message+std::to_string(i1)+" "+std::to_string(i2)+" "+std::to_string(i3)+" "+std::to_string(i4));
}

bool DrawLogic::MakeGLContext(){
   glewInit();

//shader sources
   const char *vertexShaderSource = "#version 330 core\n"
       "layout (location = 0) in vec2 aPos;\n"
       "layout (location = 1) in vec2 aTexCoord;\n\n"
       "out vec2 TexCoord;\n"
       "void main()\n"
       "{\n"
       "   gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);\n"
       "   TexCoord = vec2(aTexCoord.x,aTexCoord.y);\n"
       "}\0";

   const char *fragmentShaderSource = "#version 330 core\n" //
       "out uvec4 FragColor;\n"
       "in vec2 TexCoord;\n"
       "uniform usampler2D texture1;\n"
       "void main()\n"
       "{\n"
       "   FragColor = texture(texture1, TexCoord);\n"
       "}\n\0";

   // vertex shader compile
   unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
   glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
   glCompileShader(vertexShader);
   //myself->checkCompileErrors(vertexShader, "VERTEX");

   //fragment shader compile
   unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
   glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
   glCompileShader(fragmentShader);
   //myself->checkCompileErrors(fragmentShader, "FRAGMENT");

  // merge shaders in Program
   shaderProgram = glCreateProgram();
   glAttachShader(shaderProgram, vertexShader);
   glAttachShader(shaderProgram, fragmentShader);
   glLinkProgram(shaderProgram);
  // myself-> checkCompileErrors(shaderProgram, "PROGRAM");
   WriteDebug("vertex and fragment shaders compiled");//Let the world know that 2 more shaders are now on the GPU

// Make space
   glDeleteShader(vertexShader);
   glDeleteShader(fragmentShader);

//generate Vertex data
   float vertices[] = {
                 1.0f,  1.0f, 1.0f, 1.0f,  // right top
                 1.0f, -1.0f, 1.0f, 0.0f,  // right bottom
                -1.0f, -1.0f, 0.0f, 0.0f,  // left bottom
                -1.0f,  1.0f, 0.0f, 1.0f  // left top
   };
   unsigned int indices[] = {
                  0, 1, 3,  // first Triangle
                  1, 2, 3   // second Triangle
    };

   glGenVertexArrays(1, &VAO);
   glGenBuffers(1, &VBO);
   glGenBuffers(1, &EBO);

   glBindVertexArray(VAO);

   glBindBuffer(GL_ARRAY_BUFFER, VBO);
   glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

   glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)nullptr);
   glEnableVertexAttribArray(0);

   glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
   glEnableVertexAttribArray(1);
   //Prime uniform for use of texture unit 0
   glUseProgram(shaderProgram);
   glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);

   //hopefully this it not needed but don't want anybody to mess with my VAO
   glBindBuffer(GL_ARRAY_BUFFER, 0);
   glBindVertexArray(0);

   return true; //If things get more complex I'll check for errors and return as required
}

void DrawLogic::checkCompileErrors(unsigned int shader, std::string type)
{
    int success;
    char infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
            string ilog=infoLog;
            WriteDebug("ERROR::SHADER_COMPILATION_ERROR of type: "+ilog+" "+type);        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
            string ilog=infoLog;
            WriteDebug("ERROR::PROGRAM_LINKING_ERROR of type: "+ilog+" "+ type);
        }
    }
}

void DrawLogic::Initiate(){
    myself=this;
    if (windowWidth&&windowHeight){
        XPLMBindTexture2d(textNum, 0);
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,windowWidth,windowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE,&textureZone);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }
}

void DrawLogic::SetWindowHandle(XPLMWindowID in_WH){
    myWindow=in_WH;
}

void DrawLogic::CloseWindow(){
  myWindow=nullptr;
}

void DrawLogic::ToUpperLevel(){
   if (myself!=this) Initiate();
}

void DrawLogic::SetNewSize(int in_Width, int in_Height){
    windowWidth=in_Width; if (windowWidth>MaxWWidth) windowWidth=MaxWWidth;
    windowHeight=in_Height; if (windowHeight>MaxWHeight) windowHeight=MaxWHeight;
    screenR=screenL+windowWidth;
    screenT=screenB+windowHeight;
    Initiate();
}

void DrawLogic::FillAll(char in_Color){
    textureColor allCol;
    allCol.red  =globals::colors[in_Color].red;
    allCol.green=globals::colors[in_Color].green;
    allCol.blue =globals::colors[in_Color].blue;
    allCol.alpha=globals::colors[in_Color].alpha;

    for (auto &pixel : myself->textureZone){
        pixel=allCol;
    }
}

void DrawLogic::PrintRectOnTexture(int in_l, int in_b, int in_r, int in_t, char in_color){
    myself->DrawRectOnTexture(in_l, in_b, in_r, in_t,in_color);
}

void DrawLogic::DrawBox(int in_l, int in_b, int in_r, int in_t, char in_color){
    if (windowWidth&&windowHeight)
    {
        ulong ul=static_cast<ulong>(in_l);
        ulong ur=static_cast<ulong>(in_r);

        textureColor rc;
        rc.red  =globals::colors[in_color].red;
        rc.green=globals::colors[in_color].green;
        rc.blue =globals::colors[in_color].blue;
        rc.alpha=globals::colors[in_color].alpha;
        for (int line(in_b), idx(in_b*windowWidth);line<in_t;line++,idx+=windowWidth){
            if ((line==in_b)||(line==in_t-1)){
                for (ulong pos(static_cast<ulong>(idx)+ul),wd(ul);wd<ur;pos++,wd++){
                    textureZone[pos]=rc;
                }
            }
            else{
                ulong uidx=static_cast<ulong>(idx);
                textureZone[uidx+ul]=rc;
                textureZone[uidx+ur-1]=rc;
            }
        }
    }
}

void DrawLogic::DrawRectOnTexture(int in_l, int in_b, int in_r, int in_t, char in_color){
    if (windowWidth&&windowHeight)
    {
        hasToUpdate=true;

        if (in_r<0||in_t<0) return;
        if (in_l<0) in_l=0;
        if (in_b<0) in_b=0;
        if (in_t>windowHeight) in_t=windowHeight;
        if (in_r>windowWidth) in_r=windowWidth;
        ulong ul=static_cast<ulong>(in_l);
        ulong ur=static_cast<ulong>(in_r);

        textureColor rc;
        rc.red  =globals::colors[in_color].red;
        rc.green=globals::colors[in_color].green;
        rc.blue =globals::colors[in_color].blue;
        rc.alpha=globals::colors[in_color].alpha;
        for (int line(in_b), idx(in_b*windowWidth);line<in_t;line++,idx+=windowWidth){
            for (ulong pos(static_cast<ulong>(idx)+ul),wd(ul);wd<ur;pos++,wd++){
                textureZone[pos]=rc;
            }
        }
    }
}

void DrawLogic::UpdateRectangle(ulong tag_Rect){
    if (myself->windowWidth&&myself->windowHeight)
    {
        if ((*(myself->rects))[tag_Rect]->IsVisible())
            myself->DrawRectOnTexture((*myself->rects)[tag_Rect]->GetLeft(),
                                      (*myself->rects)[tag_Rect]->GetBottom(),
                                      (*myself->rects)[tag_Rect]->GetRight(),
                                      (*myself->rects)[tag_Rect]->GetTop(),
                                      (*myself->rects)[tag_Rect]->GetColor());
    }
}

void DrawLogic::WipeRectangle(rectangles const * const in_rect){
    hasToUpdate=true;
    DrawRectOnTexture(in_rect->GetLeft(),
                      in_rect->GetBottom(),
                      in_rect->GetRight(),
                      in_rect->GetTop(),
                      backGroundColor);
}

void DrawLogic::GenerateCurrentTexture(){
    if (windowWidth&&windowHeight)
    {
         FillAll(backGroundColor);
        for (const auto &rct: (*rects))
        {
            if ((*(rct.second)).IsVisible())
                DrawRectOnTexture((*(rct.second)).GetLeft(),
                                  (*(rct.second)).GetBottom(),
                                  (*(rct.second)).GetRight(),
                                  (*(rct.second)).GetTop(),
                                  (*(rct.second)).GetColor());
        }

        if (!fontMan::FreeTypeError){
            for (auto st:*strings){
                if ((!st.is_deleted)&st.s_visible) {
                    DrawStringOnTexture(st.s_String,st.s_Color_ident,st.s_BckGrdColor,st.s_location,st.s_fontSize);
                }
            }
        }
    }
}

void DrawLogic::DrawStringOnTexture(string in_String, char in_Color,char bckCol, point start_point,int fontSize){
    //Draws a string on a general background
    if (windowWidth&&windowHeight){
        if (start_point.GetX()>0&&start_point.GetY()>0){
            hasToUpdate=true;
            ulong debut=static_cast<ulong>(start_point.GetY()*windowWidth);
            ulong xP=static_cast<ulong>(start_point.GetX());

            double bg[3],fg[3];
            bg[0]=globals::colors[bckCol].red/255.0;
            bg[1]=globals::colors[bckCol].green/255.0;
            bg[2]=globals::colors[bckCol].blue/255.0;

            fg[0]=globals::colors[in_Color].red/255.0;
            fg[1]=globals::colors[in_Color].green/255.0;
            fg[2]=globals::colors[in_Color].blue/255.0;

            charrecord cBitmap;
            int height(0),width(0),advance(0),offset(0);
            if (in_String.length()==0) return;
            vInt to_Draw;
            fontMan::StringToCode(in_String,to_Draw);
            ulong leftTabOffset=xP%12;
            for (auto charInt: to_Draw){
                if (charInt==0x20){
                    xP+=4;
                    continue;
                }
                if (charInt==0x09){
                    xP=static_cast<ulong>(fontMan::NextTab(static_cast<int>(xP)))+leftTabOffset;
                    continue;
                }
                cBitmap=fontMan::GetCharFromMap(charInt,width,height,offset,advance,fontSize);

                if (height&&((xP+advance)<=windowWidth)&&(start_point.GetY()+offset)<=windowHeight){
                    ulong lecteur(0);
                    debut=static_cast<ulong>((start_point.GetY()+offset)*windowWidth);
                             //draw text from top to bottom      |  |
                    for (ulong line=debut,ln=0;ln<height;ln++,line-=static_cast<ulong>(windowWidth)){
                        for (ulong px=line+xP,it(0);it<width;it++,px++ ) {
                            double rap(cBitmap.bitmap[lecteur]/255.0);
                            textureZone[px].alpha=255;
                            textureZone[px].red  =static_cast<unsigned char>(255*(fg[0]*rap+bg[0]*(1-rap)));
                            textureZone[px].green=static_cast<unsigned char>(255*(fg[1]*rap+bg[1]*(1-rap)));
                            textureZone[px].blue =static_cast<unsigned char>(255*(fg[2]*rap+bg[2]*(1-rap)));
                            lecteur++;
                        }
                    }
                    xP+=static_cast<ulong>((advance));
                }
            }
        }
    }
}

void DrawLogic::DrawStringOnLocalTexture(string in_String, char in_Color, point start_point, int fontSize){
    //Draws a string, reading the texture color before drawing, doesn't work if background wasn't wiped
    if (windowWidth&&windowHeight){
        if (start_point.GetX()>0&&start_point.GetY()>0){
            hasToUpdate=true;
            ulong debut=static_cast<ulong>(start_point.GetY()*windowWidth);
            ulong xP=static_cast<ulong>(start_point.GetX());
            double bg[3],fg[3];

            fg[0]=globals::colors[in_Color].red/255.0;
            fg[1]=globals::colors[in_Color].green/255.0;
            fg[2]=globals::colors[in_Color].blue/255.0;

            charrecord cBitmap;
            int height(0),width(0),advance(0),offset(0);
            if (in_String.length()==0) return;
            vInt to_Draw;
            fontMan::StringToCode(in_String,to_Draw);
            ulong leftTabOffset=xP%12;
            for (auto charInt: to_Draw){
                //go try to print
                if (charInt==0x20){
                    xP+=4;
                    continue;
                }
                if (charInt==0x09){
                    xP=static_cast<ulong>(fontMan::NextTab(static_cast<int>(xP)))+leftTabOffset;
                    continue;
                }
                cBitmap=fontMan::GetCharFromMap(charInt,width,height,offset,advance,fontSize);
                if (height&&((xP+advance)<=windowWidth)&&(start_point.GetY()+offset+height)<=windowHeight){
                    ulong lecteur=0;
                    debut=static_cast<ulong>((start_point.GetY()+offset)*windowWidth);

                    bg[0]=textureZone[debut+xP+1].red/255.0;
                    bg[1]=textureZone[debut+xP+1].green/255.0;
                    bg[2]=textureZone[debut+xP+1].blue/255.0;
                    for (ulong line=debut,ln=0;ln<height;ln++,line-=static_cast<ulong>(windowWidth)){
                        for (ulong px=line+xP,it(0);it<width;it++,px++ ) {
                            double rap(cBitmap.bitmap[lecteur]/255.0);
                            textureZone[px].alpha=255;
                            textureZone[px].red  =static_cast<unsigned char>(255*(fg[0]*rap+bg[0]*(1-rap)));
                            textureZone[px].green=static_cast<unsigned char>(255*(fg[1]*rap+bg[1]*(1-rap)));
                            textureZone[px].blue =static_cast<unsigned char>(255*(fg[2]*rap+bg[2]*(1-rap)));
                            lecteur++;
                        }
                    }
                    xP+=static_cast<ulong>((advance));
                }
            }
        }
    }
}

void DrawLogic::UpdateTexture(){
    myself->GenerateCurrentTexture();
}

ulong DrawLogic::AddRectangle(rectangles *in_Rect){
    ulong retval=myself->GetNewRectangleNumber();
    (*myself->rects)[retval]=in_Rect;
    return retval;
}

void DrawLogic::AddAsFirstRectangle(rectangles *in_Rect){
    if ((*myself->rects).size()){
        ulong nNumber=myself->GetNewRectangleNumber();
        for (ulong it(nNumber);it>0;it--){
            (*myself->rects)[it]=(*myself->rects)[it-1];
            (*myself->rects)[it]->SetNewDrawNumber(it);
        }
    }
    (*myself->rects)[0]=in_Rect;
    in_Rect->SetNewDrawNumber(0);
}

ulong DrawLogic::GetNewRectangleNumber(){
    ulong nbrects=rects->size();
    if (hasDeletedRectangles){
        for (ulong it(0);it<nbrects;it++){
            if (rects->find(it)==rects->end()) return it;
        }
        hasDeletedRectangles=false;
        return nbrects;
    }else {
    return nbrects;
    }
}

void DrawLogic::ReleaseRectangle(ulong tag_Rect){
    if ((*myself->rects).size()>0){
        if ((*myself->rects).find(tag_Rect)!=(*myself->rects).end()){
            (*myself->rects).erase(tag_Rect);
            myself->hasDeletedRectangles=true;
        }
    }
}

void DrawLogic::HideRectangle(ulong tag_Rect){
    if (myself->windowWidth&&myself->windowHeight){
        rectangles * rectToUpdate=(*myself->rects)[tag_Rect];
        myself->WipeRectangle(rectToUpdate);
    }
}

void DrawLogic::SwapRectangle(ulong tag_Rect1, ulong tag_Rect2){
    rectangles *swaprect;
    if ((*myself->rects).find(tag_Rect1)!=(*myself->rects).end()&&
            (*myself->rects).find(tag_Rect2)!=(*myself->rects).end()){
        swaprect=(*(myself->rects))[tag_Rect1];
        (*(myself->rects))[tag_Rect1]=(*(myself->rects))[tag_Rect2];
        (*(myself->rects))[tag_Rect2]=swaprect;
    }
}


int DrawLogic::AddString(const string &in_String, const char in_Color, const char bckgrdcol, point where,int fontsize){
    //char * fromString=myself->ToC(in_String);
    StringToDraw thisString;
    int retVal=myself->currentStringNumber; ulong siz=myself->strings->size();
    if (myself->hasDeletedStrings){
        for (ulong Idx(0);Idx<siz;Idx++)
        {   if ((*(myself->strings))[Idx].is_deleted)
            {
                retVal=(*(myself->strings))[Idx].s_identifier;
                (*(myself->strings))[Idx].s_String=in_String;
                (*(myself->strings))[Idx].s_Color_ident=in_Color;
                (*(myself->strings))[Idx].s_Color[0]=globals::colors[in_Color].colorf[0];
                (*(myself->strings))[Idx].s_Color[1]=globals::colors[in_Color].colorf[1];
                (*(myself->strings))[Idx].s_Color[2]=globals::colors[in_Color].colorf[2];
                (*(myself->strings))[Idx].s_visible=true;
                (*(myself->strings))[Idx].s_location=where;
                (*(myself->strings))[Idx].is_deleted=false;
                (*(myself->strings))[Idx].s_BckGrdColor=bckgrdcol;
                (*(myself->strings))[Idx].s_fontSize=fontsize;
                myself->numberOfDeletedStrings--;
                myself->hasDeletedStrings=myself->numberOfDeletedStrings!=0;
                return retVal;
            }
        }
    }
    else myself->currentStringNumber++;
    thisString.s_identifier=retVal;
    thisString.s_String=in_String;//if doesn't work try strcpy_s
    thisString.s_Color_ident=in_Color;
    thisString.is_deleted=false;
    thisString.s_Color[0]=globals::colors[in_Color].colorf[0];
    thisString.s_Color[1]=globals::colors[in_Color].colorf[1];
    thisString.s_Color[2]=globals::colors[in_Color].colorf[2];
    thisString.s_BckGrdColor=bckgrdcol;
    thisString.s_visible=true;
    thisString.s_location=where;
    thisString.s_fontSize=fontsize;
    myself->strings->push_back(thisString);
    return retVal;
}

void DrawLogic::ChangeString(const int in_Element, const string &to_String){
    ulong here=IndexOfString(in_Element);
    (*(myself->strings))[here].s_String=to_String;
}

void DrawLogic::ChangeColorString(const int in_Element,const char &to_Color){
   ulong here=IndexOfString(in_Element);
   if (here<(*(myself->strings)).size()){
       (*(myself->strings))[here].s_Color_ident=to_Color;
       (*(myself->strings))[here].s_Color[0]=globals::colors[to_Color].colorf[0];
       (*(myself->strings))[here].s_Color[1]=globals::colors[to_Color].colorf[1];
       (*(myself->strings))[here].s_Color[2]=globals::colors[to_Color].colorf[2];
   }
}

void  DrawLogic::ChangeBckGrdColorString (const int in_Element,const char &to_Color){
    ulong here=IndexOfString(in_Element);
    if (here<(*(myself->strings)).size()){
        (*(myself->strings))[here].s_BckGrdColor=to_Color;
    }
}

void DrawLogic::ChangeFontSize(const int in_Element, int new_size){
    ulong here=IndexOfString(in_Element);
        if (here<(*(myself->strings)).size()){
           (*(myself->strings))[here].s_fontSize=new_size;
           }
}

void  DrawLogic::DeleteString(const int in_Element){
    ulong here=IndexOfString(in_Element);
    if (here<(*(myself->strings)).size()){
        (*(myself->strings))[here].is_deleted=true;
        (*(myself->strings))[here].s_visible=false;
        myself->hasDeletedStrings=true;
        myself->numberOfDeletedStrings++;
    }
}

void DrawLogic::RelocateString(const int in_Element, point &to_where){
    ulong here=IndexOfString(in_Element);
    if (here<(*(myself->strings)).size()){
        (*(myself->strings))[here].s_location.SetCoords(to_where.GetX(),to_where.GetY());
        (*(myself->strings))[here].s_screenX=(*(myself->strings))[here].s_location.GetX()+myself->screenL;
        (*(myself->strings))[here].s_screenY=(*(myself->strings))[here].s_location.GetY()+myself->screenB;
    }
}
void DrawLogic::SetVisibilityString(const int in_Element,const bool &is_Visible){
    ulong here=IndexOfString(in_Element);
    if (here<(*(myself->strings)).size()){
        (*(myself->strings))[here].s_visible=is_Visible;
    }
}

bool  DrawLogic::GetVisibilityString(const int in_Element){
    ulong here=IndexOfString(in_Element);
    if (here<(*(myself->strings)).size()){
        return (*(myself->strings))[here].s_visible;
    }
    return false;
}

char  DrawLogic::GetColorCodeString (const int in_Element){
    ulong here=IndexOfString(in_Element);
    if (here<(*(myself->strings)).size()){
        return (*(myself->strings))[here].s_Color_ident;
    }
    return 0;
}

void DrawLogic::EraseStrings(){
    strings->clear();
    currentStringNumber=0;
}

ulong DrawLogic::IndexOfString(const int &in_Element){
    ulong uLimit=(*(myself->strings)).size();
    if (in_Element<0) return (uLimit);
    ulong retVal=static_cast<ulong>(in_Element);
    if ((*(myself->strings))[retVal<=uLimit?retVal:(uLimit-1)].s_identifier!=in_Element)
    {
        if ((*(myself->strings))[retVal].s_identifier<in_Element){
            while((*(myself->strings))[retVal].s_identifier<in_Element&&retVal<uLimit)
                retVal++;
            return retVal;
        }
        else{
            while((*(myself->strings))[retVal].s_identifier>in_Element&&retVal>0)
                retVal--;
            return retVal;
        }
    }
    return retVal;
}

void DrawLogic::PrintString(const int in_Element){
    ulong here=IndexOfString(in_Element);
    myself->DrawStringOnTexture((*(myself->strings))[here].s_String,
                                (*(myself->strings))[here].s_Color_ident,
                                (*(myself->strings))[here].s_BckGrdColor,
                                (*(myself->strings))[here].s_location,
                                (*(myself->strings))[here].s_fontSize);
}

void DrawLogic::PrintStringOnLocalT(const int in_Element){
    ulong here=IndexOfString(in_Element);
    myself->DrawStringOnLocalTexture((*(myself->strings))[here].s_String,
                                     (*(myself->strings))[here].s_Color_ident,
                                     (*(myself->strings))[here].s_location,
                                     (*(myself->strings))[here].s_fontSize);
}

void DrawLogic::RenderContent(){

    myself->RenderElements();
}

void DrawLogic::RenderElementsDirectGL(){

    XPLMBindTexture2d(textNum, 0);
        if (hasToUpdate){
            glTexSubImage2D(GL_TEXTURE_2D,
                        0,  // mipmap level
                        0,  // x-offset
                        0,  // y-offset
                        windowWidth,
                        windowHeight,
                        GL_RGBA,           // color bytes of data we are sending
                        GL_UNSIGNED_BYTE,  // encoding of data we are sending
                        &textureZone);
            hasToUpdate=false;
        }
        XPLMSetGraphicsState(
            0,   // No fog, equivalent to glDisable(GL_FOG);
            1,   // One texture, equivalent to glEnable(GL_TEXTURE_2D);
            0,   // No lighting, equivalent to glDisable(GL_LIGHT0);
            0,   // No alpha testing, e.g glDisable(GL_ALPHA_TEST);
            1,   // Use alpha blending, e.g. glEnable(GL_BLEND);
            1,   // No depth read, e.g. glDisable(GL_DEPTH_TEST);
            0);  // No depth write, e.g. glDepthMask(GL_FALSE);

        //Direct rendering with screen coords, works in VR
        glColor3f(1, 1, 1);
        glBegin(GL_QUADS);
            glTexCoord2f(0, 0);
            glVertex2i(screenL, screenB);
            glTexCoord2f(0, 1);
            glVertex2i(screenL, screenT);
            glTexCoord2f(1, 1);
            glVertex2i(screenR, screenT);
            glTexCoord2f(1, 0);
            glVertex2i(screenR, screenB);
        glEnd();

}

void DrawLogic::RenderElements(){

    int wW(0),wH(0);/*screenL(0),screenR(0),screenT(0),screenB(0);
     XPLMGetWindowGeometry(myself->myWindow,&screenL,&screenT,&screenR,&screenB);*/

        if (XPLMWindowIsInVR(myself->myWindow)){
            XPLMGetWindowGeometryVR(myself->myWindow,&wW,&wH);
            if (wW!=windowWidth||wH!=windowHeight){
                windowWidth=wW;
                windowHeight=wH;
            }
        }
        else {
            XPLMGetWindowGeometry(myself->myWindow,&screenL,&screenT,&screenR,&screenB);
           wW=screenR-screenL;
           wH=screenT-screenB;
           glPushAttrib(GL_VIEWPORT_BIT);
           glViewport(screenL,screenB,wW,wH);
        }

        XPLMSetGraphicsState(
            0,   // No fog, equivalent to glDisable(GL_FOG);
            0,   // No texture
            0,   // No lighting, equivalent to glDisable(GL_LIGHT0);
            0,   // No alpha testing, e.g glDisable(GL_ALPHA_TEST);
            1,   // Use alpha blending, e.g. glEnable(GL_BLEND);
            1,   // No depth read, e.g. glDisable(GL_DEPTH_TEST);
            0);
        XPLMBindTexture2d(textNum,0);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        glBindVertexArray(0);
        glUseProgram(0);
        if (!XPLMWindowIsInVR(myself->myWindow)) glPopAttrib();

}

void DrawLogic::UpdateDrawPad(int wW, int wH, int nleft, int nbottom, int nright, int ntop){
    if (XPLMWindowIsInVR(myWindow)){
        if (wW!=windowWidth||wH!=windowHeight){
            XPLMSetWindowGeometryVR(myWindow,windowWidth,windowHeight);
        }
    else

       {
           screenB=nbottom;
           screenL=nleft;
           screenR=nright;
           screenT=ntop;
           if (wW!=windowWidth||wH!=windowHeight)
               XPLMSetWindowGeometry(myWindow,screenL,screenB+windowHeight,screenL+windowWidth,screenB);
        }
    }
    if (hasToUpdate){
        XPLMBindTexture2d(textNum,0);
        glTexSubImage2D(GL_TEXTURE_2D,
                        0,  // mipmap level
                        0,  // x-offset
                        0,  // y-offset
                        windowWidth,
                        windowHeight,
                        GL_RGBA,           // color bytes of data we are sending
                        GL_UNSIGNED_BYTE,  // encoding of data we are sending
                        &(textureZone));
        hasToUpdate=false;

    }

}

void  DrawLogic::DrawStrings(){
    /*for (auto st:*strings){
        if ((!st.is_deleted)&st.s_visible) {
           XPLMDrawString(st.s_Color,st.s_screenX,st.s_screenY,st.s_String,nullptr,xplmFont_Proportional);
        }
    }*/
}

void DrawLogic::RenderCursor(){
    XPLMDrawString(myself->black,myself->cursorX,myself->cursorY,myself->cursor,nullptr,xplmFont_Proportional);
}

void DrawLogic::SetCursorPosition(int x, int y){
    myself->cursorX=x+myself->screenL;
    myself->cursorY=y-1+myself->screenB;
}

char* DrawLogic::ToC(const string &in_String){
    unsigned long long sz=in_String.size();sz++;//all this to only cast a std::string to a c-string without compiler warning
    char * fromString=new char [sz];            //otherwise just do XPLMDebugString((char*)in_String.c_str());
    if (in_String!="")
    {
        #if IBM
        strncpy_s(fromString,sz,in_String.c_str(),sz);
        #else
        strcpy(fromString,in_String.c_str());
        #endif
    }
    *fromString=*fromString+'\0';

    return fromString;
}

void  DrawLogic::PrintMyStats(){
    myself->WriteDebug("DrawLogic Stats");
    myself->WriteDebug("number of strings "+std::to_string(myself->strings->size()));
    myself->WriteDebug("current String Number "+std::to_string(myself->currentStringNumber));
    myself->WriteDebug("number of deleted strings "+std::to_string(myself->numberOfDeletedStrings));
    myself->PrintRectanglesAssessment();
    myself->PrintStringAssessment();
}

void DrawLogic::ShowAllStrings(){
    for (auto str:(*myself->strings)){
        myself->WriteDebug("string N° "+std::to_string(str.s_identifier)+" color N° "+std::to_string(str.s_Color_ident)+(str.is_deleted?" deleted":" non deleted"));
        myself->WriteDebug(str.s_String);
    }
}

void DrawLogic::FlushContent(){//Only for the modal windows, the others don't recreate their basic rects
    myself->rects->clear();
    myself->strings->clear();
    myself->numberOfDeletedStrings=0;
    myself->lastTriangleCreated=0;
    myself->hasDeletedStrings=false;
    myself->hasDeletedRectangles=false;
    myself->windowWidth=0;
    myself->windowHeight=0;
    myself->currentRectNumber=0;
    myself->currentStringNumber=0;
    myself->currentTriangleNumber=0;
    myself->cursorX=0;
    myself->cursorY=0;
    myself->FillAll(Clr_Black);
}

void DrawLogic::SetId(string in_ID){
    ident=in_ID;
}

string DrawLogic::GetId(){
    return ident;
}

string DrawLogic::GetSId(){
    return myself->ident;
}

void DrawLogic::StringAssesment(){
    myself->PrintStringAssessment();
}

void DrawLogic::PrintCurrentIdent(){
    WriteDebug("current myself pointer is pointing to "+myself->ident);
}
void DrawLogic::PrintStringAssessment(){
    WriteDebug("Strings logged for "+ident);
    WriteDebug("number of strings logged :"+std::to_string(strings->size()));
    for (auto st:*strings){
        WriteDebug("String "+st.s_String+" "
                   +" is Visible "+ std::to_string(st.s_visible)
                   +" is deleted "+ std::to_string(st.is_deleted)
                   +" left "+ std::to_string(st.s_screenX)
                   +" bottom "+ std::to_string(st.s_screenY)
                   +" fontsize "+std::to_string(st.s_fontSize));
        }
    WriteDebug("End of String report");
}

void DrawLogic::PrintRectanglesAssessment(){
    WriteDebug("Rectangles logged for "+ident);
    WriteDebug("number of rectangles logged "+std::to_string(rects->size()));
    for (auto rect:*rects){
        WriteDebug("rectangle : "+(*(rect.second)).GetDebugName()+
                   "Map Nb "+std::to_string(rect.first)+
                   " drawNb "+ std::to_string((*(rect.second)).GetId())+
                   " left "+ std::to_string((*(rect.second)).GetLeft())+
                   " bottom "+ std::to_string((*(rect.second)).GetBottom())+
                   " right "+ std::to_string((*(rect.second)).GetRight())+
                   " top "+ std::to_string((*(rect.second)).GetTop())+
                   " height "+ std::to_string((*(rect.second)).GetHeight())+
                   " width "+ std::to_string((*(rect.second)).GetWidth())+
                   " color "+ std::to_string((*(rect.second)).GetColor()));
    }
    WriteDebug("End of rectangles report");
}

int DrawLogic::GetWidth(){
    return windowWidth;
}
int DrawLogic::GetHeight(){
    return windowHeight;
}

bool DrawLogic::VerifyPointer(ulong tag, rectangles *in_rect){
    if ((*myself->rects).find(tag)!=(*myself->rects).end()) return false;
    else{
        if ((*myself->rects)[tag]==in_rect) return true;
    }
    return false;
}

void DrawLogic::PrintMemoryStats(){
    PROCESS_MEMORY_COUNTERS pmc;
    if(GetProcessMemoryInfo(GetCurrentProcess(),&pmc,sizeof(PROCESS_MEMORY_COUNTERS))){
        WriteDebug("This process uses the following number of kBytes : ",static_cast<int>(pmc.WorkingSetSize/1024));
    }
    MEMORYSTATUSEX statex;
      statex.dwLength = sizeof (statex);
      GlobalMemoryStatusEx (&statex);
    WriteDebug("% of memory in use : ",static_cast<int>(statex.dwMemoryLoad));
    WriteDebug("free kB in RAM : ",static_cast<int>(statex.ullTotalPhys/1024));
}
