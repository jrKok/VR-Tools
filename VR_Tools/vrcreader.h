#ifndef VRCREADER_H
#define VRCREADER_H

#include <string>
#include "VR_Tools_global.h"
#include "XPLMPlanes.h"
#include "stringops.h"


/*This class locates and reads the active plane's VR config file,
 *
 * or generates one if it doesn't exist
 * it also makes a sort of iterator in the list of hotspots
 */

using std::string;
using std::vector;
typedef   std::vector<string> Lines;
struct LineDescriptor
{
    int linenumber;
    bool isHotspotDef;
    int hotspotNumber;
};

struct Hotspot
{
    string name; //comes after hotspot type in the file
    string type; //SITTING(hotspot=head) or STANDING (hotspot=foot)
    int    hotspotNumber; //internal ordering number
    float  AABBminX,AABBminY,AABBminZ,AABBmaxX,AABBmaxY,AABBmaxZ;//Binding box defs
    float  PresetX, PresetY, PresetZ;//the position of the hotspot
    float  psi;//0-360°,rotation of the head 90 = right wind
    float  the;//-90 to 90°, pitch of the head 90 to the sky, -90 to the bottom
    float  phi;//-180 to +180, tilt of the head -90 tilt on left ear, 90 tilt on right ear
};

class VRCReader
{
public:
                  VRCReader();
                  ~VRCReader();
    static string GetVRConfigFileName();
    static bool   HasVRConfig();
    static bool   HasHotspots();
    static void   BuildVRConfig();
    static void   AddLine(int &nb, string in_line, bool hsdef, int hsnum=0);
    static void   ResetVRCReader();
    static bool   ReadVRConfig();
    static void   AnalyzeFile();
    static void   AnalyzeLine(ulong in_Line, LineDescriptor &descriptor);
    static string LeftWord(const std::string &in_Line, string &remnant);
    static ulong  RetrieveHotspot(ulong beginLineNumber);
    static float  NextFloat(string &in_left, string &in_right);
    static void   SaveVRConfig();
    static void   SwapHotspots(int old_number, int new_number);
    static void   CreateHotspot(string in_name,float x,float y,float z,float rot, float pitch, float tilt);
    static void   ReloadAircraft();
    static void   NextHotspot();
    static void   PreviousHotspot();
    static int    GetActiveHotspotNumber();
    static void   GotoHotspotNumber(int in_nb);
    static int    GetHotspotCount();
    static void   GetCurrentHotspotCoords(float &x,float &y,float &z,float &rot, float&pitch, float &tilt);
    static string GetCurrentHotspotName();
    static void   SetHotspotName(int in_Line,string in_Name);
    static bool   IsCurrentHotspotSitting();
    static void   SetHotspotCoords(int in_idx, float x, float y, float z, float rot, float pitch, float tilt);//buils binding box also, parameters to be set from ini file
    static void   SetAdvancedOptions(Hotspot in_HS);
    static void   WriteHotspotToFile(std::fstream &cFile,ulong indx);
    static Hotspot GetCurrentHotspot();
    static vector<string> ExportHotspotNames();
    static void   DeleteHotspot(int in_idx);

private:
    static int activeHotspotNumber,numberOfHotspotsInfile,numberOfGeneratedHotspots;
    static string planeModelName,vrConfigFileName,vrConfigBackup;
    static string digits;
    static Lines vrconfigFile;
    static vector<LineDescriptor> analysis;
    static vector<Hotspot> * hotspots;

};

#endif // VRCREADER_H
