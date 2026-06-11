#pragma once

#include "snake.hpp"
#include "audiomanager.hpp"
#include "spike.hpp"

class Game {
private:
    AudioManager audio;
    int score;
    std::string highest;
    bool running;
    bool gameover;
    bool paused;
    bool firstrun;
    bool keypressed;
    int difficulty;
    bool difficultyselect;

    bool isDraggingWindow;
    Vector2 dragOffset;
    float gridWidth;
    float gridHeight;
    float gridYStart;

    Texture2D snakehead;
    Texture2D snakebody;
    Texture2D snaketurn;
    Texture2D apple;
    Texture2D Snow;
    std::vector<Spike*> spikes;
    RenderTexture2D gridCanvas;

    Rectangle sourceRec;
    Vector2 origin;
    Rectangle bodysourceRec;
    Rectangle turnsourceRec;

    void SetWindow();
    void setFood();
    Texture2D LoadMyImage(const char* path, const float& width, const float& height);
    void setHighScore(std::string& h, int& score);
    int DrawGameOverlayMenu(const char* title, const char* subtext, Color titleColor, const char* leftBtnText, const char* rightBtnText);
    int DrawGameOverlayMenu(const char* title, const char* subtext, Color titleColor, const char* leftBtnText, const char* middleBtnText, const char* rightBtnText);
    void handleWindowDragging();

    void Initialize();
    void Update();
    void Draw();
    void Unload();

public:
    Game();
    void Run();
};

