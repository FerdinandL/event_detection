#ifndef COLORMAP_HPP
#define COLORMAP_HPP

#include <QVector>

#define COLORMAP_SIZE 1024

class ColorMap {

public:
    ColorMap(){}

    // assumes val is between 0 and 1
    unsigned int getColor(float val);
    const QVector<unsigned int> getTransferFunction();
    const QVector<float> getTransferFunctionF();
    QString getColorName(float val);
    bool isDirty();
    void clearFlag();

protected:
    // A value of 0 represents invalid / not defined.
    QVector<unsigned int> transferFunction;
    QVector<float> transferFunctionF;
    // transver function creation
    virtual void transferFunctionCreator() = 0;

    bool dirtyFlag;
};


#endif // COLORMAP_HPP

