#include "GUI_Interface.h"
#include "GUI_Manager.h"
#include "SharedContext.h"

GUI_Interface::GUI_Interface(const std::string& l_name, GUI_Manager* l_guiMgr) : GUI_Element(l_name, GUI_ElementType::Window, this), 
    m_guiManager(l_guiMgr), m_scrollHorizontal(0), m_scrollVertical(0), m_showTitleBar(false), m_movable(false), m_beingMoved(false),
    m_contentRedraw(true), m_controlRedraw(true), m_parent(nullptr) {
    m_backdropTexture = new sf::RenderTexture();
    m_contentTexture = new sf::RenderTexture();
    m_controlTexture = new sf::RenderTexture();
} 

GUI_Interface::~GUI_Interface() {
    if (m_backdropTexture) delete m_backdropTexture;
    if (m_contentTexture) delete m_contentTexture;
    if (m_controlTexture) delete m_controlTexture;
    for (auto& itr : m_elements) {
        delete itr.second;
    }
}

void GUI_Interface::SetPosition(const sf::Vector2f& l_vec) {
    GUI_Element::SetPosition(l_vec);
    m_backdrop.setPosition(m_position);
    m_content.setPosition(m_position);
    m_controls.setPosition(m_position);
    m_titleBar.setPosition(m_position.x, m_position.y - m_titleBar.getSize().y);
    m_visual.m_text.setPosition(m_position - m_style[m_state].m_textPadding);
}

bool GUI_Interface::AddElement(const GUI_ElementType& l_type, const std::string& l_name) {
    auto itr = m_elements.find(l_name);
    if (itr != m_elements.end()) return false;
    GUI_Element* element = m_guiManager->CreateElement(l_type, this);
    if (!element) return false;
    element->SetOwner(this);
    element->SetName(l_name);
    m_elements.emplace(l_name, element);
    m_contentRedraw = true;
    m_controlRedraw = true;
    return true;
}

bool GUI_Interface::RemoveElement(const std::string& l_name) {
    auto itr = m_elements.find(l_name);
    if (itr == m_elements.end()) return false;
    delete itr->second;
    m_elements.erase(itr);
    m_contentRedraw = true;
    m_controlRedraw = true;
    AdjustContentSize();
    return true;
}

bool GUI_Interface::HasElement(const std::string& l_name) {
    auto itr = m_elements.find(l_name);
    return (itr != m_elements.end());
}

bool GUI_Interface::HasParent() const {
    return (m_parent != nullptr);
}

GUI_Manager* GUI_Interface::GetManager() const {
    return m_guiManager;
}

bool GUI_Interface::IsInside(const sf::Vector2f& l_point) const {
    if (GUI_Element::IsInside(l_point)) return true;
    else return m_titleBar.getGlobalBounds().contains(l_point);
}

void GUI_Interface::Focus() { m_focused = true; }
void GUI_Interface::Defocus() { m_focused = false; }
bool GUI_Interface::IsFocused() { return m_focused; }

void GUI_Interface::ReadIn(std::stringstream& l_stream) {
    std::string movableState;
    std::string showTitle;
    std::string title;
    l_stream >> m_elementPadding.x >> m_elementPadding.y >> movableState >> showTitle;
    Utils::ReadQuotedString(l_stream, title);
    m_visual.m_text.setString(title);
    if (movableState == "Movable") m_movable = true;
    if (showTitle == "Title") m_showTitleBar = true;
}

void GUI_Interface::Update(float l_dT) {
    sf::Vector2f mousePos = sf::Vector2f(m_guiManager->GetContext()->m_eventManager->GetMousePos(
        m_guiManager->GetContext()->m_window->GetRenderWindow()
    ));
    if (m_beingMoved && m_mouseMoveLast != mousePos) {
        sf::Vector2f difference = mousePos - m_mouseMoveLast;
        m_mouseMoveLast = mousePos;
        sf::Vector2f newPosition = m_position + difference;
        SetPosition(newPosition);
    }
    for (auto& itr : m_elements) {
        if (itr.second->NeedsRedraw()) {
            if (itr.second->IsControl()) m_controlRedraw = true;
            else m_contentRedraw = true;
        }
        if (!itr.second->IsActive()) continue;
        itr.second->Update(l_dT);
        if (itr.second->IsControl()) continue;
        GUI_Event event;
        event.m_interface = m_name.c_str();
        event.m_element = itr.second->m_name.c_str();
        event.m_clickCoords.y = mousePos.x;
        event.m_clickCoords.y = mousePos.y;
        if (itr.second->IsInside(mousePos) && !m_titleBar.getGlobalBounds().contains(mousePos) && IsInside(mousePos)) {
            if (itr.second->GetState() != GUI_ElementState::Neutral) continue;
            itr.second->OnHover(mousePos);
            event.m_type = GUI_EventType::Hover;
            m_guiManager->AddEvent(event);
        } else if (itr.second->GetState() == GUI_ElementState::Focused) {
            itr.second->OnLeave();
            event.m_type = GUI_EventType::Leave;
            m_guiManager->AddEvent(event);
        }
    }
}

void GUI_Interface::Draw(sf::RenderTarget* l_target) {
    l_target->draw(m_backdrop);
    l_target->draw(m_controls);
    l_target->draw(m_content);
    if (!m_showTitleBar) return;
    l_target->draw(m_titleBar);
    l_target->draw(m_visual.m_text);
}

void GUI_Interface::OnClick(const sf::Vector2f& l_mousePos) {
    DefocusAllTextfields();
    if (m_titleBar.getGlobalBounds().contains(l_mousePos) && m_movable && m_showTitleBar) {
        m_beingMoved = true;
    } else {
        GUI_Event event;
        event.m_interface = m_name.c_str();
        event.m_element = "";
        event.m_clickCoords.x = l_mousePos.x;
        event.m_clickCoords.y = l_mousePos.y;
        event.m_type = GUI_EventType::Click;
        m_guiManager->AddEvent(event);
        for (auto& itr : m_elements) {
            if (!itr.second->IsInside(l_mousePos)) continue;
            itr.second->OnClick(l_mousePos);
            event.m_element = itr.second->m_name.c_str();
            m_guiManager->AddEvent(event);
        }
        SetState(GUI_ElementState::Clicked);
    }
}

void GUI_Interface::OnRelease() {
    GUI_Event event;
    event.m_interface = m_name.c_str();
    event.m_element = "";
    event.m_type = GUI_EventType::Release;
    m_guiManager->AddEvent(event);
    for (auto& itr : m_elements) {
        if (!itr.second->GetState() != GUI_ElementState::Clicked) {
            continue;
        }
        itr.second->OnRelease();
        event.m_element = itr.second->m_name.c_str();
        m_guiManager->AddEvent(event);
    }
    SetState(GUI_ElementState::Neutral);
}

void GUI_Interface::OnHover(const sf::Vector2f& l_mousePos) {
    GUI_Event event;
    event.m_interface = m_name.c_str();
    event.m_element = "";
    event.m_type = GUI_EventType::Hover;
    event.m_clickCoords.x = l_mousePos.x;
    event.m_clickCoords.y = l_mousePos.y;
    m_guiManager->AddEvent(event);
    SetState(GUI_ElementState::Focused);
}

void GUI_Interface::OnLeave() {
    GUI_Event event;
    event.m_interface = m_name.c_str();
    event.m_element = "";
    event.m_type = GUI_EventType::Leave;
    m_guiManager->AddEvent(event);
    SetState(GUI_ElementState::Neutral);
}

void GUI_Interface::OnTextEntered(const char& l_text) {
    for (auto& itr : m_elements) {
        if (itr.second->GetType() != GUI_ElementType::Textfield) continue;
        if (itr.second->GetState() != GUI_ElementState::Clicked) continue;
        if (l_text == 8) {
            const auto& text = itr.second->GetText();
            itr.second->SetText(text.substr(0, text.length() - 1));
            return;
        }
        if (l_text < 32 || l_text > 126) return;
        std::string text = itr.second->GetText();
        text.push_back(l_text);
        itr.second->SetText(text);
        return;
    }
}

bool GUI_Interface::IsBeingMoved() const {
    return m_beingMoved;
}

bool GUI_Interface::IsMovable() const {
    return m_movable;
}

void GUI_Interface::BeginMoving() {
    if (!m_showTitleBar || !m_movable) return;
    m_beingMoved = true;
    SharedContext* m_context = m_guiManager->GetContext();
    m_mouseMoveLast = sf::Vector2f(m_context->m_eventManager->GetMousePos(m_context->m_window->GetRenderWindow()));
}

void GUI_Interface::StopMoving() {
    m_beingMoved = false;
}

const sf::Vector2f& GUI_Interface::GetPadding() const { return m_elementPadding; }
void GUI_Interface::SetPadding(const sf::Vector2f& l_vec) { m_elementPadding = l_vec; }

sf::Vector2f GUI_Interface::GetGlobalPosition() const {
    sf::Vector2f position = m_position;
    GUI_Interface* i = m_parent;
    while (i) {
        position += i->GetGlobalPosition();
        i = i->m_parent;
    }
    return position;
}

void GUI_Interface::ApplyStyle() {
    GUI_Element::ApplyStyle();
    m_visual.m_backgroundSolid.setPosition(0.f, 0.f);
    m_visual.m_backgroundImage.setPosition(0.f, 0.f);
    m_titleBar.setPosition(m_position.x, m_position.y - m_titleBar.getSize().y);
    m_titleBar.setSize(sf::Vector2f(m_style[m_state].m_size.x, 16.f));
    m_titleBar.setFillColor(m_style[m_state].m_elementColor);
    m_visual.m_text.setPosition(m_position + m_style[m_state].m_textPadding);
    m_visual.m_glyph.setPosition(m_position + m_style[m_state].m_glyphPadding);
}

void GUI_Interface::Redraw() {
    if (m_backdropTexture->getSize().x != m_style[m_state].m_size.x || m_backdropTexture->getSize().y != m_style[m_state].m_size.y) {
        m_backdropTexture->create(m_style[m_state].m_size.x, m_style[m_state].m_size.y);
    }
    m_backdropTexture->clear(sf::Color(0, 0, 0, 0));
    ApplyStyle();
    m_backdropTexture->draw(m_visual.m_backgroundSolid);
    if (m_style[m_state].m_backgroundImage != "") {
        m_backdropTexture->draw(m_visual.m_backgroundImage);
    }
    m_backdropTexture->display();
    m_backdrop.setTexture(m_backdropTexture->getTexture());
    m_backdrop.setTextureRect(sf::IntRect(0, 0, m_style[m_state].m_size.x, m_style[m_state].m_size.y));
    SetRedraw(false);
}

void GUI_Interface::RedrawContent() {
    if (m_contentTexture->getSize().x != m_style[m_state].m_size.x || m_contentTexture->getSize().y != m_style[m_state].m_size.y) {
        m_contentTexture->create(m_style[m_state].m_size.x, m_style[m_state].m_size.y);
    }
    m_contentTexture->clear(sf::Color(0, 0, 0, 0));
    for (auto& itr : m_elements) {
        if (!itr.second->IsActive() || itr.second->IsControl()) continue;
        ApplyStyle();
        itr.second->Draw(m_contentTexture);
        itr.second->SetRedraw(false);
    }
    m_contentTexture->display();
    m_content.setTexture(m_contentTexture->getTexture());
    m_content.setTextureRect(sf::IntRect(m_scrollHorizontal, m_scrollVertical, m_style[m_state].m_size.x, m_style[m_state].m_size.y));
    m_contentRedraw = false;
}

bool GUI_Interface::NeedsContentRedraw() const { return m_contentRedraw; }

void GUI_Interface::RedrawControls() {
    if (m_controlTexture->getSize().x != m_style[m_state].m_size.x || m_controlTexture->getSize().y != m_style[m_state].m_size.y) {
        m_controlTexture->create(m_style[m_state].m_size.x, m_style[m_state].m_size.y);
    }
    m_controlTexture->clear(sf::Color(0, 0, 0, 0));
    for (auto& itr : m_elements) {
        if (!itr.second->IsControl()) continue;
        ApplyStyle();
        itr.second->Draw(m_controlTexture);
        itr.second->SetRedraw(false);
    }
    m_controlTexture->display();
    m_controls.setTexture(m_controlTexture->getTexture());
    m_controls.setTextureRect(sf::IntRect(0, 0, m_style[m_state].m_size.x, m_style[m_state].m_size.y));
    m_controlRedraw = false;
}

bool GUI_Interface::NeedsControlRedraw() const { return m_controlRedraw; }

void GUI_Interface::ToggleTitleBar() { m_showTitleBar = !m_showTitleBar; }

void GUI_Interface::AdjustContentSize(GUI_Element* l_reference) {
    if (l_reference) {
        sf::Vector2f bottomRight = l_reference->GetPosition() + l_reference->GetSize();
        if (bottomRight.x > m_contentSize.x) m_contentSize.x = bottomRight.x;
        if (bottomRight.y > m_contentSize.y) m_contentSize.y = bottomRight.y;
    }
    sf::Vector2f farthest = m_contentSize;
    for (auto& itr : m_elements) {
        if (itr.second->IsActive() && !itr.second->IsControl()) continue;
        sf::Vector2f bottomRight = itr.second->GetPosition() + itr.second->GetSize();
        if (bottomRight.x > farthest.x) farthest.x = bottomRight.x;
        if (bottomRight.y > farthest.y) farthest.y = bottomRight.y;
    }
    SetContentSize(farthest);
}

void GUI_Interface::SetContentSize(const sf::Vector2f& l_vec) {
    m_contentSize = l_vec;
}

void GUI_Interface::UpdateScrollHorizontal(unsigned int l_percentage) {
    if (l_percentage > 100) return;
    m_scrollHorizontal = ((m_contentSize.x - GetSize().x) / 100) * l_percentage;
    sf::IntRect rect = m_content.getTextureRect();
    m_content.setTextureRect(sf::IntRect(m_scrollHorizontal, m_scrollVertical, rect.width, rect.height));
}

void GUI_Interface::UpdateScrollVertical(unsigned int l_percentage) {
    if (l_percentage > 100) return;
    m_scrollVertical = ((m_contentSize.y - GetSize().y) / 100) * l_percentage;
    sf::IntRect rect = m_content.getTextureRect();
    m_content.setTextureRect(sf::IntRect(m_scrollHorizontal, m_scrollVertical, rect.width, rect.height));
}

void GUI_Interface::DefocusAllTextfields() {
    GUI_Event event;
    event.m_interface = m_name.c_str();
    event.m_element = "";
    event.m_type = GUI_EventType::Release;
    for (auto& itr : m_elements) {
        if (itr.second->GetType() != GUI_ElementType::Textfield) continue;
        itr.second->SetState(GUI_ElementState::Neutral);
        event.m_element = itr.second->m_name.c_str();
        m_guiManager->AddEvent(event);
    }
}

GUI_Element* GUI_Interface::GetElement(const std::string& l_name) {
    auto itr = m_elements.find(l_name);
    return (itr != m_elements.end() ? itr->second : nullptr);
}