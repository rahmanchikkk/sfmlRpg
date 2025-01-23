#pragma once
#include <string>

using Frame = unsigned int;

class SpriteSheet;

class AnimBase {
    friend class SpriteSheet;
public:
    AnimBase();
    ~AnimBase();

    SpriteSheet* GetSpriteSheet();
    Frame GetFrame();
    Frame GetStartFrame();
    Frame GetEndFrame();
    unsigned int GetFrameRow();
    int GetActionStart();
    int GetActionEnd();
    float GetElapsed();
    float GetFrameTime();
    const std::string& GetName();
    bool IsLooping();
    bool IsPlaying();
    bool IsInAction();
    bool CheckMoved();

    void SetSpriteSheet(SpriteSheet* l_spriteSheet);
    bool SetFrame(Frame l_frame);
    void SetStartFrame(Frame l_frame);
    void SetEndFrame(Frame l_frame);
    void SetFrameRow(Frame l_frame);
    void SetActionStart(int l_frame);
    void SetActionEnd(int l_frame);
    void SetFrameTime(float l_time);
    void SetName(const std::string& l_name);
    void SetLooping(bool l_loop);

    void Play();
    void Pause();
    void Reset();
    void Stop();

    virtual void Update(float l_dT);

    friend std::stringstream& operator >>(std::stringstream& l_stream, AnimBase& b) {
        b.ReadIn(l_stream);
        return l_stream;
    };
protected:
    virtual void CropSprite() = 0;
    virtual void FrameStep() = 0;
    virtual void ReadIn(std::stringstream& l_stream) = 0;

    Frame m_frameCurrent;
    Frame m_frameStart;
    Frame m_frameEnd;
    Frame m_frameRow;
    int m_frameActionStart;
    int m_frameActionEnd;
    float m_frameTime;
    float m_elapsedTime;
    std::string m_name;
    SpriteSheet* m_spriteSheet;
    bool m_loop;
    bool m_playing;
    bool m_hasMoved; 
};