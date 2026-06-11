#include "game.hpp"
#include <iostream>
#include <cmath>
#include <cstring> 

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

// defines the externs declared in snake.hpp
float cellSize = 0;
float timer = moveTimeDuration;
float cooldown = 0;
float cooldownpause = 0;
float cooldownDuration = 0.2;
std::vector<Coordinate> SnakePos;
Coordinate fruitpos(8, 6);
Coordinate Dir(0, 0, 90);

Game::Game() {
    score = 0;
    highest = "-";
    running = true;
    gameover = false;
    paused = true;
    firstrun = true;
    keypressed = false;
    isDraggingWindow = false;
    dragOffset = { 0.0f, 0.0f };
    difficulty = 1;
    difficultyselect = false;
}

void Game::SetWindow() {
    SetConfigFlags(FLAG_WINDOW_UNDECORATED);
    SetConfigFlags(FLAG_WINDOW_TRANSPARENT);
    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(120 * cellCountX, (cellCountY + tableyoffset) * 120, "Game View");

    //icon of game in taskbar
    Image icon = LoadImage(RESOURCES_PATH "Apple.png"); 
    SetWindowIcon(icon);
    UnloadImage(icon);

    //so that it works with any device
    int currentMonitor = GetCurrentMonitor(); 
    int monitorWidth = GetMonitorWidth(currentMonitor);
    int monitorHeight = GetMonitorHeight(currentMonitor);
    int windowHeight = monitorHeight * 0.75f;
    cellSize = (float)windowHeight / cellCountY; 
    int windowWidth = cellCountX * cellSize;
    int windowX = (monitorWidth - cellSize * cellCountX) / 2;
    int windowY = (monitorHeight - (cellCountY + tableyoffset + 1) * cellSize) / 2;

    SetWindowSize(cellSize * cellCountX, (cellCountY + tableyoffset) * cellSize);
    SetWindowPosition(windowX, windowY);
}

Texture2D Game::LoadMyImage(const char* path, const float& width, const float& height) {
    Image i = LoadImage(TextFormat("%s%s", RESOURCES_PATH, path));
    ImageResizeNN(&i, width, height);
    Texture2D t = LoadTextureFromImage(i);
    SetTextureFilter(t, TEXTURE_FILTER_POINT);
    UnloadImage(i);
    return t;
}

void Game::setFood() {
    while (true) {
        fruitpos.x = GetRandomValue(0, cellCountX - 1);
        fruitpos.y = GetRandomValue(0, cellCountY - 1);
        bool collision = false;
        for (auto& i : SnakePos) {
            if (fruitpos == i) {
                collision = true;
            }
        }
        if (!collision) break;
    }
}

void Game::setHighScore(std::string& h, int& score) {
    if (h == "-" || atoi(h.c_str()) < score) {
        h = std::to_string(score);
    }
}

int Game::DrawGameOverlayMenu(const char* title, const char* subtext, Color titleColor, const char* leftBtnText, const char* rightBtnText) {
    GuiSetStyle(DEFAULT, BASE_COLOR_NORMAL, ColorToInt({ 2, 62, 138, 255 }));   
    GuiSetStyle(DEFAULT, BORDER_COLOR_NORMAL, ColorToInt(SKYBLUE));
    GuiSetStyle(DEFAULT, BORDER_WIDTH, 2);
    GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, ColorToInt(WHITE));
    GuiSetStyle(DEFAULT, BASE_COLOR_FOCUSED, ColorToInt(SKYBLUE));
    GuiSetStyle(DEFAULT, TEXT_COLOR_FOCUSED, ColorToInt({ 2, 62, 138, 255 }));

    float screenCenterX = (cellCountX * cellSize) / 2.0f;
    float screenCenterY = ((cellCountY + tableyoffset) * cellSize) / 2.0f;

    int titleFontSize = 80;
    int textWidth = MeasureText(title, titleFontSize);
    float textX = screenCenterX - (textWidth / 2.0f);
    float textY = screenCenterY - 120;

    DrawText(title, textX + 3, textY + 3, titleFontSize, BLACK); 
    DrawText(title, textX, textY, titleFontSize, titleColor);     

    int subFontSize = 32;
    int subTextWidth = MeasureText(subtext, subFontSize);
    DrawText(subtext, screenCenterX - (subTextWidth / 2.0f), textY + 80, subFontSize, WHITE);

    float btnWidth = 220;
    float btnHeight = 60;
    float btnGap = 40; 
    float btnY = textY + 140;

    Rectangle leftBtnRect  = { screenCenterX - btnWidth - (btnGap / 2), btnY, btnWidth, btnHeight };
    Rectangle rightBtnRect = { screenCenterX + (btnGap / 2), btnY, btnWidth, btnHeight };

    GuiSetStyle(DEFAULT, TEXT_SIZE, 24);
    bool leftPressed = GuiButton(leftBtnRect, leftBtnText);
    bool rightPressed = GuiButton(rightBtnRect, rightBtnText);

    if (leftPressed || rightPressed) {
        GuiLoadStyleDefault(); 
        if (leftPressed) return 1;
        if (rightPressed) return 2;
    }
    return 0; 
}


int Game::DrawGameOverlayMenu(const char* title, const char* subtext, Color titleColor, const char* leftBtnText, const char* middleBtnText, const char* rightBtnText) {
    float screenCenterX = (cellCountX * cellSize) / 2.0f;
    float screenCenterY = ((cellCountY + tableyoffset) * cellSize) / 2.0f;

    int titleFontSize = 80;
    int textWidth = MeasureText(title, titleFontSize);
    float textX = screenCenterX - (textWidth / 2.0f);
    float textY = screenCenterY - 120;

    DrawText(title, textX + 3, textY + 3, titleFontSize, BLACK); 
    DrawText(title, textX, textY, titleFontSize, titleColor);     

    int subFontSize = 32;
    int subTextWidth = MeasureText(subtext, subFontSize);
    DrawText(subtext, screenCenterX - (subTextWidth / 2.0f), textY + 80, subFontSize, WHITE);

    float btnWidth = 220;
    float btnHeight = 60;
    float btnGap = 40; 
    float btnY = textY + 140;

    Rectangle leftBtnRect   = { screenCenterX - (btnWidth * 1.5f) - btnGap, btnY, btnWidth, btnHeight };
    Rectangle middleBtnRect = { screenCenterX - (btnWidth / 2.0f),          btnY, btnWidth, btnHeight };
    Rectangle rightBtnRect  = { screenCenterX + (btnWidth / 2.0f) + btnGap,  btnY, btnWidth, btnHeight };

    GuiSetStyle(DEFAULT, BORDER_WIDTH, 2);
    GuiSetStyle(DEFAULT, TEXT_SIZE, 24);
    GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, ColorToInt(WHITE));

    bool leftPressed = false;
    bool middlePressed = false;
    bool rightPressed = false;

    // Pointer to hold our dynamic description text
    const char* hoverDescription = nullptr;
    Color descriptionColor = WHITE;

    if (strcmp(leftBtnText, "EASY") == 0) {
        GuiSetStyle(DEFAULT, BASE_COLOR_NORMAL, ColorToInt(LIME));
        GuiSetStyle(DEFAULT, BORDER_COLOR_NORMAL, ColorToInt(GREEN));
        GuiSetStyle(DEFAULT, BASE_COLOR_FOCUSED, ColorToInt(GREEN));
        GuiSetStyle(DEFAULT, TEXT_COLOR_FOCUSED, ColorToInt(WHITE));
        leftPressed = GuiButton(leftBtnRect, leftBtnText);

        GuiSetStyle(DEFAULT, BASE_COLOR_NORMAL, ColorToInt(YELLOW));
        GuiSetStyle(DEFAULT, BORDER_COLOR_NORMAL, ColorToInt(GOLD));
        GuiSetStyle(DEFAULT, BASE_COLOR_FOCUSED, ColorToInt(GOLD));
        GuiSetStyle(DEFAULT, TEXT_COLOR_FOCUSED, ColorToInt(BLACK));
        GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, ColorToInt(BLACK)); 
        middlePressed = GuiButton(middleBtnRect, middleBtnText);

        GuiSetStyle(DEFAULT, BASE_COLOR_NORMAL, ColorToInt(RED));
        GuiSetStyle(DEFAULT, BORDER_COLOR_NORMAL, ColorToInt(MAROON));
        GuiSetStyle(DEFAULT, BASE_COLOR_FOCUSED, ColorToInt(MAROON));
        GuiSetStyle(DEFAULT, TEXT_COLOR_FOCUSED, ColorToInt(WHITE));
        GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, ColorToInt(WHITE));
        rightPressed = GuiButton(rightBtnRect, rightBtnText);

        Vector2 mousePos = GetMousePosition();
        if (CheckCollisionPointRec(mousePos, leftBtnRect)) {
            hoverDescription = "Speed: Slow | Obstacles: None";
            descriptionColor = LIME;
        } 
        else if (CheckCollisionPointRec(mousePos, middleBtnRect)) {
            hoverDescription = "Speed: Normal | Obstacles: 1 Spike";
            descriptionColor = YELLOW;
        } 
        else if (CheckCollisionPointRec(mousePos, rightBtnRect)) {
            hoverDescription = "Speed: Fast | Obstacles: 3 Spikes";
            descriptionColor = RED;
        }

    } else {
        GuiSetStyle(DEFAULT, BASE_COLOR_NORMAL, ColorToInt({ 2, 62, 138, 255 }));   
        GuiSetStyle(DEFAULT, BORDER_COLOR_NORMAL, ColorToInt(SKYBLUE));
        GuiSetStyle(DEFAULT, BASE_COLOR_FOCUSED, ColorToInt(SKYBLUE));
        GuiSetStyle(DEFAULT, TEXT_COLOR_FOCUSED, ColorToInt({ 2, 62, 138, 255 }));
        GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, ColorToInt(WHITE));

        leftPressed   = GuiButton(leftBtnRect, leftBtnText);
        middlePressed = GuiButton(middleBtnRect, middleBtnText);
        rightPressed  = GuiButton(rightBtnRect, rightBtnText);
    }

    if (hoverDescription != nullptr) {
        int descFontSize = 24;
        int descWidth = MeasureText(hoverDescription, descFontSize);
        float descY = btnY + btnHeight + 30; 
        
        DrawText(hoverDescription, screenCenterX - (descWidth / 2.0f) + 2, descY + 2, descFontSize, BLACK);
        DrawText(hoverDescription, screenCenterX - (descWidth / 2.0f), descY, descFontSize, descriptionColor);
    }

    if (leftPressed || middlePressed || rightPressed) {
        GuiLoadStyleDefault(); 
        if (leftPressed)   return 1;
        if (middlePressed) return 2;
        if (rightPressed)  return 3;
    }

    return 0; 
}

void Game::Initialize() {
    SetWindow();
    //intialize strting position of snake
    SnakePos.push_back(Coordinate(2, 6, 90));
    SnakePos.push_back(Coordinate(1, 6, 90));

    //loads assets
    snakehead = LoadMyImage("Snakehead.png", cellSize, cellSize);
    sourceRec = { 0.0f, 0.0f, (float)snakehead.width, (float)snakehead.height };
    origin = { cellSize / 2.0f, cellSize / 2.0f };

    snakebody = LoadMyImage("body.png", cellSize / 2, cellSize);
    bodysourceRec = { cellSize / 4, 0.0f, (float)snakebody.width, (float)snakebody.height };

    snaketurn = LoadMyImage("turn.png", cellSize * 3 / 4, cellSize * 3 / 4);
    turnsourceRec = { 0, 0.0f, (float)snaketurn.width, (float)snaketurn.height };

    apple = LoadMyImage("Apple.png", cellSize, cellSize);
    Snow = LoadMyImage("Snow.png", cellSize, cellSize);

    SetTargetFPS(60);

    gridWidth = cellCountX * cellSize;
    gridHeight = (cellCountY - tableyoffset + 1) * cellSize;
    gridYStart = tableyoffset * cellSize;

    gridCanvas = LoadRenderTexture(gridWidth, gridHeight);
}

void Game::handleWindowDragging() {
    if(GetMouseY() > cellSize && !paused && !gameover){
        HideCursor();
    }else{
        ShowCursor();
    }
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && GetMouseY() < cellSize) {
        isDraggingWindow = true;
        dragOffset = GetMousePosition();
    }

    if (isDraggingWindow) {
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            isDraggingWindow = false;
        } else {
            Vector2 windowPos = GetWindowPosition();
            Vector2 currentMouse = GetMousePosition();
            int nextWindowX = (int)(windowPos.x + (currentMouse.x - dragOffset.x));
            int nextWindowY = (int)(windowPos.y + (currentMouse.y - dragOffset.y));
            SetWindowPosition(nextWindowX, nextWindowY);
        }
    }
}

void Game::Update() {
    handleWindowDragging();
    for(auto& spike : spikes){
        spike->UpdateFrame();
    }

    if (cooldown > 0) cooldown -= GetFrameTime();
    if (!keypressed) {
        if (GetKeyPressed() != KEY_NULL) {
            keypressed = true;
        }
    } else if (!paused) {
        if (cooldown <= 0) {
            Coordinate OldDir = Dir;
            float speed = 1;
            if ((IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) && Dir.y == 0) {
                Dir.x = 0; 
                Dir.y = -speed; 
                Dir.rotation = 0;
                cooldown = cooldownDuration;
                audio.PlaySoundEffect("Move");
            } else if ((IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) && Dir.x == 0) {
                Dir.x = -speed; 
                Dir.y = 0; 
                Dir.rotation = 270;
                cooldown = cooldownDuration;
                audio.PlaySoundEffect("Move");
            } else if ((IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) && Dir.y == 0) {
                Dir.x = 0; 
                Dir.y = speed; 
                Dir.rotation = 180;
                cooldown = cooldownDuration;
                audio.PlaySoundEffect("Move");
            } else if ((IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) && Dir.x == 0) {
                Dir.x = speed; 
                Dir.y = 0; 
                Dir.rotation = 90;
                cooldown = cooldownDuration;
                audio.PlaySoundEffect("Move");
            }
            if (OldDir.x == -Dir.x && OldDir.y == -Dir.y) {
                Dir = OldDir;
            }
            if (OldDir.rotation != Dir.rotation) {
                SnakePos[0].corner = true; 
                SnakePos[0].prev_rotation = OldDir.rotation;
                SnakePos[0].rotation = Dir.rotation;
            }
        }

        timer -= GetFrameTime();
        if (timer <= 0) {
            difficulty == 1 ? timer += moveTimeDuration : difficulty == 2 ? timer += moveTimeDurationlvl2 : timer += moveTimeDurationlvl3; 
            for (int i = SnakePos.size() - 1; i > 0; i--) {
                SnakePos[i] = SnakePos[i - 1];
            }
            SnakePos[0].x += Dir.x;
            SnakePos[0].y += Dir.y;
            SnakePos[0].corner = false; 
        }

        if (SnakePos[0] == fruitpos) {
            setFood();
            for(auto& spike : spikes){
                spike->setPos(spikes);
            }
            audio.PlaySoundEffect("Eat");
            score++;
            if (SnakePos.size() == 1) {
                SnakePos.push_back(Coordinate(SnakePos[0].x - Dir.x, SnakePos[0].y - Dir.y, SnakePos[0].rotation));
            } else {
                SnakePos.push_back(SnakePos[(SnakePos.size() - 1)]);
            }
        }
        if(difficulty >=2){
            for (auto& i : SnakePos) {
                for(auto& spike : spikes){
                    if (spike->pos.x == i.x && spike->pos.y == i.y) {
                        gameover = true;
                        break;
                    }
                }
                if(gameover) break;
            }
        }
    }

    bool isfirst = true;
    for (auto& i : SnakePos) {
        if (i.x > cellCountX - 1 || i.x < 0 || i.y > cellCountY - 1 || i.y < 0) gameover = true;
        if (SnakePos[0] == i) {
            if (isfirst) isfirst = false;
            else gameover = true;
        }
    }

    if (cooldownpause > 0) cooldownpause -= GetFrameTime();
    if (IsKeyDown(KEY_ESCAPE) && !gameover && !firstrun && cooldownpause <= 0 && !paused) {
        paused = true;
        cooldownpause = cooldownDuration;
    }

    if (gameover) {
        paused = true;
    }
}

void Game::Draw() {
    BeginDrawing();
    ClearBackground(BLANK);

    BeginTextureMode(gridCanvas);
        ClearBackground(BLANK);

        for (int i = 0; i < cellCountX; i++) {
            for (int j = tableyoffset; j <= cellCountY; j++) {
                int canvasY = j - tableyoffset; 
                if ((i + j) % 2)
                    DrawTexture(Snow, i * cellSize, canvasY * cellSize, WHITE);
                else
                    DrawRectangle(i * cellSize, canvasY * cellSize, cellSize, cellSize, Color{2, 62, 138, 255});
            }
        }

        for (int i = 0; i < SnakePos.size(); i++) {
            if (i == 0) {
                Rectangle destRec = { cellSize * SnakePos[0].x + cellSize / 2, cellSize * SnakePos[0].y + cellSize / 2, (float)snakehead.width, (float)snakehead.height };

                DrawTexturePro(snakehead, sourceRec, destRec, origin, SnakePos[0].rotation, WHITE); 
            } else {
                if (SnakePos[i].corner) {
                    Rectangle turndestRec;
                    float turnrotation = 0.0f;
                    int current_rot = SnakePos[i].rotation;
                    int prev_rot = SnakePos[i].prev_rotation;
                    int offsetx = 0, offsety = 0;

                    if ((current_rot == 90 && prev_rot == 180) || (current_rot == 0 && prev_rot == 270)) { turnrotation = 0.0f; offsetx = cellSize / 4; } 
                    else if ((current_rot == 0 && prev_rot == 90) || (current_rot == 270 && prev_rot == 180)) { turnrotation = 270.0f; offsety = -cellSize / 4; } 
                    else if ((current_rot == 180 && prev_rot == 90) || (current_rot == 270 && prev_rot == 0)) { turnrotation = 180.0f; offsetx = -cellSize / 4; } 
                    else if ((current_rot == 90 && prev_rot == 0) || (current_rot == 180 && prev_rot == 270)) { turnrotation = 90.0f; offsety = cellSize / 4; }

                    turndestRec = { cellSize * SnakePos[i].x + cellSize / 2.0f + offsetx, cellSize * SnakePos[i].y + cellSize / 2.0f + offsety, (float)snaketurn.width, (float)snaketurn.width };
                    DrawTexturePro(snaketurn, turnsourceRec, turndestRec, origin, turnrotation, WHITE);
                } else {            
                    Rectangle bodydestRec;
                    if (SnakePos[i].rotation == 0) bodydestRec = { cellSize * SnakePos[i].x + cellSize / 2 + cellSize / 4, cellSize * SnakePos[i].y + cellSize / 2, (float)snakebody.width, (float)snakebody.height};
                    else if (SnakePos[i].rotation == 90) bodydestRec = { cellSize * SnakePos[i].x + cellSize / 2, cellSize * SnakePos[i].y + cellSize / 2 + cellSize / 4, (float)snakebody.width, (float)snakebody.height};
                    else if (SnakePos[i].rotation == 180) bodydestRec = { cellSize * SnakePos[i].x + cellSize / 2 - cellSize / 4, cellSize * SnakePos[i].y + cellSize / 2, (float)snakebody.width, (float)snakebody.height};
                    else if (SnakePos[i].rotation == 270) bodydestRec = { cellSize * SnakePos[i].x + cellSize / 2, cellSize * SnakePos[i].y + cellSize / 2 - cellSize / 4, (float)snakebody.width, (float)snakebody.height};
                    
                    DrawTexturePro(snakebody, bodysourceRec, bodydestRec, origin, SnakePos[i].rotation, WHITE);
                }
            }
        }
        DrawTexture(apple, fruitpos.x * cellSize, fruitpos.y * cellSize, WHITE); 
        for(auto& spike : spikes)
            spike->Draw();

        if ((paused || gameover) && !firstrun) {
            DrawRectangle(0, 0, gridWidth, gridHeight, Fade(BLACK, 0.3f));
        }
    EndTextureMode();

    BeginBlendMode(BLEND_ALPHA);
        Rectangle gridRect = { 0, gridYStart, gridWidth, gridHeight };
        DrawRectangleRounded(gridRect, 0.05f, 16, WHITE);
    EndBlendMode(); 

    BeginBlendMode(BLEND_MULTIPLIED); 
        DrawTextureRec(gridCanvas.texture, Rectangle{ 0, 0, (float)gridCanvas.texture.width, (float)-gridCanvas.texture.height }, Vector2{ 0, gridYStart }, WHITE);
    EndBlendMode();
    
    //Score area
    std::string s = "Score: " + std::to_string(score) + "  High Score: " + highest;
    const char* str = s.c_str();
    int posX = (cellCountX / 2 - 1) * cellSize;
    int posY = cellSize / 2;
    int fontSize = 35;
    int outlineThickness = 2; 

    DrawText(str, posX - outlineThickness, posY, fontSize, BLACK);
    DrawText(str, posX + outlineThickness, posY, fontSize, BLACK);
    DrawText(str, posX, posY - outlineThickness, fontSize, BLACK);
    DrawText(str, posX, posY + outlineThickness, fontSize, BLACK);
    DrawText(str, posX, posY, fontSize, SKYBLUE);
    
    //Start game menu
    if (!gameover && firstrun && paused && !difficultyselect) {
        int choice = DrawGameOverlayMenu("SNAKE GAME", "What would you like to do?", GREEN, "START GAME","DIFFICULTY","EXIT");
        if (choice == 1) { 
            paused = false; 
            firstrun = false; 
        } 
        else if(choice == 2){
            difficultyselect = true;
            cooldownpause = cooldownDuration;
        }
        else if (choice == 3){
            running = false;
        }
    }

    //Paused game menu
    if (paused && !gameover && !firstrun) {
        int choice = DrawGameOverlayMenu("PAUSED", "Ready to continue?", SKYBLUE, "RESUME", "EXIT GAME");
        if (choice == 1 || (IsKeyPressed(KEY_ESCAPE) && cooldownpause <= 0)) {       
            paused = false;
            cooldownpause = cooldownDuration;
        } else if (choice == 2) running = false;
    }

    //Game over game menu
    if (gameover && !difficultyselect) {
        int choice = DrawGameOverlayMenu("GAME OVER", "What would you like to do?", RED, "TRY AGAIN","DIFFICULTY", "EXIT");
        if (choice == 1) {
            SnakePos.clear();
            SnakePos.push_back(Coordinate(2, 6, 90));
            SnakePos.push_back(Coordinate(1, 6, 90));
            fruitpos = Coordinate(8, 6);
            Dir.x = 0;
            Dir.y = 0;
            Dir.rotation = 90; 
            for(auto& spike : spikes){
                spike->setPos(spikes);
            }
            cooldown = 0;
            setHighScore(highest, score);
            score = 0;
            gameover = false;
            paused = false;
            keypressed = false;
        }
        else if(choice == 2){
            difficultyselect = true;
            cooldownpause = cooldownDuration;
        }
        else if (choice == 3 || IsKeyDown(KEY_ESCAPE)) {
            gameover = false;
            running = false;
        }
    }
    
    if(difficultyselect){
        int choice = DrawGameOverlayMenu("DIFFICULTY","", WHITE, "EASY","MEDIUM","HARD");
        if(choice>=1 && cooldownpause <=0){
            if(choice == 1){
                spikes.clear();
                timer = moveTimeDuration;
                cooldownDuration = moveTimeDuration;
            }
            else if(choice == 2){
                timer = moveTimeDurationlvl2;
                cooldownDuration = moveTimeDurationlvl2;
                spikes.clear();
                spikes.push_back(new Spike());
            }
            else{
                timer = moveTimeDurationlvl3;
                cooldownDuration = moveTimeDurationlvl3;
                spikes.clear();
                for (int i = 0; i < 3; ++i) {
                    spikes.push_back(new Spike());
                }
            }
            for(auto& spike : spikes){
                spike->setPos(spikes);
            }
            difficulty = choice;
            difficultyselect = false;
        }
    }

    EndDrawing();
}

void Game::Unload() {
    UnloadTexture(apple);
    UnloadTexture(snakehead);
    UnloadTexture(snakebody);
    UnloadTexture(snaketurn);
    UnloadTexture(Snow);
    UnloadRenderTexture(gridCanvas);
    CloseWindow();
}

void Game::Run() {
    Initialize();
    while (running) {
        Update();
        Draw();
    }
    Unload();
}