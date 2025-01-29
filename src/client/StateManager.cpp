#include "StateManager.h"
#include "State_Game.h"
#include "State_Intro.h"
#include "State_Paused.h"
#include "State_MainMenu.h"
#include "State_GameOver.h"

StateManager::StateManager(SharedContext* l_context) : m_context(l_context) {
    RegisterState<State_Intro>(StateType::Intro);
    RegisterState<State_MainMenu>(StateType::MainMenu);
    RegisterState<State_Game>(StateType::Game);
    RegisterState<State_Paused>(StateType::Paused);
    RegisterState<State_GameOver>(StateType::GameOver);
}

StateManager::~StateManager() {
    for (auto& itr: m_states) {
        itr.second->OnDestroy();
        delete itr.second;
    }
}

void StateManager::Update(const sf::Time& l_time) {
    if (m_states.empty()) return;
    if (m_states.back().second->IsTranscedent() && m_states.size() > 1) {
        auto itr = m_states.end();
        while (itr != m_states.begin()) {
            if (itr != m_states.end()) {
                if (!itr->second->IsTranscedent()) {
                    break;
                }
            }
        }
        for (; itr != m_states.end(); ++itr) {
            itr->second->Update(l_time);
        }
    } else {
        m_states.back().second->Update(l_time);
    }
}

void StateManager::Draw() {
    if (m_states.empty()) return;
    if (m_states.back().second->IsTransparent() && m_states.size() > 1) {
        auto itr = m_states.end();
        while (itr != m_states.begin()) {
            if (itr != m_states.end()) {
                if (!itr->second->IsTransparent()) {
                    break;
                }
            }
        }
        for (; itr != m_states.end(); ++itr) {
            m_context->m_window->GetRenderWindow()->setView(itr->second->GetView());
            itr->second->Draw();
        }
    } else {
        m_states.back().second->Draw();
    }
}

SharedContext* StateManager::GetContext() { return m_context; }

bool StateManager::HasState(const StateType& l_state) {
    for (auto itr = m_states.begin(); itr != m_states.end(); ++itr) {
        if (itr->first == l_state) {
            auto removed = std::find(m_toRemove.begin(), m_toRemove.end(), l_state);
            if (removed == m_toRemove.end()) return true;
            return false;
        }
    }
}

void StateManager::ProcessRequests() {
    while (m_toRemove.begin() != m_toRemove.end()) {
        RemoveState(*m_toRemove.begin());
        m_toRemove.erase(m_toRemove.begin());
    }
}

void StateManager::SwitchTo(const StateType& l_state) {
    m_context->m_soundManager->ChangeState(l_state);
    m_context->m_eventManager->SetCurrentState(l_state);
    m_context->m_guiManager->SetCurrentState(l_state);
    for (auto itr = m_states.begin(); itr != m_states.end(); ++itr) {
        if (itr->first == l_state) {
            m_states.back().second->Deactivate();
            StateType tmpType = itr->first;
            BaseState* tmpState = itr->second;
            m_states.erase(itr);
            m_states.emplace_back(std::make_pair(tmpType, tmpState));
            m_states.back().second->Activate();
            m_context->m_window->GetRenderWindow()->setView(tmpState->GetView());
            return;
        }
    }
    if (!m_states.empty()) m_states.back().second->Deactivate();
    CreateState(l_state);
    m_states.back().second->Activate();
    m_context->m_window->GetRenderWindow()->setView(m_states.back().second->GetView());
}

void StateManager::Remove(const StateType& l_state) {
    m_toRemove.push_back(l_state);
}

void StateManager::CreateState(const StateType& l_state) {
    auto itr = m_factory.find(l_state);
    if (itr == m_factory.end()) return;
    BaseState* state = itr->second();
    state->m_view = m_context->m_window->GetRenderWindow()->getDefaultView();
    m_states.emplace_back(std::make_pair(l_state, state));
    state->OnCreate();
}

void StateManager::RemoveState(const StateType& l_state) {
    for (auto itr = m_states.begin(); itr != m_states.end(); ++itr) {
        if (itr->first == l_state) {
            itr->second->OnDestroy();
            delete itr->second;
            m_states.erase(itr);
            m_context->m_soundManager->RemoveState(l_state);
            return;
        }
    }
}