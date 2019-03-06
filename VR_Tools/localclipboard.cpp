#include "localclipboard.h"
#include "WinUser.h"
#include "layout.h"
#include "drawlogic.h"

string LocalClipBoard::clip ("");

LocalClipBoard::LocalClipBoard()
{

}

void LocalClipBoard::PushText(string text){
    clip=text;
    if (text=="") return;
    //push onto windows clipboard if available
    if (OpenClipboard(nullptr)){
        EmptyClipboard();
        char* toClip=DrawLogic::ToC(text);
        HGLOBAL buff=GlobalAlloc(GMEM_MOVEABLE,text.size()+1);
        if (buff!=nullptr){
            void* finalStr=GlobalLock(buff);
            memcpy(finalStr,toClip,text.size()+1);
            GlobalUnlock(buff);
            SetClipboardData(CF_TEXT,buff);
        }
        CloseClipboard();
    }
}

string LocalClipBoard::PullText(){
    if (clip==""){
        //try windows clipboard
        if (OpenClipboard(nullptr)){
            HGLOBAL hText;
            hText=GetClipboardData(CF_TEXT);
            if (hText!=nullptr){
                void * str=GlobalLock(hText);
                if (str!=nullptr)
                {
                    Layout::WriteDebug("read sring");
                    char* wClipStr=reinterpret_cast<char*>(str);
                    clip=wClipStr;
                }
                GlobalUnlock(hText);
            }
            else clip="";
            CloseClipboard();
        }
    }
    return clip;
}

bool LocalClipBoard:: HasContent(){
    if (clip!="") return true;
    else return false;
}
