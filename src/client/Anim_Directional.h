#pragma once
#include "AnimBase.h"
#include "Directions.h"

class Anim_Directional : public AnimBase {
protected:
    void FrameStep();
    void CropSprite();
    void ReadIn(std::stringstream& l_stream);
};