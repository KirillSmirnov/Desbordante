#pragma once

#include <functional>
#include <list>
#include "PartialFD.h"
#include "PartialKey.h"

class DependencyConsumer {
protected:
    std::function<void (PartialFD const&)> fdConsumer_;
    std::function<void (PartialKey const&)> uccConsumer_;
public:
    std::list<PartialFD> discoveredFDs_;
    std::list<PartialKey> discoveredUCCs_;

    [[nodiscard]] PartialFD registerFd(std::shared_ptr<Vertical> lhs, std::shared_ptr<Column> rhs, double error, double score) const;
    [[nodiscard]] PartialKey registerUcc(std::shared_ptr<Vertical> keyVertical, double error, double score) const;

    [[nodiscard]] std::string fdsToString() const;
    [[nodiscard]] std::string uccsToString() const;
};
