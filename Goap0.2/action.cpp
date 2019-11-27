#include "action.hpp"

void goapAction::apply_action(goapReality &reality) const {
  for (realityEffect::value_type pair : postconditions) {
    goapReality::varProxy proxy = reality[pair.first];

    switch (pair.second.flag) {
    case stateEffect::setN:
      proxy = pair.second.N;
      return;
    case stateEffect::incN:
      proxy += pair.second.N;
      return;
    case stateEffect::decN:
      return;
    case stateEffect::setAny:
    case stateEffect::incAny:
    case stateEffect::decAny:
      return;
    }
  }
}

bool check_state_demand(const enumType variable, const stateDemand &demand,
                        const goapReality &reality) {

  const auto value = reality[variable];

  // -- Figure out if lt/gt needs to be strict or not
  switch (demand.flag) {
  case stateDemand::flagType::isNum:
    return value == demand.numExact;
  case stateDemand::flagType::notNum:
    return value != demand.numExact;
  case stateDemand::flagType::inBounds:
    return demand.bounds[0] <= value and value <= demand.bounds[1];
  case stateDemand::flagType::outBounds:
    return !(demand.bounds[0] <= value and value <= demand.bounds[1]);
  case stateDemand::flagType::callFunc:;
    return demand.satisfied(reality, variable);
  }
  throw "Invalid stateDemand flag";
}

bool check_reality_demand(const realityDemand &demand,
                          const goapReality &reality) {
  for (realityDemand::value_type pair : demand)
    if (!check_state_demand(pair.first, pair.second, reality))
      return false; // False if any is false
  return true;      // True if all are true
}

bool goapAction::check_action(const goapReality &reality) const {
  return check_reality_demand(this->preconditions, reality);
}
