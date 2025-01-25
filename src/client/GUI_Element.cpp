#include "GUI_Element.h"
#include "GUI_Interface.h"
#include "GUI_Manager.h"
#include "SharedContext.h"

GUI_Element::GUI_Element(const std::string& l_name, const GUI_ElementType& l_type, GUI_Interface* l_owner) :
    m_name(l_name), m_type(l_type), m_owner(l_owner), m_state(GUI_ElementState::Neutral), m_needsRedraw(false),
    m_active(true), m_isControl(false) {}

GUI_Element::~GUI_Element() {
    ReleaseResources();
}

void GUI_Element::ReleaseResources() {
    for (auto& itr : m_style) {
        ReleaseTexture(itr.second.m_backgroundImage);
        ReleaseTexture(itr.second.m_glyph);
        ReleaseFont(itr.second.m_textFont);
    }
}

void GUI_Element::UpdateStyle(const GUI_ElementState& l_state, const GUI_Style& l_style) {
    auto& style = m_style[l_state];
    if (style.m_backgroundImage != l_style.m_backgroundImage) {
        ReleaseTexture(style.m_backgroundImage);
        RequireTexture(l_style.m_backgroundImage);
    }
    if (style.m_glyph != l_style.m_glyph) {
        ReleaseTexture(style.m_glyph);
        ReleaseTexture(l_style.m_glyph);
    }
    if (style.m_textFont != l_style.m_textFont) {
        ReleaseFont(style.m_textFont);
        RequireFont(l_style.m_textFont);
    }
    m_style[l_state] = l_style;
    if (l_state == m_state) {
        SetRedraw(true);
        ApplyStyle();
    }
}

void GUI_Element::ApplyStyle() {
    ApplyBgStyle();
    ApplyGlyphStyle();
    ApplyTextStyle();
    if (m_owner != this && !IsControl()) m_owner->AdjustContentSize(this);
}

GUI_ElementType GUI_Element::GetType() {
    return m_type;
}

void GUI_Element::SetName(const std::string& l_name) { m_name = l_name; }
const std::string& GUI_Element::GetName() const { return m_name; }

const sf::Vector2f& GUI_Element::GetPosition() const { return m_position; }

void GUI_Element::SetPosition(const sf::Vector2f& l_vec) { 
    m_position = l_vec;
    if (m_owner == this || m_owner == nullptr) return;
    sf::Vector2f padding = m_owner->GetPadding();
    if (m_position.x < padding.x) m_position.x = padding.x;
    if (m_position.y < padding.y) m_position.y = padding.y; 
}

GUI_Interface* GUI_Element::GetOwner() const { return m_owner; }
bool GUI_Element::HasOwner() const { return (m_owner != nullptr); }
void GUI_Element::SetOwner(GUI_Interface* l_owner) { m_owner = l_owner; }

const sf::Vector2f& GUI_Element::GetSize() const { return m_style.at(m_state).m_size; }

GUI_ElementState GUI_Element::GetState() const { return m_state; }
void GUI_Element::SetState(const GUI_ElementState& l_state) { m_state = l_state; }

std::string GUI_Element::GetText() const { return m_visual.m_text.getString(); }
void GUI_Element::SetText(const std::string& l_text) { m_visual.m_text.setString(l_text); SetRedraw(true); }

bool GUI_Element::NeedsRedraw() const { return m_needsRedraw; }
void GUI_Element::SetRedraw(const bool& l_redraw) { m_needsRedraw = l_redraw; }

bool GUI_Element::IsActive() const { return m_active; }

void GUI_Element::SetActive(const bool& l_active) { 
    if (l_active != m_active) {
        m_active = l_active;
        SetRedraw(true);
    }
}

bool GUI_Element::IsControl() const { return m_isControl; }

bool GUI_Element::IsInside(const sf::Vector2f& l_point) const {
    sf::Vector2f pos = GetGlobalPosition();
    return (l_point.x >= pos.x && l_point.y >= pos.y &&
            l_point.x <= m_style.at(m_state).m_size.x + pos.x &&
            l_point.y <= m_style.at(m_state).m_size.y + pos.y);
}

sf::Vector2f GUI_Element::GetGlobalPosition() const {
    sf::Vector2f position = GetPosition();
    if (m_owner == this || m_owner == nullptr) return position;
    position += m_owner->GetGlobalPosition();
    if (IsControl()) return position;
    position.x -= m_owner->m_scrollHorizontal;
    position.y -= m_owner->m_scrollVertical;
    return position;
}

void GUI_Element::ApplyBgStyle() {
    TextureManager* textureManager = m_owner->GetManager()->GetContext()->m_textureManager;
    const auto& currentStyle = m_style[m_state];
    if (currentStyle.m_backgroundImage != "") {
        m_visual.m_backgroundImage.setTexture(*textureManager->GetResource(currentStyle.m_backgroundImage));
        m_visual.m_backgroundImage.setColor(currentStyle.m_backgroundImageColor);
    }
    m_visual.m_backgroundImage.setPosition(m_position);
    m_visual.m_backgroundSolid.setSize(sf::Vector2f(currentStyle.m_size));
    m_visual.m_backgroundSolid.setFillColor(currentStyle.m_backgroundColor);
    m_visual.m_backgroundSolid.setPosition(m_position);
}

void GUI_Element::ApplyTextStyle() {
    FontManager* fontManager = m_owner->GetManager()->GetContext()->m_fontManager;
    const auto& currentStyle = m_style[m_state];
    if (currentStyle.m_textFont != "") {
        m_visual.m_text.setFont(*fontManager->GetResource(currentStyle.m_textFont));
        m_visual.m_text.setCharacterSize(currentStyle.m_textSize);
        m_visual.m_text.setColor(currentStyle.m_textColor);
        if (currentStyle.m_textCenterOrigin) {
            sf::FloatRect rect = m_visual.m_text.getLocalBounds();
            m_visual.m_text.setOrigin(rect.left + rect.width / 2.0f, rect.top + rect.height / 2.0f);
        } else {
            m_visual.m_text.setOrigin(0.f, 0.f);
        }
    }
    m_visual.m_text.setPosition(m_position + currentStyle.m_textPadding);
}

void GUI_Element::ApplyGlyphStyle() {
    TextureManager* textureManager = m_owner->GetManager()->GetContext()->m_textureManager;
    const auto& currentStyle = m_style[m_state];
    if (currentStyle.m_glyph != "") {
        m_visual.m_glyph.setTexture(*textureManager->GetResource(currentStyle.m_glyph));
    }
    m_visual.m_glyph.setPosition(m_position + currentStyle.m_glyphPadding);
}

void GUI_Element::RequireTexture(const std::string& l_name) {
    if (l_name == "") return;
    m_owner->GetManager()->GetContext()->m_textureManager->RequireResource(l_name);
}

void GUI_Element::ReleaseTexture(const std::string& l_name) {
    if (l_name == "") return;
    m_owner->GetManager()->GetContext()->m_textureManager->ReleaseResource(l_name);
}

void GUI_Element::RequireFont(const std::string& l_name) {
    if (l_name == "") return;
    m_owner->GetManager()->GetContext()->m_fontManager->RequireResource(l_name);
}

void GUI_Element::ReleaseFont(const std::string& l_name) {
    if (l_name == "") return;
    m_owner->GetManager()->GetContext()->m_fontManager->ReleaseResource(l_name);
}