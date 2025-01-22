#include "SoundManager.h"
#include "StateManager.h"

SoundManager::SoundManager(AudioManager* l_audioMgr) : m_audioManager(l_audioMgr), m_elapsed(0.f), m_lastID(0), m_numSounds(0) {}

SoundManager::~SoundManager() { Cleanup(); }

void SoundManager::ChangeState(const StateType& l_state) {
    if (l_state == m_currentState) return;
    PauseAll(m_currentState);
    UnpauseAll(l_state);
    auto m = m_musics.find(l_state);
    if (m != m_musics.end()) return;
    sf::Music* music = nullptr;
    SoundInfo info("");
    m_musics.emplace(l_state, std::make_pair(info, music));
}

void SoundManager::RemoveState(const StateType& l_state) {
    auto stateSounds = m_audio.find(l_state);
    if (stateSounds == m_audio.end()) return;
    for (auto& itr : stateSounds->second) {
        RecycleSound(itr.first, itr.second.second, itr.second.first.m_sound);
        --m_numSounds; 
    }
    auto m = m_musics.find(l_state);
    if (m == m_musics.end()) return;
    if (m->second.second) {
        delete m->second.second;
        --m_numSounds;
    }
    m_musics.erase(l_state);
}

void SoundManager::Update(float l_dT) {
    auto stateSounds = m_audio.find(m_currentState);
    if (stateSounds == m_audio.end()) return;
    for (auto itr = stateSounds->second.begin(); itr != stateSounds->second.end(); ) {
        if (!itr->second.second->getStatus()) {
            RecycleSound(itr->first, itr->second.second, itr->second.first.m_sound);
            itr = stateSounds->second.erase(itr);
            --m_numSounds;
            continue;
        }
        ++itr;
    }
    auto m = m_musics.find(m_currentState);
    if (m == m_musics.end()) return;
    if (m->second.second->getStatus()) return;
    delete m->second.second;
    m->second.second = nullptr;
    --m_numSounds;
};

SoundID SoundManager::Play(const std::string& l_sound, const sf::Vector3f& l_position, bool l_loop, bool l_relative) {
    SoundProps* props = GetSoundProperties(l_sound);
    if (!props) return -1;
    SoundID id;
    sf::Sound* sound = CreateSound(id, props->m_audioName);
    if (!sound) return -1;
    SetUpSound(sound, props, l_loop, l_relative);
    sound->setPosition(l_position);
    SoundInfo info(props->m_audioName);
    m_audio[m_currentState].emplace(id, std::make_pair(info, sound));
    ++m_numSounds;
    return id;
};

bool SoundManager::Play(const SoundID& l_sound) {
    auto stateSounds = m_audio.find(m_currentState);
    if (stateSounds == m_audio.end()) return false;
    auto itr = stateSounds->second.find(l_sound);
    if (itr == stateSounds->second.end()) return false;
    itr->second.first.m_manualPaused = false;
    itr->second.second->play();
    return true;
}

bool SoundManager::Pause(const SoundID& l_sound) {
    auto stateSounds = m_audio.find(m_currentState);
    if (stateSounds == m_audio.end()) return false;
    auto itr = stateSounds->second.find(l_sound);
    if (itr == stateSounds->second.end()) return false;
    itr->second.first.m_manualPaused = true;
    itr->second.second->pause();
    return true;
}

bool SoundManager::Stop(const SoundID& l_sound) {
    auto stateSounds = m_audio.find(m_currentState);
    if (stateSounds == m_audio.end()) return false;
    auto itr = stateSounds->second.find(l_sound);
    if (itr == stateSounds->second.end()) return false;
    itr->second.first.m_manualPaused = true;
    itr->second.second->stop();
    return true;
}

bool SoundManager::PlayMusic(const std::string& l_sound, float l_volume, bool l_loop) {
    auto s = m_musics.find(m_currentState);
    if (s == m_musics.end()) return false;
    std::string path = m_audioManager->GetPath(l_sound);
    if (path == "") return false;
    if (!s->second.second) {
        s->second.second = new sf::Music();
        ++m_numSounds;
    }
    sf::Music* music = s->second.second;
    if (!music->openFromFile(Utils::GetWorkingDirectory() + path)) {
        std::cout << "Failed to open music from file: " << path << std::endl;
        delete s->second.second;
        s->second.second = nullptr;
        --m_numSounds;
        return false;
    }
    music->setLoop(l_loop);
    music->setRelativeToListener(false);
    music->setVolume(l_volume);
    music->play();
    s->second.first.m_sound = l_sound;
    return true;
}

bool SoundManager::PlayMusic(const StateType& l_state) {
    auto m = m_musics.find(l_state);
    if (m == m_musics.end()) return false;
    if (!m->second.second) return false;
    m->second.second->play();
    m->second.first.m_manualPaused = false;
    return true;
}

bool SoundManager::PauseMusic(const StateType& l_state) {
    auto m = m_musics.find(l_state);
    if (m == m_musics.end()) return false;
    if (!m->second.second) return false;
    m->second.second->pause();
    m->second.first.m_manualPaused = true;
    return true;
}

bool SoundManager::StopMusic(const StateType& l_state) {
    auto m = m_musics.find(l_state);
    if (m == m_musics.end()) return false;
    if (!m->second.second) return false;
    m->second.second->stop();
    delete m->second.second;
    m->second.second = nullptr;
    --m_numSounds;
    return true;
}

bool SoundManager::SetPosition(const SoundID& l_sound, const sf::Vector3f& l_position) {
    auto stateSounds = m_audio.find(m_currentState);
    if (stateSounds == m_audio.end()) return false;
    auto itr = stateSounds->second.find(l_sound);
    if (itr == stateSounds->second.end()) return false;
    itr->second.second->setPosition(l_position);
    return true;
}

bool SoundManager::IsPlaying(const SoundID& l_sound) {
    auto stateSounds = m_audio.find(m_currentState);
    if (stateSounds == m_audio.end()) return false;
    auto itr = stateSounds->second.find(l_sound);
    if (stateSounds == m_audio.end()) return false;
    return itr->second.second->getStatus();
}

SoundProps* SoundManager::GetSoundProperties(const std::string& l_sound) {
    auto itr = m_properties.find(l_sound);
    if (itr == m_properties.end()) return nullptr;
    return &itr->second;
}

bool SoundManager::LoadProperties(const std::string& l_propsFile) {
    std::ifstream file;
    file.open(Utils::GetWorkingDirectory() + l_propsFile);
    if (!file.is_open()) {
        std::cout << "Failed to load sound properties from file: " << l_propsFile << std::endl;
        return false;
    }
    std::string line;
    SoundProps props("");
    while (std::getline(file, line)) {
        if (line[0] == '|') continue;
        std::stringstream keystream(line);
        std::string type;
        keystream >> type;
        if (type == "AudioName") {
            keystream >> props.m_audioName;
        } else if (type == "Volume") {
            keystream >> props.m_volume;
        } else if (type == "Pitch") {
            keystream >> props.m_pitch;
        } else if (type == "Attenuation") {
            keystream >> props.m_attenuation;
        } else if (type == "MinDistance") {
            keystream >> props.m_minDistance;
        } else {
            //
        }
    }
    file.close();
    if (props.m_audioName == "") return false;
    m_properties.emplace(props.m_audioName, props);
    return true;
}

void SoundManager::PauseAll(const StateType& l_state) {
    auto stateSounds = m_audio.find(l_state);
    if (stateSounds == m_audio.end()) return;
    for (auto itr = stateSounds->second.begin(); itr != stateSounds->second.end(); ) {
        if (!itr->second.second->getStatus()) {
            RecycleSound(itr->first, itr->second.second, itr->second.first.m_sound);
            itr = stateSounds->second.erase(itr);
            continue;
        }
        itr->second.second->pause();
        ++itr;
    }
    auto m = m_musics.find(l_state);
    if (m == m_musics.end()) return;
    if (!m->second.second) return;
    m->second.second->pause();
}

void SoundManager::UnpauseAll(const StateType& l_state) {
    auto stateSounds = m_audio.find(l_state);
    if (stateSounds == m_audio.end()) return;
    for (auto& itr : stateSounds->second) {
        if (itr.second.first.m_manualPaused) continue;
        itr.second.second->play();
    }
    auto m = m_musics.find(l_state);
    if (m == m_musics.end()) return;
    if (!m->second.second || m->second.first.m_manualPaused) return;
    m->second.second->play();
}

sf::Sound* SoundManager::CreateSound(SoundID& l_sound, const std::string& l_name) {
    sf::Sound* sound = nullptr;
    if (!m_recycled.empty() && (m_numSounds >= Max_Sounds || m_recycled.size() >= Sound_Cache)) {
        auto itr = m_recycled.begin();
        while (itr != m_recycled.end()) {
            if (itr->first.second == l_sound) break;
            ++itr;
        }
        if (itr == m_recycled.end()) {
            auto element = m_recycled.begin();
            l_sound = element->first.second;
            m_audioManager->ReleaseResource(element->first.first);
            m_audioManager->RequireResource(l_name);
            sound = element->second;
            sound->setBuffer(*m_audioManager->GetResource(l_name));
            m_recycled.erase(element);
        } else {
            l_sound = itr->first.second;
            sound = itr->second;
            m_recycled.erase(itr);
        }
        return sound;
    }
    if (m_numSounds < Max_Sounds) {
        if (m_audioManager->RequireResource(l_name)) {
            sound = new sf::Sound();
            l_sound = m_lastID;
            ++m_lastID;
            ++m_numSounds;
            sound->setBuffer(*m_audioManager->GetResource(l_name));
            return sound;
        }
    }
    std::cout << "Failed to create sound: " << l_name << std::endl;
    return nullptr;
}

void SoundManager::SetUpSound(sf::Sound* l_sound, const SoundProps* l_props, bool l_loop, bool l_relative) {
    l_sound->setLoop(l_loop);
    l_sound->setRelativeToListener(l_relative);
    l_sound->setVolume(l_props->m_volume);
    l_sound->setAttenuation(l_props->m_attenuation);
    l_sound->setMinDistance(l_props->m_minDistance);
    l_sound->setPitch(l_props->m_pitch);
}

void SoundManager::RecycleSound(const SoundID& l_id, sf::Sound* l_sound, const std::string& l_name) {
    m_recycled.emplace_back(std::make_pair(std::make_pair(l_name, l_id), l_sound));
}

void SoundManager::Cleanup() {
    for (auto& state : m_audio) {
        for (auto& itr : state.second) {
            m_audioManager->ReleaseResource(itr.second.first.m_sound);
            delete itr.second.second;
        }
    }
    m_audio.clear();
    for (auto& state : m_musics) {
        if (state.second.second) {
            delete state.second.second;
        }
    }
    m_musics.clear();
    m_properties.clear();
    m_numSounds = 0;
    m_lastID = 0;
}