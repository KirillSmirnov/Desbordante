
#include "DependencyConsumer.h"

PartialFD DependencyConsumer::registerFd(std::shared_ptr<Vertical> lhs, std::shared_ptr<Column> rhs, double error, double score) const {
    PartialFD partialFd(lhs, rhs, error, score);
    fdConsumer_(partialFd);
    return partialFd;
}

PartialKey DependencyConsumer::registerUcc(std::shared_ptr<Vertical> keyVertical, double error, double score) const {
    PartialKey partialKey(keyVertical, error, score);
    uccConsumer_(partialKey);
    return partialKey;
}


