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

void Game::resetGame() {
    health = 100;
    cityTitleTimer = 5.0f;
    player.setPosition({ 400.0f, 300.0f});
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
        case GAMEPLAY: {
            Vector2 oldPos = player.getPosition();
            player.update(GetFrameTime());
            Rectangle playerRect = { player.getPosition().x - 20, player.getPosition().y - 20, 40, 40};
            if (CheckCollisionRecs(playerRect, treeCollider)) {
                player.setPosition(oldPos);
            }
            camera.target = player.getPosition();
            if (cityTitleTimer > 0) {
                cityTitleTimer -= GetFrameTime();
            }
            if (IsKeyPressed(KEY_MINUS)) {
                health -= 10;
                if (health < 0) health = 0;
                if (health <= 0) currentState = GAMEOVER;
            }
            if (IsKeyPressed(KEY_G)) currentState = GAMEOVER;
            break;
        }
        case GAMEOVER:
            Vector2 mousePos = GetMousePosition();
            Rectangle resetButton = { screenWidth / 2.0f - 100, screenHeight / 2.0f, 200, 50 };
            if (CheckCollisionPointRec(mousePos, resetButton)) {
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    currentState = GAMEPLAY;
                    resetGame();
                }
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
        case GAMEPLAY: {
            BeginMode2D(camera);
            DrawRectangleRec(treeCollider, GREEN);
            player.draw();
            EndMode2D();
            DrawRectangle(20, 20, 200, 25, DARKGRAY);
            float healthBarWidth = (health / 100.0f)* 200.0f;
            DrawRectangle(20, 20, (int)healthBarWidth, 25, RED);
            if (cityTitleTimer > 0) {
                int textWidth = MeasureText("Echo City", 30);
                DrawText("Echo City", (screenWidth / 2) - (textWidth / 2), 100, 30, VIOLET);
            }
            break;
        }
        case GAMEOVER:
            Rectangle resetButton = { screenWidth / 2.0f - 100, screenHeight / 2.0f, 200, 50 };
            Vector2 mousePos = GetMousePosition();
            bool isHovering = CheckCollisionPointRec(mousePos, resetButton);
            Color btnColor = isHovering ? GRAY : DARKGRAY;
            DrawRectangleRec(resetButton, btnColor);
            DrawRectangleLinesEx(resetButton, 2, GOLD);
            int textWidth = MeasureText("Reset", 20);
            DrawText("Reset", resetButton.x + (resetButton.width/2 - textWidth/2), resetButton.y + 15, 20, RAYWHITE);
            DrawText("Game Over", screenWidth/2 - MeasureText("Echo Rift", 40)/2, screenHeight/2 - 100, 40, GOLD);
            break;
    }

    EndDrawing();
}