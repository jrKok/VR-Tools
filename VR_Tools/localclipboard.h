#ifndef LOCALCLIPBOARD_H
#define LOCALCLIPBOARD_H

#include <string>
#include <stringops.h>

using std::string;

class LocalClipBoard
{
public:
    LocalClipBoard();
    static void PushText(string);
    static string PullText();
    static bool HasContent();
private:
    static string clip;
};

#endif // LOCALCLIPBOARD_H
