#ifndef SHOW_FPS_TEST_H

/****************************************************************************************
 * DISCLAIMERS                                                                          *
 * **************************************************************************************
 * PROGRAM VR_TOOLS by Jeroen Kok (jrKok)
 * a plugin for X Plane
 * Implements functionality aimed at enhancing the use of virtual reality in X Plane
 * code published on Github
 * DEVELOPED in the Qt IDE (Qt company https://www.qt.io/company)
 * under GPL licence : https://www.gnu.org/licenses/gpl-3.0.txt
      This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

    Libraries used (credits to all developers) :

    *** Glew, The OpenGL Extension Wrangler Library github : https://github.com/nigels-com/glew
    *** Freetype, for font management https://www.freetype.org/
 **********************************************************************************************

* version 1.3.5
 *  Change log :
 *   modified the startup sequence to enable the plugin to be reloaded with leeceebaker's DataRefTool
 *   performance optimization (the plugin renders much faster now and is compliant with LR guidelines)
 *   rendering for text made "kneeboard" style
 *   improved text windows layout
 *   improved the layout of the window for file opening
 *   relabeled and reordered buttons
 *   remade the "streaming" function for text files
 *   made a kneeboard background (in real size)
 *   corrected bug in the hotspot editor when hotspot file wasn't generated when the config.txt was corrupt
 *     will now overwrite a correctly formated config.txt
 *   implementation of a file stack : open easily the previous file opened by a single click, with 5 files in the stack
 *  new menu, mirroring corresponding data commands
 *  integrated a font processor (GPL distributed freetype) to be able to render with different text sizes
 *  linux version
 *  show the GPL licence with the plugin (in documents section)
 *  new illustrated user manuals in docx format
 *
 * to come in version 1.3.6
 *
 *
 * for Data tool :
 *   FPS with bigger font if required
 *   show shear and turbulence in weather
 *   incorporate more data as per user request
 *   make a command disappear when click in center of data window either by option or general
 *
 * for Text tool :
 *   make clicking text easier by making touching lines, will be better for selection also
 *   reimplement UTF button
 *   linux system clipboard
 *   implement tab with fixed positions and a backtab
 *   reimplement delete lines in textreader (with a stack for deleted lined)
 *   reimplement inisettings with a map for parameters
 *   ATC reader (makes a commit to log, then reads ATC instructions), will do it on command
 *   make international keyboards (french, german, dutch, swedish, norsk, dansk, italian, espagnol, then greek, russian for 1.3.6 and 1.3.7)
 *   try to make a background image for layout and layout with edit, instead of kneeboard if interested
 *   try to implement graphic representation of advanced options (low priority)
 *    change logic for simultaneous keypresses
 *    create a "new file" command (for edit)
 *
 * for Hotspot :
 *  make visual assitance for hotspot advanced (upon difficulty or length, might be a 1.3.7 feature)
 *
 * General Code :
 *   make more error checks and workarounds for invalid file/directory names
 *   make a line drawer, a triangle drawer, a circle/disk drawer
 *   clean up code (segments in different directories : basic geometry, drivers, widgets, UI)
 *   rebuild iniparameter logic with map instead of vector (look at public json reader/writers)
     see if read file can be made asynchronous
 *   bring DRef window under drawlogic
 *
 * */


#if IBM
    #include <Windows.h>
#endif
#if LIN
    #include <cstring>
    #include <cstdint>
#endif

#ifndef XPLM303
    #error This is made to be compiled against the XPLM303 SDK
#endif
#include <memory>
#endif // SHOW_FPS_TEST_H
