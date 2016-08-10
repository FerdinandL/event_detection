#ifndef GENERATESCALARFILES_H
#define GENERATESCALARFILES_H

#include "../DataLayer/DataManager.hpp"
#include "../DataLayer/kdindex/KdIndex.hpp"
#include "../DataLayer/GridIndex.hpp"
#include "../DataLayer/kdindex/Neighborhoods.hpp"
#include "../BaseGui.hpp"
#include "../MapView/Camera.hpp"

#include <QVector>
#include <QStringList>
#include <QRectF>
#include <QString>
#include <QStringList>
#include <QPointF>
#include <QFile>

class GenerateScalarFiles: public BaseGui
{
public:
    GenerateScalarFiles();
    ~GenerateScalarFiles();
    void run();
    QVector<float> computeScalarFunction(QPoint gridSize, QString cityName, QString dataName, QString columnName, int columnValue, QVector<Filter<int>>& filters);
    void saveScalarFunction(QString cityName, QString dataName, QString columnName, int columnValue, QVector<Filter<int>>& filters, QString fileNameSuffix);

    void taxi();
    void load();
    void createScalarFunctions();
    QPoint getGridSize(QString city);
private:
    QVector<QString> cities;
    QMap<QString,QStringList> datasets;
    QMap<QString,QPointF> lls;
    QMap<QString,QPointF> trs;
    QVector<Filter<int>> filters;
    QMap<QString,QStringList> columns;
    QMap<QString,QVector<QVector<int>>> values;
    QString countColumn;
    float cellSizeInMeters;

};

#endif // GENERATESCALARFILES_H
