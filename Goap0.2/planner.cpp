#include "planner.hpp"
#include "iostream" // TEMP

struct planTreeNode : public planStep {
  planTreeNode(const goapAction *_action, const goapReality &_origin,
               const numType _cost, planTreeNode &_parent)
      : planStep(_action, _origin, _cost), totalCost(_cost + _parent.cost),
        parent(_parent) {
    parent.child_created();
  }

  planTreeNode(const goapReality &_origin) // Parent's self.
      : planStep(&dummyAction, _origin, 0), totalCost(0), parent(*this) {
    parent.child_created();
  }

  ~planTreeNode() {
    this->parent.child_destroyed();
    //    delete this;
  }

  numType totalCost;
  planTreeNode &parent;
  unsigned short children = 0; // Children counter.

  short child_destroyed() { // Dec counter and return number of nodes destroyed
    if (children == 0 or children == 1) {    // One or fewer children
      planTreeNode *_parent = &parent;       // Get parent's address
      delete this;                           // Destuct self,
      return 1 + _parent->child_destroyed(); // Number of nodes deleted
    } else
      children -= 1; // Otherwise decrese by 1 & return 0
    return 0;
  }
  void child_created() { children += 1; }

  struct _pred {
    bool operator()(const planTreeNode *lhs, const planTreeNode *rhs) const {
      return lhs->totalCost < rhs->totalCost;
    }
  };
};

typedef std::priority_queue<planTreeNode *, std::vector<planTreeNode *>,
                            planTreeNode::_pred>
    planTreeQueue;

const planType backtrack(const planTreeNode &leaf) {
  planType plan;
  const planTreeNode *node = &leaf;
  while (&node->parent != node) { // While parent is not itself
                                  // Cast back from parent class to child class.
    plan.push_back(static_cast<const planStep>(*node)); // Add to plan
    const planTreeNode *parent = &node->parent;
    delete node;
    node = parent;
  }

  return plan;
};

planType planner::create_plan(const goapReality &reality,
                              bool (*goalChecker)(const goapReality &),
                              const unsigned short max_cost) const {
  planTreeQueue queue; // Queue of nodes to search
  planTreeNode root{reality};
  queue.push(&root);

  static const auto cleanup = [&]() {
    while (!queue.empty()) {
      planTreeNode *current_plan = queue.top();
      queue.pop();
      delete current_plan;
    }
  };

  while (!queue.empty()) {
    planTreeNode *current_plan = queue.top();
    queue.pop(); // Get & pop best plan

    // If cheapest action exceeds max cost, throw
    if (current_plan->cost > max_cost)
      throw "Maximum cost exceeded";

    // Evaluate the action
    auto *varMap = new realityVarMap{}; // TODO- DELETE THIS PROPERLY
    auto *sub_reality = new goapReality{current_plan->result, *varMap};

    // Print reality addresses
    std::cout << "GoapReality @" << sub_reality << "\n";
    std::cout.flush();

    current_plan->action->apply_action(*sub_reality);

    if (goalChecker(*sub_reality)) {
      auto plan = backtrack(*current_plan);
      cleanup(); // Cleanup function
      return plan;
    }

    // if no subactions, prune thw branch
    bool has_children = false;
    for (const goapAction *action : actions) {
      // If action viable
      if (action->check_action(*sub_reality)) {
        has_children = true;

        numType h_cost = 1; // heuristic cost

        // Additional heuristics;
        //   Reduced cost for actions that affect the desired states

        auto *sub_plan =
            new planTreeNode{action, *sub_reality, h_cost, *current_plan};
        current_plan->child_created();

        // Print planNode addresses
        std::cout << "PlanTreeNode @" << sub_plan << "\n";
        std::cout.flush();

        queue.push(sub_plan);
      }
    }

    if (!has_children)
      delete current_plan;
  }

  throw "No plan found.";
  // NOTE: ROOT NODE WILL NEVER DESTROY ITSELF.
  // NOTE:
}

/*
planType planner::create_plan(const goapReality &reality,
                              bool (*goalChecker)(const goapReality &),
                              const unsigned short max_cost) const {
  planTreeQueue queue;
  planTreeNode root{reality};

  planTreeQueue queue2;
  queue.push(&root);

  std::list<planTreeNode> nodes;

  while (!queue.empty()) {
    const planTreeNode *current_plan = queue.top();
    queue.pop(); // Get & pop best plan

    // If cheapest action exceeds max cost, throw`
    if (current_plan->cost > max_cost)
      throw "Maximum cost exceeded";

    // Evaluate the action
    realityVarMap varMap{};
    goapReality sub_reality{current_plan->result, varMap};
    current_plan->action->apply_action(sub_reality);

    if (goalChecker(sub_reality))
      return backtrack(*current_plan);

    // -- NEED TO IMPLEMENT HEURISTIC

    for (const goapAction *action : actions) {
      // If action viable
      if (action->check_action(sub_reality)) {

        numType h_cost = 1; // heuristic cost
        planTreeNode sub_plan{action, sub_reality, h_cost, *current_plan};
        queue.push(&sub_plan);

        nodes.push_front(
            planTreeNode{action, sub_reality, h_cost, *current_plan});
        queue2.push(&nodes.front());

        void *_ = nullptr;
      }
    }
  }
  throw "No plan found.";
}*/
