#include "EventManager.h"
#include "StateManager.h"

EventManager::EventManager() : m_hasFocus(false), m_currentState(StateType(0)) {
    LoadBindings();
}

EventManager::~EventManager() {
    for (auto& itr : m_bindings) {
        delete itr.second;
    }
}

bool EventManager::AddBinding(Binding* l_binding) {
    auto itr = m_bindings.find(l_binding->m_name);
    if (itr != m_bindings.end()) return false;
    return m_bindings.emplace(l_binding->m_name, l_binding).second;
}

bool EventManager::RemoveBinding(const std::string& l_bindName) {
    auto itr = m_bindings.find(l_bindName);
    if (itr == m_bindings.end()) return false;
    delete itr->second;
    m_bindings.erase(itr);
    return true;
}

void EventManager::SetCurrentState(const StateType& l_state) { m_currentState = l_state; }

void EventManager::SetFocus(bool l_focus) { m_hasFocus = l_focus; }

void EventManager::HandleEvent(sf::Event& l_event) {
    for (auto& b_itr : m_bindings) {
        Binding* bind = b_itr.second;
        for (auto& e_itr : bind->m_events) {
            EventType sfmlEvent = (EventType)l_event.type;
            if (e_itr.first == EventType::GUI_Click || e_itr.first == EventType::GUI_Release ||
                e_itr.first == EventType::GUI_Hover || e_itr.first == EventType::GUI_Leave) {
                    continue;
                }
            if (sfmlEvent != e_itr.first) continue;
            if (sfmlEvent == EventType::KeyDown || sfmlEvent == EventType::KeyUp) {
                if (e_itr.second.m_code == l_event.key.code) {
                    if (bind->m_details->m_keyCode != -1) {
                        bind->m_details->m_keyCode = e_itr.second.m_code;
                    }
                    ++(bind->m_c);
                }
            } else if (sfmlEvent == EventType::MButtonDown || sfmlEvent == EventType::MButtonUp) {
                if (e_itr.second.m_code == l_event.mouseButton.button) {
                    if (bind->m_details->m_keyCode != -1) {
                        bind->m_details->m_keyCode = e_itr.second.m_code;
                    }
                    bind->m_details->m_mouse.x = l_event.mouseButton.x;
                    bind->m_details->m_mouse.y = l_event.mouseButton.y;
                    ++(bind->m_c);
                }
            } else {
                if (sfmlEvent == EventType::MouseWheel) {
                    bind->m_details->m_mouseWheelDelta = l_event.mouseWheel.delta;
                } else if (sfmlEvent == EventType::WindowResized) {
                    bind->m_details->m_size.x = l_event.size.width;
                    bind->m_details->m_size.y = l_event.size.height;
                } else if (sfmlEvent == EventType::TextEntered) {
                    bind->m_details->m_textEntered = l_event.text.unicode;
                }
                ++(bind->m_c);
            }
        }
    }
}

void EventManager::HandleEvent(GUI_Event& l_event) {
    for (auto& b_itr : m_bindings) {
        Binding* bind = b_itr.second;
        for (auto& e_itr : bind->m_events) {
            if (e_itr.first != EventType::GUI_Click && e_itr.first != EventType::GUI_Release &&
                e_itr.first != EventType::GUI_Hover && e_itr.first != EventType::GUI_Leave) {
                    continue;
                }
            if ((e_itr.first == EventType::GUI_Click && l_event.m_type != GUI_EventType::Click) ||
                (e_itr.first == EventType::GUI_Release && l_event.m_type != GUI_EventType::Release) ||
                (e_itr.first == EventType::GUI_Hover && l_event.m_type != GUI_EventType::Hover) ||
                (e_itr.first == EventType::GUI_Leave && l_event.m_type != GUI_EventType::Leave)) {
                    continue;
                }
            if (strcmp(e_itr.second.m_gui.m_interface, l_event.m_interface) ||
                strcmp(e_itr.second.m_gui.m_element, l_event.m_element)) {
                    continue;
                }
            bind->m_details->m_guiElement = l_event.m_element;
            bind->m_details->m_guiInterface = l_event.m_interface;
            ++(bind->m_c);
        }
    }
}

void EventManager::Update() {
    if (!m_hasFocus) return;
    for (auto& b_itr : m_bindings) {
        Binding* bind = b_itr.second;
        for (auto& e_itr : bind->m_events) {
            switch (e_itr.first) {
            case EventType::Keyboard:
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key(e_itr.second.m_code))) {
                    if (bind->m_details->m_keyCode != -1) {
                        bind->m_details->m_keyCode = e_itr.second.m_code;
                    }
                    ++(bind->m_c);
                }
                break;
            case EventType::Mouse:
                if (sf::Mouse::isButtonPressed(sf::Mouse::Button(e_itr.second.m_code))) {
                    if (bind->m_details->m_keyCode != -1) {
                        bind->m_details->m_keyCode = e_itr.second.m_code;
                    }
                    ++(bind->m_c);
                }
                break;
            case EventType::Joystick:
                break;
            } 
        }
        if (bind->m_c == bind->m_events.size()) {
            auto stateCallbacks = m_callbacks.find(m_currentState);
            auto otherCallbacks = m_callbacks.find(StateType(0));
            if (stateCallbacks != m_callbacks.end()) {
                auto itr = stateCallbacks->second.find(bind->m_details->m_name);
                if (itr != stateCallbacks->second.end()) {
                    itr->second(bind->m_details);
                }
            }
            if (otherCallbacks != m_callbacks.end()) {
                auto itr = otherCallbacks->second.find(bind->m_details->m_name);
                if (itr != otherCallbacks->second.end()) {
                    itr->second(bind->m_details);
                }
            }
        }
        bind->m_c = 0;
        bind->m_events.clear();
    }
}

void EventManager::LoadBindings() {
    std::string delimeter = ":";
    std::ifstream file;
    file.open(Utils::GetWorkingDirectory() + "keys.cfg");
    if (!file.is_open()) {
        std::cout << "Failed to open keys.cfg file!" << std::endl;
        return;
    }
    std::string line;
    while (std::getline(file, line)) {
        std::stringstream keystream(line);
        std::string callbackName;
        keystream >> callbackName;
        Binding* bind = new Binding(callbackName);
        while (!keystream.eof()) {
            std::string keyval;
            keystream >> keyval;
            int start = 0;
            int end = keyval.find(delimeter);
            if (end == std::string::npos) {
                delete bind;
                bind = nullptr;
                break;
            }
            EventType eType = (EventType)stoi(keyval.substr(start, end - start));
            EventInfo eInfo;
            if (eType == EventType::GUI_Click || eType == EventType::GUI_Release ||
                eType == EventType::GUI_Hover || eType == EventType::GUI_Leave) {
                start = end + delimeter.length();
                end = keyval.find(delimeter, start);
                std::string window = keyval.substr(start, end - start);
                std::string element;
                if (end != std::string::npos) {
                    start = end + delimeter.length();
                    end = keyval.length();
                    element = keyval.substr(start, end);
                }
                char* w = new char[window.length() + 1];
                char* e = new char[element.length() + 1];
                strcpy_s(w, window.length() + 1, window.c_str());
                strcpy_s(e, element.length() + 1, element.c_str());
                eInfo.m_gui.m_interface = w;
                eInfo.m_gui.m_element = e;
            } else {
                int code = stoi(keyval.substr(end + delimeter.length(), keyval.find(delimeter, end + delimeter.length())));
                eInfo.m_code = code;
            }
            bind->m_events.emplace_back(std::make_pair(eType, eInfo));
        }
        if (!AddBinding(bind)) {
            delete bind;
        }
        bind = nullptr;
    }
    file.close();
}