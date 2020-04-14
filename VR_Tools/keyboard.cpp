#include "keyboard.h"
#include "layoutwithedit.h"
#include "drawlogic.h"
//size of a space = 4px

lineOfKeys Keyboard::physicalKeys;
string Keyboard::activeKeyName("");
string Keyboard::activeKeyString("");
bool Keyboard::activeIsSpecialKey(false);
Keyboard* Keyboard::myKeyboard(nullptr);
bool Keyboard::physicalKeyPressed(false);
int Keyboard::activeVKCode(0);
float Keyboard::physicalKeyTimer(0);
float Keyboard::latency(0.6f);

Keyboard::Keyboard(bool modal):

    allKeyboard(false),
    rL1(false),rL2(false),rL3(false),rL4(false),rL5(false),//rectangles for localisation of clics
    isModal(modal),
    kL1(),kL2(),kL3(),kL4(),kL5(),//vectors of keys
    activeKey(nullptr),
    shiftKeyLeft(nullptr),
    shiftKeyRight(nullptr),
    ctrlKey(nullptr),
    altKey(nullptr),
    abbrvKey(nullptr),
    capsKey(nullptr),
    offX(0), offY(0),//initial offsets to place keys
    currentKeyLine(0),currentKeyIdx(0),//indices for localisation of keys
    virtualKeyTimer(0.0),
    virtualKeyLatency(0.6f),
    physicalKyBdEnabled(false),
    keyPressed(false),
    shiftToggle(false),capsToggle(false),ctrlToggle(false),altToggle(false),abbreviationsToggle(false),
    specialKey(false)//state variable

{
   keyHeight=20;
   keyWidth=20; //A keyboard
   for (int I(0);I<251;I++)
       physicalKeys.push_back(nullptr);
}

Keyboard::~Keyboard(){
    for (auto k: kL1)  delete k;
    for (auto k: kL2)  delete k;
    for (auto k: kL3)  delete k;
    for (auto k: kL4)  delete k;
    for (auto k: kL5)  delete k;
    myKeyboard=nullptr;
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
                  string &abbrName,
                  string &abbrString,
                  int &spacing,
                  int &keyWidth,
                  ulong &vk_tag)
{
    std::vector<string> specs;
    string item;
    std::istringstream fd(in_Def);
    while (getline(fd,item,' ')){
        specs.push_back(item);
    }
    unsigned long long iterator(0),nbSpecs(specs.size());
    if (nbSpecs>4)
    {
        isSpec=false;
        showString="";
        mainString="";
        shiftString="";
        altString="";
        abbrName="";
        abbrString="";
        spacing=0;
        keyWidth=20;
        int vk_num(0);
        vk_tag=0;
        if ((specs[0]=="")||(specs[1]=="")||(specs[2]=="")||(specs[3]=="")) {
            DrawLogic::WriteDebug("Invalid key definition in statement "+in_Def);
            return;
        }
        try{vk_num=std::stoi(specs[0]);}
        catch(std::invalid_argument& e){
            iterator--;
            DrawLogic::WriteDebug("Keyboard definition file invalid, must contain a reference to a VK_key for every virtual key");
            return;
        }
        if (vk_num>189){
            DrawLogic::WriteDebug("Keyboard definition file contains an invalid reference to a VK_Key : >189");
        }
        else {
            if (vk_num>0)
                vk_tag=static_cast<ulong>(vk_num);
        }
        iterator++;
        try{spacing=std::stoi(specs[1]);
        }
        catch(std::invalid_argument& e){
            iterator--;
        }
        iterator++;
        try{keyWidth=std::stoi(specs[2]);}
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
            if (iterator<nbSpecs) {abbrName=ConvString(specs[iterator]);abbrString=abbrName;}
            //if no other string is provided the name of the key will be its value
            iterator++;
            if (iterator<nbSpecs) abbrString=ConvString(specs[iterator]);
        }
    }
    else DrawLogic::WriteDebug("invalid keyboard specification file");
}

int Keyboard::MakeLine(const int &offY, const string &keyDefLine,int lineNumber,lineOfKeys &keyLine){
    std::vector<string>* keyDefs=new(std::vector<string>);
    bool isSpecialKey(false);
    string showS(""),mainS(""),shiftS(""),altS(""),abbrS(""),abbrN("");
    int offset(0),width(0),stepper(0),col(0);
    MakeKeyDefs(keyDefLine,(*keyDefs));
    for (auto &newkeyString:*keyDefs){
        ulong vk_tag;
        FindKeySpecs(newkeyString,isSpecialKey,showS,mainS,shiftS,altS,abbrN,abbrS,offset,width,vk_tag);
        //auto nextkey=std::make_shared<Key>(false);
        Key *nextkey=new Key(false);
        nextkey->setVisibility(true);
        nextkey->SetOrigin(offX+stepper+offset,offY);
        stepper+=width+offset;
        nextkey->SetDimensions(width,keyHeight);
        nextkey->DefineKey(char(0),char(0),false,isSpecialKey,showS,mainS,shiftS,altS,abbrN, abbrS,lineNumber,col);    
        if (isSpecialKey){
            if (nextkey->GetKeyName()=="Shift"&&col==0) shiftKeyLeft=nextkey->GetMyPointer();
            if (nextkey->GetKeyName()=="Shift"&&col>0) shiftKeyRight=nextkey->GetMyPointer();
            if (nextkey->GetKeyName()=="Ctrl") ctrlKey=nextkey->GetMyPointer();
            if (nextkey->GetKeyName()=="Alt") altKey=nextkey->GetMyPointer();
            if (nextkey->GetKeyName()=="Abbr") abbrvKey=nextkey->GetMyPointer();
            if (nextkey->GetKeyName()=="CpsLck") capsKey=nextkey->GetMyPointer();
        }
        keyLine.push_back(nextkey);
        if (vk_tag<=250) physicalKeys[vk_tag]=nextkey;
        else DrawLogic::WriteDebug("keyboard implementation, can't take virtual key codes over 250 : check keyb definition file");
        col++;

    }
    delete keyDefs;
    return stepper;
}

void Keyboard::MakeKeyboard(int oX, int oY, bool numpad){
    /*A keyboard has 5 Lines and an undefinded number of keys per line
     *
     * functionality
     * extract values from defL for each line, set values, regular, shift alt, define physical correspondance
     * SetText of Button for regular values in the beginning, or symbol for shift, ret, backsp, suppr
     * CTRL, ALT for control buttons
     * include the special keys into each line
     * adjust width for special keys to make a rectangular keyboard

     keyboard is described in a text file with one text line per keyboard line
     keys are defined in the following way
     keys separated by commas
     strings separated by space
     first string is a digit for the physical key evoking this virtual key when pressed (0: no physical key)
     second string is a digit defining spacing between adjacent keys
     third string is the width of the key (20 by default)
     next String is the general name of the key and the value to display on the key
     next String is the main value returned when pressing the key
     then value returned when shift or caps lock is active
     then value returned when alt is active
     then value returned when abbr is active
     if a value is not defined and empty string is written for subsequent values
*/

    myKeyboard=this;
    offX=oX;offY=oY;
    kL1.clear();
    kL2.clear();
    kL3.clear();
    kL4.clear();
    kL5.clear();
    allKeyboard.SetOrigin(offX,offY);

    rL5.SetOrigin(offX,offY);
    rL4.SetOrigin(offX,rL5.GetBottom()+keyHeight+2);
    rL3.SetOrigin(offX,rL4.GetBottom()+keyHeight+2);
    rL2.SetOrigin(offX,rL3.GetBottom()+keyHeight+2);
    rL1.SetOrigin(offX,rL2.GetBottom()+keyHeight+2);
    //read the file containing definitions of keys, one text line per keyboard line
    string fileName("");
    if (numpad){
        fileName="Resources/plugins/VR_Tools/keyboards/numpad2.cfg";}
    else{
        fileName="Resources/plugins/VR_Tools/keyboards/USKB.cfg";
    }    // in a next version, file name will come from ini, with a custom def possible
    std::fstream textFile;
    textFile.open(fileName,std::ifstream::in);
    if (textFile.is_open()){
        //read lines, define the lines of keys in a vector per line, calculate widths
        string inputL;
        int maxWidth(0);
        getline(textFile,inputL);
        rL1.SetDimensions(MakeLine(rL1.GetBottom(),inputL,1,kL1),keyHeight+2);
        if (rL1.GetWidth()>maxWidth) maxWidth=rL1.GetWidth();
        getline(textFile,inputL);
        rL2.SetDimensions(MakeLine(rL2.GetBottom(),inputL,2,kL2),rL1.GetHeight());
        if (rL2.GetWidth()>maxWidth) maxWidth=rL2.GetWidth();
        getline(textFile,inputL);
        rL3.SetDimensions(MakeLine(rL3.GetBottom(),inputL,3,kL3),rL1.GetHeight());
        if (rL3.GetWidth()>maxWidth) maxWidth=rL3.GetWidth();
        getline(textFile,inputL);
        rL4.SetDimensions(MakeLine(rL4.GetBottom(),inputL,4,kL4),rL1.GetHeight());
        if (rL4.GetWidth()>maxWidth) maxWidth=rL4.GetWidth();
        getline(textFile,inputL);
        rL5.SetDimensions(MakeLine(rL5.GetBottom(),inputL,5,kL5),rL1.GetHeight());
        if (rL5.GetWidth()>maxWidth) maxWidth=rL5.GetWidth();
        allKeyboard.SetDimensions(maxWidth,rL1.GetTop()-rL5.GetBottom());
           }
}

void Keyboard::SetOrigin (int x, int y){
    offX=x;offY=y;
}

void Keyboard::Relocate (int newX, int newY){
    int deltaX(newX-offX),deltaY(offY==0?(newY):newY-offY);
    offX=newX;offY=newY;
    allKeyboard.SetOrigin(newX,newY);
    rL1.SetOrigin(rL1.GetLeft()+deltaX,rL1.GetBottom()+deltaY);
    rL2.SetOrigin(rL2.GetLeft()+deltaX,rL2.GetBottom()+deltaY);
    rL3.SetOrigin(rL3.GetLeft()+deltaX,rL3.GetBottom()+deltaY);
    rL4.SetOrigin(rL4.GetLeft()+deltaX,rL4.GetBottom()+deltaY);
    rL5.SetOrigin(rL5.GetLeft()+deltaX,rL5.GetBottom()+deltaY);
    for (auto k: kL1)  k->Shift(deltaX,deltaY);
    for (auto k: kL2)  k->Shift(deltaX,deltaY);
    for (auto k: kL3)  k->Shift(deltaX,deltaY);
    for (auto k: kL4)  k->Shift(deltaX,deltaY);
    for (auto k: kL5)  k->Shift(deltaX,deltaY);
}

bool  Keyboard::ReadLine(int cx, int cy, lineOfKeys in_line, bool &special, std::string &keyName, std::string &keyVal){
    for (auto k: in_line) {
        if (k->isHere(cx,cy)){
            keyVal=ReadKey(k,special,keyName);
            activeKey=k->GetMyPointer();
            return true;
        }
    }
    return false;
}

bool Keyboard::PressKey(int cx, int cy, bool &special, string &keyName, string &keyVal){
    /* this routine receives a click event at cx cy, tries to define which key, if any, was pressed
     * first locate the click in the keyboard, if not return false and keyName as ""
     * then locate in line
     * then find the key
     * if normal key return keyVal,if special key return name of key
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

    if (rL1.isHere(cx,cy)) ReadLine(cx,cy,kL1,special,keyName,keyVal);
    if (rL2.isHere(cx,cy)) ReadLine(cx,cy,kL2,special,keyName,keyVal);
    if (rL3.isHere(cx,cy)) ReadLine(cx,cy,kL3,special,keyName,keyVal);
    if (rL4.isHere(cx,cy)) ReadLine(cx,cy,kL4,special,keyName,keyVal);
    if (rL5.isHere(cx,cy)) ReadLine(cx,cy,kL5,special,keyName,keyVal);

    if (activeKey!=nullptr){
        activeKey->Press();
        specialKey=special;

        if (special){//treat the tab and modifications keys : shift, alt, caps lock, ctrl and abbr
            if (keyVal=="TAB") {
                if (IsShiftKeyActive()) {
                    keyName="ShTAB";
                    ShiftPressed();}
            }
            if (keyVal=="Shift") ShiftPressed();
            if (keyVal=="CpsLck")  ShiftCapsPressed();
            if (keyVal=="Ctrl")  CTRLPressed();
            if (keyVal=="Alt") ALTPressed();
            if (keyVal=="Abbr") AbbrPressed();

        }
    }
    }
    return isInKeyB;
}

string Keyboard::ReadKey(Key* key, bool &special, string &keyName){
    keyPressed=true;
    special=key->IsSpecialKey();
    keyName=key->GetKeyName();
    string keyVal("");
    if (special) return (keyName);
    if (capsToggle) {
        keyVal=key->GetKeyShift();
        if (shiftToggle) {
            keyVal=key->GetKeyMainVal();
            ShiftPressed();
        }
        return keyVal;
    }
    if (ctrlToggle){
        keyVal=key->GetKeyName();
        CTRLPressed();
        return keyVal;
    }
    if (shiftToggle){
        keyVal=key->GetKeyShift();
        ShiftPressed();
        return keyVal;
    }
    if (altToggle) {
        keyVal=key->GetKeyAlt();
        ALTPressed();
        return keyVal;
        }
    if (abbreviationsToggle){
        keyVal=key->GetKeyAbbrVal();
        AbbrPressed();
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
    shiftKeyLeft->setSelect(shiftToggle);
    shiftKeyRight->setSelect(shiftToggle);
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
    for (auto k: kL1) {
        if (!k->IsSpecialKey()) {k->setTextFixedSize(k->GetKeyShift());k->ReDrawButton();}
    }
    for (auto k: kL2) {
        if (!k->IsSpecialKey()) {k->setTextFixedSize(k->GetKeyShift());k->ReDrawButton();}
    }
    for (auto k: kL3) {
        if (!k->IsSpecialKey()) {k->setTextFixedSize(k->GetKeyShift());k->ReDrawButton();}
    }
    for (auto k: kL4) {
        if (!k->IsSpecialKey()) {k->setTextFixedSize(k->GetKeyShift());k->ReDrawButton();}
    }
    for (auto k: kL5) {
        if (!k->IsSpecialKey()) {k->setTextFixedSize(k->GetKeyShift());k->ReDrawButton();}
    }
}

void Keyboard::ToNormalKeys(){
    for (auto k: kL1) {
        if (!k->IsSpecialKey()) {k->setTextFixedSize(k->GetKeyName());k->ReDrawButton();}
    }
    for (auto k: kL2) {
        if (!k->IsSpecialKey()) {k->setTextFixedSize(k->GetKeyName());k->ReDrawButton();}
    }
    for (auto k: kL3) {
        if (!k->IsSpecialKey()) {k->setTextFixedSize(k->GetKeyName());k->ReDrawButton();}
    }
    for (auto k: kL4) {
        if (!k->IsSpecialKey()) {k->setTextFixedSize(k->GetKeyName());k->ReDrawButton();}
    }
    for (auto k: kL5) {
        if (!k->IsSpecialKey()) {k->setTextFixedSize(k->GetKeyName());k->ReDrawButton();}
    }
}

void Keyboard::ToAltKeys(){
    for (auto k: kL1) {
        if (!k->IsSpecialKey()) {k->setTextFixedSize(k->GetKeyAlt());k->ReDrawButton();}
    }
    for (auto k: kL2) {
        if (!k->IsSpecialKey()) {k->setTextFixedSize(k->GetKeyAlt());k->ReDrawButton();}
    }
    for (auto k: kL3) {
        if (!k->IsSpecialKey()) {k->setTextFixedSize(k->GetKeyAlt());k->ReDrawButton();}
    }
    for (auto k: kL4) {
        if (!k->IsSpecialKey()) {k->setTextFixedSize(k->GetKeyAlt());k->ReDrawButton();}
    }
    for (auto k: kL5) {
        if (!k->IsSpecialKey()) {k->setTextFixedSize(k->GetKeyAlt());k->ReDrawButton();}
    }
}

void Keyboard::ToAbbrKeys(){
    for (auto k: kL1) {
        if (!k->IsSpecialKey()) {k->setTextFixedSize(k->GetKeyAbbrName());k->ReDrawButton();}
    }
    for (auto k: kL2) {
        if (!k->IsSpecialKey()){k->setTextFixedSize(k->GetKeyAbbrName());k->ReDrawButton();}
    }
    for (auto k: kL3) {
        if (!k->IsSpecialKey()) {k->setTextFixedSize(k->GetKeyAbbrName());k->ReDrawButton();}
    }
    for (auto k: kL4) {
        if (!k->IsSpecialKey()) {k->setTextFixedSize(k->GetKeyAbbrName());k->ReDrawButton();}
    }
    for (auto k: kL5) {
        if (!k->IsSpecialKey()) {k->setTextFixedSize(k->GetKeyAbbrName());k->ReDrawButton();}
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
    capsKey->setSelect(capsToggle);
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
ctrlKey->setSelect(ctrlToggle);
}

void Keyboard::SetCTRLToggle(bool val){
    ctrlToggle=val;
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
    altKey->setSelect(altToggle);
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
    abbrvKey->setSelect(abbreviationsToggle);
}

int Keyboard::MyWidth(){
    return allKeyboard.GetWidth();
}
int Keyboard::MyHeight(){
    return allKeyboard.GetHeight();
}

int Keyboard::MyBottom(){
    return allKeyboard.GetBottom();
}

int Keyboard::MyLeft(){
    return allKeyboard.GetLeft();
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

void Keyboard::SetWarningMode(bool warning){
    if (warning){
        for (auto k: kL1) k->SetToWarningColor();
        for (auto k: kL2) k->SetToWarningColor();
        for (auto k: kL3) k->SetToWarningColor();
        for (auto k: kL4) k->SetToWarningColor();
        for (auto k: kL5) k->SetToWarningColor();
        }
    else{
        for (auto k: kL1) k->SetToStateColor();
        for (auto k: kL2) k->SetToStateColor();
        for (auto k: kL3) k->SetToStateColor();
        for (auto k: kL4) k->SetToStateColor();
        for (auto k: kL5) k->SetToStateColor();
        }
}

int  Keyboard::Physical_Key_Handler(char in_char,XPLMKeyFlags flag,char in_VKs,void* inRefcon){
    /*Options for reading the physical keyboard :
     * read through XPlane API
     * processes one discreet keypress (simultaneous keypresses will only handled for first key)
     *   ie, the initial state of the keyboard is conserved once a keypress is detected
     * all modifiers are processed, keeping a close relation to the behavior of the virtual keyboard
     * the values generated by the keys match those found on the virtual keyboard
     * */
     unsigned char in_VK=static_cast<unsigned char>(in_VKs);
     string bckStr("");
     LayoutWithEdit * caller=reinterpret_cast<LayoutWithEdit*>(inRefcon);
     caller->SetTextToShow("VK code = "+std::to_string(in_VK));

     if ((flag&XPLM_KEY_DOWN)&&!physicalKeyPressed){
         //initial press, no other key pressed
         physicalKeyPressed=true;
         activeVKCode=in_VK;
         physicalKeyTimer=XPLMGetElapsedTime();
      //examin not mapped VK_codes from larger keyboards
         if (in_VK==46){//this is the suppress key
             activeKeyName="D";
             activeKeyString="d";
             activeVKCode=46;
             myKeyboard->SetCTRLToggle(true);
             caller->ProcessKeyPress(activeKeyName,activeKeyString);
             caller->CheckButtonsVisibility();
             return 0;
         }
         if (in_VK==27) {//escape key, don't handle
             return 0;}

         if (in_VK>=96 && in_VK<=105){
             bckStr="1";
             bckStr[0]=in_char;
             caller->ProcessKeyPress(bckStr,bckStr);
             caller->CheckButtonsVisibility();
             return 0;
         }
         if (in_VK>=106 && in_VK<=111){
             bckStr="1";
             bckStr[0]=in_char;
             caller->ProcessKeyPress(bckStr,bckStr);
             caller->CheckButtonsVisibility();
             return 0;
         }

        //mapped keys and commands
         Key* pushed=Keyboard::physicalKeys[static_cast<ulong>(in_VK)];
         Key push;
         if (pushed!=nullptr){
             activeIsSpecialKey=pushed->IsSpecialKey();
             activeKeyName=pushed->GetKeyName();
             activeKeyString=pushed->GetKeyMainVal();
             if (activeKeyName=="TAB")
             {
                 if (flag&xplm_ShiftFlag)
                     activeKeyName="ShTAB";
             }

             if (flag&xplm_ShiftFlag) activeKeyString=pushed->GetKeyShift();
             if (flag&xplm_OptionAltFlag) activeKeyString=pushed->GetKeyAlt();
             if (flag&xplm_ControlFlag){
                 activeKeyString=activeKeyName;
                 myKeyboard->SetCTRLToggle(true);
             }
         }else DrawLogic::WriteDebug("key for in_VK "+std::to_string(in_VK)+" is not attributed");

         if (activeKeyName!=""){
             caller->SetSpecialKey(activeIsSpecialKey);
             caller->ProcessKeyPress(activeKeyName,activeKeyString);
             caller->CheckButtonsVisibility();
         }
         return 0;
     }


     if (!(flag&XPLM_KEY_UP)&&physicalKeyPressed&&(activeVKCode==in_VK)){
         //handle repeat
       float epoch=XPLMGetElapsedTime();
       if ((epoch-physicalKeyTimer)>latency){
           physicalKeyTimer=epoch;
           latency=0.2f;
           if (activeKeyName!=""){
              caller->SetSpecialKey(activeIsSpecialKey);
              caller->ProcessKeyPress(activeKeyName,activeKeyString);
              caller->CheckButtonsVisibility();
           }
       }
     }


     if ((flag&XPLM_KEY_UP)&&(in_VK==activeVKCode)){
         //first key pressed is going up
         physicalKeyPressed=false;
         activeVKCode=0;
         activeKeyName="";
         activeKeyString="";
         activeIsSpecialKey=false;
         caller->SetSpecialKey(false);
         myKeyboard->SetCTRLToggle(false);
         physicalKeyTimer=0;
         latency=0.6f;
     }

 return 0;
}

void    Keyboard::EnablePhysicalKeyboard(void *caller){
    if (!physicalKyBdEnabled) {
        XPLMRegisterKeySniffer(Physical_Key_Handler,0,caller);
        physicalKyBdEnabled=true;
        SetWarningMode(true);
    }
}

void    Keyboard::DisablePhysicalKeyboard(void *caller){
    if (physicalKyBdEnabled){
        XPLMUnregisterKeySniffer(Physical_Key_Handler,0,caller);
        physicalKyBdEnabled=false;
        SetWarningMode(false);
    }
}

void Keyboard::SetVisibility(bool vis){
    for (auto k: kL1) k->setVisibility(vis);
    for (auto k: kL2) k->setVisibility(vis);
    for (auto k: kL3) k->setVisibility(vis);
    for (auto k: kL4) k->setVisibility(vis);
    for (auto k: kL5) k->setVisibility(vis);
}

void Keyboard::ShowDebugKeys(){
for (auto k: kL1)  DrawLogic::WriteDebug("key "+k->GetKeyName()+" "+k->GetDebugName()+" drawNumber ",k->GetDrawNumber());
for (auto k: kL2)  DrawLogic::WriteDebug("key "+k->GetKeyName()+" "+k->GetDebugName()+" drawNumber ",k->GetDrawNumber());
for (auto k: kL3)  DrawLogic::WriteDebug("key "+k->GetKeyName()+" "+k->GetDebugName()+" drawNumber ",k->GetDrawNumber());
for (auto k: kL4)  DrawLogic::WriteDebug("key "+k->GetKeyName()+" "+k->GetDebugName()+" drawNumber ",k->GetDrawNumber());
for (auto k: kL5)  DrawLogic::WriteDebug("key "+k->GetKeyName()+" "+k->GetDebugName()+" drawNumber ",k->GetDrawNumber());
}
