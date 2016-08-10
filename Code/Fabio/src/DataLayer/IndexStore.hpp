#ifndef INDEXSTORE_H
#define INDEXSTORE_H

#include "DataStore.hpp"
#include "kdindex/Trip.hpp"
#include "kdindex/CudaDb.hpp"
#include "GridIndex.hpp"

#include <QString>
#include <QSettings>

#define STING_NAME "spacetime"
#define NO_BLOCKS "noBlocks"
#define BLOCK_START "startId"


class IndexStore
{
public:
    enum IndexType {
        STING, GRID
    };

    IndexStore(DataStore *dataStore, QString dataFolder);
    ~IndexStore();

    void createStingIndex(QString dataName);
    void createGridIndex(QString dataName, int xs = 1024, int ys = 1024);

    // TODO : define functions corresponding to different types of queries.

    // return indexes for the spatio temporal query. Can send multiple queries in through the request array.
    vector<long> querySting(QString dataName, const KdRequest* request, int noQueries);
    QVariant getValue(QString key);
    GridIndex loadGridIndex(QString dataName);

private:
    void createStingIndex(QString dataName, int startBlock, int endBlock);

private:
    DataStore* ds;
    QSettings* indexSettings;
    QString dataFolder;
};

#endif // INDEXSTORE_H
