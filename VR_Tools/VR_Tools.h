#ifndef SHOW_FPS_TEST_H
/*
 *
 * version 1.3.5

 * redecorate layout and layoutwedit windows :
 *   relocate buttons the new ones also
 *   make next/previous buttons for layout stack, close and hide button for above's zone
 *   that is button for hide kneeboard, hide paper only (show decoration and next/previous/load button)
 * decorate kneeboard with upper banner with file name
 * implement file stack
 * decorate kneeboard with texts
 *
 * try to see if move commands can be accelerated in 11.50
 * try to find a private dataref for log.txt
 *
 *  compile on linux
 *
 * send out for tests
 * bug fixes
 * publish
 *
 * version 1.3.6
 *
 * bug fixes
 * make international keyboards (french, german, dutch, swedish, norsk, dansk, italian, espagnol, then greek, russion for 1.3.6)
 *  * try to make a background image for layout and layout with edit
 * try to implement graphic representation of advanced options (low priority)
   bring DRef window under drawlogic
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
