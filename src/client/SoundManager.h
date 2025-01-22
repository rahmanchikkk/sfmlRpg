#pragma once
#include <SFML/Audio.hpp>
#include "AudioManager.h"
#include "SoundProps.h"
#include "Utilities.h"
#include <unordered_map>

enum class StateType;
using SoundID = int;

struct SoundInfo {
    SoundInfo(const std::string& l_sound) : m_sound(l_sound), m_manualPaused(false) {};

    std::string m_sound;
    bool m_manualPaused;
};

using SoundProperties = std::unordered_map<std::string, SoundProps>;
using SoundContainer = std::unordered_map<SoundID, std::pair<SoundInfo, sf::Sound*>>;
using Sounds = std::unordered_map<StateType, SoundContainer>;
using MusicContainer = std::unordered_map<StateType, std::pair<SoundInfo, sf::Music*>>;
using RecycledSounds = std::vector<std::pair<std::pair<std::string, SoundID>, sf::Sound*>>;

class SoundManager {
public:
    SoundManager(AudioManager* l_audioMgr);
    ~SoundManager();

    void ChangeState(const StateType& l_state);
    void RemoveState(const StateType& l_state);

    void Update(float l_dT);
    SoundID Play(const std::string& l_sound, const sf::Vector3f& l_position, bool l_loop = false, bool l_relative = false);
    
    bool Play(const SoundID& l_sound);
    bool Pause(const SoundID& l_sound);
    bool Stop(const SoundID& l_sound);

    bool PlayMusic(const std::string& l_sound, float l_volume = 100.f, bool l_loop = false);
    bool PlayMusic(const StateType& l_state);
    bool PauseMusic(const StateType& l_state);
    bool StopMusic(const StateType& l_state);

    bool SetPosition(const SoundID& l_sound, const sf::Vector3f& l_position);
    bool IsPlaying(const SoundID& l_sound);
    SoundProps* GetSoundProperties(const std::string& l_sound);
    bool LoadProperties(const std::string& l_propsFile);

    void PauseAll(const StateType& l_state);
    void UnpauseAll(const StateType& l_state);
    sf::Sound* CreateSound(SoundID& l_sound, const std::string& l_name);

    void SetUpSound(sf::Sound* l_sound, const SoundProps* l_props, bool l_loop = false, bool l_relative = false);
    void RecycleSound(const SoundID& l_id, sf::Sound* l_sound, const std::string& l_name);
    void Cleanup();

    static const int Max_Sounds = 150;
    static const int Sound_Cache = 75;
private:
    SoundProperties m_properties;
    Sounds m_audio;
    MusicContainer m_musics;
    RecycledSounds m_recycled;

    AudioManager* m_audioManager;
    float m_elapsed;
    unsigned int m_numSounds;
    SoundID m_lastID;
    StateType m_currentState;
};