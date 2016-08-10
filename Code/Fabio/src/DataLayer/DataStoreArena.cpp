
#include <algorithm>
#include <sstream>
#include <fstream>
#include <string>
#include <QSettings>
#include <QFile>
#include <QRegExp>
#include <QTextStream>
#include <QStringList>
#include <QDebug>
#include <QString>
#include <QPolygonF>
#include <QRgb>
#include <QVector2D>

#include "DataStore.hpp"
#include "DataStoreArena.hpp"

#include "./MapView/Camera.hpp"

#define ARENA_SIZE (1ULL << 33)
#define BLOCK_SIZE (1ULL << 25)

//-----------------------------------------------------------------------------
// Block
//-----------------------------------------------------------------------------
Block::Block(size_t totalsize, QString dn, QString cn, size_t typesize, QString dt, bool isPoly, int ns, QString stn)
{
    totalSize = totalsize;
    typeSize = typesize;
    currentSize = 0;
    prev = nullptr;
    next = nullptr;
    data_prev = nullptr;
    data_next = nullptr;
    first = nullptr;
    dateTypeName = dt;
    numSpatialAttributes = ns;
    isPolygon = isPoly;

    datasetname = dn;
    columnname = cn;
    spacetimename = stn;

    start_id = std::numeric_limits<unsigned int>::max();
    end_id = 0;
}

void Block::read(unsigned int id, void* value)
{
    unsigned int blockId = id - this->start_id;

    // important: SPATIOTEMPORAL has 32 bytes (lat,lon,time,index)
    std::copy(((char*)first + typeSize*blockId), ((char*)first + typeSize*(blockId+1)), (char*) value);
    //std::cout << (char*)first + type_size*position << " " << *(int*)value << std::endl;
}

void Block::write(unsigned int id, void *value)
{
//    qDebug() << "mem content:" << *(char*)first;
//    memset(first, '1', 1);
//    first = realloc(first, currentSize+typeSize);
    std::copy((char*)value, ((char*)value+typeSize), ((char*)first + currentSize));
    start_id = std::min(id, start_id);
    end_id = std::max(id, end_id);
    //std::cout << *(uint64_t*)value << std::endl;
    //std::cout << (char*)first + type_size*position << " "<< *(uint64_t*)value << std::endl;

    currentSize+=typeSize;
}

void Block::restore(QString filename)
{
    std::ifstream ifs(filename.toStdString());

    ifs.read((char*)this->first, (this->end_id - this->start_id + 1) * this->typeSize);

    ifs.close();

}

void Block::persist(QSettings *settings)
{
    QString name = this->datasetname+"."+this->columnname+"_"+QString::number(this->data_index);

    QString filename = settings->value("storage/path").toString()+"/"+name;
    std::ofstream ofs(filename.toStdString());

    ofs.write((char*)this->first, (this->end_id - this->start_id + 1) * this->typeSize);

    ofs.close();

    // save settings
    QString index = "block_"+QString::number(this->global_index);
    settings->setValue(index+"/filename", filename);
    settings->setValue(index+"/dataset", this->datasetname);
    settings->setValue(index+"/column", this->columnname);
    settings->setValue(index+"/start_id", QString::number(this->start_id));
    settings->setValue(index+"/end_id", QString::number(this->end_id));
    settings->setValue(index+"/type_size", QString::number(this->typeSize));
    settings->setValue(index+"/total_size", QString::number(this->totalSize));
    settings->setValue(index+"/current_size", QString::number(this->currentSize));
    settings->setValue(index+"/data_type", this->dateTypeName);
    settings->setValue(index+"/num_spatial", QString::number(this->numSpatialAttributes));
    settings->setValue(index+"/spacetime_name", this->spacetimename);
    settings->setValue(index+"/is_polygon", this->isPolygon);
    settings->sync();
}

//-----------------------------------------------------------------------------
// DataStoreArena
//-----------------------------------------------------------------------------
DataStoreArena::DataStoreArena(QString dataFolder)
{
    const size_t arena_size = ARENA_SIZE;

    mmap = new ::storage::util::MMap(arena_size);
    blocks.first = nullptr;
    blocks.last = nullptr;
    end_block = mmap->base;//, mmap->size;         //nullptr;
    block_map = std::map<QString, Block*>();
    num_blocks = 0;

    this->dataFolder = dataFolder;
    this->settings = new QSettings(dataFolder + "/meta.ini",QSettings::Format::IniFormat);
    settings->setValue("storage/path", dataFolder);
    settings->sync();
    this->restore();
}

void DataStoreArena::restore()
{
    //QString path = settings.value("storage/path").toString();
    unsigned int num_blocks = settings->value("storage/size").toUInt();

    for(int i=0; i<num_blocks; ++i) {
        QString index = "block_"+QString::number(i);
        QString filename = settings->value(index+"/filename").toString();
        QString datasetname = settings->value(index+"/dataset").toString();
        QString columnname = settings->value(index+"/column").toString();
        unsigned int start_id = settings->value(index+"/start_id").toUInt();
        unsigned int end_id = settings->value(index+"/end_id").toUInt();
        size_t type_size = settings->value(index+"/type_size").toUInt();
        size_t total_size = settings->value(index+"/total_size").toUInt();
        size_t current_size = settings->value(index+"/current_size").toUInt();
        QString data_type = settings->value(index+"/data_type").toString();
        int num_spatial = settings->value(index+"/num_spatial").toUInt();
        QString spacetimename = settings->value(index+"/spacetime_name").toString();
        bool isPolygon = settings->value(index+"/is_polygon").toBool();


        Block *block = this->createBlock(total_size, datasetname, columnname, type_size, data_type, isPolygon, num_spatial, spacetimename);
        block->start_id = start_id;
        block->end_id = end_id;
        block->currentSize = current_size; // no need to do this. This is done at block::write
        block->spacetimename = spacetimename;
        block->restore(filename);
    }
}

void DataStoreArena::persist()
{
    Block* current = this->blocks.first;

    this->settings->setValue("/storage/size", this->num_blocks);
    this->settings->sync();

    while(current != nullptr) {
        current->persist(this->settings);
        current = current->next;
    }
}

void DataStoreArena::loadPolyData(QString dataName, QVector<QPolygonF> polygons)
{
    const size_t block_size = BLOCK_SIZE;
    QVector<int> layerNumPoints;
    QVector<uint64_t> layerXs;
    QVector<uint64_t> layerYs;

    for(int i=0; i<polygons.size(); ++i) {
        QPolygonF poly = polygons[i];
        layerNumPoints.push_back(poly.size());
        for(int j=0; j<poly.size(); ++j) {
            QPointF geo = QPointF(poly[j].x(),poly[j].y());//Camera::world2geo(QVector2D(poly[j].x(),poly[j].y()).toPointF());
            layerXs.push_back(double2uint(geo.x()));
            layerYs.push_back(double2uint(geo.y()));
        }
    }

    this->insertIntegers(block_size, dataName, "layerNumPoints", layerNumPoints, true);
    this->insertLongs(block_size, dataName, "layerXs", layerXs, true);
    this->insertLongs(block_size, dataName, "layerYs", layerYs, true);

    this->persist();
}

void DataStoreArena::loadPolyData(QString dataName, QString polyFile)
{
    const size_t block_size = BLOCK_SIZE;

    QFile fi(polyFile);
    if(!fi.open( QIODevice::ReadOnly | QIODevice::Text )) {
        qDebug() << "Could not open file" << polyFile;
        return;
    }

    QTextStream input(&fi);

    // Fixed origin for now
//    QPointF origin = Camera::geo2level(QPointF(41.099334, -74.522011), WORLD_ZOOM_LEVEL);


    // Data to save in datastore
    QVector<QString> names;
    QVector<QString> layerNames;
    QVector<int> layerNumItems;
    QVector<QString> layerWords;
    QVector<QString> layerStrokes;
    QVector<float> layerStrokeWidths;
    QVector<QString> layerFills;
    QVector<int> layerNumPolys;
    QVector<int> layerNameId;
    QVector<int> layerNumPoints;
    QVector<uint64_t> layerXs;
    QVector<uint64_t> layerYs;
    QVector<float> layerDepths;


    // Read headers
    bool offset = false;
    {
        double fw, fh;
        double r[4];
        input >> fw >> fh;
        input >> r[0] >> r[1] >> r[2] >> r[3];
        qDebug() << "Ignoring headers";
    }

    // Read textures
    {
        int nTexture;
        QString word;
        input >> word >> nTexture;
        input.readLine();
        for (int i=0; i<nTexture; i++)
            qDebug() << "Ignoring texture" << i;
    }

    // Read names
    {
        int nName;
        QString word;
        input >> word >> nName;
        input.readLine();
        for (int i=0; i<nName; i++) {
            QString name = input.readLine().toLatin1();
            names << name.remove("?");
        }
        qDebug() << "Name Count: " << nName;
    }

    // Read layers
    {
        int n;
        QString word, stroke, fill;
        while (true) {
            qreal x, y, depth;
            QString layerName;
            input >> layerName >> n;
            if (input.atEnd()) break;
            input.readLine();
            qDebug() << "Layer:" << layerName << "-" << n << "items";

            layerNames << layerName;
            layerNumItems << n;

            for (int i=0; i<n; i++) {
                float strokeWidth;
                input >> word >> stroke >> strokeWidth >> fill;

                layerWords << word;
                layerStrokes << stroke;
                layerStrokeWidths << strokeWidth;
                layerFills << fill;

                if (word=="poly" || word=="fill") {
                    int nPoly, nPoint, nameId;
                    input >> nPoly >> depth >> nameId;

                    layerNumPolys << nPoly;
                    layerDepths << depth;
                    layerNameId << nameId;

                    for (int j=0; j<nPoly; j++) {
                        input >> nPoint;

                        layerNumPoints << nPoint;

                        for (int k=0; k<nPoint; k++) {
                            input >> x >> y;
                            QPointF geo = Camera::world2geo(QVector2D(x,y).toPointF());
                            layerXs << double2uint(geo.x());
                            layerYs << double2uint(geo.y());
                        }
                    }
                }
                else {
                    qDebug() << "UNKNOWN ITEM TYPE" << word;
                    exit(0);
                }
            }
        }
    }


//    qDebug() << names;
//    qDebug() << layerNames;
//    qDebug() << layerNumItems;
//    qDebug() << layerWords;
//    qDebug() << layerStrokes;
//    qDebug() << layerStrokeWidths;
//    qDebug() << layerFills;
//    qDebug() << layerNumPolys;
//    qDebug() << layerNameId;
//    qDebug() << "read from file:" << layerNumPoints;
//    qDebug() << layerXs;
//    qDebug() << layerYs;
//    qDebug() << layerDepths;

    this->insertStrings(block_size, dataName, "names", names, true);
    this->insertStrings(block_size, dataName, "layerNames", layerNames, true);
    this->insertIntegers(block_size, dataName, "layerNumItems", layerNumItems, true);
    this->insertStrings(block_size, dataName, "layerWords", layerWords, true);
    this->insertStrings(block_size, dataName, "layerStrokes", layerStrokes, true);
    this->insertStrings(block_size, dataName, "layerFills", layerFills, true);
    this->insertFloats(block_size, dataName, "layerStrokeWidths", layerStrokeWidths, true);
    this->insertIntegers(block_size, dataName, "layerNumPolys", layerNumPolys, true);
    this->insertIntegers(block_size, dataName, "layerNameId", layerNameId, true);
    this->insertIntegers(block_size, dataName, "layerNumPoints", layerNumPoints, true);
    this->insertLongs(block_size, dataName, "layerXs", layerXs, true);
    this->insertLongs(block_size, dataName, "layerYs", layerYs, true);
    this->insertFloats(block_size, dataName, "layerDepths", layerDepths, true);

//    std::vector<long> ids = {};
//    qDebug() << "read from ds:" << this->getIntegerTuples(dataName, "layerNumPoints", ids);

    this->persist();
}

QVector<QPolygonF> DataStoreArena::getPolygons(QString dataName)
{

    std::vector<long> allIds = {};

    QString columnName = "layerNumPoints";
    int *layerNumPoints;
    int layerNumPointsSize;
    this->getTuples(dataName, columnName, allIds, (void**)&layerNumPoints, &layerNumPointsSize);

    columnName = "layerXs";
    uint64_t *layerXs;
    int layerXsSize;
    this->getTuples(dataName, columnName, allIds, (void**)&layerXs, &layerXsSize);

    columnName = "layerYs";
    uint64_t *layerYs;
    int layerYsSize;
    this->getTuples(dataName, columnName, allIds, (void**)&layerYs, &layerYsSize);

    qDebug() << layerXsSize << layerYsSize;

    QVector<QPolygonF> polygons;
    int sum = 0;
    for(int i=0; i<layerNumPointsSize; i++) {
        int np = layerNumPoints[i];
        QPolygonF poly;
        for(int j=sum; j<sum+np; j++) {
            double x = uint2double(layerXs[j]);
            double y = uint2double(layerYs[j]);
            poly.append(QPointF(x,y));
        }
        sum+=np;

        polygons.push_back(poly);
    }

    free(layerNumPoints);
    free(layerXs);
    free(layerYs);

    return polygons;
}

void DataStoreArena::insertStrings(size_t blockSize, QString dataName, QString columnName, QVector<QString>& vec, bool isPoly)
{
    for(int i=0; i<vec.size(); i++) {
        char value[256];
        memset(value, '\0', 256);
        std::string aux = vec[i].toStdString();
        std::copy(aux.begin(), aux.end(), value);
        this->insert(blockSize, dataName, columnName, i, &value, 256, DataTypeArena::STRING, isPoly, 0);
    }
}

void DataStoreArena::insertIntegers(size_t blockSize, QString dataName, QString columnName, QVector<int>& vec, bool isPoly)
{
    for(int i=0; i<vec.size(); i++) {
        this->insert(blockSize, dataName, columnName, i, &vec[i], sizeof(int), DataTypeArena::INT, isPoly, 0);
    }
}

void DataStoreArena::insertFloats(size_t blockSize, QString dataName, QString columnName, QVector<float>& vec, bool isPoly)
{
    for(int i=0; i<vec.size(); i++) {
        this->insert(blockSize, dataName, columnName, i, &vec[i], sizeof(float), DataTypeArena::FLOAT, isPoly, 0);
    }
}

void DataStoreArena::insertLongs(size_t blockSize, QString dataName, QString columnName, QVector<uint64_t>& vec, bool isPoly)
{
    for(int i=0; i<vec.size(); i++) {
        this->insert(blockSize, dataName, columnName, i, &vec[i], sizeof(uint64_t), DataTypeArena::LONG, isPoly, 0);
    }
}

void DataStoreArena::loadCSVData(QString dataName, QString csvFile)
{
    const size_t block_size = BLOCK_SIZE;
    
    QFile inputFile(csvFile);
    if(!inputFile.open( QIODevice::ReadOnly | QIODevice::Text )) {
        qDebug() << "Could not open file" << csvFile;
        return;
    }


    QTextStream in(&inputFile);
    
    // column names
    QStringList names = in.readLine().split(",");
    
    // column types
    QStringList types = in.readLine().split(",");

//    qDebug() << "Inserting dataset" << dataName;
    
    unsigned int id = 0;
    while (!in.atEnd())
    {
        QStringList tokens = in.readLine().split(",");

//        if(id > 0)
//            break;
        
        
        // Reads input line, storing values into valuesmap
        std::map<QString,std::vector<std::pair<QString,QString>>> valuesmap;
        for(int i=0; i<tokens.length(); ++i) {
            
            QString col = names[i];
            QString type = types[i];
            QString value = tokens[i];
            std::pair<QString,QString> pair = std::pair<QString,QString>(col,value);

            if(type.compare(DataTypeInput::LATITUDE)==0 || type.compare(DataTypeInput::LONGITUDE)==0) {
                valuesmap[DataTypeArena::SPACE].push_back(pair);
            }
            else if(type.compare(DataTypeInput::DATE)==0 || type.compare(DataTypeInput::TIME)==0) {
                valuesmap[DataTypeArena::TIME].push_back(pair);
            }
            else {
                valuesmap[type].push_back(pair);
            }
        }

        if(id % 100000 == 0)
            qDebug() << "Inserting id" << id;
        
        
        // Iterates through valuesmap, inserting each column value into datastore
        int num_space = valuesmap[DataTypeArena::SPACE].size() / 2;
        int num_time = valuesmap[DataTypeArena::TIME].size();
        std::map<QString,std::vector<std::pair<QString,QString>>>::iterator iter;
        for(iter=valuesmap.begin(); iter!=valuesmap.end(); ++iter) {
            
            QString type = iter->first;
            std::vector<std::pair<QString,QString>> pairs = iter->second;

//            qDebug() << type;
            
            //std::cout << type.toStdString() << std::endl;
            
            if(type.compare(DataTypeArena::SPACE) == 0) {
                QString columname;
                
                // pack [lat,lon,lat,lon...][time,time,...][id] and call insert
                uint64_t uvalue[2*num_space+num_time + 1];
                for(int i=0; i<num_space; ++i) {
                    
                    columname += valuesmap[DataTypeArena::SPACE][2*i].first+",";
                    columname += valuesmap[DataTypeArena::SPACE][2*i+1].first+",";
                    
                    uvalue[2*i]   = double2uint(valuesmap[DataTypeArena::SPACE][2*i].second.toDouble());
                    uvalue[2*i+1] = double2uint(valuesmap[DataTypeArena::SPACE][2*i+1].second.toDouble());
                }
                for(int i=0; i<num_time; ++i) {
                    columname += valuesmap[DataTypeArena::TIME][i].first;
                    if(i != num_time -1)
                        columname+=",";
                    
                    uvalue[2*num_space+i] = long2uint(valuesmap[DataTypeArena::TIME][i].second.toLong());
                }
                
                // add an extra column, with the data index
                uvalue[2*num_space+num_time] = (uint64_t)id;

                this->insert(block_size, dataName, DataTypeArena::SPACETIME, id, &uvalue, sizeof(uint64_t)*(2*num_space+num_time + 1), DataTypeArena::SPACETIME, false, num_space, columname); // each spacetime is 24 bytes
                
            }
            else if(type.compare(DataTypeArena::LONG) == 0) {
                for(int i=0; i<pairs.size(); ++i) {
                    uint64_t uvalue = pairs[i].second.toLong();
                    this->insert(block_size, dataName, pairs[i].first, id, &uvalue, sizeof(uint64_t), DataTypeArena::LONG, false);
                }
            }
            else if(type.compare(DataTypeArena::FLOAT) == 0) {
                for(int i=0; i<pairs.size(); ++i) {
                    float value = pairs[i].second.toFloat();
                    this->insert(block_size, dataName, pairs[i].first, id, &value, sizeof(float), DataTypeArena::FLOAT, false);
                }
            }
            else if(type.compare(DataTypeArena::INT) == 0) {
                for(int i=0; i<pairs.size(); ++i) {
                    int value = pairs[i].second.toInt();
                    this->insert(block_size, dataName, pairs[i].first, id, &value, sizeof(int), DataTypeArena::INT, false);
                }
            }
            else if(type.compare(DataTypeArena::STRING) == 0) {
                for(int i=0; i<pairs.size(); ++i) {
                    // Careful about null terminator here
                    // see: http://stackoverflow.com/questions/10735990/c-size-of-a-char-array-using-sizeof
                    char value[256];
                    memset(value, '\0', 256);
                    std::string aux = pairs[i].second.toStdString();
                    std::copy(aux.begin(), aux.end(), value);
                    this->insert(block_size, dataName, pairs[i].first, id, &value, 256, DataTypeArena::STRING, false); // fixed size
                }
            }
        }
        
        id++;
        
    }
    inputFile.close();
    this->persist();

    if(datasets.indexOf(dataName) == -1) {
        datasets.append(dataName);
    }
}

void DataStoreArena::getTuples(QString datasetname, QString columnname, std::vector<long> &ids, void** result, int *size)
{
    QString name = datasetname+"."+columnname;
    std::map<QString, Block*>::iterator iter = block_map.find(name);
    *result = nullptr;
    *size = 0;

    if(iter == block_map.end()) {
        qDebug() << "Error: Could not find" << name;
        return;
    }

    Block* current = iter->second;

    int i = 0;
    // return all ids
    if(ids.size() == 0) {
        long cur_id = 0;
        while(current != nullptr) {
            if(cur_id >= current->start_id && cur_id <= current->end_id) {
                *result = realloc(*result, (i+1) * current->typeSize);

                int offset = current->typeSize * i;
                current->read(cur_id, (char*)*result+offset);
                cur_id++;
                i++;
            }
            else {
                current = current->data_next;
            }
        }
    }
    else {
        *result = malloc(ids.size() * current->typeSize);
        while(i < ids.size()) {
            long cur_id = ids[i];

            if(cur_id >= current->start_id && cur_id <= current->end_id) {
                int offset = current->typeSize * i;
                current->read(cur_id, (char*)*result+offset);
                i++;
            }
            else {
                current = current->data_next;
            }
        }
    }

    *size = i;
}



QVector<QString> DataStoreArena::getStringTuples(QString dataName, QString columnName, std::vector<long> ids)
{
    char *names;
    int namesSize;
    this->getTuples(dataName, columnName, ids, (void**)&names, &namesSize);

    QVector<QString> vecNames;
    for(int i=0; i<namesSize; ++i) {
        std::string aux = std::string(names+256*i);
        QString n = QString::fromStdString(aux);
        vecNames.push_back(n);
    }
    free(names);
    return vecNames;
}

QVector<int> DataStoreArena::getIntegerTuples(QString dataName, QString columnName, std::vector<long> ids)
{
    int *values;
    int size;
    this->getTuples(dataName, columnName, ids, (void**)&values, &size);

    QVector<int> vecValues(size);
    for(int i=0; i<size; ++i) {
        vecValues[i] = values[i];
    }
    free(values);
    return vecValues;
}

QVector<float> DataStoreArena::getFloatTuples(QString dataName, QString columnName, std::vector<long> ids)
{
    float *values;
    int size;
    this->getTuples(dataName, columnName, ids, (void**)&values, &size);

    QVector<float> vecValues(size);
    for(int i=0; i<size; ++i) {
        vecValues[i] = values[i];
    }
    free(values);
    return vecValues;
}

Block* DataStoreArena::getBlock(QString datasetname, QString columnname)
{
    QString name = datasetname+"."+columnname;
    std::map<QString, Block*>::iterator iter = block_map.find(name);

    // block not found
    if(iter == block_map.end()) {
        throw std::runtime_error("block not found!");
    }
    else {
        return iter->second;
    }
}

Block* DataStoreArena::createBlock(size_t size, QString datasetname, QString columnname, size_t type_size, QString data_type, bool isPoly, int num_spatial, QString spacetimename)
{
    // create a new block at position end_block
    Block* newblock = new Block(size, datasetname, columnname, type_size, data_type, isPoly, num_spatial, spacetimename);

    // this is the first block created
    if(blocks.first == nullptr) {
        blocks.first = newblock;
        blocks.last = newblock;
        newblock->first = end_block; //malloc(size);//end_block;
        end_block = (char*)newblock->first + size;
    }
    else {
//        qDebug() << "not first block" << end_block << "size" << size;
        blocks.last->next = newblock;
        newblock->prev = blocks.last;

        blocks.last = newblock;
        newblock->first = end_block;   // malloc(size);
        end_block = (char*)newblock->first + size;
    }

    // register on block map
    QString name = datasetname+"."+columnname;
    std::map<QString, Block*>::iterator iter = block_map.find(name);

//            std::cout << "Creating block for " << name.toStdString() << std::endl;
//            std::cout << size << std::endl;
//            std::cout << (newblock->first) << " " << (end_block) << std::endl;

    // first block of datasetname.columnname
    if(iter == block_map.end()) {
        block_map.insert(std::pair<QString,Block*>(name,newblock));
        num_blocks_map.insert(std::pair<QString,unsigned int>(name,1));
        newblock->global_index = num_blocks;
        newblock->data_index = 0;
    }
    // find the last block of datasetname.columnname and make it point to newblock
    else {
        Block* current = iter->second;
        while(current != nullptr) {
            if(current->data_next == nullptr) {
                current->data_next = newblock;
                newblock->data_prev = current;
                newblock->global_index = num_blocks;
                newblock->data_index = current->data_index + 1;
                num_blocks_map.find(name)->second++;
                current = nullptr;
            }
            else {
                current = current->data_next;
            }
        }
    }

    num_blocks++;

    if(num_spatial > 0) {
        std::map<QString, QVector<QString>>::iterator iter2 = spatiotemporal_cols_map.find(datasetname);
        if(iter2 == spatiotemporal_cols_map.end()) {
            spatiotemporal_cols_map[datasetname] << spacetimename.split(",").toVector();
        }
    }
    else {
        std::map<QString, QVector<QString>>::iterator iter2 = other_cols_map.find(datasetname);
        if(iter2 == other_cols_map.end()) {
            other_cols_map[datasetname].append(columnname);
        }
    }

    if(isPoly) {
        if(this->polygonDatasets.indexOf(datasetname) == -1) {
            this->polygonDatasets.append(datasetname);
        }
    }
    else {
        if(this->datasets.indexOf(datasetname) == -1) {
            this->datasets.append(datasetname);
        }
    }

    return newblock;
}

void DataStoreArena::insert(size_t size, QString datasetname, QString columnname, unsigned int id, void *value, size_t type_size, QString data_type, bool isPoly, int num_spatial, QString spacetimename)
{
    QString name = datasetname+"."+columnname;
    std::map<QString, Block*>::iterator iter = block_map.find(name);

//    qDebug() << name << num_blocks;
    //std::cout << "Inserting into block " << name.toStdString() << std::endl;

    // first block of datasetname.columname
    // create a new block
    if(iter == block_map.end()) {
        qDebug() << datasetname << columnname << data_type;
        //std::cout << "Creating block " << name.toStdString() << std::endl;
        Block* block = createBlock(size, datasetname, columnname, type_size, data_type, isPoly, num_spatial, spacetimename);
        block->write(id, value);
    }
    // find the last block of datasetname.columnname and insert into it
    else {
        Block* current = iter->second;
        while(current != nullptr) {

            // Check also if id is outside range of ids already inserted
            if(id > current->end_id && current->data_next == nullptr) {

                // make sure there is space in the block
//                qDebug() << datasetname << current->currentSize << " of max " << current->totalSize;
                if(current->currentSize + type_size < current->totalSize) {
                    current->write(id, value);
                    current = nullptr;
                }
                else {
                    current = createBlock(size, datasetname, columnname, type_size, data_type, isPoly, num_spatial, spacetimename);
                    current->write(id, value);
                    current = nullptr;
                }
            }
            else {
                current = current->data_next;
            }
        }
    }
}

void DataStoreArena::getData(QString datasetname, QString columnname, int block_index, void **start, int *startid, int *endid)
{
    QString name = datasetname+"."+columnname;
    std::map<QString, Block*>::iterator iter = block_map.find(name);
    Block* current = iter->second;

    // find the block with block_index
    while(current != nullptr) {
        if(current->data_index == block_index) {
            *start = current->first;
            *startid = current->start_id;
            *endid = current->end_id;
            qDebug() << current->start_id << current->end_id;
            return;
        }
        else {
            current = current->data_next;
        }
    }
}

bool DataStoreArena::getTupleSize(QString datasetname, QString columnname, QString *dataType, int *size, int *noSpatialAttributes)
{
    QString name = datasetname+"."+columnname;
    std::map<QString, Block*>::iterator iter = block_map.find(name);

    if(iter == block_map.end()) {
        return false;
    }
    else {
        Block* current = iter->second;

        *dataType = current->dateTypeName;
        *noSpatialAttributes = current->numSpatialAttributes;
        *size = current->typeSize;
        return true;
    }
}

int DataStoreArena::getNumBlocks(QString datasetname, QString columnname)
{
    QString name = datasetname+"."+columnname;
    std::map<QString, unsigned int>::iterator iter = num_blocks_map.find(name);

    if(iter == num_blocks_map.end())
        return 0;
    else
        return iter->second;
}

QVector<QString> DataStoreArena::getPolygonDatasets()
{
    return this->polygonDatasets;
}

QVector<QString> DataStoreArena::getDatasets()
{
    return this->datasets;
}

QVector<QString> DataStoreArena::getSpatialTemporalColumns(QString datasetname)
{
    return this->spatiotemporal_cols_map[datasetname];
}

QVector<QString> DataStoreArena::getOtherColumns(QString datasetname)
{
    return this->other_cols_map[datasetname];
}

bool DataStoreArena::hasDataset(QString datasetname)
{
    for(int i=0; i<this->datasets.size(); ++i) {
        if(this->datasets[i] == datasetname)
            return true;
    }
    return false;
}

bool DataStoreArena::hasPolygonDataset(QString datasetname)
{
    for(int i=0; i<this->polygonDatasets.size(); ++i) {
        if(this->polygonDatasets[i] == datasetname)
            return true;
    }
    return false;
}

int DataStoreArena::getTypeSize(QString datasetname, QString columnname)
{
    QString name = datasetname+"."+columnname;
    std::map<QString, Block*>::iterator iter = block_map.find(name);

    if(iter != block_map.end()) {
        Block* current = iter->second;
        return current->typeSize;
    }
    else
        return 0;
}

