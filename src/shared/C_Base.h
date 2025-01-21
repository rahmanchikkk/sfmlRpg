#pragma once
#include "ECS_Types.h"
#include <sstream>

class C_Base {
public:
    C_Base(const Component& l_type) : m_type(l_type) {};
    virtual ~C_Base();

    friend std::stringstream& operator >>(std::stringstream& l_stream, C_Base& b) {
        b.ReadIn(l_stream);
        return l_stream;
    };

    virtual void ReadIn(std::stringstream& l_stream) = 0;
    Component GetType() { return m_type; };
protected:
    Component m_type;
};