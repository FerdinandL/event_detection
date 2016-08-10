#ifndef COLORMAPRED_H
#define COLORMAPRED_H

#include "ColorMap.hpp"

class ColorMapRed: public ColorMap
{
public:
    ColorMapRed();

protected:
    void transferFunctionCreator();
};

#endif // COLORMAPRED_H
