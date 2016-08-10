#ifndef DATASTOREARENA_HPP
#define DATASTOREARENA_HPP

#include <QString>
#include <QVector>
#include <QPolygon>
#include <QPair>

//---------------------------------------------------------------------
// Block
//---------------------------------------------------------------------
class Block {
public:
    Block(size_t totalsize, QString datasetname, QString columnname, size_t typesize, QString dateType, bool isPoly, int num_spatial, QString stn);
    Block() = delete;

    void write(unsigned int id, void* value);
    void read(unsigned int id, void* value);
    void restore(QString filename);
    void persist(QSettings *settings);

    size_t totalSize;
    size_t currentSize; // min: 0, max: total_size
    size_t typeSize;
    QString dateTypeName;
    int numSpatialAttributes;

    // pointer to first byte in memory
    void* first;

    // previous/next physical block
    Block* prev;
    Block* next;
    // previous/next block (where the column data continues)
    Block* data_prev;
    Block* data_next;

    // colum data start and end ids
    unsigned int start_id;
    unsigned int end_id;

    bool isPolygon;
    QString datasetname;
    QString columnname; // if it is spacetime, then column name is "spacetime"
    QString spacetimename;
    unsigned int global_index; // index among all blocks
    unsigned int data_index; // index among all blocks of the same column
};



class DataStoreArena : public DataStore {
public:

    DataStoreArena(QString dataFolder);

    void loadPolyData(QString dataName, QVector<QPolygonF> polygons);
    void loadPolyData(QString dataName, QString polyFile);
    void loadCSVData(QString dataName, QString csvFile);

    void getTuples(QString datasetname, QString columnname, std::vector<long>& ids, void **values, int *size);
    QVector<QString> getStringTuples(QString dataName, QString columnName, std::vector<long> ids);
    QVector<int> getIntegerTuples(QString dataName, QString columnName, std::vector<long> ids);
    QVector<float> getFloatTuples(QString dataName, QString columnName, std::vector<long> ids);
    void insert(size_t size, QString datasetname, QString columnname, unsigned int id, void *value, size_t type_size, QString data_type, bool isPoly, int num_spatial = 0, QString stn = "");
    void insertStrings(size_t blockSize, QString dataName, QString columnName, QVector<QString> &vec, bool isPoly);
    void insertLongs(size_t blockSize, QString dataName, QString columnName, QVector<uint64_t> &vec, bool isPoly);
    void insertIntegers(size_t blockSize, QString dataName, QString columnName, QVector<int> &vec, bool isPoly);
    void insertFloats(size_t blockSize, QString dataName, QString columnName, QVector<float> &vec, bool isPoly);

    // Get a start position and size in memory of dataset.colum, block_index
    void getData(QString datasetname, QString columnname, int block_index, void **start, int *startid, int *endid);
    int getNumBlocks(QString datasetname, QString columnname);

    QVector<QPolygonF> getPolygons(QString dataName);
    QVector<QString> getPolygonDatasets();
    QVector<QString> getDatasets();
    QVector<QString> getSpatialTemporalColumns(QString datasetname);
    QVector<QString> getOtherColumns(QString datasetname);
    int getNumSpatial();
    bool hasDataset(QString datasetname);
    bool hasPolygonDataset(QString datasetname);

    void restore();
    void persist();
    Block *getBlock(QString datasetname, QString columnname);
    Block *createBlock(size_t size, QString datasetname, QString columnname, size_t type_size, QString data_type, bool isPoly, int num_spatial = 0, QString stn = "");

    // returns false if column not found
    bool getTupleSize(QString datasetname, QString columnname, QString *dataType, int *size, int *noSpatialAttributes);
    int getTypeSize(QString datasetname, QString columnname);

    /*
     * Access points for all the blocks in the Arena. Starting at the last block,
     * we can follow the prev links to traverse all blocks.
     */
    struct {
        Block* first;
        Block* last;
    } blocks;

    /*
     * Address that indicates the first point beyond a valid Block address.
     */
    void*  end_block;

    unsigned int num_blocks;

    /*
     * Map between datasetname,columnname and first block position
     * If block.data_next == null, then there is only one block for that
     * data column
     */
    std::map<QString, Block*> block_map;
    std::map<QString, unsigned int> num_blocks_map;
    QVector<QString> datasets;
    QVector<QString> polygonDatasets;
    std::map<QString, QVector<QString>> spatiotemporal_cols_map;
    std::map<QString, QVector<QString>> other_cols_map;
private:
    ::storage::util::MMap *mmap;
    QString dataFolder;
    QSettings* settings;
};



#endif // DATASTOREARENA_HPP
