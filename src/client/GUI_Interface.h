#pragma once
#include "GUI_Element.h"

class GUI_Interface : public GUI_Element {
    friend class GUI_Element;
public:
    GUI_Interface();
    ~GUI_Interface();
    const sf::Vector2f& GetPadding();
    SharedContext* GetContext();
private:
    void AdjustContentSize(GUI_Element* l_element = nullptr);
    unsigned int m_scrollHorizontal;
    unsigned int m_scrollVertical;
};