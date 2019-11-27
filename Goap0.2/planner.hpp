#pragma once
#include "action.hpp"
#include "queue"
#include "reality.hpp"
#include "types.hpp"

class planner {
public:
  //  planner(actionListType &actions) : actions(actions) {} // By reference
  planner(actionListType actions) : actions(actions) {} // By value

  planType create_plan(const goapReality &, const realityDemand &,
                       const unsigned short max_cost = 65535);
  planType create_plan(const goapReality &reality,
                       bool (*goalChecker)(const goapReality &),
                       const unsigned short max_cost = 65535) const;

  actionListType actions;
};
