#include "localclipboard.h"

string LocalClipBoard::clip ("");

LocalClipBoard::LocalClipBoard()
{

}

void LocalClipBoard::PushText(string text){
    clip=text;
}

string LocalClipBoard::PullText(){
    return clip;
}

bool LocalClipBoard:: HasContent(){
    if (clip!="") return true;
    else return false;
}
