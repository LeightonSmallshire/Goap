#pragma once
#include "algorithm"
#include "list"
#include "map"
#include "reality.hpp"
#include "types.hpp"

class goapAction {
public:
  goapAction(const realityDemand &preconditions,
             const realityEffect &postconditions)
      : preconditions(preconditions), postconditions(postconditions) {}
  goapAction() : preconditions({}), postconditions({}) {}
  virtual ~goapAction() {} // Destructor

  virtual void apply_action(goapReality &reality) const;
  virtual bool check_action(const goapReality &reality) const;

protected:
  const realityDemand preconditions;  // Assert beforehand.
  const realityEffect postconditions; // Assert afterwards.
};

class goapAction2 {
public:
  void (*const apply_action)(goapReality &reality);       // Perform the action
  bool (*const check_action)(const goapReality &reality); // Can perform action?
  const realityDemand &preconditions;  // Must all be true beforehand.
  const realityEffect &postconditions; // Made true afterwards.
};

static const goapAction dummyAction{};
