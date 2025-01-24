#pragma once
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <functional>
#include <unordered_map>
#include <assert.h>
#include <string.h>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include "GUI_Event.h"
#include "Utilities.h"

enum class EventType {
    KeyDown = sf::Event::KeyPressed,
    KeyUp = sf::Event::KeyReleased,
    MButtonDown = sf::Event::MouseButtonPressed,
    MButtonUp = sf::Event::MouseButtonReleased,
    MouseWheel = sf::Event::MouseWheelMoved,
    WindowResized = sf::Event::Resized,
    GainedFocus = sf::Event::GainedFocus,
    LostFocus = sf::Event::LostFocus,
    MouseEntered = sf::Event::MouseEntered,
    MouseLeft = sf::Event::MouseLeft,
    Closed = sf::Event::Closed,
    TextEntered = sf::Event::TextEntered,
    Keyboard = sf::Event::Count + 1, Mouse, Joystick,
    GUI_Click, GUI_Release, GUI_Hover, GUI_Leave
};

struct EventInfo {
    EventInfo() { m_code = 0; };
    EventInfo(int l_event) { m_code = l_event; };
    EventInfo(const GUI_Event& l_event) { m_gui = l_event; };
    union {
        int m_code;
        GUI_Event m_gui;
    };
};

struct EventDetails {
    EventDetails(const std::string& l_bindName) : m_name(l_bindName) {
        Clear();
    };

    std::string m_name;
    sf::Vector2i m_size;
    sf::Vector2i m_mouse;
    int m_keyCode;
    int m_mouseWheelDelta;
    std::string m_guiInterface;
    std::string m_guiElement;
    GUI_EventType m_guiEvent;
    sf::Uint32 m_textEntered;

    void Clear() {
        m_size = sf::Vector2i(0, 0);
        m_mouse = sf::Vector2i(0, 0);
        m_textEntered = 0;
        m_keyCode = -1;
        m_mouseWheelDelta = 0;
        m_guiInterface = "";
        m_guiElement = "";
        m_guiEvent = GUI_EventType::None;
    };
};

using Events = std::vector<std::pair<EventType, EventInfo>>;

struct Binding {
    Binding(const std::string& l_name) : m_name(l_name), m_c(0) {};

    ~Binding() {
        for (auto itr = m_events.begin(); itr != m_events.end(); ++itr) {
            if (itr->first == EventType::GUI_Click || itr->first == EventType::GUI_Release ||
                itr->first == EventType::GUI_Hover || itr->first == EventType::GUI_Leave) {
                    delete [] itr->second.m_gui.m_interface;
                    delete [] itr->second.m_gui.m_element;
                }
        }
    };

    void AddEvent(const EventType& l_type, EventInfo l_info = EventInfo()) {
        m_events.emplace_back(std::make_pair(l_type, l_info));
    };

    std::string m_name;
    Events m_events;
    EventDetails* m_details;
    int m_c;
};

using Bindings = std::unordered_map<std::string, Binding*>;
using CallbackContainer = std::unordered_map<std::string, std::function<void(EventDetails*)>>;
enum class StateType;
using Callbacks = std::unordered_map<StateType, CallbackContainer>;

class EventManager {
public:
    EventManager();
    ~EventManager();

    bool AddBinding(Binding* l_binding);
    bool RemoveBinding(const std::string& l_bindName);

    void SetCurrentState(const StateType& l_state);
    void SetFocus(bool l_focus);

    template <class T>
    bool AddCallback(const StateType& l_state, const std::string& l_name, void(T::*l_callback)(EventDetails*), T* l_instance) {
        auto container = m_callbacks.emplace(l_state, CallbackContainer()).first;
        auto callback = std::bind(l_callback, l_instance, std::placeholders::_1);
        return container.emplace(l_name, callback).second;
    };

    bool RemoveCallback(const StateType& l_state, const std::string& l_name) {
        auto container = m_callbacks.find(l_state);
        if (container == m_callbacks.end()) return false;
        auto itr = container->second.find(l_name);
        if (itr == container->second.end()) return false;
        container->second.erase(itr);
        return true;
    };

    void HandleEvent(sf::Event& l_event);
    void HandleEvent(GUI_Event& l_event);
    void Update();

    sf::Vector2i GetMousePos(sf::RenderWindow* l_wind = nullptr) {
        return (l_wind ? sf::Mouse::getPosition(*l_wind) : sf::Mouse::getPosition());
    };
private:
    void LoadBindings();
    Bindings m_bindings;
    Callbacks m_callbacks;
    StateType m_currentState;
    bool m_hasFocus;
};