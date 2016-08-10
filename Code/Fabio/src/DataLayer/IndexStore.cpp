#include "IndexStore.hpp"

#include"kdindex/KdIndex.hpp"
#include "GridIndex.hpp"

#include <QDebug>

IndexStore::IndexStore(DataStore *dataStore, QString dataFolder): ds(dataStore), dataFolder(dataFolder) {
    indexSettings = new QSettings(dataFolder + "/indexSettings.ini",QSettings::Format::IniFormat);
    indexSettings->sync();
}

IndexStore::~IndexStore() { }

void IndexStore::createStingIndex(QString dataName) {
    int noBlocks = this->ds->getNumBlocks(dataName,STING_NAME);
    qDebug() << noBlocks;
    if(noBlocks == 0) {
        return;
    }
    indexSettings->setValue(dataName + "." + NO_BLOCKS, noBlocks);
    indexSettings->sync();
    createStingIndex(dataName,0,noBlocks - 1);
}

vector<long> IndexStore::querySting(QString dataName, const KdRequest* request, int noQueries) {
    QString fileName = dataFolder + "/" + dataName;
    QString dataType;
    int sizeInBytes, noSpatial;

    int noBlocks = indexSettings->value(dataName + "." + NO_BLOCKS).toInt();
    this->ds->getTupleSize(dataName,STING_NAME,&dataType,&sizeInBytes,&noSpatial);
    int size = sizeInBytes / sizeof(Trip);

    vector<long> sel;
    for(int i = 0;i < noBlocks;i ++) {
        QString keysFile = QString(fileName + ".sting.%1.keys").arg(i);
        QString rangeFile = QString(fileName + ".sting.%1.range").arg(i);
        QString treeFile = QString(fileName + ".sting.%1.tree").arg(i);


        CudaDb db(keysFile.toStdString().c_str(),rangeFile.toStdString().c_str(),treeFile.toStdString().c_str(),size);
        for(int i = 0;i < noQueries;i ++) {
            db.requestQuery(request[i]);
        }
        RequestResult result = KdRequest::emptyResult();
        for(int i = 0;i < noQueries;i ++) {
            db.getResult(result);
        }
        sel.insert(sel.end(), result->begin(), result->end());
    }
    std::sort(sel.begin(), sel.end());
    return sel;
}

QVariant IndexStore::getValue(QString key) {
    return this->indexSettings->value(key);
}

void IndexStore::createStingIndex(QString dataName, int startBlock, int endBlock) {
    QString dataType;
    int sizeInBytes, noSpatial;
    this->ds->getTupleSize(dataName,STING_NAME,&dataType,&sizeInBytes,&noSpatial);
    assert(dataType == DataTypeArena::SPACETIME);

    int size = sizeInBytes / sizeof(Trip) - 1;
    int EXTRA_BLOCKS_PER_LEAF = size + 1;
    QString fileName = dataFolder + "/" + dataName;
    qDebug() << fileName;
    for(int i = startBlock;i <= endBlock;i ++) {
        Trip* trips;
        int startId, endId;
        this->ds->getData(dataName,STING_NAME,i,(void **)(&trips),&startId,&endId);
        indexSettings->setValue(QString(dataName + "." + BLOCK_START + ".%1").arg(i), startId);

        int n = endId - startId + 1;
        // Ceil
        uint64_t blocks = (n / KdBlock::MAX_RECORDS_PER_BLOCK) + 1;
        KdBlock::KdNode *nodes = (KdBlock::KdNode*) malloc(sizeof(KdBlock::KdNode) * (2 * blocks * EXTRA_BLOCKS_PER_LEAF));
        uint64_t *tmp = (uint64_t*) malloc(sizeof(uint64_t) * n * 2);
        uint64_t *range = new uint64_t[size * 2];
        TripKey * keys =  new TripKey[size + 1];
        qDebug() << "Finished allocating memory";

        std::vector<uint64_t> blockRange;
        QString keysFile = QString(fileName + ".sting.%1.keys").arg(i);
        qDebug() << keysFile;
        FILE *fdata = fopen(keysFile.toStdString().c_str(), "wb");
        uint64_t freeNode = 1;
        uint64_t offset = 0;
        buildKdTree(nodes, tmp, trips, n, 0, 0, freeNode, fdata, blockRange,size, range, keys, offset);
        fclose(fdata);

        QString rangeFile = QString(fileName + ".sting.%1.range").arg(i);
        qDebug() << rangeFile;
        FILE *fblock = fopen(rangeFile.toStdString().c_str(), "wb");
        fwrite(&blockRange[0], sizeof(uint64_t), blockRange.size(), fblock);
        fclose(fblock);

        // Writing new indices file
        QString treeFile = QString(fileName + ".sting.%1.tree").arg(i);
        qDebug() << "Writing " << freeNode << " nodes to " << treeFile;
        FILE *fo = fopen(treeFile.toStdString().c_str(), "wb");
        fwrite(nodes, sizeof(KdBlock::KdNode), freeNode, fo);
        fclose(fo);

        free(nodes);
        free(tmp);
        delete[] range;
        delete[] keys;
        indexSettings->sync();
    }
}

void IndexStore::createGridIndex(QString dataName, int xs, int ys) {
    QVector<QPolygonF> polygons = this->ds->getPolygons(dataName);
    indexSettings->setValue(dataName + ".grid", 1);
    indexSettings->sync();

    GridIndex grid(xs,ys);
    grid.buildGrid(polygons);
    QString fileName = dataFolder + "/" + dataName;
    grid.outputGrid(fileName);
    qDebug() << "finished building grid index";
}


GridIndex IndexStore::loadGridIndex(QString dataName) {
    if(indexSettings->contains(dataName + ".grid")) {
        GridIndex grid;
        QString fileName = dataFolder + "/" + dataName;
        grid.readGrid(fileName);
        grid.setPolygons(this->ds->getPolygons(dataName));
        return grid;
    }
    else {
        qDebug() << "Error: Could not load grid" << dataName;
    }
    return GridIndex();
}
