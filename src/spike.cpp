#include "spike.hpp"
Spike::Spike(){
    pos = {-1,-1};
    spritesheet = LoadMyImage();
    frameTimer = frameTimerDuration;
    currentframe = 0;
    Frame = {0,0,(float)spritesheet.width/6, (float)spritesheet.height };
}
Spike::~Spike(){
    UnloadTexture(spritesheet);
}
Texture2D Spike::LoadMyImage() {
    Image i = LoadImage(RESOURCES_PATH "trap3.png");
    ImageResizeNN(&i, cellSize*6, cellSize);
    Texture2D t = LoadTextureFromImage(i);
    SetTextureFilter(t, TEXTURE_FILTER_POINT);
    UnloadImage(i);
    return t;
}
void Spike::setPos(const std::vector<Spike*>& existingSpikes){
    while (true) {
        pos.x = GetRandomValue(0, cellCountX - 1);
        pos.y = GetRandomValue(0, cellCountY - 1);
        bool collision = false;
        for (auto& i : SnakePos) {
            if (pos == i) {
                collision = true;
            }
        }
        for (const auto& otherSpike : existingSpikes) {
            if(pos == otherSpike->pos && this != otherSpike) collision =true;
        }
        if(pos.x == fruitpos.x && pos.y == fruitpos.y) collision =true;

        int safeX = SnakePos[0].x + (Dir.x);
        int safeY = SnakePos[0].y + (Dir.y);
        
        if (pos.x == safeX && pos.y == safeY) {
            collision = true;
        }

        if (!collision) break;
    }
}
void Spike::UpdateFrame(){
    if(frameTimer>0) frameTimer-=GetFrameTime();
    if(frameTimer<=0){
        Frame = {cellSize*currentframe,0,cellSize, cellSize};
        currentframe = (currentframe+1) % maxframes;
        frameTimer = frameTimerDuration;
    }
}
void Spike::Draw(){
    Vector2 screenPos = { pos.x * cellSize - cellSize/12, pos.y * cellSize };
    DrawTextureRec(spritesheet,Frame,screenPos,WHITE);
}