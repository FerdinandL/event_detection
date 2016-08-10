#ifndef COLORMAPBLUE_H
#define COLORMAPBLUE_H

#include "ColorMap.hpp"

class ColorMapBlue: public ColorMap
{
public:
    ColorMapBlue();
protected:
    void transferFunctionCreator();
};

#endif // COLORMAPBLUE_H
