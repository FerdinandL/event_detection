TEMPLATE = app

QT += qml quick widgets opengl webkitwidgets

TARGET = "UrbanPulse"

SOURCES += \
    ./DataLayer/IndexStore.cpp \
    ./DataLayer/kdindex/KdIndex.cpp \
    ./DataLayer/mmap.cpp \
    ./DataLayer/kdindex/CudaDb.cpp \
    ./DataLayer/DataManager.cpp \
    ./DataLayer/GridIndex.cpp \
    ./DataLayer/DataStoreArena.cpp \
    ./DataLayer/DataLayerTest.cpp \
    ./DataLayer/FunctionCache.cpp \
    ./Util/ColorMapCategorical.cpp \
    ./Util/OpenGLFuncs.cpp \
    ./main.cpp \
    MapView/Camera.cpp \
    Pulse/GenerateScalarFiles.cpp \
    MapView/QMatrix4x4D.cpp \
    MapView/QVector3DD.cpp \
    Util/UsefulFuncs.cpp

HEADERS += \
    ./Util/ColorMap.hpp \
    ./Util/UsefulFuncs.hpp \
    ./Util/ColorMapRed.hpp \
    ./Util/ColorMapBlue.hpp \
    ./Util/Typefunctions.hpp \
    ./Util/ColorMapCategorical.hpp \
    ./Util/OpenGLFuncs.hpp \
    ./DataLayer/DataStore.hpp \
    ./DataLayer/DataManager.hpp \
    ./DataLayer/IndexStore.hpp \
    ./DataLayer/kdindex/KdIndex.hpp \
    ./DataLayer/kdindex/KdBlock.hpp \
    ./DataLayer/kdindex/Trip.hpp \
    ./DataLayer/kdindex/KdQuery.hpp \
    ./DataLayer/kdindex/Neighborhoods.hpp \
    ./DataLayer/mmap.hpp \
    ./DataLayer/kdindex/radix_pair.h \
    ./DataLayer/kdindex/CudaDb.hpp \
    ./DataLayer/kdindex/RequestQueue.hpp \
    ./DataLayer/GridIndex.hpp \
    ./DataLayer/DataStoreArena.hpp \
    ./DataLayer/DataLayerTest.hpp \
    ./DataLayer/FunctionCache.hpp \
    ./BaseGui.hpp \
    MapView/Camera.hpp \
    Pulse/GenerateScalarFiles.hpp \
    MapView/QMatrix4x4D.hpp \
    MapView/QVector3DD.hpp

# Unix configuration
unix:!macx{
    QMAKE_CXXFLAGS += -std=c++0x
    QMAKE_CXXFLAGS += -fopenmp
    QMAKE_LFLAGS   += -fopenmp

    LIBS += -L/usr/local/lib/ -lGLEW
    LIBS += -lboost_thread -lboost_filesystem -lboost_program_options -lboost_system -lboost_iostreams

}
# OsX configuration
macx{

    QMAKE_CXX = /usr/local/bin/clang-omp++
    QMAKE_CC = /usr/local/bin/clang-omp
    QMAKE_LINK = /usr/local/bin/clang-omp++

    QMAKE_CXXFLAGS += -I/usr/local/opt/qt5/include -stdlib=libc++ -std=c++11 -Wno-c++11-narrowing
    QMAKE_LFLAGS   += -I/usr/local/opt/qt5/include -stdlib=libc++

    QMAKE_CXXFLAGS += -I/usr/local/Cellar/libiomp/20150701/include/libiomp/
    QMAKE_CXXFLAGS += -fopenmp
    QMAKE_LFLAGS   += -fopenmp

    INCLUDEPATH += -I /usr/local/include/

    LIBS += -L/usr/local/lib/ -lGLEW
    LIBS += -lboost_thread-mt -lboost_filesystem -lboost_program_options -lboost_system -lboost_iostreams
}

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

DISTFILES += \
    DataLayer/CMakeLists.txt
