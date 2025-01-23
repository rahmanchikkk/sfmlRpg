#pragma once
#include "S_Base.h"
#include "C_SoundEmitter.h"
#include "C_SoundListener.h"
#include "C_State.h"
#include "C_Position.h"
#include "Directions.h"

class S_Sound : public S_Base {
public:
    S_Sound(SystemManager* l_sysMgr);
    ~S_Sound();

    void Update(float l_dT);
    void HandleEvent(const EntityId& l_entity, const EntityEvent& l_event);
    void Notify(const Message& l_msg);

    void Setup(AudioManager* l_audioMgr, SoundManager* l_soundMgr);
private:
    sf::Vector3f MakeSoundPosition(const sf::Vector2f& l_position, unsigned int l_elevation);
    void EmitSound(const EntityId& l_entity, const EntitySound& l_sound, bool l_useId, bool l_relative, int l_checkFrame = -1);

    AudioManager* m_audioManager;
    SoundManager* m_soundManager;
};