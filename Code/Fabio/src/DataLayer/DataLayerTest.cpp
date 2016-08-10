#include "DataLayerTest.hpp"

#include <QPolygonF>
#include <QApplication>
#include <QVector>
#include <QFile>

#include "DataLayer/DataManager.hpp"
#include "DataLayer/DataStoreArena.hpp"
#include "DataLayer/kdindex/KdIndex.hpp"
#include "DataLayer/GridIndex.hpp"
#include "DataLayer/kdindex/Neighborhoods.hpp"

//#include "MapView/VectorMapRenderingLayer.hpp"


QString rawDataFolder = "/Users/fmiranda/Dropbox/vgc/urbane/rawdata/";
QString dataFolder = "/Users/fmiranda/Dropbox/vgc/urbane/data/nyc/";

//QString rawDataFolder = "/home/harishd/Desktop/Projects/3D-GIS/code/urbane/rawdata/";
//QString dataFolder = "/home/harishd/Desktop/Projects/3D-GIS/code/urbane/data/nyc/";

//QString rawDataFolder = "/Users/marcoslage/Git/urbane/rawdata/";
//QString dataFolder = "/Users/marcoslage/Git/urbane/data/nyc/";


DataLayerTest::DataLayerTest()
{
}

void DataLayerTest::run()
{
    DataManager::setDataFolder("sf","../data/sf/datastore/");
//    DataManager::setDataFolder("nyc","../data/nyc/datastore/");
//    writeGridFile();
//    rebuildAll();
//    testDataStore();
//    testGridIndex();
//    testMetaData();
//    testGridIndex();
//    testMetaData();
    loadPulseData();
//    writeScalars();
}

//void DataLayerTest::writeScalars() {
//    DataManager* manager = DataManager::getInstance();
//    DataStore* store = manager->getDataStore();
//    IndexStore *index = manager->getIndexStore();

//    store->loadCSVData("flickr", "../rawdata/nyc/flickr_table_extra.csv");
//    index->createStingIndex("flickr");

//    QString columnName = "HOUR";
//    QString columnValue = 0;
//    int xs = 253;
//    int ys = 267;
//    QRectF boundingRect;
//    boundingRect.setTopLeft(QPointF(40.477761,-74.258834));
//    boundingRect.setBottomRight(QPointF(40.917576,-73.700537));

//    QVector<float> = manager->createDensityFunction(boundingRect,ys,xs,dataName,-1,-1,-1,0,columnName,columnValue);

//    qDebug() << "writing density function";
//    QString fileName = this->folderPath+getIndex(dataName,columnName,columnValue)+".scalars";
//    QFile file(fileName);
//    if(!file.open(QIODevice::WriteOnly)) {
//        qDebug() << "could not write file";
//        assert(false);
//    }
//    QTextStream op(&file);
//    op << this->xs << "," << this->ys << "\n";

//    QRectF boundingRect;
//    boundingRect.setTopLeft(this->lb);
//    boundingRect.setBottomRight(this->tr);
//    op << boundingRect.left() << "," << boundingRect.top() << "," << boundingRect.right() << "," << boundingRect.bottom() << "\n\n";
//    for(int i = 0;i < fn.size();i ++) {
//        op << fn[i] << "\n";
//    }
//    file.close();

//    exit(0);
//}

void DataLayerTest::writeGridFile() {
    DataManager* manager = DataManager::getInstance("nyc");
    GridIndex grid = manager->getIndexStore()->loadGridIndex("pulse_grid");
    QVector<QPolygonF> cells = grid.getPolygons();

    QFile fileGrid("../data/grid.txt");
    if(!fileGrid.open(QIODevice::WriteOnly)) {
        qDebug() << "could not write file";
        assert(false);
    }
    QTextStream stream(&fileGrid);
    stream << grid.getXs() << "," << grid.getYs() << "\n";
    stream << qSetRealNumberPrecision(10) << grid.getBounds().topRight().x() << "," << grid.getBounds().topRight().y() << ","
                                          << grid.getBounds().bottomLeft().x() << "," << grid.getBounds().bottomLeft().y() << "\n\n";

    for(int i=0; i<cells.size(); i++) {
        stream << qSetRealNumberPrecision(10) << cells[i][0].x() << "," << cells[i][0].y() << "," << cells[i][2].x() << "," << cells[i][2].y() << "\n";
    }
    fileGrid.close();
    exit(0);
}

void DataLayerTest::loadPulseData() {
    DataManager* manager = DataManager::getInstance("sf");
    DataStore* store = manager->getDataStore();
    IndexStore *index = manager->getIndexStore();

//    store->loadCSVData("flickr", "../data/nyc/datastore/rawdata/flickr_table_extra.csv");
//    store->loadCSVData("flickr", "../rawdata/nyc/flickr_table.csv");
//    index->createStingIndex("flickr");
//    store->loadCSVData("twitter", "../rawdata/sf/twitter_table.csv");
//    index->createStingIndex("twitter");
    store->loadCSVData("taxi", "../rawdata/sf/taxi_table.csv");
    index->createStingIndex("taxi");


    exit(0);
}

void DataLayerTest::rebuildAll() {

    DataManager* manager = DataManager::getInstance("nyc");
    DataStore* store = manager->getDataStore();
    IndexStore *index = manager->getIndexStore();

    // loading data
    store->loadCSVData("taxi",rawDataFolder+"taxigreen(06-15)_table.csv");
    store->loadCSVData("subway",rawDataFolder+"nyc_subway_table.csv");
    store->loadCSVData("weather",rawDataFolder+"nyc_weather_table.csv");
    store->loadCSVData("restaurants",rawDataFolder+"nyc_restaurants_table.csv");
    store->loadPolyData("landuse", dataFolder+"landuse.txt");
    store->loadPolyData("neighbourhoods", dataFolder+"neighbourhood_fills.txt");

    index->createStingIndex("weather");
    index->createStingIndex("taxi");
    index->createStingIndex("subway");
    index->createGridIndex("neighbourhoods"); // to check if a point is inside manhattan
    index->createStingIndex("restaurants"); // for restaurant classification
    index->createGridIndex("landuse"); // for park classification

    exit(0);

}

//void DataLayerTest::testPulse() {

//    DataManager* manager = DataManager::getInstance();
//    manager->init("../data/datastore/");
//    DataStore* store = manager->getDataStore();

//    GridIndex gi;
//    gi.readGrid(dataFolder+"pulse/features.txt");
//    store->loadPolyData("pulse6", gi.getCellPolygons());

//    exit(0);
//}

void DataLayerTest::testDataStore() {

    DataManager* manager = DataManager::getInstance("nyc");

    // loading data
    DataStore* store = manager->getDataStore();
    qDebug() << "loading data";
    store->loadCSVData("taxi",rawDataFolder+"taxigreen(06-15)_table.csv");
    qDebug() << "loaded data";

//    QVector<QString> datasets = store->getDatasets();
//    qDebug() << datasets;
//    QVector<QString> polyDatasets = store->getPolygonDatasets();
//    qDebug() << "polygonDatasets" << polyDatasets;
//    exit(0);

//    Trip* trips;
//    int startId, endId;
//    store->getData("taxi",STING_NAME,0,(void **)(&trips),&startId,&endId);
//    qDebug() << trips[0] << trips[1] << trips[2] << trips[3] << uint2long(trips[4]) << uint2long(trips[5]);

    // loading subway data
    qDebug() << "loading data";
    store->loadCSVData("subway",rawDataFolder+"nyc_subway_table.csv");
    qDebug() << "loaded data";

    // loading restaurant data
    qDebug() << "loading data";
    store->loadCSVData("restaurants",rawDataFolder+"nyc_restaurants_table.csv");
    qDebug() << "loaded data";

    // loading weather data
    qDebug() << "loading data";
    store->loadCSVData("weather",rawDataFolder+"nyc_weather_table.csv");
    qDebug() << "loaded data";

//    qDebug() << "loaded data";
//    std::vector<long> tuples = {};
//    std::vector<uint64_t> result;
//    store->getTuplesSpaceTime("taxi", tuples, result);

//    QString dataName = "taxi";
//    QString columnName = "spacetime";
//    uint64_t* pts;
//    int npts;
//    std::vector<long> tuples = {};
//    qDebug() << "Getting tuples";
//    store->getTuples(dataName, columnName, tuples, (void**)&pts,&npts);
//    qDebug() << "Number of points" << npts;

//    store->getTuples<float>("subway", "Trip_distance", tuples, result);

//    qDebug() << "loading data";
//    store->loadCSVData("taxi","/Users/fmiranda/Dropbox/vgc/urbane-shadows/urbane/rawdata/taxigreen_table.csv");
//    qDebug() << "loaded data";
//    std::vector<long> tuples = {0,1,2,10};
//    std::vector<float> result;
//    store->getTuples<float>("taxi", "Trip_distance", tuples, result);
//    store->loadPolyData("neighbourhood", "/Users/fmiranda/Dropbox/vgc/urbane-shadows/urbane/data/neighbourhood_fills.txt");

//    qDebug() << "loading polygon data";
//    store->loadPolyData("neighbourhoods", "../data/nyc/neighbourhood_fills.txt");
//    qDebug() << "loaded data";
//    qDebug() << "Query:" << tuples;
//    qDebug() << "Query result:" << result;

//    // loading restaurant data
//    DataStore* store = manager->getDataStore();
//    qDebug() << "loading data";
//    store->loadCSVData("restaurant","/home/harishd/Desktop/Projects/3D-GIS/code/urbane/rawdata/nyc_restaurants_table.csv");
//    qDebug() << "loaded data";


    // Testing meta data
//    DataStore* store = manager->getDataStore();
//    QVector<QString> datasets = store->getDatasets();
//    qDebug() << "datasets" << datasets;

//    QVector<QString> polyDatasets = store->getPolygonDatasets();
//    qDebug() << "polygonDatasets" << polyDatasets;

//    qDebug() << store->getSpatialTemporalColumns(datasets[0]);
//    qDebug() << store->getOtherColumns(datasets[0]);
//    qDebug() << store->getNumBlocks(datasets[0], "spacetime");

    // creating string index
    IndexStore *index = manager->getIndexStore();
    index->createStingIndex("weather");
    index->createStingIndex("taxi");
//    index->createStingIndex("taxi");
    index->createStingIndex("subway");
    index->createStingIndex("restaurants");
    qDebug() << "finished creating indexes";

    // querying sting index
//    IndexStore *index = manager->getIndexStore();
//    KdRequest queryRequest;
//    int geomSize = 0;
//    int size = 6;
//    REQUEST_TYPE requestType = RT_CPU;
//    queryRequest.type = requestType;
//    queryRequest.query = new KdQuery(size);
//    queryRequest.regions = new Neighborhoods::Geometry[geomSize];
//    queryRequest.noRegions = geomSize;
//    // setup query paremeters - all trips starting between June 10-17 2015
//    updateQuery(&queryRequest,4,1433894400,0,Gt,false);
//    updateQuery(&queryRequest,4,1434585600,0,Lt,false);
//    // execute query
//    vector<long> results = index->querySting("taxi",&queryRequest,1);
//    qDebug() << "query returned" << results.size() << "trips";
//    QVector<double> pts = manager->getSpatialData("taxi",0,results);
//    qDebug() << "Pt: " << pts[0] << pts[1];

//    IndexStore *index = manager->getIndexStore();
//    KdRequest queryRequest;
//    int geomSize = 0;
//    int size = 3;
//    REQUEST_TYPE requestType = RT_CPU;
//    queryRequest.type = requestType;
//    queryRequest.query = new KdQuery(size);
//    queryRequest.regions = new Neighborhoods::Geometry[geomSize];
//    queryRequest.noRegions = geomSize;
//    // 40.576 40.9031 -74.0309 40.9031
//    // 40.6829 40.879 -74.0477 -73.9066
//    updateQuery(&queryRequest,0,0,40.576,Gt,true);
//    updateQuery(&queryRequest,0,0,40.9031,Lt,true);
//    updateQuery(&queryRequest,1,0,-74.0309,Gt,true);
//    updateQuery(&queryRequest,1,0,-73.9031,Lt,true);

//    // execute query
//    vector<long> results = index->querySting("subway",&queryRequest,1);
//    qDebug() << "query returned" << results.size() << "trips";
//    QVector<double> pts = manager->getSpatialData("subway",0,results);
//    qDebug() << "Pt: " << pts[0] << pts[1];
    exit(0);
}

void DataLayerTest::testGridIndex() {
//    Neighborhoods neigh("../data/datastore/neighborhoods.txt");
//    std::vector<Neighborhoods::Geometry> geoms = neigh.getGeometries();

//    QVector<QPolygonF> polys;
//    foreach(Neighborhoods::Geometry g, geoms) {
//        QPolygonF poly;
//        int n = g.size();
//        for(int i = 0;i < n;i ++) {
//            std::pair<float,float> pt = g.at(i);
//            poly << QPointF(pt.first,pt.second);
//        }
//        polys << poly;
//    }

//    qDebug() << "building grid index" << polys.size();
//    GridIndex grid(1024,1024);
//    grid.buildGrid(polys);
//    qDebug() << "built grid index";
//    grid.test();

    DataManager* manager = DataManager::getInstance("nyc");
    DataStore* store = manager->getDataStore();
//    QString layerFile = dataFolder+"neighbourhood_fills.txt";
//    store->loadPolyData("neighbourhoods", layerFile);
//    qDebug() << "loaded polygonal data" << layerFile;

//    qDebug() << "creating grid index";
//    IndexStore* indexStore = manager->getIndexStore();
//    indexStore->createGridIndex("neighbourhoods");
//    qDebug() << "Finished creating grid index";

    QString layerFile = dataFolder+"landuse.txt";
    store->loadPolyData("landuse", layerFile);
    qDebug() << "loaded polygonal data" << layerFile;

    qDebug() << "creating grid index";
    IndexStore* indexStore = manager->getIndexStore();
    indexStore->createGridIndex("landuse");
    qDebug() << "Finished creating grid index";


////    DataStore* store = manager->getDataStore();
//    QVector<QString> datasets = store->getDatasets();
//    qDebug() << datasets;
//    QVector<QString> polyDatasets = store->getPolygonDatasets();
//    qDebug() << "polygonDatasets" << polyDatasets;

//    qDebug() << store->getPolygons("neighbourhoods");
//    qDebug() << store->getPolygonNames("neighbourhoods");

    exit(0);
}

void DataLayerTest::testMetaData() {
    DataManager* manager = DataManager::getInstance("nyc");
    DataStore* store = manager->getDataStore();
    QVector<QString> datasets = store->getDatasets();
    qDebug() << datasets;
    QVector<QString> polyDatasets = store->getPolygonDatasets();
    qDebug() << "polygonDatasets" << polyDatasets;
    exit(0);
}
