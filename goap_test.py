from heapq import *
from types import FunctionType

inf = float('inf')


def _defaults_wrapper(f):  # Kind of a static method...
    def wrapper(self, *args, **kwargs):
        keys = kwargs.keys()

        for k in {'actions', } - keys:
            if k not in keys:
                kwargs[k] = self.__getattribute__(k)

        # noinspection PyCallingNonCallable
        return f(self, *args, **kwargs)

    return wrapper


class Planner:
    def __init__(self, actions=None):
        self.actions = actions

    @_defaults_wrapper
    def make_plan(self, world_state: set, agent_state: set, goal: str, *, actions=None):
        current_state = world_state.union(agent_state)

        return self.__astar(state=current_state, start_actions=actions, goal=goal, max_weight=100)

    @staticmethod
    def __astar(state: set, start_actions: list, goal: str, *, max_weight=inf):
        # g_cost, state, seq, c_action
        # Nodes to explore initially
        open_heap = [(a.cost, state, [a], a) for a in start_actions if a.preconditions.issubset(state)]
        heapify(open_heap)

        while open_heap:  # while nodes to explore
            g_cost, state, seq, c_action = heappop(open_heap)  # Current tile 'selected'

            valid_actions = start_actions + (c_action.act(state) or [])
            state.update(c_action.effects)

            if goal in state:
                return seq  # TODO: implement action sequences

            valid_actions = [a for a in valid_actions if a.preconditions.issubset(state)]

            for action in valid_actions:
                _f_score = g_cost + action.cost

                if _f_score < max_weight:
                    _seq = seq.copy()
                    _seq.append(action)
                    _state = state.copy()

                    heappush(open_heap, (_f_score, _state, _seq, action))  # Push the tile onto the stack

        return False  # Heap empty AKA path not found.


class Action:
    def __init__(self,
                 name: str,  # arbitrary
                 cost: int,  # Cost to do this compared to other actions
                 preconditions: set,  # Flags that must be set prior
                 effects: set,  # Flags that will be set after
                 act_function: FunctionType):  # Function to run so set other flags, etc.
        self.name = name
        self.cost = cost
        self.preconditions = preconditions
        self.effects = effects
        self.act = act_function

    def __str__(self):
        return f'Action<{self.name}>'

    def __repr__(self):
        return f'Action<{self.name}>'


def actionify(*, cost: int, preconditions: set = None, effects: set = None):
    def wrapper(fn: FunctionType):
        name = fn.__name__
        return Action(name, cost, preconditions or set(), effects or set(), fn)

    return wrapper


# Actions
# @actionify(cost=0)
# def idle():
#     pass


# @actionify(cost=0)
# def move():
#     pass


# @actionify(cost=0)
# def grab_firewood():
#     pass


# @actionify(cost=0)
# def drop_firewood():
#     pass


# @actionify(cost=0)
# def grab_log():
#     pass


@actionify(cost=4, preconditions={'HasAxe'}, effects={'MakeFirewood'})
def chop_log(state: set):
    pass  # No special actions


@actionify(cost=2, preconditions={'AxeAvailable'}, effects={'HasAxe'})
def get_axe(state: set):
    state.discard('AxeAvailable')


@actionify(cost=8, effects={'MakeFirewood'})
def collect_branches(state: set):
    pass  # No special actions


"""
    Terms
Agent/Actor - The thing following the plan
Planner     - Only one needed per set of possible actions

Action      - An action has;
                Name            arbitrary string
                cost            bias against this choice relative to others
                preconditions   requirements for the action to run
                effects         sets flags for preconditions
                act             function that is called while planning to do more complicated state changes if needed.


world_state - Current state of the world at plan start.
        Implemented as flags here but see Cpp implementation for a much better design.
        
agent_state - Current state of the agent at plan start.
        Used to track flags per-agent but again, not used in this version.
        Easy enough to modify so that these are kept separate and is probably needed for next suggestion;

The planner can be refactored to create multiple plans for multiple agents simultaneously, taking into account 
    one-another's plans and resulting in teamwork. This is fairly simple if agent states are separated but I expect 
    a lot of tweaking and debugging to be needed for this one.



"""

if __name__ == '__main__':
    actions = [chop_log, get_axe, collect_branches]
    planner = Planner(actions)

    world_state = {'AxeAvailable'}
    agent_state = set()  # Can have special uses but not implemented
    desired_state = {'MakeFirewood'}

    plan = planner.make_plan(world_state, agent_state, desired_state)

    print(plan)
