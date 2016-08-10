#include "ColorMapBlue.hpp"

#include <QColor>
#include <QVector>
#include <QDebug>

static bool print = false;

ColorMapBlue::ColorMapBlue()
{
    this->transferFunctionCreator();
}

void ColorMapBlue::transferFunctionCreator()
{
    // colors vector
    QVector<QColor> colors;
    colors.append(QColor(255,247,251));
    colors.append(QColor(236,231,242));
    colors.append(QColor(208,209,230));
    colors.append(QColor(166,189,219));

    colors.append(QColor(116,169,207));

    colors.append(QColor( 54,144,192));
    colors.append(QColor(  5,112,176));
    colors.append(QColor(  4, 90,141));
    colors.append(QColor(  2, 56, 88));

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


