#ifndef FILENAMEPICKER_H
#define FILENAMEPICKER_H

#include "list_box_with_scrb.h"
#include "VR_Tools_global.h"

class FileNamePicker:public List_Box_With_ScrB
{
public:
    FileNamePicker();
    void SetDirToRead(std::string in_dirPath);
    std::string GetCompleteSelectedFName();
    std::string GetSelectedName();
    void ToggleAllTxt();
    bool GetTxtOption();
    void ShowAll();
    bool DirectoryEmpty();

private:
    std::string currentDir,systemCharSep;
    bool txtOnly,directoryEmpty;
};

#endif // FILENAMEPICKER_H
