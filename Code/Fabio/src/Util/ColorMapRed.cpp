#include "ColorMapRed.hpp"

#include <QColor>
#include <QVector>
#include <QDebug>

static bool print = false;

ColorMapRed::ColorMapRed()
{
    this->transferFunctionCreator();
    dirtyFlag = true;
}

void ColorMapRed::transferFunctionCreator()
{
    // colors vector
    QVector<QColor> colors;
    colors.append(QColor(255,245,235,90));
    colors.append(QColor(254,230,206,120));
    colors.append(QColor(253,208,162,150));
    colors.append(QColor(253,174,107,180));

    colors.append(QColor(253,141,60,210));

    colors.append(QColor(241,105,19,255));
    colors.append(QColor(217, 72, 1,255));
    colors.append(QColor(166, 54, 3,255));
    colors.append(QColor(127, 39, 4,255));

    // current color
    QColor current(0,0,0,0);
    int ncolors = colors.length()-1;

    // creates the colors
    for(int id=0; id<COLORMAP_SIZE; id++){
        // gets the color bin
        float t = (float)id/COLORMAP_SIZE;
        int cid = int(t*ncolors);

        // boundary colors
        if(cid == ncolors){
            this->transferFunction  << colors[ cid ].rgba();
            this->transferFunctionF << colors[ cid ].redF()  << colors[ cid ].greenF()
                                    << colors[ cid ].blueF() << colors[ cid ].alphaF();
            continue;
        }

        float d = 1.0 / ncolors;
        t = (t-cid*d) / d;

        // get the current colors
        const QColor sColor = colors[ cid ];
        const QColor eColor = colors[cid+1];

        // compute color
        current.setRed  ( (int)((1-t)*sColor.red()   + t*eColor.red()  ) );
        current.setGreen( (int)((1-t)*sColor.green() + t*eColor.green()) );
        current.setBlue ( (int)((1-t)*sColor.blue()  + t*eColor.blue() ) );
        current.setAlpha( (int)((1-t)*sColor.alpha() + t*eColor.alpha()) );

        this->transferFunction  << current.rgba();
        this->transferFunctionF << current.redF() << current.greenF() << current.blueF() << current.alphaF();
    }

    if(print){
        print = false;
        qDebug() << transferFunctionF;
    }
}


