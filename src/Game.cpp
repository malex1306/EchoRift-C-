#include "../include/Game.h"
#include "../include/Player.h"

Game::Game() : player({ 400.0f, 300.0f}, 300.0f) {
    InitWindow(screenWidth, screenHeight, "Echo Rift");
    SetTargetFPS(60);
    camera.target = { 400.0f, 300.0f};
    camera.offset = { screenWidth / 2.0f, screenHeight / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
    currentState = TITLE;
    health = 100;
}

Game::~Game() {
    CloseWindow();
}

void Game::run() {
    while (!WindowShouldClose()) {
        update();
        draw();
    }
}

//update
void Game::update() {
    switch (currentState) {
        case TITLE: {
            Vector2 mousePos = GetMousePosition();
            Rectangle startButton = { screenWidth / 2.0f - 100, screenHeight / 2.0f, 200, 50 };
            if (CheckCollisionPointRec(mousePos, startButton)) {
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    currentState = GAMEPLAY;
                }
            }
            if (IsKeyPressed(KEY_ENTER)) currentState = GAMEPLAY;
            break;
        }
        case GAMEPLAY:
            player.update(GetFrameTime());
            camera.target = player.getPosition();
            if (cityTitleTimer > 0) {
                cityTitleTimer -= GetFrameTime();
            }
            if (IsKeyPressed(KEY_G)) currentState = GAMEOVER;
            break;
        case GAMEOVER:
            if (IsKeyPressed(KEY_R)) {
                health = 0;
                currentState = TITLE;
            }
            break;
    }
}

//draw
void Game::draw() {
    BeginDrawing();
    ClearBackground(BLACK);

    switch (currentState) {
        case TITLE:
        {
            Rectangle startButton = { screenWidth / 2.0f - 100, screenHeight / 2.0f, 200, 50 };
            Vector2 mousePos = GetMousePosition();
            bool isHovering = CheckCollisionPointRec(mousePos, startButton);

            // 2. Optic hover effect
            Color btnColor = isHovering ? GRAY : DARKGRAY;

            DrawRectangleRec(startButton, btnColor);
            DrawRectangleLinesEx(startButton, 2, GOLD); // Goldener Rahmen

            // 3. center button text
            int textWidth = MeasureText("START", 20);
            DrawText("START", startButton.x + (startButton.width/2 - textWidth/2), startButton.y + 15, 20, RAYWHITE);

            // Title above
            DrawText("Echo Rift", screenWidth/2 - MeasureText("Echo Rift", 40)/2, screenHeight/2 - 100, 40, GOLD);
            break;
        }
        case GAMEPLAY:
            BeginMode2D(camera);
            DrawRectangle(100, 100, 50, 50, GREEN);
            player.draw();
            EndMode2D();
            if (cityTitleTimer > 0) {
                int textWidth = MeasureText("Echo City", 30);
                DrawText("Echo City", (screenWidth / 2) - (textWidth / 2), 100, 30, VIOLET);
            }
            break;
        case GAMEOVER:
            DrawText("Game Over", 300, 250, 40, RED);
            DrawText("Drücke R um den letzeten Speicherpunkt zu laden", 280, 320, 20, GRAY);
    }

    EndDrawing();
}