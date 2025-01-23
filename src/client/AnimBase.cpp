#include "AnimBase.h"
#include "SpriteSheet.h"

AnimBase::AnimBase() : m_frameCurrent(0), m_frameStart(0), m_frameEnd(0), m_frameRow(0), m_frameActionStart(-1),
    m_frameActionEnd(-1), m_elapsedTime(0.f), m_frameTime(0.f), m_loop(false), m_playing(false), m_hasMoved(false) {};

AnimBase::~AnimBase() {}

bool AnimBase::SetFrame(Frame l_frame) {
    if ((l_frame >= m_frameStart && l_frame <= m_frameEnd) || 
        (l_frame <= m_frameStart && l_frame >= m_frameEnd)) {
            m_frameCurrent = l_frame;
            m_hasMoved = true;
            return true;
        }
    return false;
}

bool AnimBase::IsInAction() {
    if (m_frameActionStart == -1 || m_frameActionEnd == -1) {
        return false;
    }
    return (m_frameCurrent >= m_frameActionStart && m_frameCurrent <= m_frameActionEnd);
}

void AnimBase::Stop() { m_playing = false; Reset(); }

bool AnimBase::CheckMoved() {
    bool result = m_hasMoved;
    m_hasMoved = false;
    return result;
}

void AnimBase::Reset() {
    m_elapsedTime = 0.f;
    m_frameCurrent = m_frameStart;
    CropSprite();
}

void AnimBase::Update(float l_dT) {
    if (!m_playing) return;
    m_elapsedTime += l_dT;
    if (m_elapsedTime < m_frameTime) return;
    FrameStep();
    CropSprite();
    m_elapsedTime = 0.f;
}

SpriteSheet* AnimBase::GetSpriteSheet() { return m_spriteSheet; };
void AnimBase::SetSpriteSheet(SpriteSheet* l_spriteSheet) { m_spriteSheet = l_spriteSheet; }

const std::string& AnimBase::GetName() { return m_name; }
void AnimBase::SetName(const std::string& l_name) { m_name = l_name; }

Frame AnimBase::GetFrame() { return m_frameCurrent; }
Frame AnimBase::GetStartFrame() { return m_frameStart; }
void AnimBase::SetStartFrame(Frame l_frame) { m_frameStart = l_frame; }

Frame AnimBase::GetEndFrame() { return m_frameEnd; }
void AnimBase::SetEndFrame(Frame l_frame) { m_frameEnd = l_frame; }

Frame AnimBase::GetFrameRow() { return m_frameRow; }
void AnimBase::SetFrameRow(Frame l_frame) { m_frameRow = l_frame; }

int AnimBase::GetActionStart() { return m_frameActionStart; }
void AnimBase::SetActionStart(int l_actionStart) { m_frameActionStart = l_actionStart; }

int AnimBase::GetActionEnd() { return m_frameActionEnd; }
void AnimBase::SetActionEnd(int l_frame) { m_frameActionEnd = l_frame; }

float AnimBase::GetElapsed() { return m_elapsedTime; }
float AnimBase::GetFrameTime() { return m_frameTime; }
void AnimBase::SetFrameTime(float l_time) { m_frameTime = l_time; }

bool AnimBase::IsLooping() { return m_loop; }
void AnimBase::SetLooping(bool l_loop) { m_loop = l_loop; }

bool AnimBase::IsPlaying() { return m_playing; }
void AnimBase::Play() { m_playing = true; }
void AnimBase::Pause() { m_playing = false; }