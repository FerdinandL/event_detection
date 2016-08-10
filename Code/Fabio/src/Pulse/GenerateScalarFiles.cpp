#include "GenerateScalarFiles.hpp"
#include "../DataLayer/DataManager.hpp"

#include <omp.h>

GenerateScalarFiles::GenerateScalarFiles()
{

}

GenerateScalarFiles::~GenerateScalarFiles()
{
}

void GenerateScalarFiles::run()
{

    this->cities = {"sf", "nyc"};

    this->lls = {{"sf",QPointF(37.654199,-122.533951)}, {"nyc", QPointF(40.7003,-74.0278)}};   // Coordinates fit to raw monitored area in taxi1110b.csv
    this->trs = {{"sf",QPointF(37.83609,-122.34375)}, {"nyc", QPointF(40.7904,-73.9437)}};
    this->cellSizeInMeters = 10.0f;                                                            // Grid resolution in meters

    this->taxi();
    qDebug() << "Done!!!";
    exit(0);
}


void GenerateScalarFiles::taxi()
{

    this->cities = {"nyc"};
    this->datasets = {{"nyc",{"taxi2"}}};

    this->columns = {{"nyc_taxi2",{"DAY"}}};

    this->values = {{"nyc_taxi2",{{1,31}}}};


    this->filters.clear();

    this->load();
    this->createScalarFunctions();
}



void GenerateScalarFiles::load()
{
    for(int ci=0; ci<this->cities.size(); ci++) {
        QString city = this->cities[ci];
        DataManager::setDataFolder(this->cities[ci],"../data/"+this->cities[ci]+"/datastore/");
        DataManager* manager = DataManager::getInstance(this->cities[ci]);
        DataStore* store = manager->getDataStore();
        IndexStore *index = manager->getIndexStore();

        for(int di=0; di<this->datasets[city].size(); di++) {
            store->loadCSVData(this->datasets[city][di], "../rawdata/"+city+"/"+this->datasets[city][di]+"_table.csv");
            index->createStingIndex(this->datasets[city][di]);
        }
    }
}

void GenerateScalarFiles::createScalarFunctions()
{
    for(int ci=0; ci<this->cities.size(); ci++) {
        QString city = this->cities[ci];
        DataManager::setDataFolder(this->cities[ci],"../data/"+this->cities[ci]+"/datastore/");

        for(int di=0; di<this->datasets[city].size(); di++) {
            QString dataName = this->datasets[city][di];
            QString name = city+"_"+dataName;

            // without filter
            {
                for(int co=0; co<this->columns[name].size(); co++) {
                    QString column = this->columns[name][co];
                    int start = this->values[name][co][0];
                    int end = this->values[name][co][1];
                    for(int vi=start; vi<=end; vi++) {
                        QVector<Filter<int>> aux;
                        aux << QVector<Filter<int>>();
                        this->saveScalarFunction(city, dataName, column, vi, aux, "");
                    }
                }
            }

            // with filter
            {
                for(int fi=0; fi<this->filters.size(); fi++) {
                    for(int co=0; co<this->columns[name].size(); co++) {
                        QString column = this->columns[name][co];

                        if(column == this->filters[fi].ignoreColumn)
                            continue;


                        int start = this->values[name][co][0];
                        int end = this->values[name][co][1];
                        for(int vi=start; vi<=end; vi++) {
                            QVector<Filter<int>> aux;
                            aux << this->filters[fi];
                            this->saveScalarFunction(city, dataName, column, vi, aux, this->filters[fi].name);
                        }
                    }
                }
            }
        }
    }
}

QVector<float> GenerateScalarFiles::computeScalarFunction(QPoint gridSize, QString cityName, QString dataName, QString columnName, int columnValue, QVector<Filter<int>>& filters)
{
    qDebug() << "creating density function for " << cityName << dataName << columnName << columnValue;
    DataManager* manager = DataManager::getInstance(cityName);

    QRectF boundingRect;
    boundingRect.setTopLeft(this->lls[cityName]);
    boundingRect.setBottomRight(this->trs[cityName]);
    qDebug() << "finished";

    QVector<Filter<int>> allFilters;
    if(columnName.size() > 0)
        allFilters.push_back(Filter<int>(columnName,"",QPair<int,int>(columnValue,columnValue)));
    for(int i=0; i<filters.size(); i++) {
        if(filters[i].columnName.size() > 0)
            allFilters.push_back(filters[i]);
    }
    return manager->createDensityFunction(boundingRect,gridSize.y(),gridSize.x(),dataName,-1,-1,-1,0,allFilters,this->countColumn);
}

QPoint GenerateScalarFiles::getGridSize(QString city)
{
    QPointF lb = Camera::geo2world(this->lls[city]);
    QPointF rt = Camera::geo2world(this->trs[city]);
    QPointF center = (lb + rt)/2;
//    double gres = this->cam3->getGroundResolution();
    double gres = Camera::getGroundResolution((this->lls[city] + this->trs[city]) / 2, WORLD_ZOOM_LEVEL);
    float cellSize = this->cellSizeInMeters / gres;
    QPointF diff = rt - lb;
//    qDebug() << qSetRealNumberPrecision(10) << lb << rt << center << diff;
    int latRes = std::ceil(std::abs(diff.y()) / cellSize);
    int lonRes = std::ceil(std::abs(diff.x()) / cellSize);

    qDebug() << "Grid size:" << lonRes << latRes << "cell size:" << cellSize;

    return QPoint(lonRes,latRes);
}

void GenerateScalarFiles::saveScalarFunction(QString cityName, QString dataName, QString columnName, int columnValue, QVector<Filter<int>>& filters, QString fileNameSuffix)
{
    QString fileName;
    if(columnName == "ALL") {
        columnName = "";
        columnValue = -1;
        fileName = "../data/"+cityName+"/pulse/"+dataName+"_ALL_0"+fileNameSuffix+".scalars";
    }
    else {
        fileName = "../data/"+cityName+"/pulse/"+dataName+"_"+columnName+"_"+QString::number(columnValue)+""+fileNameSuffix+".scalars";
    }

    QPoint gridSize = this->getGridSize(cityName);
    QVector<float> fn = this->computeScalarFunction(gridSize, cityName, dataName, columnName, columnValue, filters);

    qDebug() << "writing density function";

    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly)) {
        qDebug() << "could not write file" << fileName;
        assert(false);
    }
    qDebug() << "Saving file" << fileName;
    QTextStream op(&file);
    op << gridSize.x() << "," << gridSize.y() << "\n";

    QRectF boundingRect;
    boundingRect.setTopLeft(this->lls[cityName]);
    boundingRect.setBottomRight(this->trs[cityName]);
    op << boundingRect.left() << "," << boundingRect.top() << "," << boundingRect.right() << "," << boundingRect.bottom() << "\n\n";
    for(int i = 0;i < fn.size();i ++) {
        op << fn[i] << "\n";
    }
    file.close();
}
