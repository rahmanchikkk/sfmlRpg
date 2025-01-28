#pragma once
#include <SFML/Graphics.hpp>
#include "S_Base.h"
#include "C_Drawable.h"
#include "C_Position.h"
#include "Window.h"
#include "Directions.h"
#include <algorithm> 

class S_Renderer : public S_Base {
public:
    S_Renderer(SystemManager* l_sysMgr);
    ~S_Renderer();

    void Update(float l_dT);
    void HandleEvent(const EntityId& l_entity, const EntityEvent& l_event);
    void Notify(const Message& l_msg);
    void Render(Window* l_wind, unsigned int l_layer);

    void SortDrawables();
private:
    void SetSheetDirection(const EntityId& l_entity, const Direction& l_direction);
};