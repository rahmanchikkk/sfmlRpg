#pragma once
#include <queue>

using EventID = unsigned int;

class EventQueue {
public:
    void AddEvent(const EventID& l_event) {
        m_events.push(l_event);
    };

    bool ProcessEvents(EventID& l_event) {
        if (m_events.empty()) return false;
        l_event = m_events.front();
        m_events.pop();
        return true;
    };

    void Clear() { 
        while (!m_events.empty()) {
            m_events.pop();
        }
    };
private:
    std::queue<EventID> m_events;
};