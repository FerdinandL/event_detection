#pragma once
#include <iostream>
#include <fstream>
#include <QDebug>
#include <QString>

#include "Trip.hpp"
#include "KdBlock.hpp"

#include "../../Util/Typefunctions.hpp"

#define hlog qDebug()

using namespace std;

enum QueryType {
    Equal, Lt, Gt
};

inline uint64_t getUKey(const Trip * trip, int keyIndex);
void buildKdTree(KdBlock::KdNode *nodes, uint64_t *tmp, Trip *trips, uint64_t n, int depth, uint64_t thisNode,
                 uint64_t &freeNode, FILE *fo, std::vector<uint64_t> &blockRange, int size, uint64_t * range, TripKey * keys, uint64_t& offset);
void createKdTree(std::string inputTrips, std::string keysFile, std::string nodeFile, std::string rangeFile, int size);
inline void swap(Trip * trips, int currentId, int wantedId, int size);


// Helper functions

void updateQuery(KdRequest *queryRequest, int index, int64_t lval, double dval, QueryType qtype, bool isdouble);
void addPoly(KdRequest *queryRequest, int index, const Neighborhoods::Geometry &poly);
