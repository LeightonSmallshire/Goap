//#include "action.hpp"
#include "iostream"
#include "planner.hpp"
#include "reality.hpp"
//#include "types.hpp"

class goapTest {
public:
  static void test1() {
    enum realityVars : enumType {
      // T1
      stock_wood,
      stock_iron,
      stock_coal,
      // T2
      stock_tool,
    };

    goapAction act_chop_wood{
        realityDemand{
            {enumType{stock_tool},
             stateDemand{stateDemand::flagType::inBounds, {0, 5}}} // 0-5 tools
        }, // Preconditions
        realityEffect{
            {stock_wood, {stateEffect::flagType::incN, {2}}} // Add 2 wood
        }                                                    // Postconditions
    };

    goapAction act_mine_coal{realityDemand{}, realityEffect{}};
    //    goapAction act_mine_iron{realityDemand{}, realityEffect{}};
    //    goapAction act_make_tool{realityDemand{}, realityEffect{}};

    planner pl{actionListType{
        &act_chop_wood, // 0-5 tools; +5 wood
        &act_mine_coal, // nil
                        // &act_mine_iron,
                        // &act_make_tool
    }};

    realityVarMap real{
        {stock_wood, 0}, {stock_iron, 0}, {stock_coal, 0}, {stock_tool, 0}};
    goapReality start{real};

    auto goalChecker = [](const goapReality &reality) {
      return reality.getVariable(stock_wood) > 5;
    };

    planType plan = pl.create_plan(start, goalChecker);

    static std::map<const void *, std::string> nameMap{
        {&act_chop_wood, "act_chop_wood"}, //
        {&act_mine_coal, "act_mine_coal"}, //
        // {&act_mine_iron, "act_mine_iron"}  //
    };

    for (planStep step : plan) {
      std::cout << nameMap[step.action]
                << std::endl
                // << step.result << std::endl
                << step.cost << std::endl;
    }
  }
};

int main() {
  //  std::cout << std::string(__FILE__) + " " + std::string(__LINE__) <<
  //  std::endl;
  goapTest::test1();
  //  std::cout << "2" << std::endl;

  return 0;
}
