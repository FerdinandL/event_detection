#ifndef FUNCTIONCACHE_HPP
#define FUNCTIONCACHE_HPP

#include <QVector>
#include <QMap>
#include <QString>

class DataManager;

typedef QVector<float> ScalarFunction;
typedef QMap<QString, ScalarFunction> ResolutionCache;

class FunctionCache
{
public:
    enum FunctionType {Density, Distance};

public:
    FunctionCache(QString folder, DataManager *manager);

    ScalarFunction getScalarFunction(FunctionType ftype, QString region,QString dataName, int spaceIndex);
    ScalarFunction getScalarFunction(FunctionType ftype, QString region, QString dataName, int spaceIndex, int timeIndex, long startTime, long endTime);

private:
    QMap<QString, int> regionMap;
    QMap<int,ResolutionCache> functionCache;
    QVector<QPair<int,QString> > cacheQueue;

    DataManager *dataManager;
    QString cacheFolder;

protected:
    QString getKey(FunctionType ftype, QString dataName, int spaceIndex, int timeIndex, long startTime, long endTime);
    QString getFunctionFileName(int reg, QString key);
};

#endif // FUNCTIONCACHE_HPP
