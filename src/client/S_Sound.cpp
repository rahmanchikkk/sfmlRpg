#include "S_Sound.h"
#include "SystemManager.h"
#include "Map.h"

S_Sound::S_Sound(SystemManager* l_sysMgr) : S_Base(System::Sound, l_sysMgr), m_audioManager(nullptr), m_soundManager(nullptr) {
    Bitmask req;
    req.TurnOnBit((unsigned int)Component::State);
    req.TurnOnBit((unsigned int)Component::Position);
    req.TurnOnBit((unsigned int)Component::SoundEmitter);
    m_requiredComponents.emplace_back(req);
    req.ClearBit((unsigned int)Component::SoundEmitter);
    req.TurnOnBit((unsigned int)Component::SoundListener);
    m_requiredComponents.emplace_back(req);

    m_systemManager->GetMessageHandler()->Subscribe(EntityMessage::Direction_Changed, this);
    m_systemManager->GetMessageHandler()->Subscribe(EntityMessage::Frame_Change, this);
}

S_Sound::~S_Sound() {}

void S_Sound::Update(float l_dT) {
    EntityManager* entityManager = m_systemManager->GetEntityManager();
    for (auto& itr : m_entities) {
        C_Position* pos = entityManager->GetComponent<C_Position>(itr, Component::Position);
        sf::Vector2f position = pos->GetPosition();
        unsigned int elevation = pos->GetElevation();
        bool isListener = entityManager->HasComponent(itr, Component::SoundListener);
        if (isListener) sf::Listener::setPosition(MakeSoundPosition(position, elevation));
        C_SoundEmitter* emitter = entityManager->GetComponent<C_SoundEmitter>(itr, Component::SoundEmitter);
        if (!emitter) continue;
        if (emitter->GetSoundID() == -1) continue;
        if (!isListener) {
            if (!m_soundManager->SetPosition(emitter->GetSoundID(), MakeSoundPosition(position, elevation))) {
                emitter->SetSoundID(-1);
            }
        } else {
            if (!m_soundManager->IsPlaying(emitter->GetSoundID())) emitter->SetSoundID(-1);
        }
    }
}

void S_Sound::HandleEvent(const EntityId& l_entity, const EntityEvent& l_event) {};

void S_Sound::Notify(const Message& l_msg) {
    if (!HasEntity(l_msg.m_receiver)) return;
    EntityManager* entityManager = m_systemManager->GetEntityManager();
    bool isListener = entityManager->HasComponent(l_msg.m_receiver, Component::SoundListener);
    EntityMessage m = (EntityMessage)l_msg.m_type;
    switch (m) {
    case EntityMessage::Direction_Changed:
        if (!isListener) break;
        Direction direction = (Direction)l_msg.m_int;
        switch (direction) {
        case Direction::Up: sf::Listener::setDirection(0, 0, -1); break;
        case Direction::Down: sf::Listener::setDirection(0, 0, 1); break;
        case Direction::Left: sf::Listener::setDirection(-1, 0, 0); break;
        case Direction::Right: sf::Listener::setDirection(1, 0, 0); break;
        }
        break;
    case EntityMessage::Frame_Change:
        if (!entityManager->HasComponent(l_msg.m_receiver, Component::SoundEmitter)) break;
        EntityState state = entityManager->GetComponent<C_State>(l_msg.m_receiver, Component::State)->GetState();
        EntitySound sound = EntitySound::None;
        if (state == EntityState::Walking) sound = EntitySound::Footstep;
        else if (state == EntityState::Attacking) sound = EntitySound::Attack;
        else if (state == EntityState::Hurt) sound = EntitySound::Hurt;
        else if (state == EntityState::Dying) sound = EntitySound::Death;
        if (sound == EntitySound::None) break;
        EmitSound(l_msg.m_receiver, sound, false, isListener, l_msg.m_int);
        break;
    }
}

void S_Sound::Setup(AudioManager* l_audioMgr, SoundManager* l_soundMgr) {
    m_audioManager = l_audioMgr;
    m_soundManager = l_soundMgr;
}

sf::Vector3f S_Sound::MakeSoundPosition(const sf::Vector2f& l_position, unsigned int l_elevation) {
    return sf::Vector3f(l_position.x, l_elevation * (unsigned int)Sheet::Tile_Size, l_position.y);
}

void S_Sound::EmitSound(const EntityId& l_entity, const EntitySound& l_sound, bool l_useId, bool l_relative, int l_checkFrame) {
    if (!m_systemManager->GetEntityManager()->HasEntity(l_entity)) return;
    if (!m_systemManager->GetEntityManager()->HasComponent(l_entity, Component::SoundEmitter)) return;
    C_SoundEmitter* emitter = m_systemManager->GetEntityManager()->GetComponent<C_SoundEmitter>(l_entity, Component::SoundEmitter);
    if (emitter->GetSoundID() == -1 && l_useId) return;
    if (l_checkFrame != -1 && !emitter->IsSoundParameter(l_sound, l_checkFrame)) return;
    C_Position* pos = m_systemManager->GetEntityManager()->GetComponent<C_Position>(l_entity, Component::Position);
    sf::Vector3f position = (l_relative ? MakeSoundPosition(pos->GetPosition(), pos->GetElevation()) :
        sf::Vector3f(0.f, 0.f, 0.f));
    if (l_useId) emitter->SetSoundID(m_soundManager->Play(emitter->GetSound(l_sound), position));
    else m_soundManager->Play(emitter->GetSound(l_sound), position, false, l_relative);
}