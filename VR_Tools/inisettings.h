#ifndef INISETTINGS_H
#define INISETTINGS_H
#include <string>
#include <string.h>
#include <vector>
#include <locale>
#include <filesystem>
#include <fstream>
#include <stringops.h>
#include <XPLMPlanes.h>
#include <VR_Tools_global.h>
#include <filepointer.h>

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
    void ReadIniFile();
    void WriteIniFile();
    void LogInstruction(string lf,string rg,string ct);
    void OrientFilePointer();

    bool GetOptStart();
    bool GetOptReload();
    bool GetOptUTF();
    bool GetOptFlash();
    bool GetOptLastPg();
    int  GetWidth();
    int  GetHeight();
    bool GetOptFit();
    bool GetOptFixed();
    bool GetOptKeepSize();
    bool GetOptBckG();
    string GetDir();
    string GetFile();
    bool GetOptLastFile();
    int  GetOptTrim();
    bool GetOptDelete();
    bool GetOptFreqs();
    bool GetOptFPS();
    bool GetOptReloadProc();
    int  GetReloadPeriod();

    void SetOptStart(bool opt);
    void SetOptReload(bool opt);
    void SetOptUTF(bool opt);
    void SetOptFlash(bool opt);
    void SetOptLastPg(bool opt);
    void SetWidth(int opt);
    void SetHeight(int opt);
    void SetOptFit(bool opt);
    void SetOptFixed(bool opt);
    void SetOptKeepSize(bool opt);
    void SetOptBckG(bool opt);
    void SetDir ();
    void SetFile();
    void SetOptLastFile(bool opt);
    void SetOptTrim(int opt);
    void SetOptDelete(bool opt);
    void SetOptFreqs(bool opt);
    void SetOptFPS(bool opt);
    string CodeTrimOption(int in_option);
    void SetOptReloadProc(bool opt);
    void SetReloadPeriod(int opt);
    void WriteOption(string optionName, int opt);
    void WriteOption(string optionName, bool opt);
    void WriteOption(string optionName, string opt);
    int  FindOption(string optionName);




static void WriteDebug(string message);
    stringOps ops;
    string iniFileName,iniDirectory,completeName;
    std::fstream iniFile;
    bool openWdwAtStart;
    bool autoReload;
    bool periodicReload;
    int  reloadPeriod;
    bool autoUTF8;
    bool changeClrOnTxtChg;
    bool moveLastPgOnStart;
    int width,height;
    bool fitToFile;
    bool noResize;
    bool keepSize;
    bool noBckground;
    string directory;
    string file;
    bool keepLastFile;
    int trimLineOption;
    bool deleteEnable;
    bool navsSetEnable;
    bool displayFPS;
    std::vector <string> leftH,rightH,comment;

};

#endif // INISETTINGS_H
