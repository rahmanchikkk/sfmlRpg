#include "S_Renderer.h"
#include "SystemManager.h"
#include "C_SpriteSheet.h"

S_Renderer::S_Renderer(SystemManager* l_sysMgr) : S_Base(System::Renderer, l_sysMgr) {
    Bitmask req;
    req.TurnOnBit((unsigned int)Component::Position);
    req.TurnOnBit((unsigned int)Component::SpriteSheet);
    m_requiredComponents.push_back(req);
    m_systemManager->GetMessageHandler()->Subscribe(EntityMessage::Direction_Changed, this);
}

S_Renderer::~S_Renderer() {}

void S_Renderer::Update(float l_dT) {
    EntityManager* entityManager = m_systemManager->GetEntityManager();
    for (auto& itr : m_entities) {
        C_Position* pos = entityManager->GetComponent<C_Position>(itr, Component::Position);
        C_Drawable* drawable = nullptr;
        if (entityManager->HasComponent(itr, Component::SpriteSheet)) {
            drawable = entityManager->GetComponent<C_SpriteSheet>(itr, Component::SpriteSheet);
        } else continue;
        drawable->UpdatePosition(pos->GetPosition());
    }
}

void S_Renderer::HandleEvent(const EntityId& l_entity, const EntityEvent& l_event) {
    if (l_event == EntityEvent::Moving_Up || l_event == EntityEvent::Moving_Down ||
        l_event == EntityEvent::Moving_Left || l_event == EntityEvent::Moving_Right ||
        l_event == EntityEvent::Elevation_Change || l_event == EntityEvent::Spawned) {
            SortDrawables();
        }
}

void S_Renderer::Notify(const Message& l_msg) {
    if (!HasEntity(l_msg.m_receiver)) return;
    EntityMessage m = (EntityMessage)l_msg.m_type;
    switch (m) {
    case EntityMessage::Direction_Changed:
        SetSheetDirection(l_msg.m_receiver, (Direction)l_msg.m_int);
        break;
    }
}

void S_Renderer::Render(Window* l_wind, unsigned int l_layer) {
    EntityManager* entityManager = m_systemManager->GetEntityManager();
    for (auto& itr : m_entities) {
        C_Position* pos = entityManager->GetComponent<C_Position>(itr, Component::Position);
        if (pos->GetElevation() < l_layer) continue;
        if (pos->GetElevation() > l_layer) break;
        C_Drawable* drawable = nullptr;
        if (!entityManager->HasComponent(itr, Component::SpriteSheet)) continue;
        drawable = entityManager->GetComponent<C_Drawable>(itr, Component::SpriteSheet);
        sf::FloatRect drawableBounds;
        drawableBounds.left = pos->GetPosition().x - (drawable->GetSize().x / 2);
        drawableBounds.top = pos->GetPosition().y - drawable->GetSize().y;
        drawableBounds.width = drawable->GetSize().x;
        drawableBounds.height = drawable->GetSize().y;
        if (!l_wind->GetViewSpace().intersects(drawableBounds)) continue;
        drawable->Draw(l_wind->GetRenderWindow());
    }
}

void S_Renderer::SetSheetDirection(const EntityId& l_entity, const Direction& l_direction) {
    EntityManager* entityManager = m_systemManager->GetEntityManager();
    if (!entityManager->HasComponent(l_entity, Component::SpriteSheet)) return;
    C_SpriteSheet* sheet = entityManager->GetComponent<C_SpriteSheet>(l_entity, Component::SpriteSheet);
    sheet->GetSpriteSheet()->SetDirection(l_direction);
}

void S_Renderer::SortDrawables() {
    EntityManager* entityManager = m_systemManager->GetEntityManager();
    std::sort(m_entities.begin(), m_entities.end(), [&entityManager](const EntityId& a, const EntityId& b) {
        C_Position* p1 = entityManager->GetComponent<C_Position>(a, Component::Position);
        C_Position* p2 = entityManager->GetComponent<C_Position>(b, Component::Position);
        if (p1->GetElevation() == p2->GetElevation()) {
            return p1->GetPosition().y < p2->GetPosition().y;
        }
        return p1->GetElevation() < p2->GetElevation();
    });
}