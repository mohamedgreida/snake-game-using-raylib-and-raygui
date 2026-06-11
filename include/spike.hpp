#pragma once
#include "snake.hpp"
class Spike{
    private:
        Texture2D spritesheet;
        Rectangle Frame;
        int currentframe;
        const int maxframes = 6;
        const float frameTimerDuration  = 0.1f;
        float frameTimer;
        Texture2D LoadMyImage();

    public:
        Coordinate pos;
        
        Spike();
        ~Spike();
        void setPos(const std::vector<Spike*>& existingSpikes);
        void Draw();
        void UpdateFrame();
};