#include "keyboard.h"
//size of a space = 4px
Keyboard::Keyboard():
    allKeyboard(),rL1(),rL2(),rL3(),rL4(),rL5(),//rectangles for localisation of clics
    kL1(),kL2(),kL3(),kL4(),kL5(),//vectors of keys
    defL1(""),defL2(""),defL3(""),defL4(""),defL5(""), //Line definitions
    activeKey(nullptr),
    shiftKeyLeft(nullptr),
    shiftKeyRight(nullptr),
    ctrlKey(nullptr),
    altKey(nullptr),
    abbrvKey(nullptr),
    capsKey(nullptr),
    offX(0), offY(0),//initial offsets to place keys
    currentKeyLine(0),currentKeyIdx(0),//indices for localisation of keys
    keyPressed(false),shiftToggle(false),capsToggle(false),ctrlToggle(false),altToggle(false),abbreviationsToggle(false),
    specialKey(false)//state variable

{
   keyHeight=20;
   keyWidth=20; //A keyboard
}

void Keyboard::WriteDebug(string message){
    string in_String="VR Tools : " +message+"\n";
    XPLMDebugString((char*)in_String.c_str());
}

void Keyboard::DefineKeyboard (int type){//0 : american english, then for others waiting for development
    if (type==0){
        defL1="`~1!2@3#4$5%6^7&8*9(0)-_=+";//then special backspace
        defL2="qQwWeErRtTyYuUiIoOpP[{]}\\|";
        defL3="aAsSdDfFgGhHjJkKlL;:'\"";//begins with cap lock, ends with ret
        defL4="zZxXcVvVbBnNmM,<.>/?";//begins and ends with shift
        //Line 5 CTRL and Space, add a suppress key
        //Control Keys : XCV (cut copy paste) S (save), Q (end edit), Z (end of file)
    }
}

void Keyboard::MakeKeyDefs(const string &in_Line,std::vector<string> &out_keys){
    out_keys.clear();
    string item;
    std::istringstream fd(in_Line);
    while (getline(fd,item,',')){
        out_keys.push_back(item);
    }
}

string Keyboard::ConvString(const string &in_String){
    //reads a string for keyboard defs, returns an interpreted string for special keywords
    if (in_String=="comma") return (",");
    if (in_String=="Space") return(" ");
    if (in_String=="nope") return("");
    return in_String;
}

void Keyboard::FindKeySpecs(const string &in_Def,//  find Key specifications from a defining string
                  bool &isSpec,
                  string &showString,
                  string &mainString,
                  string &shiftString,
                  string &altString,
                  string &abbrString,
                  int &spacing,
                  int &keyWidth)
{
    std::vector<string> specs;
    string item;
    std::istringstream fd(in_Def);
    while (getline(fd,item,' ')){
        specs.push_back(item);
    }
    unsigned long long iterator(0),nbSpecs(specs.size());
    isSpec=false;
    showString="";
    mainString="";
    shiftString="";
    altString="";
    abbrString="";
    spacing=0;
    keyWidth=20;
    try{spacing=std::stoi(specs[0]);
    }
    catch(std::invalid_argument& e){
        iterator--;
    }
    iterator++;
    try{keyWidth=std::stoi(specs[1]);}
    catch(std::invalid_argument& e){
        iterator--;
    }
    iterator++;
    if (specs[iterator]=="SPEC"){
        isSpec=true;
        iterator++;
        if (iterator<nbSpecs) mainString=ConvString(specs[iterator]);
        iterator++;
        if (iterator<nbSpecs) showString=ConvString(specs[iterator]);
    }else{
        if (iterator<nbSpecs) showString=ConvString(specs[iterator]);
        iterator++;
        if (iterator<nbSpecs) mainString=ConvString(specs[iterator]);
        iterator++;
        if (iterator<nbSpecs) shiftString=ConvString(specs[iterator]);
        iterator++;
        if (iterator<nbSpecs) altString=ConvString(specs[iterator]);
        iterator++;
        if (iterator<nbSpecs) abbrString=ConvString(specs[iterator]);
    }
}

int Keyboard::MakeLine(const int &offY, const string &keyDefLine,int lineNumber,std::vector<std::shared_ptr<Key>> &keyLine){
    std::vector<string> keyDefs;
    bool isSpecialKey(false);
    string showS(""),mainS(""),shiftS(""),altS(""),abbrS("");
    int offset(0),width(0),stepper(0),col(0);
    MakeKeyDefs(keyDefLine,keyDefs);
    for (auto newkeyString:keyDefs){
        FindKeySpecs(newkeyString,isSpecialKey,showS,mainS,shiftS,altS,abbrS,offset,width);
        auto nextkey=std::make_shared<Key>();
        nextkey->isVisible=true;
        nextkey->offsetX=offX+stepper+offset;
        stepper+=offset+width;
        nextkey->offsetY=offY;
        nextkey->height=keyHeight;
        nextkey->width=width;
        nextkey->DefineKey(char(0),char(0),false,isSpecialKey,showS,mainS,shiftS,altS,abbrS,lineNumber,col);
        if (isSpecialKey){
            if (nextkey->GetKeyName()=="Shift"&&col==0) shiftKeyLeft=nextkey->GetMyPointer();
            if (nextkey->GetKeyName()=="Shift"&&col>0) shiftKeyRight=nextkey->GetMyPointer();
            if (nextkey->GetKeyName()=="Ctrl") ctrlKey=nextkey->GetMyPointer();
            if (nextkey->GetKeyName()=="Alt") altKey=nextkey->GetMyPointer();
            if (nextkey->GetKeyName()=="Abbr") abbrvKey=nextkey->GetMyPointer();
            if (nextkey->GetKeyName()=="CpsLck") capsKey=nextkey->GetMyPointer();
        }
        keyLine.push_back(nextkey);
        col++;
    }
    return stepper;
}

void Keyboard::MakeKeyboard(int oX, int oY){
    /*A keyboard has 5 Lines and an undefinded number of keys per line
     *
     * functionality
     * extract values from defL for each line, set values, regular, shift alt
     * SetText of Button for regular values in the beginning, or symbol for shift, ret, backsp, suppr
     * CTRL, ALT for control buttons
     * include the special keys into each line
     * adjust width for special keys to make a rectangular keyboard

     keyboard is described in a text file with one text line per keyboard line
     keys are defined in the following way
     keys separated by commas
     strings separated by space
     first string is a digit defining spacing between adjacent keys
     second string is the width of the key (20 by default)
     next String is the general name of the key and the value to display on the key
     next String is the main value returned when pressing the key
     then value returned when shift or caps lock is active
     then value returned when alt is active
     then value returned when abbr is active
     if a value is not defined and empty string is written for subsequent values
*/
    offX=oX;offY=oY;
    kL1.clear();
    kL2.clear();
    kL3.clear();
    kL4.clear();
    kL5.clear();
    rL1.offsetX=offX; rL1.offsetY=offY;rL1.height=keyHeight+2;
    rL2.offsetX=offX; rL2.offsetY=offY+keyHeight+2;rL2.height=rL1.height;
    rL3.offsetX=offX; rL3.offsetY=rL2.offsetY+keyHeight+2;rL3.height=rL1.height;
    rL4.offsetX=offX; rL4.offsetY=rL3.offsetY+keyHeight+2;rL4.height=rL1.height;
    rL5.offsetX=offX; rL5.offsetY=rL4.offsetY+keyHeight+2;rL5.height=rL1.height;
    allKeyboard.offsetX=offX;allKeyboard.offsetY=offY;
    allKeyboard.height=rL5.offsetY+keyHeight+2;
    //read the file containing definitions of keys, one text line per keyboard line
    string fileName="Resources\\plugins\\VR_Tools\\USKB.rsc"; // in a next version, file name will come from ini, with a custom def possible
    std::fstream textFile;
    textFile.open(fileName,std::ifstream::in);
    if (textFile.is_open()){
        //read lines, define the lines of keys in a vector per line, calculate widths
        string inputL;
        int maxWidth(0);
        getline(textFile,inputL);
        rL1.width=MakeLine(rL1.offsetY,inputL,1,kL1);
        if (rL1.width>maxWidth) maxWidth=rL1.width;
        getline(textFile,inputL);
        rL2.width=MakeLine(rL2.offsetY,inputL,1,kL2);
        if (rL2.width>maxWidth) maxWidth=rL2.width;
        getline(textFile,inputL);
        rL3.width=MakeLine(rL3.offsetY,inputL,1,kL3);
        if (rL3.width>maxWidth) maxWidth=rL3.width;
        getline(textFile,inputL);
        rL4.width=MakeLine(rL4.offsetY,inputL,1,kL4);
        if (rL4.width>maxWidth) maxWidth=rL4.width;
        getline(textFile,inputL);
        rL5.width=MakeLine(rL5.offsetY,inputL,1,kL5);
        if (rL5.width>maxWidth) maxWidth=rL5.width;
        allKeyboard.width=maxWidth;
           }


}

void Keyboard::SetOrigin (int x, int y){
    offX=x;offY=y;
}

void Keyboard::Relocate (int newX, int newY){
    int deltaX(newX-offX),deltaY(newY-offY);
    offX=newX;offY=newY;
    allKeyboard.offsetY=allKeyboard.offsetY+deltaY;
    allKeyboard.offsetX=allKeyboard.offsetX+deltaX;
    allKeyboard.recalculate();
    rL1.offsetX=rL1.offsetX+deltaX;
    rL1.offsetY=rL1.offsetY+deltaY;
    rL1.recalculate();
    rL2.offsetX=rL2.offsetX+deltaX;
    rL2.offsetY=rL2.offsetY+deltaY;
    rL2.recalculate();
    rL3.offsetX=rL3.offsetX+deltaX;
    rL3.offsetY=rL3.offsetY+deltaY;
    rL3.recalculate();
    rL4.offsetX=rL4.offsetX+deltaX;
    rL4.offsetY=rL4.offsetY+deltaY;
    rL4.recalculate();
    rL5.offsetX=rL5.offsetX+deltaX;
    rL5.offsetY=rL5.offsetY+deltaY;
    rL5.recalculate();
    for (std::shared_ptr<Key> k: kL1)  k->reposition(deltaX,deltaY);
    for (std::shared_ptr<Key> k: kL2)  k->reposition(deltaX,deltaY);
    for (std::shared_ptr<Key> k: kL3)  k->reposition(deltaX,deltaY);
    for (std::shared_ptr<Key> k: kL4)  k->reposition(deltaX,deltaY);
    for (std::shared_ptr<Key> k: kL5)  k->reposition(deltaX,deltaY);

}

void Keyboard::Recalculate(int l, int t){
    rL1.recalculate(l,t);
    rL2.recalculate(l,t);
    rL3.recalculate(l,t);
    rL4.recalculate(l,t);
    rL5.recalculate(l,t);
    allKeyboard.recalculate(l,t);
    for (std::shared_ptr<Key> k: kL1) {k->in_top=t;k->in_left=l;k->recalculate();}
    for (std::shared_ptr<Key> k: kL2) {k->in_top=t;k->in_left=l;k->recalculate();}
    for (std::shared_ptr<Key> k: kL3) {k->in_top=t;k->in_left=l;k->recalculate();}
    for (std::shared_ptr<Key> k: kL4) {k->in_top=t;k->in_left=l;k->recalculate();}
    for (std::shared_ptr<Key> k: kL5) {k->in_top=t;k->in_left=l;k->recalculate();}
}

void Keyboard::DrawKb(){
    for (std::shared_ptr<Key> k: kL1) {k->drawButton();}
    for (std::shared_ptr<Key> k: kL2) {k->drawButton();}
    for (std::shared_ptr<Key> k: kL3) {k->drawButton();}
    for (std::shared_ptr<Key> k: kL4) {k->drawButton();}
    for (std::shared_ptr<Key> k: kL5) {k->drawButton();}
}

bool Keyboard::PressKey(int cx, int cy, bool &special, std::string &keyName, std::string &keyVal){
    /*first locate in keyboard, if no return ""
     * then locate in line
     * then find the key
     * functionality : find which key is under cx, cy
     * if normal return keyVal,if special key return name of key
     * if Shift do SetText of nonSpecial keys to Shift value (same for Alt when enabled)
     * if one key pressed after shift, revert to normal again
     * if CTRL keep XCVSXZ keys, hide the others
     * if again CTRL revert to normal again, if command pressed after control, revert display to normal again
     * caps lock to toggle Shift value, untoggle when repressed?
     * shift undoes caps lock if pressed while caps lock is on*/
    keyName="";
    keyVal="";
    bool isInKeyB=allKeyboard.isHere(cx,cy);
    if (isInKeyB){

    string retS("");
    if (rL1.isHere(cx,cy)){
        for (std::shared_ptr<Key> k: kL1) {
            if (k->isHere(cx,cy)){
                keyVal=ReadKey(k,special,keyName);
                activeKey=k->GetMyPointer();
            }
        }
    }
    if (rL2.isHere(cx,cy)){
        for (std::shared_ptr<Key> k: kL2) {
            if (k->isHere(cx,cy)){
                keyVal=ReadKey(k,special,keyName);
                activeKey=k->GetMyPointer();
            }
        }
    }
    if (rL3.isHere(cx,cy)){
        for (std::shared_ptr<Key> k: kL3) {
            if (k->isHere(cx,cy)){
                keyVal=ReadKey(k,special,keyName);
                activeKey=k->GetMyPointer();
            }
        }
    }

    if (rL4.isHere(cx,cy)){
        for (std::shared_ptr<Key> k: kL4) {
            if (k->isHere(cx,cy)){
                keyVal=ReadKey(k,special,keyName);
                activeKey=k->GetMyPointer();
            }
        }
    }
    if (rL5.isHere(cx,cy)){
        for (std::shared_ptr<Key> k: kL5) {
            if (k->isHere(cx,cy)){
                keyVal=ReadKey(k,special,keyName);
                activeKey=k->GetMyPointer();
            }
        }
    }
    if (activeKey!=nullptr){
        activeKey->Press();
        specialKey=special;

        if (special){//only treat the modifications keys : shift, alt, caps lock, ctrl and abbr
            if (keyVal=="Shift") {
                ShiftPressed();
                shiftKeyLeft->setSelect(shiftToggle);
                shiftKeyRight->setSelect(shiftToggle);
            }
            if (keyVal=="CpsLck") {
                ShiftCapsPressed();
                capsKey->setSelect(capsToggle);
            }
            if (keyVal=="Ctrl") {
                CTRLPressed();
                ctrlKey->setSelect(ctrlToggle);
            }
            if (keyVal=="Alt") {
                ALTPressed();
                altKey->setSelect(altToggle);
            }

            if (keyVal=="Abbr") {
                AbbrPressed();
                abbrvKey->setSelect(abbreviationsToggle);
            }
        }
    }
    }
    return isInKeyB;
}

string Keyboard::ReadKey(std::shared_ptr<Key> key, bool &special, std::string &keyName){
    keyPressed=true;
    special=key->IsSpecialKey();
    keyName=key->GetKeyName();
    std::string keyVal("");
    if (special) return (keyName);
    if (capsToggle) {
        keyVal=key->GetKeyShift();
        if (shiftToggle) {
            keyVal=key->GetKeyMainVal();
            ShiftPressed();
            shiftKeyLeft->setSelect(shiftToggle);
            shiftKeyRight->setSelect(shiftToggle);
        }
        return keyVal;
    }
    if (ctrlToggle){
        keyVal=key->GetKeyName();
        CTRLPressed();
        ctrlKey->setSelect(ctrlToggle);
        return keyVal;
    }
    if (shiftToggle){
        keyVal=key->GetKeyShift();
        ShiftPressed();
        shiftKeyLeft->setSelect(shiftToggle);
        shiftKeyRight->setSelect(shiftToggle);
        return keyVal;
    }
    if (altToggle) {
        keyVal=key->GetKeyAlt();
        ALTPressed();
        altKey->setSelect(altToggle);
        return keyVal;
        }
    if (abbreviationsToggle){
        keyVal=key->GetKeyAbbr();
        AbbrPressed();
        abbrvKey->setSelect(abbreviationsToggle);
        return keyVal;
    }
    keyVal=key->GetKeyMainVal();
    return keyVal;
}

bool Keyboard::IsKeyPressed(){
 return keyPressed;
}

void Keyboard::ReleaseCurrentKey(){
    if (keyPressed){
       keyPressed=false;
       if (activeKey!=nullptr){
          activeKey->Release();
          activeKey=nullptr;
        }
    }
}

void Keyboard::ShiftPressed(){
    bool shft=!shiftToggle;
    ReleaseStates();
    if (!shft){
        if (!capsToggle) ToNormalKeys();
        else ToShiftKeys();
    }
    else{
        if (capsToggle) ToNormalKeys();
        else ToShiftKeys();
    }
    shiftToggle=shft;
}

void Keyboard::ReleaseStates(){
   shiftToggle=false;
   altToggle=false;
   ctrlToggle=false;
   abbreviationsToggle=false;
   abbrvKey->setSelect(abbreviationsToggle);
   altKey->setSelect(altToggle);
   shiftKeyLeft->setSelect(false);
   shiftKeyRight->setSelect(false);
   ctrlKey->setSelect(false);
}

void Keyboard::ToShiftKeys(){
    for (std::shared_ptr<Key> k: kL1) {
        if (!(k->IsSpecialKey())) k->setText(k->GetKeyShift());
    }
    for (std::shared_ptr<Key> k: kL2) {
        if (!k->IsSpecialKey()) k->setText(k->GetKeyShift());
    }
    for (std::shared_ptr<Key> k: kL3) {
        if (!k->IsSpecialKey()) k->setText(k->GetKeyShift());
    }
    for (std::shared_ptr<Key> k: kL4) {
        if (!k->IsSpecialKey()) k->setText(k->GetKeyShift());
    }
    for (std::shared_ptr<Key> k: kL5) {
        if (!k->IsSpecialKey()) k->setText(k->GetKeyShift());
    }
}

void Keyboard::ToNormalKeys(){
    for (std::shared_ptr<Key> k: kL1) {
        if (!k->IsSpecialKey()) k->setText(k->GetKeyName());
    }
    for (std::shared_ptr<Key> k: kL2) {
        if (!k->IsSpecialKey()) k->setText(k->GetKeyName());
    }
    for (std::shared_ptr<Key> k: kL3) {
        if (!k->IsSpecialKey()) k->setText(k->GetKeyName());
    }
    for (std::shared_ptr<Key> k: kL4) {
        if (!k->IsSpecialKey()) k->setText(k->GetKeyName());
    }
    for (std::shared_ptr<Key> k: kL5) {
        if (!k->IsSpecialKey()) k->setText(k->GetKeyName());
    }
}

void Keyboard::ToAltKeys(){
    for (std::shared_ptr<Key> k: kL1) {
        if (!k->IsSpecialKey()) k->setText(k->GetKeyAlt());
    }
    for (std::shared_ptr<Key> k: kL2) {
        if (!k->IsSpecialKey()) k->setText(k->GetKeyAlt());
    }
    for (std::shared_ptr<Key> k: kL3) {
        if (!k->IsSpecialKey()) k->setText(k->GetKeyAlt());
    }
    for (std::shared_ptr<Key> k: kL4) {
        if (!k->IsSpecialKey()) k->setText(k->GetKeyAlt());
    }
    for (std::shared_ptr<Key> k: kL5) {
        if (!k->IsSpecialKey()) k->setText(k->GetKeyAlt());
    }
}

void Keyboard::ToAbbrKeys(){
    for (std::shared_ptr<Key> k: kL1) {
        if (!k->IsSpecialKey()) k->setText(k->GetKeyAbbr());
    }
    for (std::shared_ptr<Key> k: kL2) {
        if (!k->IsSpecialKey()) k->setText(k->GetKeyAbbr());
    }
    for (std::shared_ptr<Key> k: kL3) {
        if (!k->IsSpecialKey()) k->setText(k->GetKeyAbbr());
    }
    for (std::shared_ptr<Key> k: kL4) {
        if (!k->IsSpecialKey()) k->setText(k->GetKeyAbbr());
    }
    for (std::shared_ptr<Key> k: kL5) {
        if (!k->IsSpecialKey()) k->setText(k->GetKeyAbbr());
    }
}
void Keyboard::ShiftCapsPressed(){
    ReleaseStates();
    capsToggle=!capsToggle;
    if (capsToggle){
        ToShiftKeys();
    }else{
        ToNormalKeys();
    }
}
void Keyboard::CTRLPressed(){
    if (!ctrlToggle){ctrlToggle=true;}
    else{
        ReleaseStates();
    if (capsToggle){
        ToShiftKeys();
    }else{
        ToNormalKeys();
    }
    ctrlToggle=true;
    }

}


void Keyboard::ALTPressed(){
    bool alt=!altToggle;
    ReleaseStates();
    if (alt) ToAltKeys();
    else{
        if (capsToggle){
            ToShiftKeys();
        }else{
            ToNormalKeys();
        }
    }
    altToggle=alt;
}

void Keyboard::AbbrPressed(){
    bool abbr=!abbreviationsToggle;
    ReleaseStates();
    if (abbr) ToAbbrKeys();
    else{
        if (capsToggle){
            ToShiftKeys();
        }else{
            ToNormalKeys();
        }
    }
    abbreviationsToggle=abbr;
}

int Keyboard::MyWidth(){
    return allKeyboard.width;
}
int Keyboard::MyHeight(){
    return allKeyboard.height;
}
bool Keyboard::IsSpecialKey(){
    return specialKey;
}

bool Keyboard::IsShiftKeyActive(){
    return shiftToggle;
}

bool Keyboard::IsCapsKeyActive(){
    return capsToggle;
}

bool Keyboard::IsControlKeyActive(){
    return ctrlToggle;
}

bool Keyboard::IsAltKeyActive(){
    return altToggle;
}

bool Keyboard::IsAbbreviationsKeyActive(){
    return abbreviationsToggle;
}

string Keyboard::Physical_Key_Handler(char in_char,XPLMKeyFlags flag,char in_VK){
    if ((flag&xplm_ControlFlag)!=xplm_ControlFlag){
    string codeVKtoString(1,in_VK);
    string charToString(1,in_char);
    return charToString;};
   return ("");
}
