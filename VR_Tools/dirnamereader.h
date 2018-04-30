#ifndef DIRNAMEREADER_H
#define DIRNAMEREADER_H

#include "list_box_with_scrb.h"
#include <filesystem>

class dirNameReader:public List_Box_With_ScrB
{
public:

    dirNameReader();
    void    SetDirectory(std::string dirN);
std::string GetParentPath();
std::string GetActualDirName();
std::string GetActualPathName();
    void    ReadRootDir();
    void    ShowAll();
    void    ReadSelectedDir();

private:
    std::string parentPath,actualPath,actualDirName,systemCharSep,sysDir,shortCutBck,shortCutXP,ShortCutPlug,ShortCutF;
    //      name of parent,full path,without path ,separator

    std::string GetDirNameFromPath();
    std::string GetParentPathFromPath();
    std::string GetSelectedDirPath();
};

#endif // DIRNAMEREADER_H
