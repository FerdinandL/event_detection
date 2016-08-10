#ifndef DATALAYERTEST_HPP
#define DATALAYERTEST_HPP

#include <QPolygonF>

#include "DataLayer/DataManager.hpp"
#include "DataLayer/kdindex/KdIndex.hpp"
#include "DataLayer/GridIndex.hpp"
#include "DataLayer/kdindex/Neighborhoods.hpp"

#include "BaseGui.hpp"

class DataLayerTest: public BaseGui
{
public:
    DataLayerTest();

    void run();

    void testDataStore();
    void testGridIndex();
    void testMetaData();
    void rebuildAll();
    void writeGridFile();
    void loadPulseData();
};

#endif
