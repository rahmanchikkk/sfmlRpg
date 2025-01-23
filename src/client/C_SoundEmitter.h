#pragma once
#include "C_Base.h"
#include "SoundManager.h"
#include <array>

enum class EntitySound { None = -1, Footstep, Attack, Hurt, Death };

struct SoundParameters {
    static const int Max_SoundFrames = 5;
    SoundParameters() {
        for (int i = 0; i < Max_SoundFrames; ++i) {
            m_frames[i] = -1;
        }
    };

    std::array<int, Max_SoundFrames> m_frames;
    std::string m_sound;
};

class C_SoundEmitter : public C_Base {
public:
    static const int Max_EntitySounds = 4;
    C_SoundEmitter() : C_Base(Component::SoundEmitter), m_sound(-1) {};

    void ReadIn(std::stringstream& l_stream) {
        std::string mainDelimeter = ":";
        std::string frameDelimeter = ",";
        for (int i = 0; i < Max_EntitySounds; ++i) {
            std::string chunk;
            l_stream >> chunk;
            if (chunk == "") break;
            std::string sound = chunk.substr(0, chunk.find(mainDelimeter));
            std::string frames = chunk.substr(chunk.find(mainDelimeter) + mainDelimeter.length());
            m_params[i].m_sound = sound;
            size_t pos = 0;
            unsigned int frameNum = 0;
            while (frameNum < SoundParameters::Max_SoundFrames) {
                pos = frames.find(frameDelimeter);
                int frame = -1;
                if (pos != std::string::npos) {
                    frame = stoi(frames.substr(0, pos));
                    frames.erase(0, pos + frameDelimeter.length());
                } else {
                    frame = stoi(frames);
                    m_params[i].m_frames[frameNum] = frame;
                    break;
                }
                m_params[i].m_frames[frameNum] = frame;
                ++frameNum;
            }
        }
    };

    SoundID GetSoundID() { return m_sound; };
    void SetSoundID(const SoundID& l_sound) { m_sound = l_sound; };

    const std::string& GetSound(const EntitySound& l_sound) {
        static const std::string empty = "";
        return ((int)l_sound < Max_EntitySounds ? m_params[(int)l_sound].m_sound : empty);
    };

    bool IsSoundParameter(const EntitySound& l_sound, unsigned int l_frame) {
        if ((int)l_sound >= Max_EntitySounds) return false;
        for (int i = 0; i < SoundParameters::Max_SoundFrames; ++i) {
            if (m_params[(int)l_sound].m_frames[i] == -1) return false;
            if (m_params[(int)l_sound].m_frames[i] == l_frame) return true; 
        }
        return false;
    };

    SoundParameters* GetParameters() { return &m_params[0]; };
private:
    std::array<SoundParameters, Max_EntitySounds> m_params;
    SoundID m_sound;
};