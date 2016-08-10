#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include "DataStore.hpp"
#include "IndexStore.hpp"
#include "FunctionCache.hpp"

template<class T>
class Filter
{
public:
    Filter(){};
    Filter(QString columnName, QString ignoreColumn, QPair<T,T> range, QString name = "")
    {
        this->columnName = columnName;
        this->ignoreColumn = ignoreColumn;
        this->range = range;
        this->name = name;
    }

public:
    QString columnName;
    QString ignoreColumn;
    QString name;
    QPair<T,T> range;
};

class DataManager
{

private:
    DataManager();
    ~DataManager();
    static QMap<QString,DataManager*> instance;
    static QMap<QString,QString> dataFolder;

private:
    DataStore* dataStore;
    IndexStore* indexStore;
    FunctionCache* functionCache;

public:
    static DataManager* getInstance(QString city);
    static void setDataFolder(QString city, QString dataFolder);
    static QString getDataFolder(QString city);

    void init(QString city);
    DataStore *getDataStore() const;
    IndexStore *getIndexStore() const;

/**
 * Query API for Urbane
**/
public:
    /* Function to create layers */
    QVector<float> createDensityFunction(QRectF boundingRect, float cellSizeInMeters, QString dataName, int timeIndex, long startTime, long endTime, int spaceIndex, QVector<Filter<int>> filters);
    QVector<float> createDensityFunction(QString polygonDataName, QString dataName, int timeIndex, long startTime, long endTime, int spaceIndex, QString columnName = "", int filterColumnBy = 0);
    QVector<float> createDensityFunction(QRectF boundingRect, int latRes, int lonRes, QString dataName, int timeIndex, long startTime, long endTime, int spaceIndex, QVector<Filter<int>> filters, QString countColumn);

    QVector<float> createDistanceFunction(QString dataName, int timeIndex, long startTime, long endTime, int spaceIndex, QVector<QPolygonF> polygons);
    QVector<float> createDistanceFunction(QString dataName, int timeIndex, long startTime, long endTime, int spaceIndex, QRectF boundingRect, int xres, int yres);

    /* Function to get data for index*/
    QVector<double> getSpatialData(QString dataName,int spaceIndex, vector<long> ids);


public:
    // Sting query
    vector<long> executeQuery(QString dataName, int timeIndex, long startTime, long endTime, int spaceIndex, QRectF rect);

    // For polygons
    QVector<QVector<int> > executeGroupbyQuery(QString polygonDataName, QString dataName, int timeIndex, long startTime, long endTime, int spaceIndex, QString columnName, int filterColumnBy);

    // For grids
    QVector<QVector<int> > executeGroupbyQuery(QRectF boundingRect, int xres, int yres, QString dataName, int timeIndex, long startTime, long endTime, int spaceIndex, QVector<Filter<int>> filters, QString countColumn);

private:
    QVector<double> pts;
    vector<long> results;

public:
    static int day;
    static int getDay();
    static void setDay(int value);
};

#endif // DATAMANAGER_H
