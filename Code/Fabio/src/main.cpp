#include <QDebug>
#include <QApplication>
#include <QFont>

#include "BaseGui.hpp"

#include "Pulse/GenerateScalarFiles.hpp"

enum DerivedGui{
    GENERATESCALAR
};
// Derived class selection
DerivedGui pId = GENERATESCALAR;

int main(int argc, char *argv[])
{
    // Application creation.
    QApplication app(argc, argv);

    // Default font
    QFont font("Sans",8);
    app.setFont(font);

    // BaseGui object
    BaseGui* gui = NULL;
    // Select the desired derived class
    switch (pId) {
    case GENERATESCALAR:  gui = new GenerateScalarFiles(); break;
    }

    // connect all slots
    gui->connectSlots();
    // run
    gui->run();

    // Run the application
    app.exec();

    // return ok.
    return 0;
}
