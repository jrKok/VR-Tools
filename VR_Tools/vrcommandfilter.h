#ifndef VRCOMMANDFILTER_H
#define VRCOMMANDFILTER_H

#include "VR_Tools_global.h"
#include "XPLMDisplay.h"
#include "XPLMGraphics.h"
#include "XPLMDataAccess.h"
#include "XPLMPlugin.h"
#include "XPLMProcessing.h"
#include "XPLMUtilities.h"
#include "XPLMMenus.h"
#include <string>
#include <layout.h>

class VRCommandFilter
{
public:
    VRCommandFilter();
    void init();
    void UndoFiltering();
    static bool commandBlock,filterEnabled,g_in_vr;
    static std::vector<int> state;
    static std::vector<XPLMCommandRef> Commands;
    static std::vector<void*> refCons; //used by filter commands
    static XPLMCommandRef CommandFilter;
    static XPLMDataRef g_vr_dref;
    static int  MyFilterCommandHandler   (XPLMCommandRef inCommand, //This one receives the custom command
                                          XPLMCommandPhase   inPhase,
                                          void *             inRefcon);
    static int  MyBlockFilterCommandHandler (XPLMCommandRef     inCommand,//This one intercepts the commands of the registered stick commands
                                             XPLMCommandPhase   inPhase,
                                             void *             inRefcon);
    static bool SetupFiltering();

};

#endif // VRCOMMANDFILTER_H
