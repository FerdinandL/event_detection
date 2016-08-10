#ifndef DATASTORE_HPP
#define DATASTORE_HPP

#include <QString>
#include <QVector>
#include <QPolygonF>
#include <QPair>

#include "mmap.hpp"

#include "../Util/Typefunctions.hpp"

// Data types defined in input file
namespace DataTypeInput {
    static QString LATITUDE = "latitude";
    static QString LONGITUDE = "longitude";
    static QString DATE = "date";
    static QString TIME = "time";
    static QString STRING = "string";
    static QString LONG = "long";
    static QString FLOAT = "float";
    static QString INT = "int";
};

// Data types as stored in DataStore
namespace DataTypeArena {
    static QString SPACE = "space";
    static QString TIME = "time";
    static QString SPACETIME = "spacetime";
    static QString STRING = "string";
    static QString LONG = "long";
    static QString FLOAT = "float";
    static QString INT = "int";
};

class DataStore {

public:
    // An ID is created for each tuple.
    virtual void loadCSVData(QString dataName, QString csvFile) = 0;
    virtual void loadPolyData(QString dataName, QVector<QPolygonF> polygons) = 0;
    virtual void loadPolyData(QString dataName, QString polyFile) = 0;

    // Get a start position and size in memory of dataset.colum
    virtual void getData(QString datasetname, QString columnname, int block_index, void **start, int *startid, int *endid) = 0;
    virtual int getNumBlocks(QString datasetname, QString columnname) = 0;
    virtual bool getTupleSize(QString datasetname, QString columnname, QString *dataType, int *size, int *num_spatial) = 0;
    virtual int getTypeSize(QString datasetname, QString columnname) = 0;

    virtual void getTuples(QString datasetname, QString columnname, std::vector<long> &ids, void** result, int *size) = 0;
    virtual QVector<QString> getStringTuples(QString dataName, QString columnName, std::vector<long> ids) = 0;
    virtual QVector<int> getIntegerTuples(QString dataName, QString columnName, std::vector<long> ids) = 0;
    virtual QVector<float> getFloatTuples(QString dataName, QString columnName, std::vector<long> ids) = 0;

    // Get the polygons for the given region data
    virtual QVector<QPolygonF>  getPolygons(QString dataName) = 0;

    /**
     * Meta Data API
    **/
    virtual bool             hasDataset(QString dataName) = 0;
    virtual bool             hasPolygonDataset(QString datasetname) = 0;
    virtual QVector<QString> getDatasets() = 0;
    virtual QVector<QString> getPolygonDatasets() = 0;
    virtual QVector<QString> getSpatialTemporalColumns(QString datasetname) = 0;
    virtual QVector<QString> getOtherColumns(QString datasetname) = 0;
};

#endif // DATASTORE_HPP
