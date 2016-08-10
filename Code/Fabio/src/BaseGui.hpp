#ifndef BASEGUI_H
#define BASEGUI_H

#include <QObject>

class BaseGui: public QObject
{
public:
    virtual ~BaseGui() {}
    virtual void run() = 0;
    virtual void connectSlots(){}
};

#endif // BASEGUI_H

