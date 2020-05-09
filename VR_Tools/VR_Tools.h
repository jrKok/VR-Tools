#ifndef SHOW_FPS_TEST_H
/*
 *
 * version 1.3.5
 *  Change log :
 *   performance optimization (the plugin renders much faster now and is compliant with LR guidelines)
 *   rendering for text made "kneeboard" style
 *   improved text windows layout
 *   improved the layout of the window for file opening
 *   relabeled buttons
 *   remade the "streaming" function for text files
 *   made a kneeboard background (in real size)
 *   corrected bug in the hotspot editor when hotspot file wasn't generated when the config.txt was corrupt
 *     will now overwrite a correctly formated config.txt
 *
 *  linux version
 *  make user manual
 *
 * to come in version 1.3.6
 *
 * bug fixes
 * linux system clipboard
 * ATC reader (makes a commit to log, then reads ATC instructions), will do it on command
 * implement better "tab" usage (with fixed tags spaced every 10 pixs, at least a space, over the whole line, every 10 pix)
 * make international keyboards (french, german, dutch, swedish, norsk, dansk, italian, espagnol, then greek, russian for 1.3.6)
 * try to make a background image for layout and layout with edit, instead of kneeboard if interested
 * try to implement graphic representation of advanced options (low priority)
   bring DRef window under drawlogic
 *  change logic for simultaneous keypresses
 *  create a "new file" command (for edit)
 * see if read file can be made asynchronous
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
