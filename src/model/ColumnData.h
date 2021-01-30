//
// Created by Ilya Vologin
// https://github.com/cupertank
//


#pragma once

#include <vector>

#include "Column.h"
#include "PositionListIndex.h"

class ColumnData {
private:
    shared_ptr<Column> column;
    std::vector<int> probingTable;
    shared_ptr<PositionListIndex> positionListIndex;

public:
    ColumnData(shared_ptr<Column> const & column, std::vector<int> const & probingTable, shared_ptr<PositionListIndex>& positionListIndex);
    std::vector<int> getProbingTable();
    int getProbingTableValue(int tupleIndex);
    shared_ptr<Column> getColumn();
    shared_ptr<PositionListIndex> getPositionListIndex();
    void shuffle();
    string toString();
    bool operator==(const ColumnData& rhs);
};
