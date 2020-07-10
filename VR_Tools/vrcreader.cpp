#include "vrcreader.h"
#include "drawlogic.h"
#include "filepointer.h"

int VRCReader::activeHotspotNumber(0);
int VRCReader::numberOfHotspotsInfile(0);
int VRCReader::numberOfGeneratedHotspots(0);
string VRCReader::planeModelName("");
string VRCReader::vrConfigFileName("");
string VRCReader::vrConfigBackup("");
Lines VRCReader::vrconfigFile;
vector<LineDescriptor> VRCReader::analysis;
vector<Hotspot> *VRCReader::hotspots(new vector<Hotspot>());

VRCReader::VRCReader()
{

}

VRCReader::~VRCReader(){
    if (hotspots!=nullptr) delete(hotspots);
    hotspots=nullptr;
}

string VRCReader::GetVRConfigFileName() {
      char plName[512],plDir[1024];
      string planeDir("");
      XPLMGetNthAircraftModel(0,plName,plDir);
      string planePath=plDir;
      if (planePath!=""){
          path p=planePath;
          planeDir=p.parent_path().string();
          planeModelName=plName;
          planeModelName=planeModelName.erase(planeModelName.size()-4,4);//erase extension ".acf"
          vrConfigFileName=planeDir+"/"+planeModelName+"_vrconfig.txt";
          vrConfigBackup=planeDir+"/"+planeModelName+"_vrconfig.bck";
      }

      DrawLogic::WriteDebug("VR config file is "+vrConfigFileName);
      return planeModelName;
}

string VRCReader::GetVRConfig(){
    return vrConfigFileName;
}

 bool   VRCReader::HasVRConfig() {
      path pth=vrConfigFileName;
      return exists(pth);
  }

 bool VRCReader::HasHotspots(){
     return (hotspots->size()>0);
 }

 void   VRCReader::BuildVRConfig() {
     analysis.clear();
     vrconfigFile.clear();
     DrawLogic::WriteDebug("VR hotspots reader, going to build a valid VR config with a pilot's hotspot");
    int itr(0);
    AddLine(itr,"A",false);
    AddLine(itr,"1100",false);
    AddLine(itr,"VRCONFIG",false);
    AddLine(itr,"",false);
    AddLine(itr,"################################",false);
    AddLine(itr,"#      TELEPORT HOTSPOTS       ",false);
    AddLine(itr,"################################",false);
    AddLine(itr,"",false);
    AddLine(itr,"BEGIN_TELEPORT_HOTSPOT SITTING Pilot's Seat",true);
    AddLine(itr,"   AABB 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000",true);
    AddLine(itr,"   PRESET_XYZ 0.0000 0.0000 0.0000\n",true);
    AddLine(itr,"   PRESET_PSI 0.0000",true);
    AddLine(itr,"   PRESET_THE 0.0000",true);
    AddLine(itr,"   PRESET_PHI 0.0000",true);
    AddLine(itr,"   PRESET_PHI 0.0000",true);
    AddLine(itr,"END_TELEPORT_HOTSPOT",true);
 }

 void   VRCReader::AddLine(int &nb,string in_line,bool hsdef,int hsnum){
      vrconfigFile.push_back(in_line);

      LineDescriptor ld;
      ld.linenumber=nb;
      ld.isHotspotDef=hsdef;
      ld.hotspotNumber=hsnum;
      analysis.push_back(ld);
      nb++;
  }
 void VRCReader::ResetVRCReader(){
     numberOfGeneratedHotspots=0;
     numberOfHotspotsInfile=0;
     activeHotspotNumber=0;
     analysis.clear();
     hotspots->clear();
     vrconfigFile.clear();
 }

 bool  VRCReader::ReadVRConfig() {
      //open file
     std::fstream vrcFile;
     vrcFile.open(vrConfigFileName);
     if (vrcFile.is_open()){
      //read and copy lines
         vrconfigFile.clear();
         string nextLine;
         while (getline(vrcFile,nextLine))
             vrconfigFile.push_back(nextLine);
         vrcFile.close();
      //check first 3 lines and validate
         bool isVrValid(true);
         if (vrconfigFile.size()>=3){
             if (!stringOps::contains(vrconfigFile[0],"A")) isVrValid=false;
             if (!stringOps::contains(vrconfigFile[1],"1100")) isVrValid=false;
             if (!stringOps::contains(vrconfigFile[2],"VRCONFIG")) isVrValid=false;
             if (isVrValid==false) DrawLogic::WriteDebug("the "+vrConfigFileName+" is not valid");
         }
         else {
             isVrValid=false;
             //if not validated, return false
             DrawLogic::WriteDebug(vrConfigFileName+" is not a valid vrconfig file");
         }
         return isVrValid;
     }
     else{
         DrawLogic::WriteDebug("couldn't open "+vrConfigFileName);
         return false;
     }

  }
 void VRCReader::AnalyzeFile(){
     /*
      * This is the central routine for the vrconfig processing :
      * read the vrconfig, every line to the vector vrconfigFile
      * then put every line
      */
     ResetVRCReader();
     if (ReadVRConfig()){
         ulong itr(0),sze=vrconfigFile.size();
         DrawLogic::WriteDebug("VRCReader : read vrconfig, number of lines in file ",static_cast<int>(sze));
         LineDescriptor descr;
         while (itr<sze){
             AnalyzeLine(itr,descr);
             ulong newitr(0);
             if (descr.isHotspotDef) {
                 newitr=RetrieveHotspot(itr);
                 if (newitr!=0) itr=newitr;
             }
             itr++;
         }
     }
     else{DrawLogic::WriteDebug("VRCReader : couldn't read Vrconfig file");
         //if has VRConfig
         //the file is not valid
         //then make backup
         //and generate a valid vrconfig
     }
 }

  void   VRCReader::AnalyzeLine(ulong in_Line,LineDescriptor &descriptor) {
     descriptor.linenumber=static_cast<int>(in_Line);
     descriptor.isHotspotDef=false;
     descriptor.hotspotNumber=0;
     string processed=stringOps::Trim(vrconfigFile[in_Line]);
     string left(""),right("");
     left=LeftWord(processed,right);
     if (left=="BEGIN_TELEPORT_HOTSPOT") {
         descriptor.isHotspotDef=true;
     }
     else {
         analysis.push_back(descriptor);
     }


  }

  string VRCReader::LeftWord(const string &in_Line, string &remnant) {
      string left("");remnant="";
      if (stringOps::contains(in_Line," ")&&(in_Line!="")){
          left=stringOps::Trim(in_Line);
          auto posSpace=left.find(" ");
          if (posSpace!=string::npos){
              remnant=left.substr(posSpace+1);
              left=left.substr(0,posSpace);
              remnant=stringOps::Trim(remnant);
          }
          else  left=in_Line;
          return left;
      }else return in_Line;
  }

  ulong   VRCReader::RetrieveHotspot(ulong beginLineNumber) {
      ulong itr=beginLineNumber,sz=vrconfigFile.size();
      bool ended(false);      
      string left(""),right("");
      Hotspot hs;
      int hsN=numberOfGeneratedHotspots;
      numberOfGeneratedHotspots++;
      numberOfHotspotsInfile++;
      hs.hotspotNumber=hsN;
      left=vrconfigFile[itr];
      left=LeftWord(left,right);
      if (left=="BEGIN_TELEPORT_HOTSPOT") {
          LineDescriptor descriptor;
          descriptor.isHotspotDef=true;
          descriptor.hotspotNumber=hsN;
          descriptor.linenumber=static_cast<int>(itr);
          analysis.push_back(descriptor);
          left=right;
          if (left!=""){
              left=LeftWord(left,right);
              hs.type="SITTING";
              if (left==("SITTING")||left=="STANDING"){
                  hs.type=left;
                  hs.name=right;
              }
              else hs.name=left+" "+right;
          }
          if (hs.name=="") hs.name="hotspot"+std::to_string(hsN);
      } else return 0;

      itr++;
      while (!ended&&itr<sz){
          LineDescriptor descriptor;
          descriptor.isHotspotDef=true;
          descriptor.hotspotNumber=hsN;
          descriptor.linenumber=static_cast<int>(itr);
          analysis.push_back(descriptor);

          left=LeftWord(vrconfigFile[itr],right);
          if (left=="END_TELEPORT_HOTSPOT") ended=true;
          else{
              if (left=="AABB"){
                  hs.AABBminX=NextFloat(left,right);
                  hs.AABBminY=NextFloat(left,right);
                  hs.AABBminZ=NextFloat(left,right);
                  hs.AABBmaxX=NextFloat(left,right);
                  hs.AABBmaxY=NextFloat(left,right);
                  hs.AABBmaxZ=NextFloat(left,right);
              }
              if (left=="PRESET_XYZ"){
                  hs.PresetX=NextFloat(left,right);
                  hs.PresetY=NextFloat(left,right);
                  hs.PresetZ=NextFloat(left,right);
              }
              if (left=="PRESET_PSI"){
                  hs.psi=NextFloat(left,right);
              }
              if (left=="PRESET_THE"){
                  hs.the=NextFloat(left,right);
              }
              if (left=="PRESET_PHI"){
                  hs.phi=NextFloat(left,right);
              }
              itr++;
          }
      }
      hotspots->push_back(hs);
      return itr;
  }

float  VRCReader::NextFloat(string &in_left, string &in_right){
      in_left=in_right;
      if (in_left=="") return(0);
      else {
         in_left=LeftWord(in_left,in_right);}
      return stringOps::ConvertStringToFloat(in_left);
  }

void   VRCReader::SaveVRConfig() {
    path pO=vrConfigFileName;
    numberOfGeneratedHotspots=static_cast<int>(hotspots->size());
    int hspN(0);
    if (!HasVRConfig()||vrconfigFile.size()<=3)
        BuildVRConfig();
    else{
        path pBck=vrConfigBackup;
        if (!exists(pBck)) {
            copy_file (pO,pBck,copy_options::overwrite_existing);
            DrawLogic::WriteDebug("The original vrconfig is copied as "+vrConfigBackup);
        }
    }
    std::fstream cfgFile;
    cfgFile.open(pO,std::fstream::out|std::fstream::trunc);
    if (cfgFile.is_open()){
        ulong nbLines=vrconfigFile.size();
        ulong itr(0);
        while (itr<nbLines){
            if (analysis[itr].isHotspotDef){
                int thisHS=analysis[itr].hotspotNumber;
                if (hspN<numberOfGeneratedHotspots){
                    WriteHotspotToFile(cfgFile,static_cast<ulong>(hspN));
                    hspN++;
                }
                if (hspN>=numberOfHotspotsInfile)
                    while (hspN<numberOfGeneratedHotspots){
                        cfgFile<<"\n";
                        WriteHotspotToFile(cfgFile,static_cast<ulong>(hspN));
                        hspN++;
                    }
                while (itr<nbLines&&analysis[itr].isHotspotDef&&analysis[itr].hotspotNumber==thisHS)
                { itr++;if (itr>=nbLines) break;}
            }
            else
            {
                if (itr==3&&numberOfHotspotsInfile==0){
                    cfgFile<<"\n";
                    WriteHotspotToFile(cfgFile,0);
                }
                cfgFile<<vrconfigFile[itr]<<"\n";
                itr++;
            }
        }
        DrawLogic::WriteDebug("Replaced "+vrConfigFileName);
        cfgFile.close();
        FilePointer::MakeVRMirrorForCurrentPlane();
    }
    else DrawLogic::WriteDebug("Couldn't open vrconfig.txt "+pO.string()+" for overwriting");
}

void   VRCReader::WriteHotspotToFile(std::fstream &cFile,ulong indx){
    if (numberOfGeneratedHotspots>0){
        string toWrite("");
        toWrite="BEGIN_TELEPORT_HOTSPOT "+(*hotspots)[indx].type+" "+(*hotspots)[indx].name+"\n";
        cFile<<toWrite;
        toWrite="   AABB "
                +stringOps::ConvertFloatToString((*hotspots)[indx].AABBminX)+" "
                +stringOps::ConvertFloatToString((*hotspots)[indx].AABBminY)+" "
                +stringOps::ConvertFloatToString((*hotspots)[indx].AABBminZ)+" "
                +stringOps::ConvertFloatToString((*hotspots)[indx].AABBmaxX)+" "
                +stringOps::ConvertFloatToString((*hotspots)[indx].AABBmaxY)+" "
                +stringOps::ConvertFloatToString((*hotspots)[indx].AABBmaxZ)+"\n";
        cFile<<toWrite;
        toWrite="   PRESET_XYZ "
                +stringOps::ConvertFloatToString((*hotspots)[indx].PresetX)+" "
                +stringOps::ConvertFloatToString((*hotspots)[indx].PresetY)+" "
                +stringOps::ConvertFloatToString((*hotspots)[indx].PresetZ)+"\n";
        cFile<<toWrite;
        toWrite="   PRESET_PSI "
                +stringOps::ConvertFloatToString((*hotspots)[indx].psi)+"\n";
        cFile<<toWrite;
        toWrite="   PRESET_THE "
                +stringOps::ConvertFloatToString((*hotspots)[indx].the)+"\n";
        cFile<<toWrite;
        toWrite="   PRESET_PHI "
                +stringOps::ConvertFloatToString((*hotspots)[indx].phi)+"\n";
        cFile<<toWrite;
        toWrite="END_TELEPORT_HOTSPOT\n";
        cFile<<toWrite;
    }
}

void   VRCReader::SwapHotspots(int old_number, int new_number) {
    if (old_number<numberOfGeneratedHotspots&&new_number<numberOfGeneratedHotspots){
       ulong on=static_cast<ulong>(old_number);
       ulong nn=static_cast<ulong>(new_number);
       Hotspot ohs = (*hotspots)[on];
       Hotspot nhs = (*hotspots)[nn];
       (*hotspots)[nn]=ohs;
       (*hotspots)[on]=nhs;
       (*hotspots)[nn].hotspotNumber=new_number;
       (*hotspots)[on].hotspotNumber=old_number;
    }

}

void VRCReader::CreateHotspot(string in_name, float x, float y, float z, float rot, float pitch, float tilt){
    Hotspot hs;
    hs.name=in_name;
    hs.PresetX=x;
    hs.PresetY=y;
    hs.PresetZ=z;
    hs.psi=rot;
    hs.psi=0;
    /*if (rot>=45&&rot<135) hs.psi=90;
    if (rot>=135&&rot<225) hs.psi=180;
    if (rot>=225&&rot<315) hs.psi=270;*/
    hs.the=pitch;
    hs.phi=tilt;
    hs.AABBminX=x-0.3f;
    hs.AABBmaxX=x+0.3f;
    hs.AABBminY=y-0.65f;
    hs.AABBmaxY=y-0.45f;
    hs.AABBminZ=z-0.3f;
    hs.AABBmaxZ=z+0.3f;
    hs.type="SITTING";
    hs.hotspotNumber=numberOfGeneratedHotspots;
    hotspots->push_back(hs);
    GotoHotspotNumber(numberOfGeneratedHotspots);
    numberOfGeneratedHotspots++;
}

void   VRCReader::NextHotspot(){
    activeHotspotNumber++;
    if (activeHotspotNumber>=numberOfGeneratedHotspots)
        activeHotspotNumber=0;
}

void VRCReader::PreviousHotspot(){
    if (activeHotspotNumber==0)
        activeHotspotNumber=numberOfGeneratedHotspots-1;
    else {
        activeHotspotNumber--;
    }
}

int VRCReader::GetActiveHotspotNumber(){
    return activeHotspotNumber;
}

void   VRCReader::GotoHotspotNumber(int in_nb){
    activeHotspotNumber=in_nb;
    if (activeHotspotNumber>=numberOfGeneratedHotspots)
        activeHotspotNumber=0;
}

int VRCReader::GetHotspotCount(){
    return numberOfGeneratedHotspots;

}

bool   VRCReader::IsCurrentHotspotSitting(){
    return ((*hotspots)[static_cast<ulong>(activeHotspotNumber)].type=="SITTING");
}
void   VRCReader::GetCurrentHotspotCoords(float &x,float &y,float &z,float &rot, float&pitch, float &tilt) {
     Hotspot hs=(*hotspots)[static_cast<ulong>(activeHotspotNumber)];
     x=hs.PresetX;
     y=hs.PresetY;
     z=hs.PresetZ;
     rot=hs.psi;
     pitch=hs.the;
     tilt=hs.phi;
}

string VRCReader::GetCurrentHotspotName(){
  return ((*hotspots)[static_cast<ulong>(activeHotspotNumber)].name);
  }

void VRCReader::SetHotspotName(int in_Line,string in_Name){
    ulong uidx=static_cast<ulong>(in_Line);
    (*hotspots)[uidx].name=in_Name;
}

  void   VRCReader::SetHotspotCoords(int in_idx,float x, float y, float z, float rot, float pitch, float tilt) {
      ulong idx=static_cast<ulong>(in_idx);
      (*hotspots)[idx].PresetX=x;
      (*hotspots)[idx].PresetY=y;
      (*hotspots)[idx].PresetZ=z;
      (*hotspots)[idx].psi=rot;
      (*hotspots)[idx].psi=0;
      (*hotspots)[idx].the=pitch;
      (*hotspots)[idx].phi=tilt;
      (*hotspots)[idx].AABBminX=x-0.3f;
      (*hotspots)[idx].AABBmaxX=x+0.3f;
      (*hotspots)[idx].AABBminY=y-0.65f;
      (*hotspots)[idx].AABBmaxY=y-0.45f;
      (*hotspots)[idx].AABBminZ=z-0.3f;
      (*hotspots)[idx].AABBmaxZ=z+0.3f;
  }

Hotspot VRCReader::GetCurrentHotspot(){
    ulong aH=static_cast<ulong>(activeHotspotNumber);
    if (aH<hotspots->size())
  return (*hotspots)[aH];
    else {
        Hotspot hs;
        return hs;
    }
}

void VRCReader::SetAdvancedOptions(Hotspot in_HS){
    ulong aH=static_cast<ulong>(in_HS.hotspotNumber);
    if (in_HS.name==(*hotspots)[aH].name){
        (*hotspots)[aH].AABBmaxX=in_HS.AABBmaxX;
        (*hotspots)[aH].AABBmaxY=in_HS.AABBmaxY;
        (*hotspots)[aH].AABBmaxZ=in_HS.AABBmaxZ;
        (*hotspots)[aH].AABBminX=in_HS.AABBminX;
        (*hotspots)[aH].AABBminY=in_HS.AABBminY;
        (*hotspots)[aH].AABBminZ=in_HS.AABBminZ;
        (*hotspots)[aH].psi=in_HS.psi;
        (*hotspots)[aH].phi=in_HS.phi;
        (*hotspots)[aH].the=in_HS.the;
        (*hotspots)[aH].type=in_HS.type;
    }
    else {
        DrawLogic::WriteDebug("hotspots name don't match, advanced options not set, report this to the developer");
        DrawLogic::WriteDebug("hotspot received "+in_HS.name);
        DrawLogic::WriteDebug("hotspot number is "+std::to_string(in_HS.hotspotNumber));
        DrawLogic::WriteDebug("hotspot corresponding to number "+(*hotspots)[aH].name);
    }

}

vector<string> VRCReader::ExportHotspotNames(){
    vector<string> hsNames;
    for (auto hs:(*hotspots)){
        string hsName=hs.name;
        hsNames.push_back(hsName);
    }
    return hsNames;
}

void VRCReader::DeleteHotspot(int in_idx){
    hotspots->erase(hotspots->begin()+in_idx);
    numberOfGeneratedHotspots--;
    if (activeHotspotNumber>0) activeHotspotNumber--;
}
