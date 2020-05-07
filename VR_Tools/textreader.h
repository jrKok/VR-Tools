#ifndef TEXTREADER_H
#define TEXTREADER_H
#include <string>
#include <chrono>
#include <stringops.h>
#include <fstream>
#include <vector>
#include <inisettings.h>
#include "VR_Tools_global.h"
#include "list_box_with_scrb.h"

class TextReader:public List_Box_With_ScrB
{
public:
    TextReader();
    ~TextReader();

    int freqADF,freqNAV;
    float freqCOM;

    bool   OpenFile(); //sets textFile and FileExists
    void   PointToFile();
    string GetFileName();
    string GetDirName();
    string GetStemFileName();
    virtual bool   ReadFileToBuff(); //defines MaxLineLength and reads all lines into buffer, breaks at spaces,"-","(",or")" or at maxLineLength
    bool   HasNav();
    bool   HasCom();
    bool   HasADF();
    bool   isADigit(std::string test);
    void   ShowAll();
    bool   Reload();
    bool   ReloadIfSizeChanged();
    void   closeReader();
    void   CheckForFrequencies();
    void   WriteDebug(std::string message);
    void   SetNeedsUTF(bool utf);
    void   AppendToStream();
    void   SetStreamingMode(bool in_stream);
    bool   GetStreamingMode();
    string GetCurrentFileName();
    string GetNavStr();
    string GetComStr();
    string GetADFStr();


protected:
    string fileName;
    string strBuffADF,strBuffNav,strBuffCom;
    string digits;
    std::fstream textFile;
    bool fileExists,needsUTF8,streamOn;
    bool hasNav,hasCom,hasADF;
    file_time_type fT;
    path filePath;
    std::uintmax_t keepsize;
    int filePos;



    void ResetFrequencies();

};

#endif // TEXTREADER_H
