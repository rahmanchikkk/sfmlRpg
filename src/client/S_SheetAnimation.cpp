#include "S_SheetAnimation.h"
#include "SystemManager.h"
#include "C_SpriteSheet.h"

S_SheetAnimation::S_SheetAnimation(SystemManager* l_sysMgr) : S_Base(System::SheetAnimation, l_sysMgr) {
    Bitmask req;
    req.TurnOnBit((unsigned int)Component::SpriteSheet);
    req.TurnOnBit((unsigned int)Component::State);
    m_requiredComponents.push_back(req);
    m_systemManager->GetMessageHandler()->Subscribe(EntityMessage::State_Changed, this);
}

S_SheetAnimation::~S_SheetAnimation() {}

void S_SheetAnimation::Update(float l_dT) {
    EntityManager* entityManager = m_systemManager->GetEntityManager();
    for (auto& itr : m_entities) {
        C_SpriteSheet* sheet = entityManager->GetComponent<C_SpriteSheet>(itr, Component::SpriteSheet);
        C_State* state = entityManager->GetComponent<C_State>(itr, Component::State);
        sheet->GetSpriteSheet()->Update(l_dT);
        if (sheet->GetSpriteSheet()->GetCurrentAnimation()->GetName() == "Attack") {
            if (!sheet->GetSpriteSheet()->GetCurrentAnimation()->IsPlaying()) {
                Message msg((MessageType)EntityMessage::Switch_State);
                msg.m_receiver = itr;
                msg.m_int = (int)EntityState::Idle;
                m_systemManager->GetMessageHandler()->Dispatch(msg);
            }
        }
        if (sheet->GetSpriteSheet()->GetCurrentAnimation()->GetName() == "Death" &&
            !sheet->GetSpriteSheet()->GetCurrentAnimation()->IsPlaying()) {
            Message msg((MessageType)EntityMessage::Die);
            msg.m_receiver = itr;
            m_systemManager->GetMessageHandler()->Dispatch(msg);
        }
        if (sheet->GetSpriteSheet()->GetCurrentAnimation()->CheckMoved()) {
            int frame = sheet->GetSpriteSheet()->GetCurrentAnimation()->GetFrame();
            Message msg((MessageType)EntityMessage::Frame_Change);
            msg.m_receiver = itr;
            msg.m_int = frame;
            m_systemManager->GetMessageHandler()->Dispatch(msg);
        }
    }
}

void S_SheetAnimation::HandleEvent(const EntityId& l_entity, const EntityEvent& l_event) {
    if (!HasEntity(l_entity)) return;
    EntityManager* entityManager = m_systemManager->GetEntityManager();
    C_SpriteSheet* sheet = entityManager->GetComponent<C_SpriteSheet>(l_entity, Component::SpriteSheet);
    if (sheet->GetSpriteSheet()->GetCurrentAnimation()->GetName() != "Walk") return;
    sheet->GetSpriteSheet()->GetCurrentAnimation()->SetFrame(0);
}

void S_SheetAnimation::Notify(const Message& l_msg) {
    if (!HasEntity(l_msg.m_receiver)) return;
    EntityMessage m = (EntityMessage)l_msg.m_type;
    switch (m) {
    case EntityMessage::State_Changed:
        EntityState s = (EntityState)l_msg.m_int;
        switch (s) {
        case EntityState::Idle:
            ChangeAnimation(l_msg.m_receiver, "Idle", true, true);
            break;
        case EntityState::Walking:
            ChangeAnimation(l_msg.m_receiver, "Walk", true, true);
            break;
        case EntityState::Dying:
            ChangeAnimation(l_msg.m_receiver, "Death", true, false);
            break;
        case EntityState::Attacking:
            ChangeAnimation(l_msg.m_receiver, "Attack", true, false);
            break;
        case EntityState::Hurt:
            ChangeAnimation(l_msg.m_receiver, "Hurt", true, false);
            break;
        }
        break;
    }
}

void S_SheetAnimation::ChangeAnimation(const EntityId& l_entity, const std::string& l_anim, bool l_play, bool l_loop) {
    C_SpriteSheet* sheet = m_systemManager->GetEntityManager()->GetComponent<C_SpriteSheet>(l_entity, Component::SpriteSheet);
    sheet->GetSpriteSheet()->SetAnimation(l_anim, l_loop, l_play);
}