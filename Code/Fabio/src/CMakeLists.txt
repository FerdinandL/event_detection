cmake_minimum_required(VERSION 2.9)
project(Urbane)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(Boost_USE_MULTITHREADED ON)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Qml REQUIRED)
find_package(Qt5Quick REQUIRED)
find_package(Qt5OpenGL REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Glew REQUIRED)
find_package(Boost COMPONENTS system filesystem iostreams program_options thread REQUIRED)


set(CMAKE_CXX_COMPILER "/usr/local/bin/g++-4.9")
set(CMAKE_CXX_FLAGS "-I/usr/local/opt/qt5/include -std=c++11 -Wno-c++11-narrowing")



set(srcs
    ./DataLayer/DataManager.cpp
    #./MapView/ShadowMap.cpp
    ./MapView/Building.cpp
    ./MapView/BuildingRenderingLayer.cpp
    ./MapView/Camera.cpp
    ./MapView/CameraInteractor.cpp
    ./MapView/ConvexHull.cpp
    ./MapView/DataRenderingLayer.cpp
    ./MapView/EdgeDetectionRenderer.cpp
    ./MapView/EffectsRenderer.cpp
    ./MapView/FontManager.cpp
    ./MapView/GlyphManager.cpp
    ./MapView/LabelRenderingLayer.cpp
    ./MapView/MapLoader.cpp
    ./MapView/MapView.cpp
    ./MapView/Mesh3D.cpp
    ./MapView/Mesh3DRenderingLayer.cpp
    ./MapView/Primitive.cpp
    ./MapView/QMatrix4x4D.cpp
    ./MapView/qtriangulator.cpp
    ./MapView/QVector3DD.cpp
    ./MapView/RenderingLayer.cpp
    ./MapView/SelectionRenderer.cpp
    ./MapView/SkyRenderer.cpp
    ./MapView/TextRenderer.cpp
    ./MapView/Triangulator.cpp
    ./MapView/VectorMap.cpp
    ./MapView/VectorMapRenderingLayer.cpp
    ./Util/ColorMap.cpp
    ./Util/UsefulFuncs.cpp
    ./Util/ColorMapRed.cpp
    ./Util/ColorMapBlue.cpp
    ./DataLayer/IndexStore.cpp
    ./DataLayer/kdindex/KdIndex.cpp
    ./DataLayer/arena.cpp
    ./DataLayer/DataStore.cpp
    ./DataLayer/mmap.cpp
    ./DataLayer/kdindex/CudaDb.cpp
    #./MapView/SunLight.cpp
    #./MapView/MapViewGui.cpp
    #./Shadows/ShadowsGui.cpp
    #./Shadows/MapQuickView.cpp
    ./main.cpp

    ./DataLayer/DataManager.hpp
    #./MapView/ShadowMap.hpp
     ./MapView/Building.hpp
    ./MapView/BuildingRenderingLayer.hpp
    ./MapView/Camera.hpp
    ./MapView/CameraInteractor.hpp
    ./MapView/ConvexHull.hpp
    ./MapView/DataRenderingLayer.hpp
    ./MapView/EdgeDetectionRenderer.hpp
    ./MapView/EffectsRenderer.hpp
    ./MapView/FontManager.hpp
    ./MapView/GlyphManager.hpp
    ./MapView/KdTreeBB.hpp
    ./MapView/LabelRenderingLayer.hpp
    ./MapView/MapLoader.hpp
    ./MapView/MapView.hpp
    ./MapView/Mesh3D.hpp
    ./MapView/Mesh3DRenderingLayer.hpp
    ./MapView/Primitive.hpp
    ./MapView/qdatabuffer_p.h
    ./MapView/QMatrix4x4D.hpp
    ./MapView/qrbtree_p.h
    ./MapView/QVector3DD.hpp
    ./MapView/RenderingLayer.hpp
    ./MapView/SelectionRenderer.hpp
    ./MapView/ShapePlacement.hpp
    ./MapView/SkyRenderer.hpp
    ./MapView/TextRenderer.hpp
    ./MapView/Triangulator.hpp
    ./MapView/VectorMap.hpp
    ./MapView/VectorMapRenderingLayer.hpp
    ./MapView/Constants.hpp
    ./Util/ColorMap.hpp
    ./Util/UsefulFuncs.hpp
    ./Util/ColorMapRed.hpp
    ./Util/ColorMapBlue.hpp
    ./Util/Typefunctions.hpp
    ./DataLayer/DataStore.hpp
    ./DataLayer/DataTypes.hpp
    ./DataLayer/DataManager.hpp
    ./DataLayer/IndexStore.hpp
    ./DataLayer/kdindex/KdIndex.hpp
    ./DataLayer/kdindex/KdBlock.hpp
    ./DataLayer/kdindex/Trip.hpp
    ./DataLayer/kdindex/KdQuery.hpp
    ./DataLayer/kdindex/Neighborhoods.hpp
    ./DataLayer/arena.hpp
    ./DataLayer/mmap.hpp
    ./DataLayer/kdindex/radix_pair.h
    ./DataLayer/kdindex/CudaDb.hpp
    ./DataLayer/kdindex/RequestQueue.hpp
    #./MapView/SunLight.hpp
    #./MapView/MapViewGui.hpp
    #./Shadows/ShadowsGui.hpp
    #./Shadows/MapQuickView.hpp
    #./BaseGui.hpp
    )

INCLUDE_DIRECTORIES( ${QT_INCLUDES} 
					${OPENGL_INCLUDE_DIRS}
                     /usr/local/include/
                     ./MapView/freetype/
                     ./MapView/freetype-gl/
                     ${GLEW_INCLUDE_DIRS}
                     ${CMAKE_CURRENT_SOURCE_DIR}
                     ${GLEW_INCLUDE_DIR})

LINK_DIRECTORIES(./MapView/freetype-gl/)
LINK_DIRECTORIES(./MapView/freetype/)


file(MAKE_DIRECTORY ../build/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../build/)


add_executable(urbane ${srcs} ./MapView/MapView.qrc)


qt5_use_modules(urbane Widgets Qml Quick OpenGL)
target_link_libraries(urbane ${QT_LIBRARIES} ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} ${BOOST_LIBRARIES} ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY} ${Boost_IOSTREAMS_LIBRARY} ${Boost_PROGRAM_OPTIONS_LIBRARY} ${Boost_THREAD_LIBRARY} freetype-gl freetype)


