#include "types.hpp"

stateDemand::stateDemand(const stateDemand::flagType flag,
                         std::array<numType, 2> bounds)
    : flag(flag), bounds(bounds) {
  assert(flag == flagType::inBounds or flag == flagType::outBounds);
}

stateDemand::stateDemand(const stateDemand::flagType flag, numType numExact)
    : flag(flag), numExact(numExact) {
  assert(flag == flagType::isNum or flag == flagType::notNum);
}

// stateDemand::satisfied
