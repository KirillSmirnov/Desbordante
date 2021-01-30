//
// Created by kek on 18.07.19.
//

#include "Column.h"

#include <utility>

#include "Vertical.h"

using std::string;

unsigned int Column::getIndex() const { return index; }

string Column::getName() const { return name; }

string Column::toString() const { return "[" + name + "]"; }

bool Column::operator==(const Column &rhs) {
    if (this == &rhs) return true;
    return index == rhs.index && schema.lock().get() == rhs.schema.lock().get();
}

std::shared_ptr<RelationalSchema> Column::getSchema() const {
    return schema.lock();
}

Column::operator Vertical() const {
    return Vertical(*this);
}
