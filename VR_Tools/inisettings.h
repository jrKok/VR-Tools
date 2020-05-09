#ifndef INISETTINGS_H
#define INISETTINGS_H
#include <string>
#include <vector>
#include <locale>
#include <fstream>
#include "stringops.h"
#include "XPLMPlanes.h"
#include "VR_Tools_global.h"
#include "filepointer.h"

/*
* ini options :
*
* Presets : dev console
* start options
*  open_window_at_start : bool
*  autoreload_at_open : bool
* reload_on_file_size : bool
* reload_period_sec : int
*  flash_on_text_changed :bool
*  last_page_at_open : bool
* display :
*  width             : int
*  height            : int
*  fit_to_file       : bool
*  fixed_dimensions  : bool
*  keep_size         : bool
*  background        : bool
*
* file
*  default_directory : string
*  default_file      : string
*  keep_last_file    : bool
*  trim line option  : truncate line, split at space, split at max larg
* look into the nav/com/adf availability issue
*function :
*  can_delete        : bool
*  can_read_freqs    : bool
*Datarefs displayed
*/
using std::string;

class IniSettings
{
public:
    IniSettings();

    //build a vector of datarefs
static    void GetIniParams();
static    void ReadIniFile();
static    void WriteIniFile();
static    void LogInstruction(string lf,string rg,string ct);
static    void OrientFilePointer();

static    bool GetOptStart();
static    bool GetOptReload();
static    bool GetOptUTF();
static    bool GetOptFlash();
static    bool GetOptLastPg();
static    int  GetWidth();
static    int  GetHeight();
static    bool GetOptFit();
static    bool GetOptFixed();
static    bool GetOptKeepSize();
static    bool GetOptBckG();
static    string GetDir();
static    string GetFile();
static    bool GetOptLastFile();
static    int  GetOptTrim();
static    bool GetOptDelete();
static    bool GetOptFreqs();
static    bool GetOptFPS();
static    bool GetOptReloadProc();
static    float GetReloadPeriod();
static    int  GetSpeedMove();
static    bool GetOptReloadModel();
static    bool GetOptHSCommands();

static    void SetOptStart(bool opt);
static    void SetOptReload(bool opt);
static    void SetOptUTF(bool opt);
static    void SetOptFlash(bool opt);
static    void SetOptLastPg(bool opt);
static    void SetWidth(int opt);
static    void SetHeight(int opt);
static    void SetOptFit(bool opt);
static    void SetOptFixed(bool opt);
static    void SetOptKeepSize(bool opt);
static    void SetOptBckG(bool opt);
static    void SetDir ();
static    void SetFile();
static    void SetOptLastFile(bool opt);
static    void SetOptTrim(int opt);
static    void SetOptDelete(bool opt);
static    void SetOptFreqs(bool opt);
static    void SetOptFPS(bool opt);
static    void SetSpeedMove(int opt);
static    string CodeTrimOption(int in_option);
static    void SetOptReloadProc(bool opt);
static    void SetReloadPeriod(int opt);
static    void SetOptReloadModel(bool opt);
static    void SetOptHSCommands(bool opt);
static    void WriteOption(string optionName, int opt);
static    void WriteOption(string optionName, bool opt);
static    void WriteOption(string optionName, string opt);
static    int  FindOption(string optionName);

static    stringOps ops;
static    string iniFileName,iniDirectory,completeName;
static    std::fstream iniFile;
static    bool openWdwAtStart;
static    bool autoReload;
static    bool periodicReload;
static    float  reloadPeriod;
static    bool autoUTF8;
static    bool changeClrOnTxtChg;
static    bool moveLastPgOnStart;
static    int  width,height;
static    bool fitToFile;
static    bool noResize;
static    bool keepSize;
static    bool noBckground;
static    string directory;
static    string file;
static    bool keepLastFile;
static    int  trimLineOption;
static    bool deleteEnable;
static    bool navsSetEnable;
static    bool displayFPS;
static     int speedOfMove;
static    bool reloadModel;
static    bool makeHSCommands;
static    std::vector <string> leftH,rightH,comment;

};

#endif // INISETTINGS_H
