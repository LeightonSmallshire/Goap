#pragma once
#include "limits"
#include "list"
#include "map"
#include <cassert>

// Hardcoded types
typedef unsigned short enumType;
typedef unsigned short numType;
// Generated values
constexpr const static numType numTypeMin = std::numeric_limits<numType>::min();
constexpr const static numType numTypeMax = std::numeric_limits<numType>::max();

class goapReality;
class goapAction;

struct stateDemand {
  enum flagType : char { isNum, notNum, inBounds, outBounds, callFunc };

  stateDemand(const flagType flag, std::array<numType, 2> bounds);
  stateDemand(const flagType flag, numType numExact);

  const flagType flag;
  union { // Keep only one of;
    std::array<numType, 2> bounds = {
        numTypeMin,  // Defaults are effectivly +/- inf.
        numTypeMax}; // Lower <= VAL <= Upper.
    numType numExact;
    bool (*satisfied)(const goapReality &,
                      const enumType &); // Pointer to function.
  };
};

struct stateEffect {
  enum flagType : char { setN, incN, decN, setAny, incAny, decAny };
  flagType flag;
  union {      // Keep only one of;
    numType N; // The number itself.
    numType (*applyFn)(const goapReality *const reality,
                       const enumType *const variable);
  };
};

struct planStep {
  planStep(const goapAction *const action, const goapReality &start,
           const numType cost)
      : action(action), result(start), cost(cost) {}
  const goapAction *const action; // Constant pointer to constant
  const goapReality &result;
  const numType cost;
};

typedef std::map<enumType, stateDemand> realityDemand;
typedef std::map<enumType, stateEffect> realityEffect;
typedef std::list<goapAction *> actionListType;
typedef std::list<planStep> planType;
typedef std::map<enumType, numType> realityVarMap;
