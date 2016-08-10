#ifndef COLORMAPCATEGORICAL_H
#define COLORMAPCATEGORICAL_H

#include "ColorMap.hpp"

class ColorMapCategorical: public ColorMap
{
public:
    ColorMapCategorical();
    int getNumCategories();
protected:
    void transferFunctionCreator();
    int numCategories;
};

#endif // COLORMAPCATEGORICAL_H
