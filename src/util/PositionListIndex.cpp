#include "PositionListIndex.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <deque>
#include <map>
#include <utility>

#include <boost/dynamic_bitset.hpp>

#include "ColumnLayoutRelationData.h"
#include "Vertical.h"

using namespace std;

const int PositionListIndex::singletonValueId = 0;

int PositionListIndex::millis = 0;
int PositionListIndex::intersectionCount = 0;

// use r-value references, DO NOT copy
PositionListIndex::PositionListIndex(deque<vector<int>> index, vector<int> nullCluster, unsigned int size, double entropy,
                                     unsigned long long nep, unsigned int relationSize, unsigned int originalRelationSize,
                                     double invertedEntropy, double giniImpurity):
                                     index(std::move(index)),
                                     nullCluster(std::move(nullCluster)),
                                     size(size),
                                     entropy(entropy),
                                     invertedEntropy(invertedEntropy),
                                     giniImpurity(giniImpurity),
                                     nep(nep),
                                     relationSize(relationSize),
                                     originalRelationSize(originalRelationSize),
                                     probingTableCache(){}

shared_ptr<PositionListIndex> PositionListIndex::createFor(vector<int>& data, bool isNullEqNull) {
    map<int, vector<int>> index;
    for (unsigned long position = 0; position < data.size(); ++position){
        int valueId = data[position];
        index[valueId].push_back(position);
    }

    vector<int> nullCluster;
    if (index.count(RelationData::nullValueId) != 0) {
        nullCluster = index[RelationData::nullValueId];
    }
    if (!isNullEqNull){
        index.erase(RelationData::nullValueId); // move?
    }

    double keyGap = 0.0;
    double invEnt = 0;
    double giniGap = 0;
    unsigned long long nep = 0;
    unsigned int size = 0;
    deque<vector<int>> clusters;

    for (auto & iter : index){
        if (iter.second.size() == 1){
            giniGap += pow(1 / static_cast<double>(data.size()), 2);
            continue;
        }
        keyGap += iter.second.size() * log(iter.second.size());
        nep += calculateNep(iter.second.size());
        size += iter.second.size();
        invEnt += -(1 - iter.second.size() / static_cast<double>(data.size()))
                * log(1 - (iter.second.size() / static_cast<double>(data.size())));
        giniGap += pow(iter.second.size() / static_cast<double>(data.size()), 2);

        clusters.emplace_back(std::move(iter.second));
    }
    double entropy = log(data.size()) - keyGap / data.size();

    double giniImpurity = 1 - giniGap;
    if (giniImpurity == 0) {
        invEnt = 0;
    }

    sortClusters(clusters);
    auto pli = shared_ptr<PositionListIndex>(new PositionListIndex(clusters, nullCluster, size, entropy, nep, data.size(), data.size(), invEnt, giniImpurity));
    return pli;
}

unsigned long long PositionListIndex::calculateNep(unsigned int numElements) {
    return static_cast<unsigned long long>(numElements) * (numElements - 1) / 2;
}

void PositionListIndex::sortClusters(deque<vector<int>> &clusters) {
    sort(clusters.begin(), clusters.end(), [](vector<int> & a, vector<int> & b){
        return a[0] < b[0]; } );
}

vector<int> PositionListIndex::getProbingTable() {
    return getProbingTable(false);
}

vector<int> PositionListIndex::getProbingTable(bool isCaching) {

    if (!probingTableCache.empty()) return probingTableCache;
    vector<int> probingTable = vector<int>(originalRelationSize);
    int nextClusterId = singletonValueId + 1;
    for (auto & cluster : index){
        int valueId = nextClusterId++;
        assert(valueId != singletonValueId);
        for(int & position : cluster){
            probingTable[position] = valueId;
        }
    }
    if (isCaching) {
        probingTableCache = std::move(probingTable);
        return probingTableCache;
    }
    return probingTable;
}

deque<vector<int>> const & PositionListIndex::getIndex() {
    return index;
}

shared_ptr<PositionListIndex> PositionListIndex::intersect(shared_ptr<PositionListIndex> that) {
    assert(this->relationSize == that->relationSize);
    auto result = this->size > that->size ?
            that->probe(this->getProbingTable()) :
            this->probe(that->getProbingTable());
    return result;
}

//TODO: nullCluster некорректен
shared_ptr<PositionListIndex> PositionListIndex::probe(const vector<int>& probingTable) {
    assert(this->relationSize == probingTable.size());
    deque<vector<int>> newIndex;
    unsigned int newSize = 0;
    double newKeyGap = 0.0;
    unsigned long long newNep = 0;
    vector<int> nullCluster;

    map<int, vector<int>> partialIndex;
    //vector<int> newCluster;

    for (auto & positions : index){
        for (int & position : positions){
            int probingTableValueId = probingTable[position];
            if (probingTableValueId == singletonValueId)
                continue;
    intersectionCount++;
    //auto startTime = std::chrono::system_clock::now();
                 partialIndex[probingTableValueId].push_back(position);      //~500ms
           // millis += std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now() - startTime).count();

        }

        for (auto & iter : partialIndex){
            auto & cluster = iter.second;
            if (cluster.size() <= 1) continue;

            newSize += cluster.size();
            newKeyGap += cluster.size() * log(cluster.size());
            newNep += calculateNep(cluster.size());

            newIndex.emplace_back(std::move(cluster));              //~25ms
        }

        partialIndex.clear();           //~36
    }

    double newEntropy = log(relationSize) - newKeyGap / relationSize;

    sortClusters(newIndex);         //!! ~100-200ms

    return make_shared<PositionListIndex>(newIndex, nullCluster, newSize, newEntropy, newNep, relationSize, relationSize);
}


//TODO: nullCluster не поддерживается
shared_ptr<PositionListIndex> PositionListIndex::probeAll(Vertical probingColumns, ColumnLayoutRelationData & relationData) {
    assert(this->relationSize == relationData.getNumRows());

    deque<vector<int>> newIndex;
    unsigned int newSize = 0;
    double newKeyGap = 0.0;
    unsigned long long newNep = 0;

    map<vector<int>, vector<int>> partialIndex;
    vector<int> nullCluster;
    vector<int> probe;

    for (auto & cluster : this->index){
        for (int & position : cluster){
            if (!takeProbe(position, relationData, probingColumns, probe)){
                probe.clear();
                continue;
            }

            partialIndex[probe].push_back(position);
            probe.clear();
        }

        for (auto & iter : partialIndex){
            auto & newCluster = iter.second;
            if (newCluster.size() == 1) continue;

            newSize += newCluster.size();
            newKeyGap += newCluster.size() * log(newCluster.size());
            newNep += calculateNep(newCluster.size());

            newIndex.emplace_back(std::move(newCluster));
        }
        partialIndex.clear();
    }

    double newEntropy = log(this->relationSize) - newKeyGap / this->relationSize;

    sortClusters(newIndex);

    return std::make_shared<PositionListIndex>(
            newIndex, nullCluster, newSize, newEntropy, newNep, this->relationSize, this->relationSize
            );
}

bool PositionListIndex::takeProbe(int position, ColumnLayoutRelationData & relationData, Vertical & probingColumns, vector<int> & probe){
    dynamic_bitset<> probingIndices = probingColumns.getColumnIndices();
    for (unsigned long index = probingIndices.find_first(); index < probingIndices.size(); index = probingIndices.find_next(index)){
        int value = relationData.getColumnData(index)->getProbingTableValue(position);
        if (value == PositionListIndex::singletonValueId) return false;
        probe.push_back(value);
    }
    return true;
}

