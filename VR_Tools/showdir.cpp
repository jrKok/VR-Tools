#include "showdir.h"



ShowDir::ShowDir(DrawLogic *newPad) :
    myDrawPad(newPad),
    general(true),
    sliderBox(true),
    in_top(0),in_left(0),newT(0),newL(0),nR(0),nB(0),
    buttonPressed(-1), SectionPressed(-1),charHeight(0),hghDisp(190),
    waitForMouseUp(false),
    dirN(),
    fileN(),
    fileSelected(""),filePathSelected(""),
    myWindow(nullptr),
    displayLines(),
    buttons()

{  //Constructor

} //End Constructor

ShowDir::~ShowDir(){
    for (auto bt:buttons) {
        delete bt;
        bt=nullptr;
    }
    buttons.clear();
    for (auto ln:displayLines){
        delete ln;
        ln=nullptr;
    }
    delete myDrawPad;
    myDrawPad=nullptr;

}

//Helper functions

void ShowDir::MakeButton(){
    button_VR *button=new button_VR();
    button->SetOrigin(10,10);
    button->SetDimensions(50,20);
    button->setText("press");
    button->setVisibility(true);
    button->SetToStateColor();
    button->setTextColor(Clr_White);
    buttons.push_back(button);
}

void ShowDir::MakeLine(){
    TextLine *line=new TextLine();
    line->SetTextColor(Clr_White);
    line->SetBackGroundColor(Clr_Gray);
    displayLines.push_back(line);
}

//class members/functions

void ShowDir::SetupDirWindow(int left, int top){
    myDrawPad->Initiate();
    FilePointer::FindCurrentPlaneDir();
    if (displayLines.size()==0){

        for (int Cpt(0);Cpt<7;Cpt++)  MakeLine();
        for (int b(0);b<5;b++) MakeButton();
        XPLMGetFontDimensions(xplmFont_Proportional,nullptr,&charHeight,nullptr);
        general.setVisibility(true);
        general.setColor(Clr_Gray);
        DrawLogic::SetBackGroundColor(Clr_Gray);
        dirN.Setup(hghDisp,300,10,50);
        fileN.Setup(hghDisp,280,320,50);
        dirN.SetBckColor(Clr_PaperWhite);
        fileN.SetBckColor(Clr_PaperWhite);
        dirN.SetupDirectoryReader();
        fileN.SetDirToRead(dirN.GetActualPathName());
        in_top=top;
        in_left=left;

        //windowTitle = 0,dirTitle=1, fileTitle=2,DirSelected=3,FileSelected=4,FilePicker=5,DirReader=6

        int TitleSize=MeasureString(displayLines[dirTitle]->GetText());
        int fileTitleSize=MeasureString(displayLines[fileTitle]->GetText());
        int dirSelectedSize=MeasureString("Directory Selected :");
        int fileSelectedSize=MeasureString("File Selected :");

        general.SetOrigin(0,0);
        general.SetDimensions(fileN.GetLeft()+330,40+hghDisp+80);

        //button_ok=0,button_Cancel=1,button_SelDir=2,button_All=3,button_txt=4

        buttons[button_ok]->SetDimensions(30,20);
        buttons[button_ok]->SetOrigin(365,general.GetTop()-47);
        buttons[button_ok]->setText("OK");
        buttons[button_ok]->setVisibility(false);

        buttons[button_Cancel]->SetDimensions(40,20);
        buttons[button_Cancel]->SetOrigin(290,buttons[button_ok]->GetBottom());
        buttons[button_Cancel]->setText("Cancel");
        buttons[button_Cancel]->setVisibility(true);

        buttons[button_SelDir]->SetDimensions(40,20);
        buttons[button_SelDir]->SetOrigin(120,buttons[button_ok]->GetBottom());
        buttons[button_SelDir]->setText("Select");
        buttons[button_SelDir]->setVisibility(false);

        buttons[button_All]->SetDimensions(30,20);
        buttons[button_All]->SetOrigin(290,general.GetTop()-21);
        buttons[button_All]->setText("*.*");
        buttons[button_All]->setVisibility(true);

        buttons[button_txt]->SetDimensions(30,20);
        buttons[button_txt]->SetOrigin(buttons[button_All]->GetLeft()+30,buttons[button_All]->GetBottom());
        buttons[button_txt]->setText(".txt");
        buttons[button_txt]->setVisibility(false);

        sliderBox.SetOrigin(buttons[button_All]->GetLeft()-1,buttons[button_All]->GetBottom()-1);
        sliderBox.SetDimensions(65,22);
        sliderBox.setColor(Clr_White);
        sliderBox.setVisibility(true);

        general.SetOrigin(0,0);
        general.SetDimensions(fileN.GetLeft()+330,buttons[button_ok]->GetTop()+60);

        displayLines[windowTitle]->setText("Text files (.txt) in : .."+dirN.GetActualDirName());
        displayLines[windowTitle]->SetDimensions(320,charHeight+2);
        displayLines[windowTitle]->SetOrigin(sliderBox.GetRight()+2,sliderBox.GetBottom()+2);
        displayLines[windowTitle]->SetBackGroundColor(general.GetColor());

        displayLines[dirTitle]->setText("Choose a directory :");
        displayLines[dirTitle]->SetDimensions(TitleSize,charHeight+2);
        displayLines[dirTitle]->SetOrigin(10,fileN.GetTop()+charHeight+2);
        displayLines[dirTitle]->SetBackGroundColor(general.GetColor());

        displayLines[fileTitle]->setText("Click the file to display :");
        displayLines[fileTitle]->SetDimensions(fileTitleSize,charHeight+2);
        displayLines[fileTitle]->SetOrigin(320,fileN.GetTop()+charHeight+2);
        displayLines[fileTitle]->SetBackGroundColor(general.GetColor());

        displayLines[DirSelected]->setText("Directory Selected :");
        displayLines[DirSelected]->SetDimensions(dirSelectedSize,charHeight+2);
        displayLines[DirSelected]->SetOrigin(10,30);
        displayLines[DirSelected]->SetBackGroundColor(general.GetColor());

        displayLines[FileSelected]->setText("File Selected :");
        displayLines[FileSelected]->SetTextColor(Clr_White);
        displayLines[FileSelected]->SetDimensions(fileSelectedSize,charHeight+2);
        displayLines[FileSelected]->SetOrigin(320,displayLines[DirSelected]->GetBottom());
        displayLines[FileSelected]->SetBackGroundColor(general.GetColor());

        displayLines[FilePicker]->setText("");
        displayLines[FilePicker]->SetDimensions(fileN.GetWidth(),charHeight+4);
        displayLines[FilePicker]->SetTextColor(Clr_BlackInk);
        displayLines[FilePicker]->SetOrigin(fileN.GetLeft(),5);
        displayLines[FilePicker]->SetTextXY(2,2);
        displayLines[FilePicker]->SetBackGroundColor(Clr_PaperWhite);

        displayLines[DirReader]->setText("");
        displayLines[DirReader]->SetDimensions(dirN.GetWidth(),charHeight+4);
        displayLines[DirReader]->SetTextColor(Clr_BlackInk);
        displayLines[DirReader]->SetOrigin(dirN.GetLeft(),displayLines[FilePicker]->GetBottom());
        displayLines[DirReader]->SetTextXY(1,2);
        displayLines[DirReader]->SetBackGroundColor(Clr_PaperWhite);

        displayLines[DirReader]->PrintString();
        displayLines[FilePicker]->PrintString();
    }
    else{      
        wWidth=general.GetWidth();
        wHeight=general.GetHeight();
    }
}

void ShowDir::ActivateWindow(){
    wWidth=general.GetWidth();
    wHeight=general.GetHeight();
    myDrawPad->SetNewSize(wWidth,wHeight);
    dirN.ShowAll();
    fileN.ShowAll();
    myDrawPad->SetWindowH(myWindow);
    DrawLogic::UpdateTexture();
    if (XPLMWindowIsInVR(myWindow)){
        newT=nB+wHeight;
        nR=newL+wWidth;
        XPLMSetWindowGeometryVR(myWindow,wWidth,wHeight);
    }
    else{
        XPLMGetWindowGeometry(myWindow, &newL, &newT, &nR, &nB);
        newT=nB+wHeight;
        nR=newL+wWidth;
        XPLMSetWindowGeometry(myWindow,newL,newT,nR,nB);
    }
}

void ShowDir::DrawDirWindow(XPLMWindowID g_FileWindow){
    myDrawPad->ToUpperLevel();
    XPLMGetWindowGeometry(g_FileWindow, &newL, &newT, &nR, &nB);
    if ((newT-nB)!=wHeight|(nR-newL)!=wWidth){
        if (XPLMWindowIsInVR(myWindow)){
            newT=nB+wHeight;
            nR=newL+wWidth;
            XPLMSetWindowGeometryVR(myWindow,wWidth,wHeight);
        }
        else{
            newT=nB+wHeight;
            nR=newL+wWidth;
            XPLMSetWindowGeometry(myWindow,newL,newT,nR,nB);
        }
    }
    myDrawPad->SetScreenOrigin(newL,nB,nR,newT);
    myDrawPad->RenderElements();
}

int  ShowDir::processMouseDn(int x,int y){
    int cx(x-newL),cy(y-nB);
    buttonPressed=-1;
    SectionPressed=-1;
    if (dirN.ProceedClick(cx,cy))  {SectionPressed=DirReader; return DirReader;}
    if (fileN.ProceedClick(cx,cy)) {SectionPressed=FilePicker; return FilePicker;}
    for (ulong b(0);b<5;b++) {
        if (buttons[b]->isHere(cx,cy)) {
            buttonPressed=static_cast<int>(b);
        }
    }
    return buttonPressed;
}

void ShowDir::processMouseDrag(int x,int y){
    int cx(x-newL),cy(y-nB);

    if (SectionPressed==DirReader){
        dirN.ProceedClickCont(cx,cy);
        dirN.ColorFirstLines();
        return;
    }
    if (SectionPressed==FilePicker){
        fileN.ProceedClickCont(cx,cy);
        return;
    }
}

int  ShowDir::processMouseUp(int,int){

    int retVal=-1;
    if (buttonPressed>-1) {
        retVal=buttonPressed;
        switch (buttonPressed){
        case button_ok:{
            SelectFileLine();
            FilePointer::SetCurrentFileName(displayLines[FilePicker]->GetText());
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
                buttons[button_All]->setVisibility(false);
                buttons[button_txt]->setVisibility(true);
                UpdateDirInfo();
            }
            break;
        }
        case button_txt:{
            if (!fileN.GetTxtOption()) {
                fileN.ToggleAllTxt();
                fileN.ShowAll();
                buttons[button_All]->setVisibility(true);
                buttons[button_txt]->setVisibility(false);
                UpdateDirInfo();
            }
            break;
        }
        }
    }
    if (SectionPressed>-1) {

        if (SectionPressed==DirReader) {
            dirN.ProceedEndClick();
            if (dirN.HasSelection()){
                displayLines[DirReader]->setText(dirN.StringSelected());
                displayLines[DirReader]->PrintString();
                buttons[button_SelDir]->setVisibility(true);}
        }

        if (SectionPressed==FilePicker) {
            fileN.ProceedEndClick();
            if (fileN.HasSelection()){
                displayLines[FilePicker]->setText(fileN.StringSelected());
                displayLines[FilePicker]->PrintString();
                buttons[button_ok]->setVisibility(true);}
        }

    }

    fileN.SetInkColor(Clr_BlackInk);
    dirN.SetInkColor(Clr_BlackInk);
    dirN.ColorFirstLines();
    return retVal;
}

void ShowDir::SelectDir(){
    dirN.ReadSelectedDir();
    string current=dirN.GetActualPathName();
    fileN.SetDirToRead(current);
    buttons[button_ok]->setVisibility(false);
    buttons[button_SelDir]->setVisibility(false);
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
    displayLines[windowTitle]->setText(info);
    displayLines[windowTitle]->PrintString();
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

void ShowDir::SetWindowId(XPLMWindowID inw){
    myWindow=inw;
}
