#pragma once
#include "Observer.h"
#include <vector>
#include <algorithm>

using Observers = std::vector<Observer*>;

class Communicator {
public:
    ~Communicator() { m_observers.clear(); };

    bool AddObserver(Observer* l_observer) {
        if (HasObserver(l_observer)) return false;
        m_observers.emplace_back(l_observer);
        return true;
    };

    bool RemoveObserver(Observer* l_observer) {
        auto itr = std::find_if(m_observers.begin(), m_observers.end(), [&l_observer](Observer* o) {
            return o == l_observer;
        });
        if (itr == m_observers.end()) return false;
        m_observers.erase(itr);
        return true;
    };

    bool HasObserver(Observer* l_observer) {
        auto itr = std::find_if(m_observers.begin(), m_observers.end(), [&l_observer](Observer* o) {
            return o == l_observer;
        });
        return (itr != m_observers.end());
    };

    void Broadcast(const Message& l_msg) {
        for (auto& itr : m_observers) {
            itr->Notify(l_msg);
        }
    };
private:
    Observers m_observers;
};