#pragma once
#include "BaseState.h"
#include "SharedContext.h"
#include <vector>
#include <unordered_map>

enum class StateType { Intro = 1, MainMenu, Game, Paused, GameOver, Credits };

using StateContainer = std::vector<std::pair<StateType, BaseState*>>;
using TypeContainer = std::vector<StateType>;
using StateFactory = std::unordered_map<StateType, std::function<BaseState*(void)>>;

class StateManager {
public:
    StateManager(SharedContext* l_context);
    ~StateManager();

    void Update(const sf::Time& l_time);
    void Draw();

    bool HasState(const StateType& l_state);
    void SwitchTo(const StateType& l_state);
    void Remove(const StateType& l_state);

    SharedContext* GetContext();
    void ProcessRequests();
private:
    void CreateState(const StateType& l_state);
    void RemoveState(const StateType& l_state);

    template <class T>
    void RegisterState(const StateType& l_state) {
        m_factory[l_state] = [this]()->BaseState* {
            return new T(this);
        };
    };

    StateContainer m_states;
    TypeContainer m_toRemove;
    StateFactory m_factory;
    SharedContext* m_context;
};