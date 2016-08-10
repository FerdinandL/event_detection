#include "DataManager.hpp"

#include <QPolygonF>
#include <QRectF>
#include <QtAlgorithms>
#include <QLineF>
#include <QElapsedTimer>

#include "DataLayer/kdindex/KdIndex.hpp"
#include "DataLayer/GridIndex.hpp"
#include "DataLayer/DataStoreArena.hpp"
#include "../MapView/Camera.hpp"
#include "../Util/Typefunctions.hpp"

#include <omp.h>

QMap<QString,DataManager*> DataManager::instance = QMap<QString,DataManager*>();
QMap<QString,QString> DataManager::dataFolder = QMap<QString,QString>();
int DataManager::day = 0;

DataManager::DataManager(){

}

DataManager::~DataManager() {
    delete dataStore;
    delete indexStore;
}

QString DataManager::getDataFolder(QString city) {
    return DataManager::dataFolder[city];
}

void DataManager::setDataFolder(QString city, QString dataFolder) {
    DataManager::dataFolder[city] = dataFolder;
}

DataManager *DataManager::getInstance(QString city) {
    if(instance[city] == NULL) {
        instance[city] = new DataManager();
        instance[city]->init(city);
    }
    return instance[city];
}

void DataManager::init(QString city) {

    qDebug() << "Initializing data manager at" << DataManager::getDataFolder(city);

    // TODO initialize the data store and index store
    dataStore = new DataStoreArena(this->dataFolder[city]);
    indexStore = new IndexStore(dataStore,this->dataFolder[city]);

}

DataStore *DataManager::getDataStore() const {
    return dataStore;
}

IndexStore *DataManager::getIndexStore() const {
    return indexStore;
}

vector<long> DataManager::executeQuery(QString dataName, int timeIndex, long startTime, long endTime, int spaceIndex, QRectF rect) {
    QString dataType;
    int sizeInBytes, noSpatial;
    this->dataStore->getTupleSize(dataName,STING_NAME,&dataType,&sizeInBytes,&noSpatial);
    int size = sizeInBytes / sizeof(Trip) - 1;
    int geomSize = 0;

    vector<long> results;
    if(size <= 0)
        return results;

    REQUEST_TYPE requestType = RT_CPU;
    KdRequest queryRequest;
    queryRequest.type = requestType;
    queryRequest.query = new KdQuery(size);
    queryRequest.regions = new Neighborhoods::Geometry[geomSize];
    queryRequest.noRegions = 0;

    if(timeIndex != -1) {
        int tin = timeIndex + noSpatial * 2;
        if(startTime != -1) {
            updateQuery(&queryRequest,tin,startTime,0,Gt,false);
        }
        if(endTime != -1) {
            updateQuery(&queryRequest,tin,endTime,0,Lt,false);
        }
    }

    if(rect.width() > 0 && rect.height() > 0) {
        int sin = spaceIndex * 2;
        updateQuery(&queryRequest,sin,0,rect.left(),Gt,true);
        updateQuery(&queryRequest,sin+1,0,rect.top(),Gt,true);
        updateQuery(&queryRequest,sin,0,rect.right(),Lt,true);
        updateQuery(&queryRequest,sin+1,0,rect.bottom(),Lt,true);
    }
    results = this->indexStore->querySting(dataName,&queryRequest,1);
    return results;
}


QVector<QVector<int> > DataManager::executeGroupbyQuery(QString polygonDataName, QString dataName, int timeIndex, long startTime, long endTime, int spaceIndex, QString columnName, int filterColumnBy) {
    GridIndex grid = this->indexStore->loadGridIndex(polygonDataName);

    // using rectangles to avoid pip test on cpu.
    QRectF rect = grid.getBounds();

    QElapsedTimer timer;

    qDebug() << rect;

    timer.start();
    results = this->executeQuery(dataName,timeIndex,startTime,endTime,spaceIndex,rect);
    qDebug() << "sting querying time" << timer.elapsed() << "ms";
#ifdef OLD
//    timer.restart();
//    QVector<QVector<int> > sfn(grid.getPolygonCount());
//    pts = this->getSpatialData(dataName,spaceIndex,results);
//    qDebug() << "Elapsed time:" << timer.elapsed() << "ms";

//    int noPts = pts.size() / 2;

//    assert(noPts == results.size());

//    qDebug() << "====" << noPts;

//    for(int i = 0;i < noPts;i ++) {
//        QVector<int> in = grid.getRegion(pts[i * 2],pts[i * 2 + 1]);
//        if(in.size() > 0) {
//            foreach(int l,in) {
//                sfn[l] << i;
//            }
//        }
//    }
#endif

    QString dataType;
    int typeSize;
    int numSpatial;
    this->dataStore->getTupleSize(dataName, "spacetime", &dataType, &typeSize, &numSpatial);

    timer.restart();
    // get spatial data
    int noPts = results.size();
    uint64_t* spatialPts;
    int numSpatialPts;
    this->dataStore->getTuples(dataName,"spacetime", results, (void**)&spatialPts,&numSpatialPts);
    // get column data (just integer for now)
    int* columnPts;
    int numColumnPts;
    if(columnName.size() > 0)
        this->dataStore->getTuples(dataName,columnName, results, (void**)&columnPts,&numColumnPts);

    qDebug() << "Getting data :" << timer.elapsed() << "ms";


    int noValues = typeSize / sizeof(uint64_t);
    int sin = spaceIndex * 2;
    timer.restart();

    int polyCt = grid.getPolygonCount();
    QVector<QVector<int> > sfn(polyCt);

    int nt = omp_get_max_threads();
    int loopCt = ceil(noPts / nt);

//    qDebug() << npts << noPts << dataName << timeIndex << startTime << endTime << spaceIndex;

#pragma omp parallel for
    for(int i = 0;i < nt;i ++) {
        QVector<QVector<int> > lsfn(polyCt);

        int st = i * loopCt;
        int en = min((i + 1) * loopCt, noPts);
        while(st < en) {
            bool filteredOut = false;
            if(columnName.size() > 0) {
                int columnValue = columnPts[st];
                if(columnValue != filterColumnBy)
                    filteredOut = true;
            }

            if(filteredOut == false) {
                QVector<int> in = grid.getRegion(uint2double(spatialPts[noValues*st + sin]),uint2double(spatialPts[noValues*st + sin + 1]));
                if(in.size() > 0) {
                    foreach(int l,in) {
                        lsfn[l].push_back(i);
                    }
                }
            }
            st ++;
        }

#pragma omp critical
        for(int l = 0;l < polyCt;l ++) {
            sfn[l] << lsfn[l];
        }

    }
    free(spatialPts);
    free(columnPts);

    qDebug() << "Aggregating data :" << timer.elapsed() << "ms";
    return sfn;
}

QVector<QVector<int> > DataManager::executeGroupbyQuery(QRectF boundingRect, int xres, int yres, QString dataName, int timeIndex, long startTime, long endTime, int spaceIndex, QVector<Filter<int>> filters, QString countColumn) {
    int gridSize = xres * yres;
    results = this->executeQuery(dataName,timeIndex,startTime,endTime,spaceIndex,boundingRect);

    int noPts = results.size();
    uint64_t* spatialPts;
    int numSpatialPts;
    this->dataStore->getTuples(dataName,"spacetime", results, (void**)&spatialPts,&numSpatialPts);

    QString dataType;
    int typeSize;
    int numSpatial;
    this->dataStore->getTupleSize(dataName, "spacetime", &dataType, &typeSize, &numSpatial);

    // get column data (just integer for now)
    int** filterPts;
    int* numFilterPts;

    if(filters.size() > 0) {
        filterPts = new int*[filters.size()];
        numFilterPts = new int[filters.size()];
        for(int i=0; i<filters.size(); i++) {
            this->dataStore->getTuples(dataName, filters[i].columnName, results, (void**)&filterPts[i],&numFilterPts[i]);
        }
    }

    int* countPts;
    int numCountPts;
    bool useCount = false;
    if(countColumn.size() > 0) {
        this->dataStore->getTuples(dataName, countColumn, results, (void**)&countPts,&numCountPts);
        useCount = true;
    }

    int noValues = typeSize / sizeof(uint64_t);
    int sin = spaceIndex * 2;
    double xsize = boundingRect.width() / xres;
    double ysize = boundingRect.height() / yres;
    int count = 0;
    QVector<QVector<int> > sfn(gridSize);
#if 0
    for(int i = 0;i < noPts;i ++) {

        bool filteredOut = false;
        if(filters.size() > 0) {
            for(int j=0; j<filters.size(); j++) {
                if(filterPts[j][i] < filters[j].range.first || filterPts[j][i] > filters[j].range.second) {
                    filteredOut = true;
                    break;
                }
            }
        }

        if(filteredOut == false) {
            // get cell index
            double x = uint2double(spatialPts[noValues*i + sin]);
            double y = uint2double(spatialPts[noValues*i + sin + 1]);

            int inx = (x - boundingRect.left()) / xsize;
            int iny = (y - boundingRect.top()) / ysize;
            int in = (inx < 0 || iny < 0 || inx > xres - 1 || iny > yres - 1)? -1:iny * xres + inx;
            if(in != -1) {
                sfn[in] << i;
            }
            count++;
        }
    }
#else

    int nt = omp_get_max_threads();
    int loopCt = ceil(noPts / nt);

#pragma omp parallel for
    for(int i = 0;i < nt;i ++) {
        QVector<QVector<int> > lsfn(gridSize);

        int st = i * loopCt;
        int en = min((i + 1) * loopCt, noPts);

        while(st < en) {
            bool filteredOut = false;

            if(filters.size() > 0) {
                for(int j=0; j<filters.size(); j++) {
                    if(filterPts[j][st] < filters[j].range.first || filterPts[j][st] > filters[j].range.second) {
                        filteredOut = true;
                        break;
                    }
                }
            }

            if(filteredOut == false) {
                // get cell index
                double x = uint2double(spatialPts[noValues*st + sin]);
                double y = uint2double(spatialPts[noValues*st + sin + 1]);

                int inx = (x - boundingRect.left()) / xsize;
                int iny = (y - boundingRect.top()) / ysize;
                int in = (inx < 0 || iny < 0 || inx > xres - 1 || iny > yres - 1)? -1:iny * xres + inx;

                int value = 1;
                if(useCount) {
                    value = countPts[st];
                }
                if(in != -1) {
                    lsfn[in] << (i*value);
                }
                count++;
            }
            st ++;
        }

#pragma omp critical
        for(int l = 0;l < gridSize;l ++) {
            sfn[l] << lsfn[l];
        }
    }
#endif
    free(spatialPts);

    for(int i=0; i<filters.size(); i++) {
        free(filterPts[i]);
    }
    if(filters.size() > 0)
        free(numFilterPts);
    if(countColumn.size() > 0)
        free(countPts);

    qDebug() << "==========================" << numSpatialPts << count;

    return sfn;
}
int DataManager::getDay()
{
    return day;
}

void DataManager::setDay(int value)
{
    day = value;
}


QVector<float> DataManager::createDensityFunction(QString polygonDataName, QString dataName, int timeIndex, long startTime, long endTime, int spaceIndex, QString columnName, int filterColumnBy) {
    QVector<float> fn;
    QVector<QVector<int> > sfn = this->executeGroupbyQuery(polygonDataName,dataName,timeIndex,startTime,endTime,spaceIndex, columnName, filterColumnBy);
//    QVector<QVector<int>> sfn = this->executeGroupbyQuery(this->indexStore->loadGridIndex(polygonDataName).getBounds(),128,128,dataName,timeIndex,startTime,endTime,spaceIndex);


    // currently just giving the density function.
    int polyCt = sfn.size();
    for(int i = 0;i < polyCt;i ++) {
        fn << sfn[i].size();
    }
    return fn;
}

QVector<float> DataManager::createDensityFunction(QRectF boundingRect, float cellSizeInMeters, QString dataName, int timeIndex, long startTime, long endTime, int spaceIndex, QVector<Filter<int>> filters) {
    QPointF lb = Camera::geo2world(boundingRect.topLeft());
    QPointF rt = Camera::geo2world(boundingRect.bottomRight());
    qDebug() << qSetRealNumberPrecision(10) << lb << rt << (rt - lb);
    double gres = Camera::getGroundResolution(boundingRect.center(), WORLD_ZOOM_LEVEL);
    float cellSize = cellSizeInMeters / gres;
    QPointF diff = rt - lb;
    int latRes = std::ceil(std::abs(diff.y()) / cellSize);
    int lonRes = std::ceil(std::abs(diff.x()) / cellSize);
    return this->createDensityFunction(boundingRect,latRes,lonRes,dataName,timeIndex,startTime,endTime,spaceIndex,filters,"");
}
QVector<float> DataManager::createDensityFunction(QRectF boundingRect, int latRes, int lonRes, QString dataName, int timeIndex, long startTime, long endTime, int spaceIndex, QVector<Filter<int>> filters, QString countColumn) {
    qDebug() << "executing group by query";
    QVector<QVector<int> > bins = this->executeGroupbyQuery(boundingRect,latRes,lonRes,dataName,timeIndex,startTime,endTime,spaceIndex,filters,countColumn);
    qDebug() << "finished executing group by query";
    int noBins = bins.size();
    QVector<float> fn (noBins);

    // radius and cutoff in meters
    float radius = 500;
    float cutoff = 100;

    assert(boundingRect.bottom() >= boundingRect.top());

    // get radius in no. of cells
    QPointF lb = Camera::geo2world(boundingRect.topLeft());
    QPointF rt = Camera::geo2world(boundingRect.bottomRight());
    qDebug() << qSetRealNumberPrecision(10) << lb << rt << (rt - lb);
    double gres = Camera::getGroundResolution(boundingRect.center(), WORLD_ZOOM_LEVEL);

    QPointF mdiff = (rt - lb) * gres;

    // size of cell in meters
    double cellLat = std::abs(mdiff.y()) / latRes;
    double cellLon = std::abs(mdiff.x()) / lonRes;

    // no. of cells that are part of radius
    int nlat = std::ceil(radius / cellLat);
    int nlon = std::ceil(radius / cellLon);

    qDebug() << "createDensityFunction" <<cellLat << cellLon << nlat << nlon << latRes << lonRes << mdiff;

    int ct = 0;
    for(int i = 0;i < noBins;i ++) {
        int lat = i % latRes;
        int lon = i / latRes;
        int realIn = lon + lat * lonRes;
        fn[realIn] = 0;

#if 1
        int binId = lat + lon * latRes;
        fn[realIn] = bins[binId].size();
#else
        for(int lt = lat - nlat;lt <= lat + nlat;lt ++) {
            if(lt < 0 || lt >= latRes) {
                continue;
            }
            double dlat = std::abs(lat - lt) * cellLat;
            for(int ln = lon - nlon;ln <= lon + nlon;ln ++) {
                if(ln < 0 || ln >= lonRes) {
                    continue;
                }
                int binId = lt + ln * latRes;
                int fni = bins[binId].size();
                if(fni > 0) {
                    ct ++;
                }
                double dlon = std::abs(lon - ln) * cellLon;

                // WITH SMOOTHING
//                fn[realIn] += fni * exp(-(dlat * dlat + dlon * dlon)/ (cutoff * cutoff));

                // WITHOUT SMOOTHING
                fn[realIn] += fni;
            }
        }
#endif
    }
    qDebug() << "finished computing function" << ct;
    return fn;
}

QVector<float> DataManager::createDistanceFunction(QString dataName, int timeIndex, long startTime, long endTime, int spaceIndex, QVector<QPolygonF> polygons) {
    QRectF rect(0,0,0,0);
    bool first = true;
    foreach(QPolygonF poly, polygons) {
        if(first) {
            first = false;
            rect = poly.boundingRect();
        } else {
            rect = rect.united(poly.boundingRect());
        }
    }

    int xres, yres;
    if(rect.width() < rect.height()) {
        xres = 1024;
        yres = std::ceil(rect.height() * xres / rect.width());
    } else {
        yres = 1024;
        xres = std::ceil(rect.width() * yres / rect.height());
    }
    QVector<float> gridFn = createDistanceFunction(dataName,timeIndex,startTime,endTime,spaceIndex,rect,xres,yres);
    assert (gridFn.size() == xres*yres);
    QVector<float> fn;
    double res = rect.width() / xres;

    GridIndex grid(1024,1024);
    grid.buildGrid(polygons);

    float dist[polygons.size()];
    int ct[polygons.size()];
    for(int i = 0;i < polygons.size();i ++) {
        dist[i] = 0;
        ct[i] = 0;
    }
    for(int i = 0;i < gridFn.size();i ++) {
        if(gridFn[i] == -1) {
            continue;
        }
        int xin = i % xres;
        int yin = i / xres;
        double x = rect.left() + xin * res + res / 2;
        double y = rect.top() + yin * res + res / 2;
        QVector<int> in = grid.getRegion(x,y);
        if(in.size() > 0) {
            foreach(int l,in) {
                dist[l] += gridFn[i];
                ct[l] ++;
            }
        }
    }
    for(int i = 0;i < polygons.size();i ++) {
        if(ct[i] > 0) {
            fn << dist[i] / ct[i];
        } else {
            fn << -1;
        }
    }
    return fn;
}

QVector<float> DataManager::createDistanceFunction(QString dataName, int timeIndex, long startTime, long endTime, int spaceIndex, QRectF boundingRect, int xres, int yres) {
    QVector<QVector<int> > sfn = this->executeGroupbyQuery(boundingRect,xres,yres,dataName,timeIndex,startTime,endTime,spaceIndex, QVector<Filter<int>>(),"");
    QVector<float> fn;
    double res = boundingRect.width()  / xres;
    double gres = Camera::getGroundResolution(QPointF(40.718162, -73.991793), WORLD_ZOOM_LEVEL);
    for(int i = 0;i < sfn.size();i ++) {
        QVector<int> bin = sfn[i];
        int xin = i % xres;
        int yin = i / xres;
        QPointF orig(boundingRect.left() + xin * res + res / 2, boundingRect.top() + yin * res + res / 2);
        orig = Camera::geo2world(orig);
        double dist = 0;
        foreach(int j, bin) {
            QPointF pt(pts[j * 2],pts[j * 2 + 1]);
            pt=  Camera::geo2world(pt);
            dist += QLineF(orig,pt).length() * gres;
        }
        if(bin.size() > 0) {
            dist /= bin.size();
            fn << dist;
        } else {
            fn << -1;
        }
    }
    return fn;
}

QVector<double> DataManager::getSpatialData(QString dataName, int spaceIndex, vector<long> ids) {
    qSort(ids);
    int sizeInBytes, noSpatial;
    QString dataType;
    int noBlocks = this->indexStore->getValue(dataName + "." + NO_BLOCKS).toInt();
    this->dataStore->getTupleSize(dataName,STING_NAME,&dataType,&sizeInBytes,&noSpatial);
    int size = sizeInBytes / sizeof(Trip);

    long in = 0;
    int sin = spaceIndex * 2;
    QVector<double> pts;
    for(int i = 0;i < noBlocks;i ++) {
        Trip* trips;
        int startId, endId;
        this->dataStore->getData(dataName,STING_NAME,i,(void **)(&trips),&startId,&endId);
        while(in < ids.size() && ids[in] >= startId && ids[in] <= endId) {
            long x = ids[in] - startId;
            x *= size;
            pts << uint2double(trips[x + sin]);
            pts << uint2double(trips[x + sin + 1]);
            in ++;
        }
        if(in > ids.size()) {
            break;
        }
    }
    return pts;
}



