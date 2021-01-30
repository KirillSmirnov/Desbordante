//
// Created by Ilya Vologin
// https://github.com/cupertank
//


#pragma once

#include <string>

#include <boost/dynamic_bitset.hpp>
#include <utility>

#include "RelationalSchema.h"

using std::string, boost::dynamic_bitset;

class Column {
friend RelationalSchema;

private:
    string name;
    unsigned int index;
    std::weak_ptr<RelationalSchema> schema;

public:
    Column(std::shared_ptr<RelationalSchema> schema, string name, int index) :
            name(std::move(name)),
            index(index),
            schema(schema) {}
    explicit operator Vertical() const;
    [[nodiscard]] unsigned int getIndex() const;
    [[nodiscard]] string getName() const;
    [[nodiscard]] std::shared_ptr<RelationalSchema> getSchema() const;        // TODO: straight up bad: const-ness may be violated!
    [[nodiscard]] string toString() const;
    explicit operator std::string() const { return toString(); }
    bool operator==(const Column& rhs);
};
