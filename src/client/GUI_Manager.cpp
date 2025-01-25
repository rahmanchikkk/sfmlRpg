#include "GUI_Manager.h"
#include "SharedContext.h"

GUI_Manager::GUI_Manager(EventManager* l_eventMgr, SharedContext* l_context) : m_eventManager(l_eventMgr), m_context(l_context),
    m_currentState(StateType(0)) {
    RegisterElement<GUI_Label>(GUI_ElementType::Label);
    RegisterElement<GUI_Textfield>(GUI_ElementType::Textfield);
    RegisterElement<GUI_Scrollbar>(GUI_ElementType::Scrollbar);
    m_elemTypes.emplace("Interface", GUI_ElementType::Window);
    m_elemTypes.emplace("Button", GUI_ElementType::Button);
    m_elemTypes.emplace("Label", GUI_ElementType::Label);
    m_elemTypes.emplace("Textfield", GUI_ElementType::Textfield);
    m_elemTypes.emplace("Scrollbar", GUI_ElementType::Scrollbar);
    m_eventManager->AddCallback(StateType(0), "Mouse_Left", &GUI_Manager::HandleClick, this);
    m_eventManager->AddCallback(StateType(0), "Mouse_Left_Release", &GUI_Manager::HandleRelease, this);
    m_eventManager->AddCallback(StateType(0), "Text_Entered", &GUI_Manager::HandleTextEntered, this);
}

GUI_Manager::~GUI_Manager() {
    m_eventManager->RemoveCallback(StateType(0), "Mouse_Left");
    m_eventManager->RemoveCallback(StateType(0), "Mouse_Left_Release");
    m_eventManager->RemoveCallback(StateType(0), "Text_Entered");
    for (auto& itr : m_interfaces) {
        for (auto& itr2 : itr.second) {
            delete itr2.second;
        }
    }
}

bool GUI_Manager::AddInterface(const StateType& l_state, const std::string& l_name) {
    auto state = m_interfaces.emplace(l_state, GUI_Interfaces()).first;
    GUI_Interface* temp = new GUI_Interface(l_name, this);
    if (state->second.emplace(l_name, temp).second) return true;
    delete temp;
    return false; 
}

bool GUI_Manager::RemoveInterface(const StateType& l_state, const std::string& l_name) {
    auto state = m_interfaces.find(l_state);
    if (state == m_interfaces.end()) return false;
    auto itr = state->second.find(l_name);
    if (itr == state->second.end()) return false;
    delete itr->second;
    itr->second = nullptr;
    return state->second.erase(l_name);
}

GUI_Interface* GUI_Manager::GetInterface(const StateType& l_state, const std::string& l_name) {
    auto state = m_interfaces.find(l_state);
    if (state == m_interfaces.end()) return nullptr;
    auto itr = state->second.find(l_name);
    return (itr != state->second.end() ? itr->second : nullptr);
}

bool GUI_Manager::LoadInterface(const StateType& l_state, const std::string& l_interface, const std::string& l_name) {
    std::ifstream file;
    file.open(Utils::GetWorkingDirectory() + "media/GUI_Interfaces/" + l_interface + ".interfaces");
    std::string interfaceName;
    if (!file.is_open()) {
        std::cout << "Failed to open interface from file: " << l_interface << std::endl;
        return false;
    }
    std::string line;
    while (std::getline(file, line)) {
        if (line[0] == '|') continue;
        std::stringstream keystream(line);
        std::string key;
        keystream >> key;
        if (key == "Interface") {
            std::string style;
            keystream >> interfaceName >> style;
            if (!AddInterface(l_state, l_name)) {
                std::cout << "Failed adding interface: " << l_name << std::endl;
                continue;
            }
            GUI_Interface* i = GetInterface(l_state, l_name);
            if (!i) continue;
            keystream >> *i;
            if (!LoadStyle(style, i)) {
                std::cout << "Failed loading style for interface: " << l_name << std::endl;
                continue;
            }
        } else if (key == "Element") {
            if (interfaceName == "") {
                std::cout << "Error: element outside the interface!" << std::endl;
                continue;
            }
            std::string type;
            std::string name;
            sf::Vector2f position;
            std::string style;
            keystream >> type >> name >> position.x >> position.y >> style;
            GUI_ElementType eType = StringToType(type);
            if (eType == GUI_ElementType::None) {
                std::cout << "Unknown element type!" << std::endl;
                continue;
            }
            GUI_Interface* i = GetInterface(l_state, l_name);
            if (!i) continue;
            if (!i->AddElement(eType, name)) continue;
            GUI_Element* elem = i->GetElement(name);
            keystream >> *elem;
            elem->SetPosition(position);
            if (!LoadStyle(style, elem)) {
                std::cout << "Failed loading style for element: " << name << std::endl;
                continue;
            }
        }
    }
    file.close();
    return true;
}

bool GUI_Manager::LoadStyle(const std::string& l_style, GUI_Element* l_element) {
    std::ifstream file;
    file.open(Utils::GetWorkingDirectory() + "media/GUI_Styles/" + l_style + ".style");
    std::string currentState;
    GUI_Style parentStyle;
    GUI_Style tempStyle;
    if (!file.is_open()) {
        std::cout << "Failed opening style file: " << l_style << std::endl;
        return false;
    }
    std::string line;
    while (std::getline(file, line)) {
        if (line[0] == '|') continue;
        std::stringstream keystream(line);
        std::string type;
        keystream >> type;
        if (type == "State") {
            if (currentState != "") {
                std::cout << "State keyword found inside other state!" << std::endl;
                continue;
            }
            keystream >> currentState;
        } else if (type == "/State") {
            if (currentState == "") {
                std::cout << "/State keyword found prior to state!" << std::endl;
                continue;
            }
            GUI_ElementState state = GUI_ElementState::Neutral;
            if (currentState == "Hover") state = GUI_ElementState::Focused;
            else if (currentState == "Clicked") state = GUI_ElementState::Clicked;
            if (state == GUI_ElementState::Neutral) {
                parentStyle = tempStyle;
                l_element->UpdateStyle(GUI_ElementState::Neutral, tempStyle);
                l_element->UpdateStyle(GUI_ElementState::Focused, tempStyle);
                l_element->UpdateStyle(GUI_ElementState::Neutral, tempStyle);
            } else {
                l_element->UpdateStyle(state, tempStyle);
            }
            tempStyle = parentStyle;
            currentState = "";
        } else {
            if (currentState == "") {
                std::cout << type << " keyword found prior to State!" << std::endl;
                continue;
            }
            if (type == "Size") {
                keystream >> tempStyle.m_size.x >> tempStyle.m_size.y;
            } else if (type == "BgColor") {
                int r, g, b, a;
                keystream >> r >> g >> b >> a;
                tempStyle.m_backgroundColor = sf::Color(r, g, b, a);
            } else if (type == "BgImageColor") {
                int r, g, b, a;
                keystream >> r >> g >> b >> a;
                tempStyle.m_backgroundImageColor = sf::Color(r, g, b, a);
            } else if (type == "BgImage") {
                keystream >> tempStyle.m_backgroundImage;
            } else if (type == "ElementColor") {
                int r, g, b, a;
                keystream >> r >> g >> b >> a;
                tempStyle.m_elementColor = sf::Color(r, g, b, a);
            } else if (type == "TextColor") {
                int r, g, b, a;
                keystream >> r >> g >> b >> a;
                tempStyle.m_textColor = sf::Color(r, g, b, a);
            } else if (type == "Font") {
                keystream >> tempStyle.m_textFont;
            } else if (type == "TextCenterOrigin") {
                tempStyle.m_textCenterOrigin = true;
            } else if (type == "TextSize") {
                keystream >> tempStyle.m_textSize;
            } else if (type == "TextPadding") {
                keystream >> tempStyle.m_textPadding.x >> tempStyle.m_textPadding.y;
            } else if (type == "Glyph") {
                keystream >> tempStyle.m_glyph;
            } else if (type == "GlyphPadding") {
                keystream >> tempStyle.m_glyphPadding.x >> tempStyle.m_glyphPadding.y;
            } else {
                std::cout << type << " keyword is undefined." << std::endl;
            }
        }
    }
    file.close();
    return true;
}

void GUI_Manager::SetCurrentState(const StateType& l_state) {
    if (m_currentState == l_state) return;
    HandleRelease(nullptr);
    m_currentState = l_state;
}

SharedContext* GUI_Manager::GetContext() { return m_context; }

void GUI_Manager::HandleClick(EventDetails* l_details) {
    auto state = m_interfaces.find(m_currentState);
    if (state == m_interfaces.end()) return;
    sf::Vector2f mousePos = sf::Vector2f(m_eventManager->GetMousePos(m_context->m_window->GetRenderWindow()));
    for (auto itr = state->second.begin(); itr != state->second.end(); ++itr) {
        if (!itr->second->IsInside(mousePos)) continue;
        if (!itr->second->IsActive()) continue;
        itr->second->OnClick(mousePos);
        itr->second->Focus();
        if (itr->second->IsBeingMoved()) { itr->second->BeginMoving(); }
        return;
    }
}

void GUI_Manager::HandleRelease(EventDetails* l_details) {
    auto state = m_interfaces.find(m_currentState);
    if (state == m_interfaces.end()) return;
    for (auto itr = state->second.begin(); itr != state->second.end(); ++itr) {
        if (!itr->second->IsActive()) continue;
        if (itr->second->GetState() == GUI_ElementState::Clicked) {
            itr->second->OnRelease();
        }
        if (itr->second->IsBeingMoved()) { itr->second->StopMoving(); }
        return;
    }
}

void GUI_Manager::HandleTextEntered(EventDetails* l_details) {
    auto state = m_interfaces.find(m_currentState);
    if (state == m_interfaces.end()) return;
    for (auto itr = state->second.begin(); itr != state->second.end(); ++itr) {
        if (itr->second->GetType() != GUI_ElementType::Textfield) continue;
        if (!itr->second->IsFocused()) continue;
        itr->second->OnTextEntered(l_details->m_textEntered);
        return;
    }
}

void GUI_Manager::DefocusAllInterfaces() {
    auto state = m_interfaces.find(m_currentState);
    if (state == m_interfaces.end()) return;
    for (auto itr = state->second.begin(); itr != state->second.end(); ++itr) {
        itr->second->Defocus();
    }
}

void GUI_Manager::AddEvent(GUI_Event l_event) {
    m_events[m_currentState].emplace_back(l_event);
}

bool GUI_Manager::PollEvent(GUI_Event& l_event) {
    if (m_events[m_currentState].empty()) return false;
    l_event = m_events[m_currentState].back();
    m_events[m_currentState].pop_back();
    return true;
}

void GUI_Manager::Update(float l_dT) {
    sf::Vector2i mousePos = m_eventManager->GetMousePos(m_context->m_window->GetRenderWindow());
    auto state = m_interfaces.find(m_currentState);
    if (state == m_interfaces.end()) return;
    for (auto itr = state->second.begin(); itr != state->second.end(); ++itr) {
        if (!itr->second->IsActive()) continue;
        itr->second->Update(l_dT);
        if (itr->second->IsBeingMoved()) continue;
        if (itr->second->GetState() == GUI_ElementState::Neutral) {
            if (itr->second->IsInside(sf::Vector2f(mousePos))) {
                itr->second->OnHover(sf::Vector2f(mousePos));
            }
            return;
        } else if (itr->second->GetState() == GUI_ElementState::Focused) {
            itr->second->OnLeave();
        }
    }
}

void GUI_Manager::Render(sf::RenderWindow* l_wind) {
    auto state = m_interfaces.find(m_currentState);
    if (state == m_interfaces.end()) return;
    for (auto itr = state->second.begin(); itr != state->second.end(); ++itr) {
        if (!itr->second->IsActive()) continue;
        if (itr->second->NeedsRedraw()) itr->second->Redraw();
        if (itr->second->NeedsContentRedraw()) itr->second->RedrawContent();
        if (itr->second->NeedsControlRedraw()) itr->second->RedrawControls();
        itr->second->Draw(l_wind);
    }
}

GUI_Element* GUI_Manager::CreateElement(const GUI_ElementType& l_type, GUI_Interface* l_owner) {
    if (l_type == GUI_ElementType::Window) return new GUI_Interface("", this);
    auto s = m_guiFactory.find(l_type);
    return (s != m_guiFactory.end() ? s->second(l_owner) : nullptr);
}

GUI_ElementType GUI_Manager::StringToType(const std::string& l_type) {
    auto itr = m_elemTypes.find(l_type);
    return (itr != m_elemTypes.end() ? itr->second : GUI_ElementType::None);
}