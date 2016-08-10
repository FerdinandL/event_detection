#include "GridIndex.hpp"
#include <limits>
#include <cassert>
#include "../Util/UsefulFuncs.hpp"
#include "../Util/Typefunctions.hpp"
//#include "../MapView/Triangulator.hpp"
#include "../MapView/Camera.hpp"

#include <QFile>
#include <QDebug>
#include <QDataStream>

GridIndex::GridIndex(bool usingGeo) {
    xs = 0;
    ys = 0;
    squareCell = false;
    usingGeoCoordinates = usingGeo;
    pointGrid = false;
}

GridIndex::GridIndex(int xsize, bool usingGeo) {
    xs = xsize;
    squareCell = true;
    usingGeoCoordinates = usingGeo;
    pointGrid = false;
}

GridIndex::GridIndex(int xsize, int ysize, bool usingGeo) {
    xs = xsize;
    ys = ysize;
    squareCell = false;
    usingGeoCoordinates = usingGeo;
    pointGrid = false;
    for(int i = 0;i < xs;i ++) {
        grid << QVector<Node>(ys);
    }
}

void GridIndex::buildGrid(QVector<QPointF> points) {
    this->pointGrid = true;
    this->points = points;

    double maxx = -std::numeric_limits<double>::max();
    double maxy = -std::numeric_limits<double>::max();
    double minx = std::numeric_limits<double>::max();
    double miny = std::numeric_limits<double>::max();

    int i = 0;
    foreach(QPointF p, points) {
        minx = std::min(points[i].x(), minx);
        miny = std::min(points[i].y(), miny);
        maxx = std::max(points[i].x(), maxx);
        maxy = std::max(points[i].y(), maxy);
        i ++;
    }

    if(squareCell) {
        width = (maxx - minx) / xs;
        height = width;
        ys = ceil((maxy - miny) / height);
        for(int i = 0;i < xs;i ++) {
            grid << QVector<Node>(ys);
        }
    }
    else {
        width = (maxx - minx) / xs;
        height = (maxy - miny) / ys;
    }


    this->bound = QRectF(minx,miny,maxx-minx,maxy-miny);

    i = 0;
    foreach(QPointF p, points) {
        int stx = getXIndex(p.x());
        int sty = getYIndex(p.y());

        if(stx >= xs) {
            stx = xs - 1;
        }
        if(sty >= ys) {
            sty = ys - 1;
        }
        if(stx < 0) {
            stx = 0;
        }
        if(sty < 0) {
            sty = 0;
        }

        grid[stx][sty] << i;
        i ++;
    }
}

void GridIndex::buildGrid(QVector<QPolygonF> polygons) {
    this->pointGrid = false;
    this->polygons = polygons;

    double maxx = -std::numeric_limits<double>::max();
    double maxy = -std::numeric_limits<double>::max();
    double minx = std::numeric_limits<double>::max();
    double miny = std::numeric_limits<double>::max();
    QVector<QRectF> bounds(polygons.size());
    int i = 0;
    foreach(QPolygonF p, polygons) {
        bounds[i] = p.boundingRect();
        minx = std::min(bounds[i].left(), minx);
        miny = std::min(bounds[i].top(), miny);
        maxx = std::max(bounds[i].right(), maxx);
        maxy = std::max(bounds[i].bottom(), maxy);
        i ++;
    }

    if(squareCell) {
        width = (maxx - minx) / xs;
        height = width;
        ys = ceil((maxy - miny) / height);
        for(int i = 0;i < xs;i ++) {
            grid << QVector<Node>(ys);
        }
    }
    else {
        width = (maxx - minx) / xs;
        height = (maxy - miny) / ys;
    }

    this->bound = QRectF(minx,miny,maxx-minx,maxy-miny);

    i = 0;
    foreach(QRectF b, bounds) {
        int stx = getXIndex(b.left());
        int enx = getXIndex(b.right()) + 1;
        int sty = getYIndex(b.top());
        int eny = getYIndex(b.bottom()) + 1;

        if(enx >= xs) {
            enx = xs - 1;
        }
        if(eny >= ys) {
            eny = ys - 1;
        }
        if(stx < 0) {
            stx = 0;
        }
        if(sty < 0) {
            sty = 0;
        }


        for(int x = stx;x <= enx;x ++) {
            for(int y = sty;y <= eny;y ++) {
                QPointF leftBottom(getX(x), getY(y));
                QPointF rightTop(getX(x+1), getY(y+1));

                if(polygonRectIntersection(polygons[i],leftBottom,rightTop)) {
                    grid[x][y] << i;
                }
            }
        }
        i ++;
    }
}

QVector<QPolygonF> GridIndex::getCellPolygons() {

    QVector<QPolygonF> polys;

    // create a polygon for each cell in the grid
    for(int i=0; i<this->xs; i++) {
        for(int j=0; j<this->ys; j++) {
            QPointF leftBottom(getX(i), getY(j));
            QPointF rightTop(getX(i+1), getY(j+1));

            QPolygonF poly;
            if(!this->usingGeoCoordinates) {
                poly.push_back(Camera::world2geo(QPointF(leftBottom.x(), leftBottom.y())));
                poly.push_back(Camera::world2geo(QPointF(rightTop.x(), leftBottom.y())));
                poly.push_back(Camera::world2geo(QPointF(rightTop.x(), rightTop.y())));
                poly.push_back(Camera::world2geo(QPointF(leftBottom.x(), rightTop.y())));
            }
            else {
                poly.push_back(QPointF(leftBottom.x(), leftBottom.y()));
                poly.push_back(QPointF(rightTop.x(), leftBottom.y()));
                poly.push_back(QPointF(rightTop.x(), rightTop.y()));
                poly.push_back(QPointF(leftBottom.x(), rightTop.y()));
            }

            polys.push_back(poly);
//            qDebug() << poly;

        }
    }

    return polys;
}

void GridIndex::setPoints(QVector<QPointF> points) {
    this->points = points;
}

QVector<QPointF> GridIndex::getPoints() {
    return points;
}

void GridIndex::setPolygons(QVector<QPolygonF> polygons) {
//    for(int i=0; i<polygons.size(); i++) {
//        QPolygonF poly;
//        for(int j=0; j<polygons[i].size(); j++) {
//            poly.push_back(Camera3D::geo2world(polygons[i][j]));
//        }
//        this->polygons.push_back(poly);
//    }
   this->polygons = polygons;

}

QVector<QPolygonF> GridIndex::getPolygons() {
    return polygons;
}

QRectF GridIndex::getBounds() {
    if(this->usingGeoCoordinates){
//        qDebug() << this->bound.bottomLeft(); // bottom right
//        qDebug() << this->bound.bottomRight(); // top right
//        qDebug() << this->bound.topLeft(); // bottom left
//        qDebug() << this->bound.topRight(); // top left

//        double x1,y1,x2,y2;
//        this->bound.getCoords(&x1,&y1,&x2,&y2); // top left, bottom right
//        QRectF bound(x1,y1,x1-x2,y1-y2);
//        qDebug() << x1 << x2 << y1 << y2;
//        qDebug() << "--";
//        qDebug() << bound.bottomLeft();
//        qDebug() << bound.bottomRight();
//        qDebug() << bound.topLeft();
//        qDebug() << bound.topRight();
//        this->bound = bound;
        return this->bound;
    }
    else {
        QRectF bound;
        double x1,y1,x2,y2;
        this->bound.getCoords(&x1,&y1,&x2,&y2);
        QPointF p1 = Camera3D::world2geo(QPointF(x1,y1));
        QPointF p2 = Camera3D::world2geo(QPointF(x2,y2));
        bound.setCoords(p2.x(),p1.y(),p1.x(),p2.y());
        return bound;
    }
}

int GridIndex::getPolygonCount() {
    return polygons.size();
}

int GridIndex::getXIndex(double x) {
    double in = (x - bound.left()) / width;
    return (int) in;
}

int GridIndex::getYIndex(double y) {
    double in = (y - bound.top()) / height;
    return (int) in;
}

double GridIndex::getX(double x) {
    double in = x * width + bound.left();
    return in;
}

double GridIndex::getY(double y) {
    double in = y * height + bound.top();
    return in;
}

int GridIndex::getXs() {
    return this->xs;
}

int GridIndex::getYs() {
    return this->ys;
}

QVector<int> GridIndex::getRegion(double x, double y) {

    QVector<int> ret;
    if(grid.size() == 0)
        return ret;

    QPointF geo = QPointF(x,y);
    if(!this->usingGeoCoordinates) {
        QPointF w = Camera3D::geo2world(QPointF(x,y));
        x = w.x();
        y = w.y();
    }

    int stx = getXIndex(x);
    int sty = getYIndex(y);

    if(stx >= xs) {
        stx = xs - 1;
    }
    if(sty >= ys) {
        sty = ys - 1;
    }

    if(stx < 0) {
        stx = 0;
    }
    if(sty < 0) {
        sty = 0;
    }

    foreach(int p, grid[stx][sty]) {
        QPolygonF poly = polygons[p];
//        qDebug() << qSetRealNumberPrecision(10) << poly << x << y;
        if(poly.containsPoint(QPointF(x,y),Qt::WindingFill)) {
            ret << p;
        }
    }
//    qDebug() << stx << sty << grid[stx][sty];
    return ret;
}

void GridIndex::test() {
    int maxb = 0;
    for(int x = 0;x < xs;x ++) {
        for(int y = 0;y < ys;y ++) {
            maxb = std::max(maxb, grid[x][y].size());
        }
    }
    qDebug() << maxb;

    int n = 100000;
    double x[n];
    double y[n];
    for(int i = 0;i < n;i ++) {
        x[i] = ((double)rand() / RAND_MAX) * bound.width() + bound.left();
        y[i] = ((double)rand() / RAND_MAX) * bound.height() + bound.top();
    }

    QVector<QVector<int> >rg;
    QVector<QVector<int> >rb;

    for(int i = 0;i < n;i ++) {
        rg << getRegion(x[i],y[i]);
    }

    for(int i = 0;i < n;i ++) {
        rb << getRegionBF(x[i],y[i]);
    }

    int ct = 0;
    for(int i = 0;i < n;i ++) {
        // have no idea what this comparison does
        if(rg[i] != rb[i]) {
            qDebug() << "brute force and grid index results don't match!!!!";
        }
        if(rg[i].size() > 0) {
            ct ++;
        }
    }
    qDebug() << "No. of points within polygon set: " << ct;
}

QVector<int> GridIndex::getRegionBF(double x, double y) {
    int ct = 0;
    QVector<int> ret;
    foreach(QPolygonF poly, polygons) {
        if(poly.containsPoint(QPointF(x, y), Qt::WindingFill)) {
            ret << ct;
        }
        ct ++;
    }
    return ret;
}

void GridIndex::outputGrid(QString fileName) {
    QString fileNameGrid = fileName + ".grid";
    QString fileNameGridIndex = fileName + ".grid.index";
    QFile fileGrid(fileNameGrid);
    QFile fileGridIndex(fileNameGridIndex);
    if(!fileGrid.open(QIODevice::WriteOnly) || !fileGridIndex.open(QIODevice::WriteOnly)) {
        qDebug() << "could not write file";
        assert(false);
    }
    QDataStream opGrid(&fileGrid);
    QDataStream opGridIndex(&fileGridIndex);

    opGrid << (uint)xs << (uint)ys;
    opGrid << (bool)usingGeoCoordinates;
    opGrid << (bool)pointGrid;
    opGrid << (quint64)double2uint(bound.left()) << (quint64)double2uint(bound.right()) << (quint64)double2uint(bound.top()) << (quint64)double2uint(bound.bottom());
    for(int y = 0;y < ys;y ++) {
        for(int x = 0;x < xs;x ++) {
            opGrid << (uint)grid[x][y].size();
            foreach(int p, grid[x][y]) {
                opGridIndex << (uint)p;
            }
        }
    }
    fileGrid.close();
    fileGridIndex.close();

//    qDebug() << qSetRealNumberPrecision(10) << "Bounds:" << bound.left() << bound.right() << bound.top() << bound.bottom();
}


void GridIndex::readGrid(QString fileName) {
    QString fileNameGrid = fileName + ".grid";
    QString fileNameGridIndex = fileName + ".grid.index";
    QFile fileGrid(fileNameGrid);
    QFile fileGridIndex(fileNameGridIndex);
    if(!fileGrid.open(QIODevice::ReadOnly) || !fileGridIndex.open(QIODevice::ReadOnly)) {
        qDebug() << "could not read index file";
        assert(false);
    }
    QDataStream opGrid(&fileGrid);
    QDataStream opGridIndex(&fileGridIndex);

    opGrid >> xs >> ys;
    opGrid >> usingGeoCoordinates;
    opGrid >> pointGrid;
    quint64 left, right, top, bottom;
    opGrid >> left >> right >> top >> bottom;
    bound.setLeft(uint2double(left));
    bound.setRight(uint2double(right));
    bound.setTop(uint2double(top));
    bound.setBottom(uint2double(bottom));

    for(uint x = 0;x < xs;x ++) {
        grid << QVector<Node>(ys);
    }
    for(uint y = 0;y < ys;y ++) {
        for(uint x = 0;x < xs;x ++) {
            uint size;
            opGrid >> size;
            for(uint i = 0;i < size;i ++) {
                uint p;
                opGridIndex >> p;
                grid[x][y] << int(p);
            }
        }
    }
    fileGrid.close();
    fileGridIndex.close();

    height = (bound.bottom() - bound.top()) / ys;
    width = (bound.right() - bound.left()) / xs;
//    qDebug() << qSetRealNumberPrecision(10) << "Bounds:" << bound.left() << bound.right() << bound.top() << bound.bottom();
}
