#ifndef GRIDINDEX_HPP
#define GRIDINDEX_HPP

#include <QRectF>
#include <QPolygonF>
#include <QPointF>

typedef QVector<int> Node;
class GridIndex
{
public:
    GridIndex(bool usingGeo = true);
    GridIndex(int xsize, bool usingGeo = true);
    GridIndex(int xsize, int ysize, bool usingGeo = true);

    void buildGrid(QVector<QPointF> polygons);
    void buildGrid(QVector<QPolygonF> polygons);
    void setPoints(QVector<QPointF> points);
    void setPolygons(QVector<QPolygonF> polygons);
    QVector<int> getRegionBF(double x, double y);
    QVector<int> getRegion(double x, double y);
    void test();

    void outputGrid(QString fileName);
    void readGrid(QString fileName);

    QVector<QPointF> getPoints();
    QVector<QPolygonF> getCellPolygons();
    QVector<QPolygonF> getPolygons();
    QRectF getBounds();
    int getPolygonCount();
    int getXs();
    int getYs();


private:
    int getXIndex(double x);
    int getYIndex(double x);
    double getX(double x);
    double getY(double x);

private:
    bool pointGrid;
    bool squareCell;
    QRectF bound;
    uint xs,ys;
    QVector<QVector<Node>> grid;
    double width, height;
    QVector<QPolygonF> polygons;
    QVector<QPointF> points;
    bool usingGeoCoordinates;
};

#endif // GRIDINDEX_HPP
