#ifndef BOXED_BUTTON_H
#define BOXED_BUTTON_H
#include "button_vr.h"
#include "globals.h"
/*This derived class makes a button with a box drawn around it. The geometry of the button
 * referes to the interior rectangle, the box will have a width and height of +2 wrt the
 * interior rectangle. By default the box will be white.  ( and thus be 1 pix wide white outlining).
 * The interior is set by the bottom and geometry variables this in order to preserve handling of
 * the geometry of handling the different states of the button.
 *
 * The real initialisation is done when the button receives the origin. Dimensions should be already set
 * otherwise the outlining will not be set correctly.
 *
*/

class Boxed_Button : public button_VR
{
public:
    Boxed_Button(string btext,bool modal);
virtual void setVisibility(bool vis);
virtual void SetOrigin(int in_x, int in_y);
virtual void Shift(int dx,int dy);
virtual void ReDrawButton();
        void SetBoxColor(char to_clr);


private:
 rectangles box_rect;
       char box_clr;
      ulong box_rect_number;
};

#endif // BOXED_BUTTON_H
