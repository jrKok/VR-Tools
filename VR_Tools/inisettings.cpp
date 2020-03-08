#include "inisettings.h"


   stringOps IniSettings::ops;
 string  IniSettings::iniFileName="VR_Tools.ini";
 string  IniSettings::iniDirectory("");
 string  IniSettings::completeName("");
   std::fstream IniSettings::iniFile;
   bool  IniSettings::IniSettings::openWdwAtStart(false);//ini parameters to be read
   bool  IniSettings::IniSettings::autoReload(false);
   bool  IniSettings::periodicReload(true);
   float IniSettings::reloadPeriod(1);
   bool  IniSettings::autoUTF8(false);
   bool  IniSettings::changeClrOnTxtChg(false); //to create a flash effect when text changes
   bool  IniSettings::moveLastPgOnStart(false);
   int   IniSettings::width(350);
   int   IniSettings::height(470);
   bool  IniSettings::fitToFile(false);
   bool  IniSettings::noResize(false);
   bool  IniSettings::keepSize(false);
   bool  IniSettings::noBckground(false);
 string  IniSettings::directory("output/textfiles");
 string  IniSettings::file("flightnotes.txt");
   bool  IniSettings::keepLastFile(false);
   int   IniSettings::trimLineOption(0);
   bool  IniSettings::deleteEnable(true);
   bool  IniSettings::navsSetEnable(true);
   bool  IniSettings::displayFPS(false);
   int   IniSettings::speedOfMove(1);
   bool  IniSettings::reloadModel(false);
   bool  IniSettings::makeHSCommands(false);
   std::vector <string>  IniSettings::leftH;
   std::vector <string>  IniSettings::rightH;
   std::vector <string>  IniSettings::comment;

   IniSettings::IniSettings(){

   }

void IniSettings::GetIniParams()

{
    //look for ini file

    char fileN[512],dirP[1024];
    XPLMGetNthAircraftModel(0,fileN,dirP);
    iniDirectory=dirP;
    completeName=iniDirectory+iniFileName;
    iniFile.open(completeName,std::ifstream::in);

    if (iniFile.is_open()){
        ReadIniFile();
    }else{
        iniDirectory="Resources/plugins/VR_Tools/";
        completeName=iniDirectory+iniFileName;
        iniFile.open(completeName,std::ifstream::in);
        if (iniFile.is_open()) {
            iniFile.close();
            ReadIniFile();
        }else{
            WriteIniFile();
        }
    }
   WriteDebug ("Initialisation Parameters read from : "+completeName);
}


void IniSettings::WriteDebug(string message){
    string in_String="VR Tools : " +message+"\n";
    XPLMDebugString((char*)in_String.c_str());
}

void IniSettings::OrientFilePointer(){
    char sysPath[512];
    XPLMGetSystemPath(sysPath);
    string sysDir=sysPath;//conversion from char[]
    sysDir=sysDir.substr(0,(sysDir.size()-1));//remove trailing "/"
    string systemCharSep=XPLMGetDirectorySeparator();
    FilePointer::DirSeparator=systemCharSep;
    FilePointer::SetCurrentFileName(file);
    if (directory!=""){
        if (directory.substr(1,1)==":"){
            FilePointer::SetCurrentDirName(directory);
        }
        else {
            FilePointer::SetCurrentDirName(sysDir+systemCharSep+directory);}
    }
    else FilePointer::SetCurrentDirName(sysDir+systemCharSep);
}

void IniSettings::ReadIniFile(){

    leftH.clear();rightH.clear();comment.clear();
    iniFile.open(completeName,std::ifstream::in);
    string inputL("");
    if (iniFile.is_open()){
        while (getline(iniFile,inputL)){
            string leftSide(""),rightSide(""),commt("");
            leftSide=ops.DecodeInstruction(inputL,rightSide,commt);
            if ((leftSide=="")&&(commt!="")) LogInstruction(leftSide,rightSide,commt);
            else{//decode instruction
            if (leftSide=="OPEN_TEXT_ON_START") {
                if (rightSide=="yes") {openWdwAtStart=true; LogInstruction(leftSide,rightSide,commt);}
                if (rightSide=="no") {openWdwAtStart=false; LogInstruction(leftSide,rightSide,commt);}
            }
            if (leftSide=="AUT0_RELOAD"){
                if(rightSide=="yes"){autoReload=true; LogInstruction(leftSide,rightSide,commt);}
                if(rightSide=="no"){autoReload=false; LogInstruction(leftSide,rightSide,commt);}
            }
            if (leftSide=="AUTO_UTF8"){
                if(rightSide=="yes"){autoUTF8=true; LogInstruction(leftSide,rightSide,commt);}
                if(rightSide=="no"){autoUTF8=false; LogInstruction(leftSide,rightSide,commt);}
            }
            if (leftSide=="PERIODIC_RELOAD"){
                if (rightSide=="yes"){periodicReload=true; LogInstruction(leftSide,rightSide,commt);}
                if (rightSide=="no"){periodicReload=false; LogInstruction(leftSide,rightSide,commt);}
            }
            if (leftSide=="RELOAD_PERIOD"){
                int rP(1);
               try{
                rP=std::stoi(rightSide);}
                catch(std::invalid_argument& e){
                    rP=1;}
                reloadPeriod=static_cast<float>(rP);
                LogInstruction(leftSide,string(" "+std::to_string(rP)),commt);
            }
            if (leftSide=="FLASH_TEXT_ON_CHANGE"){
                if(rightSide=="yes"){changeClrOnTxtChg=true; LogInstruction(leftSide,rightSide,commt);}
                if(rightSide=="no"){changeClrOnTxtChg=false; LogInstruction(leftSide,rightSide,commt);}
            }
            if (leftSide=="MOVE_TO_LAST_PAGE"){
                if(rightSide=="yes"){moveLastPgOnStart=true; LogInstruction(leftSide,rightSide,commt);}
                if(rightSide=="no"){moveLastPgOnStart=false; LogInstruction(leftSide,rightSide,commt);}
            }
            if (leftSide=="WIDTH"){
                int textWidth(0);
               try{
                textWidth=std::stoi(rightSide);}
                catch(std::invalid_argument& e){
                    textWidth=350;}
                if (textWidth<230) textWidth=230;
                if (textWidth>1500) textWidth=1500;
                width=textWidth;
                LogInstruction(leftSide,string(" "+std::to_string(width)),commt);
            }
            if (leftSide=="HEIGHT"){
                int textHeight(0);
               try{
                textHeight=std::stoi(rightSide);}
                catch(std::invalid_argument& e){
                    textHeight=470;}
                if (textHeight<120) textHeight=120;
                if (textHeight>900) textHeight=900;
                height=textHeight;
                LogInstruction(leftSide,string(" "+std::to_string(height)),commt);
            }
            if (leftSide=="FIT_TO_FILE"){
                if(rightSide=="yes"){fitToFile=true; LogInstruction(leftSide,rightSide,commt);}
                if(rightSide=="no"){fitToFile=false; LogInstruction(leftSide,rightSide,commt);}
            }
            if (leftSide=="NO_RESIZE"){
                if(rightSide=="yes"){noResize=true; LogInstruction(leftSide,rightSide,commt);}
                if(rightSide=="no"){noResize=false; LogInstruction(leftSide,rightSide,commt);}
            }
            if (leftSide=="KEEP_SIZE"){
                if(rightSide=="yes"){keepSize=true; LogInstruction(leftSide,rightSide,commt);}
                if(rightSide=="no"){keepSize=false; LogInstruction(leftSide,rightSide,commt);}
            }
            if (leftSide=="NO_BACKGROUND"){
                if(rightSide=="yes"){noBckground=true; LogInstruction(leftSide,rightSide,commt);}
                if(rightSide=="no"){noBckground=false; LogInstruction(leftSide,rightSide,commt);}
            }
            if (leftSide=="DIRECTORY"){
                    if (rightSide=="") directory=iniDirectory;
                    else{
                    std::experimental::filesystem::path pt=rightSide;
                    if (std::experimental::filesystem::is_directory(pt)){directory=rightSide;}
                    else directory=iniDirectory;
                    }
                    LogInstruction(leftSide,rightSide,commt);
            }
            if (leftSide=="FILE"){
                if (rightSide!=""){
                    file=rightSide;
                    LogInstruction(leftSide,rightSide,commt);
                }
            }

            if (leftSide=="KEEP_LAST_FILE"){
                if(rightSide=="yes"){keepLastFile=true; LogInstruction(leftSide,rightSide,commt);}
                if(rightSide=="no"){keepLastFile=false; LogInstruction(leftSide,rightSide,commt);}
            }

            if (leftSide=="TRIM_LINE_OPTION"){
                if (rightSide=="split_at_space"){trimLineOption=BestSplitAtSpace;LogInstruction(leftSide,rightSide,commt);}
                if (rightSide=="split_at_width"){trimLineOption=ForceSplitAtSize;LogInstruction(leftSide,rightSide,commt);}
                if (rightSide=="no_clip")       {trimLineOption=DontTruncate;LogInstruction(leftSide,rightSide,commt);}
                if (rightSide=="clip_left")     {trimLineOption=TruncateKeepLeft;LogInstruction(leftSide,rightSide,commt);}
                if (rightSide=="clip_right")    {trimLineOption=TruncateKeepRight;LogInstruction(leftSide,rightSide,commt);}

            }

            if (leftSide=="ENABLE_DELETE"){
                if(rightSide=="yes"){deleteEnable=true; LogInstruction(leftSide,rightSide,commt);}
                if(rightSide=="no") {deleteEnable=false; LogInstruction(leftSide,rightSide,commt);}
            }

            if (leftSide=="FREQUENCY_SET_ENABLE"){
                if(rightSide=="yes "){navsSetEnable=true; LogInstruction(leftSide,rightSide,commt);}
                if(rightSide=="no")  {navsSetEnable=false; LogInstruction(leftSide,rightSide,commt);}
            }

            if (leftSide=="DISPLAY_FPS"){
                if(rightSide=="yes"){displayFPS=true; LogInstruction(leftSide,rightSide,commt);}
                if(rightSide=="no") {displayFPS=false; LogInstruction(leftSide,rightSide,commt);}
            }

            if (leftSide=="SPEED_OF_MOVE"){
                if (rightSide=="slow")    {speedOfMove=0;LogInstruction(leftSide,rightSide,commt);}
                if (rightSide=="adjusted"){speedOfMove=1;LogInstruction(leftSide,rightSide,commt);}
                if (rightSide=="fast")    {speedOfMove=2;LogInstruction(leftSide,rightSide,commt);}
            }
            if (leftSide=="RELOAD_MODEL_ON_NEW_HOTSPOT"){
                if (rightSide=="yes"){reloadModel=true;LogInstruction(leftSide,rightSide,commt);}
                if (rightSide=="no") {reloadModel=false;LogInstruction(leftSide,rightSide,commt);}
            }
            if (leftSide=="MAKE_COMMANDS_FOR_HOTSPOTS"){
                if (rightSide=="yes"){makeHSCommands=true;LogInstruction(leftSide,rightSide,commt);}
                if (rightSide=="no") {makeHSCommands=false;LogInstruction(leftSide,rightSide,commt);}
            }
        }
        }

        iniFile.close();
    }
}

void IniSettings::LogInstruction(string lf,string rg,string ct){
    leftH.push_back(lf);
    rightH.push_back(rg);
    comment.push_back(ct);
}

void IniSettings::WriteIniFile(){
    if (leftH.size()==0){
        //build standard .ini
        leftH.push_back("");rightH.push_back("");                          comment.push_back("; lines beginning with ; are comments, comments can also be written after a command");
        leftH.push_back("");rightH.push_back("");                          comment.push_back("; yes/no in undercase, names and strings can be put in quotes and can then contain a semi colon");
        leftH.push_back("         OPEN_TEXT_ON_START");rightH.push_back("no");    comment.push_back("");//; yes/no if yes then the text window will be open by default, handy for developpers");
        leftH.push_back("                  AUTO_UTF8");rightH.push_back("no");    comment.push_back("");//; yes/no if yes the text will convert all chars>128 to UTF8 chars (should not be done if file is already UTF8");
        leftH.push_back("                AUT0_RELOAD");rightH.push_back("no");    comment.push_back("");//; yes/no if yes the text will reload according to the reload option");
        leftH.push_back("            PERIODIC_RELOAD");rightH.push_back("yes");   comment.push_back("");//;if no, reload on change in file size, if yes set the period below");
        leftH.push_back("              RELOAD_PERIOD");rightH.push_back("1");     comment.push_back("");//;reload every RELOAD_PERIOD second");
        leftH.push_back("");rightH.push_back("");                          comment.push_back("");//; in a future version, check of regular UTF characters will be automated");
        leftH.push_back("       FLASH_TEXT_ON_CHANGE");rightH.push_back("no");    comment.push_back("");//;yes/no if yes the text will change color when a change is detected after reload");
        leftH.push_back("          MOVE_TO_LAST_PAGE");rightH.push_back("no");    comment.push_back("");//;yes/no if yes the last page will be immediately displayed after loading (dev mode)");
        leftH.push_back("                      WIDTH");rightH.push_back("355");   comment.push_back("");//;Width of Text display, the window is about 50px wider");
        leftH.push_back("                     HEIGHT");rightH.push_back("470");   comment.push_back("");//;Height of the Text display, the window is about 60 px higher");
        leftH.push_back("                FIT_TO_FILE");rightH.push_back("no");    comment.push_back("");//;yes/no if yes the text display will be sized to a best compromise for holding text tightly");
        leftH.push_back("                  NO_RESIZE");rightH.push_back("no");    comment.push_back("");//;yes/no if yes the user won't be able to resize the display, in a non-titled Window, no hide button");
        leftH.push_back("                  KEEP_SIZE");rightH.push_back("no");    comment.push_back("");//;yes/no if yes the size of a window will be kept in the ini after user resizes wdw");
        leftH.push_back("                  DIRECTORY");rightH.push_back("output/Textfiles");comment.push_back("");//;first directory where text files will be searched, if empty, XPlane's system directory");
        leftH.push_back("                       FILE");rightH.push_back("flightnotes.txt");comment.push_back("");//;Filename, will be searched in previous directory");
        leftH.push_back("             KEEP_LAST_FILE");rightH.push_back("no");    comment.push_back("");//;yes/no if yes the last opened file will be the first opened file on restart");
        leftH.push_back("           TRIM_LINE_OPTION");rightH.push_back("split_at_space");comment.push_back(";split_at_space,split_at_width,no_clip,clip_left,clip_right");
        leftH.push_back("              NO_BACKGROUND");rightH.push_back("no");    comment.push_back("");//;yes/no if yes the text will be white on the standard dark background of the custom window");
        leftH.push_back("              ENABLE_DELETE");rightH.push_back("yes");   comment.push_back("");//;yes/no if no the text the lines cannot be removed from the display, no buttons will be displayed in the left column");
        leftH.push_back("       FREQUENCY_SET_ENABLE");rightH.push_back("yes");   comment.push_back("");//;yes/no if no, frequencies read in the line cannot be moved to the corresponding stand-by freq");
        leftH.push_back("                DISPLAY_FPS");rightH.push_back("no");    comment.push_back("");//;yes/no if yes FPS will be permanently displayed");
        leftH.push_back("              SPEED_OF_MOVE");rightH.push_back("adjusted"); comment.push_back("");//;selects how to move to a hotspot : adjusted,fast, slow;
        leftH.push_back("RELOAD_MODEL_ON_NEW_HOTSPOT");rightH.push_back("no");    comment.push_back("//;yes/no if yes, the current AC model will be reloaded (with art assets) when a hotspot is created or relocated");
        leftH.push_back(" MAKE_COMMANDS_FOR_HOTSPOTS");rightH.push_back("no");    comment.push_back("//; yes/no, if yes the plugin will make a specific command enabling to move to every one plane's hotspot");
    }
        //write .ini from vectors
    iniFile.open(completeName,std::fstream::out|std::fstream::trunc);
    auto len=leftH.size();
    for (auto ln(0);ln<len;ln++){
        string left=leftH[ln],right=rightH[ln];
        string wrt=left+((left=="")?"":" = ")+right+((right=="")?"":" ")+comment[ln]+"\n";
        iniFile<<wrt;
    }
    iniFile.close();
}

bool IniSettings::GetOptStart()           {return openWdwAtStart;}
bool IniSettings::GetOptReload()          {return autoReload;}
bool IniSettings::GetOptUTF()             {return autoUTF8;}
bool IniSettings::GetOptFlash()           {return changeClrOnTxtChg;}
bool IniSettings::GetOptLastPg()          {return moveLastPgOnStart;}
int  IniSettings::GetWidth()              {return width;}
int  IniSettings::GetHeight()             {return height;}
bool IniSettings::GetOptFit()             {return fitToFile;}
bool IniSettings::GetOptFixed()           {return noResize;}
bool IniSettings::GetOptKeepSize()        {return keepSize;}
bool IniSettings::GetOptBckG()            {return noBckground;}
string IniSettings::GetDir()         {return directory;}
string IniSettings::GetFile()        {return file;}
bool IniSettings::GetOptLastFile()        {return keepLastFile;}
int  IniSettings::GetOptTrim()            {return trimLineOption;}
bool IniSettings::GetOptDelete()          {return deleteEnable;}
bool IniSettings::GetOptFreqs()           {return navsSetEnable;}
bool IniSettings::GetOptFPS()             {return displayFPS;}
bool IniSettings::GetOptReloadProc()      {return periodicReload;}
float IniSettings::GetReloadPeriod()      {return reloadPeriod;}
int  IniSettings::GetSpeedMove()          {return speedOfMove;}
bool IniSettings::GetOptReloadModel()     {return reloadModel;}
bool IniSettings::GetOptHSCommands()      {return makeHSCommands;}

void IniSettings::SetOptStart(bool opt)     {openWdwAtStart=opt;    WriteOption("OPEN_TEXT_ON_START",opt);}
void IniSettings::SetOptReload(bool opt)    {autoReload=opt;        WriteOption("AUT0_RELOAD",opt);}
void IniSettings::SetOptReloadProc(bool opt){periodicReload=opt;    WriteOption("PERIODIC_RELOAD",opt);}
void IniSettings::SetReloadPeriod(int opt)  {reloadPeriod=static_cast<float>(opt);      WriteOption("RELOAD_PERIOD",opt);}
void IniSettings::SetOptUTF(bool opt)       {autoUTF8 =opt;         WriteOption("AUTO_UTF8",opt);}
void IniSettings::SetOptFlash(bool opt)     {changeClrOnTxtChg=opt; WriteOption("FLASH_TEXT_ON_CHANGE",opt);}
void IniSettings::SetOptLastPg(bool opt)    {moveLastPgOnStart =opt;WriteOption("MOVE_TO_LAST_PAGE",opt);}
void IniSettings::SetWidth(int opt)         {width=opt;             WriteOption("WIDTH",opt);}
void IniSettings::SetHeight(int opt)        {height =opt;           WriteOption("HEIGHT",opt);}
void IniSettings::SetOptFit(bool opt)       {fitToFile =opt;        WriteOption("FIT_TO_FILE",opt);}
void IniSettings::SetOptFixed(bool opt)     {noResize =opt;         WriteOption("NO_RESIZE",opt);}
void IniSettings::SetOptKeepSize(bool opt)  {keepSize=opt;          WriteOption("KEEP_SIZE",opt);}
void IniSettings::SetDir ()  {directory =FilePointer::GetCurrentDirName();       WriteOption("DIRECTORY",directory);}
void IniSettings::SetFile()       {file =FilePointer::GetCurrentFileName();      WriteOption("FILE",file);}
void IniSettings::SetOptLastFile(bool opt)  {keepLastFile =opt;     WriteOption("KEEP_LAST_FILE",opt);}
void IniSettings::SetOptTrim(int opt)       {trimLineOption =opt;   WriteOption("TRIM_LINE_OPTION",CodeTrimOption(opt));}
void IniSettings::SetOptBckG(bool opt)      {noBckground =opt;      WriteOption("NO_BACKGROUND",opt);}
void IniSettings::SetOptDelete(bool opt)    {deleteEnable =opt;     WriteOption("ENABLE_DELETE",opt);}
void IniSettings::SetOptFreqs(bool opt)     {navsSetEnable =opt;    WriteOption("FREQUENCY_SET_ENABLE",opt);}
void IniSettings::SetOptFPS(bool opt)       {displayFPS =opt;       WriteOption("DISPLAY_FPS",opt);}
void IniSettings::SetSpeedMove(int opt)     {
    speedOfMove=opt;
    switch (opt){
        case 0 :{WriteOption("SPEED_OF_MOVE",string("slow"));break;}
        case 1 :{WriteOption("SPEED_OF_MOVE",string("adjusted"));break;}
        case 2 :{WriteOption("SPEED_OF_MOVE",string("fast"));break;}
    }
}
void IniSettings::SetOptHSCommands(bool opt) {makeHSCommands=opt;   WriteOption("MAKE_COMMANDS_FOR_HOTSPOTS",opt);}
void IniSettings::SetOptReloadModel(bool opt){reloadModel=opt;      WriteOption("RELOAD_MODEL_ON_NEW_HOTSPOT",opt);}

void IniSettings::WriteOption(string optionName, int opt){
    int tag=FindOption(optionName);
    if (tag>-1){
        rightH[tag]=std::to_string(opt);
        WriteIniFile();
    }
}

void IniSettings::WriteOption(string optionName, bool opt){
    int tag=FindOption(optionName);
    if (tag>-1){
        rightH[tag]=(opt?"yes":"no");
        WriteIniFile();
    }
}

void IniSettings::WriteOption(string optionName, string opt){
    int tag=FindOption(optionName);
    if (tag>-1){
        rightH[tag]=opt;
        WriteIniFile();
    }
}

int  IniSettings::FindOption(string optionName){
    for (int ret(0);ret<leftH.size();ret++){
        if (leftH[ret]==optionName) return ret;
    }
    leftH.push_back(optionName);
    rightH.push_back("");
    comment.push_back("");
    return (leftH.size()-1);
}


string IniSettings::CodeTrimOption(int in_option){
    switch(in_option){
    case(BestSplitAtSpace):{
        return "split_at_space";
    }
    case(ForceSplitAtSize):{
        return "split_at_width";
    }
    case(DontTruncate):{
        return "no_clip";
    }
    case(TruncateKeepLeft):{
        return "clip_left";
    }
    case(TruncateKeepRight):{
        return "clip_right";
    }
    }
    return("");
}

