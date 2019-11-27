#ifndef GOAP_HPP
#define GOAP_HPP
#include "algorithm"
//#include "limits"
#include "list"
#include "map"
#include "queue"
//#include "stdexcept"
//#include "string"

// template <typename numType, typename realityVariable>
class GoapSystem {
  typedef enum { a, b, c, d } realityVariable;

  static_assert(std::numeric_limits<unsigned int>::is_exact, "numType err");
  static_assert(std::is_enum<realityVariable>(),
                "'realityVariables' must be enum.");

  // ---------------------------- Constants  ----------------------------------
  class realityType;
  class actionBase;

  constexpr const static unsigned int numTypeMin =
      std::numeric_limits<unsigned int>::min();
  constexpr const static unsigned int numTypeMax =
      std::numeric_limits<unsigned int>::max();

  struct stateDemand {
    enum flag : char { isTrue, isFalse, inBounds, outBounds, callFunc };
    union { // Keep only one of;
      std::array<unsigned int, 2> bounds = {
          numTypeMin,  // Defaults are effectivly +/- inf.
          numTypeMax}; // Lower <= VAL <= Upper.
      bool (*satisfied)(const realityType &); // Pointer to function.
    };
  };

  struct stateEffect {
    enum flag : char { setN, incN, decN, setAny, incAny, decAny };
    union {                                       // Keep only one of;
      unsigned int N;                             // The number itself.
      unsigned int (*apply)(const realityType &); // Pointer to function.
    };
  };

  struct planStep {
    const actionBase &action;
    const realityType &result;
    const unsigned int cost;
  };

  typedef std::map<realityVariable, stateDemand> realityDemand;
  typedef std::map<realityVariable, stateEffect> realityEffect;
  typedef std::list<actionBase &> actionListType;
  typedef std::list<planStep> planType;
  typedef std::map<realityVariable, unsigned int> realityVarMap;

  // ------------------------------- Reality ----------------------------------
  /*
Base reality must have a value set for every variable.
A proxy reality has a parent reality and a mapping of overloaded variables.


A proxy class is needed, to ensure that irrespective of from where data is read,
it is always stored in the toplevel realityType object.


*/

  class realityType {
  public:
    class varProxy {
    public:
      varProxy(realityType &reality, realityVariable &name, unsigned int &val)
          : reality(reality), name(name), value(val) {}
      const realityType &reality;
      const realityVariable &name;
      unsigned int value;

      unsigned int &operator=(unsigned int &num) {
        reality._variables[name] = num; // Set for reality
        value = num;                    // Set for this proxy
        return num;
        // ISSUE: this will not update other proxies of the same variable
      }
    };
    realityType(realityVarMap &_variables); // Root reality
    realityType(const realityType &parent); // Proxy
    realityType(const realityType &parent,  // Proxy + initial values
                realityVarMap &_variables);
    // Read-only references to private variables;
    const realityType &parent() { return _parent; }
    const realityVarMap &variables() { return _variables; }

    void setVariable(realityVariable &name, unsigned int &value) {
      _variables[name] = value; // Set in this reality.
    }
    void updateVariables(realityVarMap &variables) {
      for (const auto &v : variables)
        _variables[v.first] = v.second;
    }

    const unsigned int &getVariable(realityVariable &name) {
      return doLookup(name);
    }
    const unsigned int &getVariable(realityVariable &) const;

    varProxy operator[](realityVariable &name) {
      return varProxy(*this, name, doLookup(name));
    }

  protected:
    realityType &_parent = *this; // Ref to parent or itself.
    realityVarMap &_variables;    // Ref to reality variable mapping

    unsigned int &doLookup(realityVariable &name) const;
    unsigned int &doLookup(realityVariable &name) {
      auto iter = _variables.find(name); // Lookup
      if (iter != _variables.end())      // >Found it
        return iter->second;             // >>Return value
      if (&_parent != this)              // Has a parent
        return _parent.doLookup(name);   // >return (Ask parent)
      throw std::out_of_range("Key does not exist");
    }
  };

  // ------------------------------ Action ------------------------------------
  class actionBase {
  public:
    actionBase();
    actionBase(std::list<stateDemand> &preconditions,
               std::list<stateEffect> &postconditions);
    virtual ~actionBase() = default; // Deconstructor

    virtual bool apply_action(realityType &reality) {
      // Apply the effect to the reality.
      for (stateEffect effect : postconditions)
        effect.apply(reality);
      return true;
    }
    virtual bool check_action(const realityType &reality) {
      return std::all_of(
          preconditions.begin(), preconditions.end(),
          [&](stateDemand &pre) { return pre.satisfied(reality); });

    } // Check if the action can be performed.

    virtual bool apply_action(realityType &reality) const;
    virtual bool check_action(const realityType &reality) const;

  protected:
    std::list<stateDemand> &preconditions;  // Must all be true beforehand.
    std::list<stateEffect> &postconditions; // Made true afterwards.
  };

  // ----------------------------- Planner  -----------------------------------
  class planner {
  public:
    planner(actionListType actions) : actions(actions) {} // Constructor

    actionListType actions;

    //    planType &create_plan(const realityType rootReality, realityDemand
    //    goals) {
    //      return planner::create_plan(
    //          rootReality, [goals](realityType checkReality) {
    //            return std::all_of(
    //                goals.begin(), goals.end(),
    //                [checkReality](std::pair<realityVariable, stateDemand>
    //                pair) {
    //                  return pair.second.satisfied(checkReality);
    //                });
    //          });
    //    }

    planType create_plan(const realityType &reality,
                         std::list<actionBase> &actions,
                         bool (*goalChecker)(const realityType &)) {

      // THIS IS THE DUMB WAY: CHECK IF ALL GOALS MATCH AFTER NEXT CHEAPEST PLAN

      /* Methodology;
          Build a tree of possable realities originating at the one given, where
        each branch is an action taken by the given agent and the child is the
        resulting reality. Keep the set of realities yet-unchecked in a
        priorityList, ordered by the total action cost, lowest first. The next
        action considered is based in the first item in the priorityList.

        ISSUE: This assumes the given agent is the only active partisipant
        ISSUE: This assumes there is only one agent/ each agent plans
        independantly.
    */
      struct planTreeNode {
        planStep &step;
        planTreeNode &parent = *this; // parent or self

        unsigned int totalCost() {
          return &parent == this ? step.cost : step.cost + parent.totalCost();
        }
        unsigned int totalCost() const;
      };

      struct ComparePlans {
        bool operator()(const planTreeNode &lhs, const planTreeNode &rhs) {
          return lhs.totalCost() < rhs.totalCost();
        }
      };

      static const actionBase dummyAction = {};
      static realityVarMap dummyVarMap = {{realityVariable::a, 4}};
      static const realityType dummyResult = {dummyVarMap};
      static planStep dummyStep = {dummyAction, dummyResult, 0};
      static const planTreeNode dummyPlan = {dummyStep};

      std::priority_queue<const planTreeNode, std::vector<const planTreeNode>,
                          ComparePlans>
          queue = {dummyPlan};
      while (!queue.empty()) {
        const planTreeNode current_plan = queue.top();
        queue.pop();

        if (goalChecker(current_plan.step.result)) {

          planType plan;
          const planTreeNode *node = &current_plan;
          while (&node->parent != node) {        // While parent is not itself
            plan.insert(plan.end(), node->step); // Add step to plan
            node = &node->parent;
          }
          delete node;
          return plan;
        }

        for (const actionBase &action : actions) {
          if (action.check_action(current_plan.step.result)) {
            realityType sub_reality = {reality};
            action.apply_action(sub_reality);
            queue.push({{
                            {action, sub_reality},
                        },
                        current_plan.totalCost(),
                        current_plan});
          }
        }
      }
      throw "No feasable plan found";
    }

  protected:
  };
}; // namespace GOAP

#endif // GOAP_HPP
