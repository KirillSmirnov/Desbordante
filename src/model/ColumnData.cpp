//
// Created by kek on 12.07.19.
//

#include "ColumnData.h"

#include <algorithm>
#include <random>
#include <utility>

using std::vector, std::random_device, std::mt19937;

ColumnData::ColumnData(shared_ptr<Column> const & column, vector<int> const & probingTable, shared_ptr<PositionListIndex>& positionListIndex):
    column(column),
    probingTable(std::move(probingTable)),
    positionListIndex(positionListIndex)
    {}

vector<int> ColumnData::getProbingTable() { return probingTable; }

int ColumnData::getProbingTableValue(int tupleIndex) { return probingTable[tupleIndex]; }

shared_ptr<Column> ColumnData::getColumn() { return column; }

shared_ptr<PositionListIndex> ColumnData::getPositionListIndex() { return positionListIndex; }

//TODO: Random проверь
void ColumnData::shuffle() {
    random_device rd;
    mt19937 random(rd());
    std::shuffle(probingTable.begin(), probingTable.end(), random);
}

string ColumnData::toString() { return "Data for " + column->toString(); }

bool ColumnData::operator==(const ColumnData &rhs) {
    if (this == &rhs) return true;
    return this->column == rhs.column;
}
