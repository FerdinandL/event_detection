#ifndef USEFULFUNCS_H
#define USEFULFUNCS_H

#include <QVector>
#include "../MapView/QVector3DD.hpp"
#include <QPolygonF>
#include <QLineF>
//#include "buildingconstants.h"
#include <stdint.h>

//#include "../ParametricBuildingOptimization/util/parametricbuilding.h"

#define SKIP_TEST_FLOOR_AREA false

#define INSIDE 0 // 0000
#define LEFT 1   // 0001
#define  RIGHT 2  // 0010
#define  BOTTOM 4 // 0100
#define  TOP 8   // 1000

//numeric
int      getSignal(double x);
double   lerp(double x, double y, double lambda);
double   sampleUniform(double minValue, double maxValue);
double   randDouble(double lowerLimit, double upperLimit);
double   clampDouble(double v, double minValue, double maxValue);

//geometry
int       getClosestDirection(const QVector<double> angles, QVector3DD baseVector, QVector3DD queryVector);
QPointF   getProportionPoint(const QPointF &point, float segment, float length, float dx, float dy);
bool      triangleContains(QVector3DD point, QVector3DD v0, QVector3DD v1, QVector3DD v2);//point is assumed to be in the plane (v0,v1,v2)
bool      triangleContains(QPointF point, QPointF v0, QPointF v1, QPointF v2);
QPolygonF rotationAroundCenter(QPolygonF,QPointF, double);
double    perimeter(QPolygonF);
QPolygonF createFilletCurve(const QPolygonF &orig, const float fillet);
double    computeArea(QPolygonF p);
double    signedTedVolume(QVector3DD v1,QVector3DD v2,QVector3DD v3);
double    computeVolume(QPolygonF bottomCurve, double bottomHeight, QPolygonF topCurve, double topHeight);
QPolygonF getPolygonAtHeight(double myHeight, QPolygonF bottomPolygon, double bottomHeight, QPolygonF topPolygon, double topHeight);
bool      rayToLineSegmentIntersection(QPointF rayO, QPointF rayDir,
                                       QPointF segmentP0, QPointF segmentP1,QPointF& result,
                                       double& interpFactor);
double    distancePointToLine(QLineF line, QPointF p, double &interp);
bool      lineRectIntersection(QPointF v1, QPointF v2, QPointF leftBottom, QPointF rightTop);
bool      triangleRectIntersection(QPolygonF triangle, QPointF leftBottom, QPointF rightTop);
bool      polygonRectIntersection(QPolygonF poly, QPointF leftBottom, QPointF rightTop);
QPolygonF simplifyPolygon(QPolygonF myPol);
int       ComputeOutCode(double x, double y, QPointF leftBottom, QPointF rightTop);

//
QPolygonF parsePolygonFile(QString filename);
QPolygonF parsePolygonFileWithResolution(QString filename, double &groundRes);

double getRandomNumber();

#endif // USEFULFUNCS_H
