#ifndef DRAWMODALLOGIC_H
#define DRAWMODALLOGIC_H

#include <drawlogic.h>

class drawModalLogic: public DrawLogic
{
public:
    drawModalLogic();
    static void  EnableModalWindow();
    static bool  IsModalWindowActive();
};

#endif // DRAWMODALLOGIC_H
