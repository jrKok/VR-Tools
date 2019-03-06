#include "showdir.h"
#include "drawlogic.h"


ShowDir::ShowDir(int in_number) :
    myWindow(in_number),
    general(true),
    leftR(true),
    rightR(true),
    slider(true),
    in_top(0),in_left(0),newT(0),newL(0),nR(0),nB(0),
    buttonPressed(-1), SectionPressed(-1),charHeight(0),hghDisp(190),
    waitForMouseUp(false),
    dirN(),
    fileN(),
    fileSelected(""),filePathSelected(""),
    displayLines(),
    buttons()

{  //Constructor


} //End Constructor

//Helper functions

void ShowDir::WriteDebug(string message){
    string in_String="VR Tools : " +message+"\n";
    XPLMDebugString((char*)in_String.c_str());
}

void ShowDir::MakeButton(){
    button_VR button;
    button.setVisibility(true);
    button.SetToStateColor();
    button.setTextColor(Clr_White);
    buttons.push_back(button);
}

void ShowDir::MakeLine(){
    TextLine line;
    line.SetTextColor(Clr_White);
    displayLines.push_back(line);
}

//class members/functions

void ShowDir::SetupDirWindow(int left, int top){
    DrawLogic::ShowMyWindow(myWindow);
    FilePointer::FindCurrentPlaneDir();
    if (displayLines.size()==0){
        for (int Cpt(0);Cpt<7;Cpt++)  MakeLine();
        for (int b(0);b<5;b++) MakeButton();
        XPLMGetFontDimensions(xplmFont_Proportional,nullptr,&charHeight,nullptr);
        general.setVisibility(true);
        general.setColor(Clr_Gray);
        dirN.Setup(hghDisp,300,10,40);
        fileN.Setup(hghDisp,280,320,40);
        dirN.SetBckColor(Clr_PaperWhite);
        fileN.SetBckColor(Clr_PaperWhite);

        dirN.SetupDirectoryReader();
        fileN.SetDirToRead(dirN.GetActualPathName());

        in_top=top;
        in_left=left;

        //windowTitle = 0,dirTitle=1, fileTitle=2,DirSelected=3,FileSelected=4,FilePicker=5,DirReader=6
        displayLines[windowTitle].setText("Text files (.txt) in : .."+dirN.GetActualDirName());
        displayLines[windowTitle].SetDimensions(280,charHeight+2);
        displayLines[windowTitle].SetOffsets(320,13);

        int TitleSize=MeasureString(displayLines[dirTitle].GetText());
        int fileTitleSize=MeasureString(displayLines[fileTitle].GetText());
        int dirSelectedSize=MeasureString(displayLines[DirSelected].GetText());
        int fileSelectedSize=MeasureString(displayLines[FileSelected].GetText());

        displayLines[dirTitle].setText("Choose a directory :");
        displayLines[dirTitle].SetDimensions(TitleSize,charHeight+2);
        displayLines[dirTitle].SetOffsets(10,27);

        displayLines[fileTitle].setText("Click the file to display :");
        displayLines[fileTitle].SetDimensions(fileTitleSize,charHeight+2);
        displayLines[fileTitle].SetOffsets(320,27);

        displayLines[DirSelected].setText("Directory Selected :");
        displayLines[DirSelected].SetDimensions(dirSelectedSize,charHeight+2);
        displayLines[DirSelected].SetOffsets(10,hghDisp+50);

        displayLines[FileSelected].setText("File Selected :");
        displayLines[FileSelected].SetDimensions(fileSelectedSize,charHeight+2);
        displayLines[FileSelected].SetOffsets(320,displayLines[DirSelected].GetOffsetY());

        displayLines[FilePicker].setText("");
        displayLines[FilePicker].SetDimensions(200,charHeight+6);
        displayLines[FilePicker].SetTextColor(Clr_BlackInk);
        displayLines[FilePicker].SetOffsets(320,displayLines[FileSelected].GetOffsetY()+charHeight+12);

        displayLines[DirReader].setText("");
        displayLines[DirReader].SetDimensions(280,charHeight+6);
        displayLines[DirReader].SetTextColor(Clr_BlackInk);
        displayLines[DirReader].SetOffsets(10,displayLines[FilePicker].GetOffsetY());

        //button_ok=0,button_Cancel=1,button_SelDir=2,button_All=3,button_txt=4

        buttons[button_ok].SetDimensions(30,20);
        buttons[button_ok].SetOffsets(365,displayLines[FilePicker].GetOffsetY()+charHeight+2);
        buttons[button_ok].setText("OK");
        buttons[button_ok].setVisibility(false);

        buttons[button_Cancel].SetDimensions(40,20);
        buttons[button_Cancel].SetOffsets(290,buttons[button_ok].GetOffsetY());
        buttons[button_Cancel].setText("Cancel");
        buttons[button_Cancel].setVisibility(true);

        buttons[button_SelDir].SetDimensions(40,20);
        buttons[button_SelDir].SetOffsets(120,buttons[button_ok].GetOffsetY());        
        buttons[button_SelDir].setText("Select");
        buttons[button_SelDir].setVisibility(false);

        buttons[button_All].SetDimensions(30,20);
        buttons[button_All].SetOffsets(500,15);        
        buttons[button_All].setText("*.*");
        buttons[button_All].setVisibility(true);

        buttons[button_txt].SetDimensions(30,20);
        buttons[button_txt].SetOffsets(buttons[button_All].GetOffsetX()+30,buttons[button_All].GetOffsetY());
        buttons[button_txt].setText(".txt");
        buttons[button_txt].setVisibility(false);

        slider.SetOffsets(buttons[button_All].GetOffsetX(),buttons[button_All].GetOffsetY()-1);
        slider.SetDimensions(62,22);
        slider.setColor(Clr_White);
        slider.setVisibility(true);

        general.SetOrigin(in_left+100,in_top-100);
        general.SetOffsets(0,0);
        general.SetDimensions(fileN.GetOffSetX()+330,buttons[button_ok].GetOffsetY()+30);
        general.recalculate();

        dirN.ShowAll();
        fileN.ShowAll();

        displayLines[FilePicker].recalculate();
        leftR.SetAngles(displayLines[FilePicker].GetX()-2,displayLines[FilePicker].GetTop()-2,displayLines[FilePicker].GetRight()+2,displayLines[FilePicker].GetBottom()-3);
        leftR.SetOffsets(displayLines[FilePicker].GetOffsetXRect(),displayLines[FilePicker].GetOffsetYRect()+2);
        leftR.setColor(Clr_PaperWhite);
        leftR.setVisibility(true);
        displayLines[DirReader].recalculate();
        rightR.SetAngles(displayLines[DirReader].GetX()-2,displayLines[DirReader].GetTop()-2,displayLines[DirReader].GetRight()+2,displayLines[DirReader].GetBottom()-3);
        rightR.SetOffsets(displayLines[DirReader].GetOffsetXRect(),displayLines[DirReader].GetOffsetYRect()+2);
        rightR.setColor(Clr_PaperWhite);
        rightR.setVisibility(true);
    }
    else{
        dirN.ShowAll();
        fileN.ShowAll();
    }
}

void ShowDir::RecalculateDirWindow(){
    if ((in_top!=newT)|(in_left!=newL)){
        in_top=newT;
        in_left=newL;
        general.recalculate(in_left,in_top);

        for (ulong lg(0);lg<7;lg++) displayLines[lg].recalculate(in_left,in_top);
        for (auto & bt:buttons) {
            bt.SetOrigin(in_left,in_top);
            bt.recalculate();
        }
        fileN.Recalculate(in_left,in_top);
        dirN.Recalculate(in_left,in_top);
        leftR.recalculate(in_left,in_top);
        rightR.recalculate(in_left,in_top);
        slider.recalculate(in_left,in_top);

    }
}

void ShowDir::DrawDirWindow(XPLMWindowID g_FileWindow){
    XPLMGetWindowGeometry(g_FileWindow, &newL, &newT, &nR, &nB);
    RecalculateDirWindow();
    DrawLogic::DrawElements();
    DrawLogic::DrawStrings();
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
        fileN.SetInkColor(Clr_BlackInk);
        dirN.SetInkColor(Clr_BlackInk);
        dirN.ColorFirstLines();
        return;
    }
    if (SectionPressed==FilePicker){
        fileN.ProceedClickCont(x,y);
        fileN.SetInkColor(Clr_BlackInk);
        dirN.SetInkColor(Clr_BlackInk);
        dirN.ColorFirstLines();
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
                buttons[button_All].setVisibility(false);
                buttons[button_txt].setVisibility(true);
                UpdateDirInfo();
            }
            break;
        }
        case button_txt:{
            if (!fileN.GetTxtOption()) {
                fileN.ToggleAllTxt();
                fileN.ShowAll();
                buttons[button_All].setVisibility(true);
                buttons[button_txt].setVisibility(false);
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
                buttons[button_ok].setVisibility(true);}
        }
        if (SectionPressed==DirReader) {
            dirN.ProceedEndClick();
            if (dirN.HasSelection()){
                displayLines[DirReader].setText(dirN.StringSelected());
                buttons[button_SelDir].setVisibility(true);}
        }
    }

    buttonPressed=-1;
    SectionPressed=-1;
    fileN.SetInkColor(Clr_BlackInk);
    dirN.SetInkColor(Clr_BlackInk);
    dirN.ColorFirstLines();
    return retVal;
}

void ShowDir::SelectDir(){
    dirN.ReadSelectedDir();
    string current=dirN.GetActualPathName();
    fileN.SetDirToRead(current);
    buttons[button_ok].setVisibility(false);
    buttons[button_SelDir].setVisibility(false);
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
    float mySize=XPLMMeasureString(xplmFont_Proportional,DrawLogic::ToC(in_String),static_cast<int>(in_String.size()));
    int retSize=static_cast<int>(mySize);
    return retSize;
}

int  ShowDir::GetRight(){return general.GetRight();}
int  ShowDir::GetTop(){return general.GetTop();}
int  ShowDir::GetBottom(){return general.GetBottom();}
int  ShowDir::GetLeft(){return general.GetLeft();}
int  ShowDir::GetWidth(){return general.GetWidth();}
int  ShowDir::GetHeight(){return general.GetHeight();}

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
