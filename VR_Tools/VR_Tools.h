#ifndef SHOW_FPS_TEST_H
/*
 * Change log : put the "filter" command under VR Tools hierarchy
 *
 * Next :
 * make a stack for text files, to navigate easily from one file to another
 *
 *
 * future developments :
 *
 *version 1.3
 *
 *
 *
 *  - key shortcuts for text : RWY,ILS, CRS, HDG, BRG, VOR,NDB, ALT, qnh, hPa, WPT, FRQ, Comm,
 *    switch to,holding,flight level ... (2 chars or 1 spec Key)
 *
 *
 * version 1.3.4 :
 *
 *
 * move commands
 * menu entry
 * make helper dialogs for control keys
 * bug corrections
 * every custom command is now under the "VR Tools" entry in the custom command list

 *
 * version 1.3.5
 *  linux
 *  stack text files (keep stack in a json)
 *  modern openGL
 *  texture keyboard (could be done earlier)
 *  change logic for simultaneous keypresses
 *  create a "new file" command (for edit)
 *
 * */


#if IBM
    #include <Windows.h>
#endif
#if LIN
    #include <GL/gl.h>
    #include <cstring>
    #include <cstdint>
#else
  /*  #include <GL/glew.h>
    #include <gl/GL.h>*/
#endif

#ifndef XPLM301
    #error This is made to be compiled against the XPLM301 SDK
#endif
#endif // SHOW_FPS_TEST_H
