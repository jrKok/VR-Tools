#include "showdir.h"


ShowDir::ShowDir() :
    general(),
    in_top(0),in_left(0),newT(0),newL(0),nR(0),nB(0),
    buttonPressed(-1), SectionPressed(-1),charHeight(0),hghDisp(190),
    ink{0.120f,0.120f,0.120f},background{0.900f,0.900f,0.850f},backgroundS{0.720f,0.720f,0.690f},bckgW{0.150f,0.180f,0.250f},
    dirN(),
    fileN(),
    fileSelected(""),filePathSelected(""),
    displayLines(),
    buttons()

{  //Constructor

    TextLine line;
    button_VR button;
    button.isVisible=true;
    for (int Cpt(0);Cpt<7;Cpt++)  displayLines.push_back(line);
    for (int b(0);b<5;b++) buttons.push_back(button);


    XPLMGetFontDimensions(xplmFont_Proportional,NULL,&charHeight,NULL);

} //End Constructor

//Helper functions

void ShowDir::WriteDebug(string message){
    string in_String="VR Tools : " +message+"\n";
    XPLMDebugString((char*)in_String.c_str());
}

//class members/functions

void ShowDir::SetupDirWindow(int left, int top){

    dirN.Setup(hghDisp,300,10,40);
    fileN.Setup(hghDisp,280,320,40);

    dirN.SetupDirectoryReader();
    fileN.SetDirToRead(dirN.GetActualPathName());

    in_top=top;
    in_left=left;

//windowTitle = 0,dirTitle=1, fileTitle=2,DirSelected=3,FileSelected=4,FilePicker=5,DirReader=6
displayLines[windowTitle].setText("Text files (.txt) in : .."+dirN.GetActualDirName());
displayLines[windowTitle].width=280;
displayLines[windowTitle].height=charHeight+2;
displayLines[windowTitle].offSetX=320;
displayLines[windowTitle].offSetY=13;

displayLines[dirTitle].setText("Choose a directory :");
displayLines[dirTitle].width=XPLMMeasureString(xplmFont_Proportional,(char*)displayLines[dirTitle].textOfLine.c_str(),displayLines[dirTitle].mySize);
displayLines[dirTitle].height=charHeight+2;
displayLines[dirTitle].offSetX=10;
displayLines[dirTitle].offSetY=27;

displayLines[fileTitle].setText("Click the file to display :");
displayLines[fileTitle].width=XPLMMeasureString(xplmFont_Proportional,(char*)displayLines[fileTitle].textOfLine.c_str(),displayLines[fileTitle].mySize);
displayLines[fileTitle].height=charHeight+2;
displayLines[fileTitle].offSetX=320;
displayLines[fileTitle].offSetY=27;

displayLines[DirSelected].setText("Directory Selected :");
displayLines[DirSelected].width=XPLMMeasureString(xplmFont_Proportional,(char*)displayLines[DirSelected].textOfLine.c_str(),displayLines[DirSelected].mySize);
displayLines[DirSelected].height=charHeight+2;
displayLines[DirSelected].offSetX=10;
displayLines[DirSelected].offSetY=hghDisp+50;

displayLines[FileSelected].setText("File Selected :");
displayLines[FileSelected].width=XPLMMeasureString(xplmFont_Proportional,(char*)displayLines[FileSelected].textOfLine.c_str(),displayLines[DirSelected].mySize);
displayLines[FileSelected].height=charHeight+2;
displayLines[FileSelected].offSetX=320;
displayLines[FileSelected].offSetY=displayLines[DirSelected].offSetY;

displayLines[FilePicker].setText("");
displayLines[FilePicker].width= 200;
displayLines[FilePicker].height=charHeight+6;
displayLines[FilePicker].offSetX=320;
displayLines[FilePicker].offSetY=displayLines[FileSelected].offSetY+charHeight+12;

displayLines[DirReader].setText("");
displayLines[DirReader].width= 280;
displayLines[DirReader].height=charHeight+6;
displayLines[DirReader].offSetX=10;
displayLines[DirReader].offSetY=displayLines[FilePicker].offSetY;

//button_ok=0,button_Cancel=1,button_SelDir=2,button_All=3,button_txt=4

buttons[button_ok].width=30;
buttons[button_ok].height=20;
buttons[button_ok].offsetX=365;
buttons[button_ok].offsetY=displayLines[FilePicker].offSetY+charHeight+2;
buttons[button_ok].isVisible=false;
buttons[button_ok].setText("OK");

buttons[button_Cancel].width=40;
buttons[button_Cancel].height=20;
buttons[button_Cancel].offsetX=290;
buttons[button_Cancel].offsetY=buttons[button_ok].offsetY;
buttons[button_Cancel].isVisible=true;
buttons[button_Cancel].setText("Cancel");

buttons[button_SelDir].width=40;
buttons[button_SelDir].height=20;
buttons[button_SelDir].offsetX=120;
buttons[button_SelDir].offsetY=buttons[button_ok].offsetY;
buttons[button_SelDir].isVisible=false;
buttons[button_SelDir].setText("Select");

buttons[button_All].width=30;
buttons[button_All].height=20;
buttons[button_All].offsetX=500;
buttons[button_All].offsetY=15;
buttons[button_All].setText("*.*");

buttons[button_txt].width=30;
buttons[button_txt].height=20;
buttons[button_txt].offsetX=530;
buttons[button_txt].offsetY=15;
buttons[button_txt].isVisible=false;
buttons[button_txt].setText(".txt");

general.in_top=in_top-100;
general.in_left=in_left+100;
general.offsetX=0;
general.offsetY=0;
general.width=fileN.GetOffSetX()+330;
general.height=buttons[button_ok].offsetY+30;
general.recalculate();

dirN.ShowAll();
fileN.ShowAll();

}

void ShowDir::RecalculateDirWindow(){
    if ((in_top!=newT)|(in_left!=newL)){
        in_top=newT;
        in_left=newL;
        general.recalculate(in_left,in_top);

        for (ulong lg(0);lg<7;lg++) displayLines[lg].recalculate(in_left,in_top);
        for (auto & bt:buttons) {
            bt.in_left=in_left;
            bt.in_top=in_top;
            bt.recalculate();
        }
        fileN.Recalculate(in_left,in_top);
        dirN.Recalculate(in_left,in_top);

    }
}

void ShowDir::DrawDirWindow(XPLMWindowID g_FileWindow){
    XPLMSetGraphicsState(
            0 /* no fog */,
            0 /* 0 texture units */,
            0 /* no lighting */,
            0 /* no alpha testing */,
            1 /* do alpha blend */,
            1 /* do depth testing */,
            0 /* no depth writing */
    );

    XPLMGetWindowGeometry(g_FileWindow, &newL, &newT, &nR, &nB);
    RecalculateDirWindow();
    glColor3fv(bckgW);
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    glBegin(GL_TRIANGLE_FAN);
      {
      glVertex2i(general.left, general.top);
      glVertex2i(general.right, general.top);
      glVertex2i(general.right, general.bottom);
      glVertex2i(general.left, general.bottom);
      }
    glEnd();
    dirN.DrawMySelf();
    fileN.DrawMySelf();
    for (auto bt:buttons) {
        bt.drawButton();}
    for (ulong l(0);l<5;l++) {
        int wdt=displayLines[l].width;
        XPLMDrawString(background,displayLines[l].x,displayLines[l].y,(char*)displayLines[l].textOfLine.c_str(),&wdt,xplmFont_Proportional);
    }
    for (ulong ll(5);ll<7;ll++){
        glColor3fv(backgroundS);
            glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
            glBegin(GL_TRIANGLE_FAN);
              {
              glVertex2i(displayLines[ll].x-2, displayLines[ll].top);
              glVertex2i(displayLines[ll].right, displayLines[ll].top);
              glVertex2i(displayLines[ll].right, displayLines[ll].y-2);
              glVertex2i(displayLines[ll].x-2, displayLines[ll].y-2);
              }
            glEnd();
            int wdt=displayLines[ll].width;
            XPLMDrawString(ink,displayLines[ll].x,displayLines[ll].y,(char*)displayLines[ll].textOfLine.c_str(),&wdt,xplmFont_Proportional);
    }
}

int  ShowDir::processMouseDn(int x,int y){
    SectionPressed=-1;
    if (dirN.ProceedClick(x,y)) {SectionPressed=DirReader; return DirReader;}
    if (fileN.ProceedClick(x,y)) {SectionPressed=FilePicker; return FilePicker;}
    for (ulong b(0);b<5;b++) {
        if (buttons[b].isHere(x,y)) {
            buttonPressed=static_cast<int>(b);
        }
    }
    return buttonPressed;
}

void ShowDir::processMouseDrag(int x,int y){
    if (SectionPressed==DirReader){
        dirN.ProceedClickCont(x,y);
        return;
    }
    if (SectionPressed==FilePicker){
        fileN.ProceedClickCont(x,y);
        return;
    }
}

int  ShowDir::processMouseUp(int x,int y){
    int retVal=-1;
    if (buttonPressed>-1) {
        retVal=buttonPressed;
        switch (buttonPressed){
        case button_ok:{
            SelectFileLine();
            FilePointer::SetCurrentFileName(displayLines[FilePicker].GetText());
            FilePointer::SetCurrentDirName(dirN.GetActualPathName());
            break;
        }
        case button_Cancel:{
            fileSelected="";
            filePathSelected="";
            break;
        }
        case button_SelDir:{
            SelectDir();
            break;
        }
        case button_All:{
            if (fileN.GetTxtOption()) {
                fileN.ToggleAllTxt();
                fileN.ShowAll();
                buttons[button_All].isVisible=false;
                buttons[button_txt].isVisible=true;
                UpdateDirInfo();
            }
            break;
        }
        case button_txt:{
            if (!fileN.GetTxtOption()) {
                fileN.ToggleAllTxt();
                fileN.ShowAll();
                buttons[button_All].isVisible=true;
                buttons[button_txt].isVisible=false;
                UpdateDirInfo();
            }
            break;
        }
        }
    }
    if (SectionPressed>-1) {
        if (SectionPressed==FilePicker) {
            fileN.ProceedEndClick();
            if (fileN.HasSelection()){
                displayLines[FilePicker].setText(fileN.StringSelected());
                buttons[button_ok].isVisible=true;}
        }
        if (SectionPressed==DirReader) {
            dirN.ProceedEndClick();
            if (dirN.HasSelection()){
                displayLines[DirReader].setText(dirN.StringSelected());
                buttons[button_SelDir].isVisible=true;}
        }
    }

    buttonPressed=-1;
    SectionPressed=-1;
    return retVal;
}

void ShowDir::SelectDir(){
    dirN.ReadSelectedDir();
    string current=dirN.GetActualPathName();
    fileN.SetDirToRead(current);
    buttons[button_ok].isVisible=false;
    buttons[button_SelDir].isVisible=false;
    UpdateDirInfo();
}

void ShowDir::SetDirToSearch(string dir){
    dirN.SetDirectory(dir);
    fileN.SetDirToRead(dir);
}

void ShowDir::UpdateDirInfo(){
    string info("");
    info = (fileN.GetTxtOption()?"Text files (.txt) in ":"All files (*.*) in ");
    info = info+dirN.GetActualDirName();
    displayLines[windowTitle].setText(info);
}

int  ShowDir::MeasureString(string in_String){
    return ((int)XPLMMeasureString(xplmFont_Proportional,(char*)in_String.c_str(),in_String.size()));
}

void ShowDir::CloseDirWindow(){

}

void ShowDir::SelectFileLine(){
    fileSelected=fileN.GetSelectedName();
    filePathSelected=fileN.GetCompleteSelectedFName();
}

string ShowDir::GetSelectedFile(){
    return fileSelected;
}
string ShowDir::GetSelectedPath(){
    return filePathSelected;
}
