#pragma once
#include "GUI_Element.h"
#include "Utilities.h"
#include <unordered_map>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RenderTexture.hpp>

using Elements = std::unordered_map<std::string, GUI_Element*>;

class GUI_Manager;

class GUI_Interface : public GUI_Element {
    friend class GUI_Element;
    friend class GUI_Manager;
public:
    GUI_Interface(const std::string& l_name, GUI_Manager* l_guiMgr);
    ~GUI_Interface();

    void SetPosition(const sf::Vector2f& l_vec);
    bool AddElement(const GUI_ElementType& l_type, const std::string& l_name);
    bool RemoveElement(const std::string& l_name);
    bool HasElement(const std::string& l_name);
    GUI_Element* GetElement(const std::string& l_name);

    bool HasParent() const;
    GUI_Manager* GetManager() const;
    bool IsInside(const sf::Vector2f& l_point) const;

    void Focus();
    void Defocus();
    bool IsFocused();

    void ReadIn(std::stringstream& l_stream);
    void Update(float l_dT);
    void Draw(sf::RenderTarget* l_target);
    void OnClick(const sf::Vector2f& l_mousePos);
    void OnRelease();
    void OnHover(const sf::Vector2f& l_mousePos);
    void OnLeave();
    void OnTextEntered(const char& l_text);

    bool IsBeingMoved() const;
    bool IsMovable() const;
    
    void BeginMoving();
    void StopMoving();
    void ApplyStyle();

    const sf::Vector2f& GetPadding() const;
    void SetPadding(const sf::Vector2f& l_vec);

    sf::Vector2f GetGlobalPosition() const;
    const sf::Vector2f& GetContentSize() const;

    void Redraw();
    void RedrawContent();
    bool NeedsContentRedraw() const;
    void RedrawControls();
    bool NeedsControlRedraw() const;

    void ToggleTitleBar();
    void UpdateScrollHorizontal(unsigned int l_percentage);
    void UpdateScrollVertical(unsigned int l_percentage);
private:
    void DefocusAllTextfields();
    void AdjustContentSize(GUI_Element* l_reference = nullptr);
    void SetContentSize(const sf::Vector2f& l_vec);

    Elements m_elements;
    sf::Vector2f m_elementPadding;
    GUI_Manager* m_guiManager;
    GUI_Interface* m_parent;

    sf::RenderTexture* m_backdropTexture;
    sf::Sprite m_backdrop;
    sf::RectangleShape m_titleBar;
    sf::Vector2f m_mouseMoveLast;
    
    bool m_showTitleBar;
    bool m_movable;
    bool m_beingMoved;
    bool m_focused;
    
    sf::RenderTexture* m_contentTexture;
    sf::Sprite m_content;
    sf::Vector2f m_contentSize;
    int m_scrollHorizontal;
    int m_scrollVertical;
    bool m_contentRedraw;
    
    sf::RenderTexture* m_controlTexture;
    sf::Sprite m_controls;
    bool m_controlRedraw;
};