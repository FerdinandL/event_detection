#include "FunctionCache.hpp"
#include "DataManager.hpp"

FunctionCache::FunctionCache(QString folder, DataManager *manager) {
    this->dataManager = manager;
    this->cacheFolder = folder + "cache/";
}

ScalarFunction FunctionCache::getScalarFunction(FunctionType ftype, QString region, QString dataName, int spaceIndex) {
    return this->getScalarFunction(ftype,region,dataName,spaceIndex,-1,-1,-1);
}

ScalarFunction FunctionCache::getScalarFunction(FunctionType ftype, QString region, QString dataName, int spaceIndex, int timeIndex, long startTime, long endTime) {
    int reg = regionMap[region];
    if(!functionCache.contains(reg)) {
        functionCache.insert(reg, ResolutionCache());
    }
    ResolutionCache &cache = functionCache[reg];
    QString key = this->getKey(ftype,dataName,spaceIndex,timeIndex,startTime,endTime);
    if(!cache.contains(key)) {
        // first check if it is stored in a file

        // create scalar function

        // add it to the cache.
    }
    return cache[key];
}

QString FunctionCache::getKey(FunctionType ftype, QString dataName, int spaceIndex, int timeIndex, long startTime, long endTime) {
    return ftype + "." + dataName + "." + spaceIndex + "." + timeIndex + "." + startTime + "." + endTime;
}

QString FunctionCache::getFunctionFileName(int reg, QString key) {
    return this->cacheFolder + "cache." + reg + "." + key;
}
