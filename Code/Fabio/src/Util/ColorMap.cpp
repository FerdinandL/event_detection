#include "ColorMap.hpp"
#include <QColor>

unsigned int ColorMap::getColor(float val) {
    int index = val * (COLORMAP_SIZE - 1);
    if(index < 0) {
        index = 0;
    }
    if(index >= COLORMAP_SIZE) {
        index = COLORMAP_SIZE - 1;
    }
    return transferFunction[index];
}

QString ColorMap::getColorName(float val) {
    int index = val * (COLORMAP_SIZE - 1);
    if(index < 0) {
        index = 0;
    }
    if(index >= COLORMAP_SIZE) {
        index = COLORMAP_SIZE - 1;
    }
    int in = index * 4;
    float r = transferFunctionF[index];
    float g = transferFunctionF[index + 1];
    float b = transferFunctionF[index + 2];
    QColor col;
    col.setRedF(r);
    col.setBlueF(b);
    col.setGreenF(g);
    return col.name();
}


const QVector<unsigned int> ColorMap::getTransferFunction() {
//    return transferFunction.constData();
    return transferFunction;
}

const QVector<float> ColorMap::getTransferFunctionF()
{
    return transferFunctionF;
}

bool ColorMap::isDirty()
{
    return dirtyFlag;
}

void ColorMap::clearFlag()
{
    dirtyFlag = false;
}
