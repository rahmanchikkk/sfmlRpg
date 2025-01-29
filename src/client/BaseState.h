#pragma once
#include <SFML/Graphics.hpp>

class StateManager;

class BaseState {
    friend class StateManager;
public:
    BaseState(StateManager* l_stateMgr) : m_stateManager(l_stateMgr), m_transcedent(false), m_transparent(false) {};
    virtual ~BaseState() {};

    virtual void Activate() = 0;
    virtual void Deactivate() = 0;
    
    virtual void OnCreate() = 0;
    virtual void OnDestroy() = 0;

    virtual void Update(const sf::Time& l_time) = 0;
    virtual void Draw() = 0;

    bool IsTranscedent() const { return m_transcedent; };
    void SetTranscedent(const bool& l_val) { m_transcedent = l_val; };
    bool IsTransparent() const { return m_transparent; };
    void SetTransparent(const bool& l_val) { m_transparent = l_val; };
    sf::View& GetView() { return m_view; };
    StateManager* GetStateManager() { return m_stateManager; };
protected:    
    StateManager* m_stateManager;
    bool m_transcedent;
    bool m_transparent;
    sf::View m_view;
};