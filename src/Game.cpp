#include "../include/Game.h"
#include "../include/Player.h"
#include "components/UiComponents.h"

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
    Vector2 mousePos = GetMousePosition();

    switch (currentState) {
        case TITLE: {
            if (const UI::Button startBtn({ screenWidth / 2.0f - 100, screenHeight / 2.0f, 200, 50 }, "START"); startBtn.isClicked() || IsKeyPressed(KEY_ENTER)) {
                currentState = GAMEPLAY;
            }
            break;
        }
        case GAMEPLAY: {
            const Vector2 oldPos = player.getPosition();
            player.update(GetFrameTime());

            if (const Rectangle playerRect = { player.getPosition().x - 20, player.getPosition().y - 20, 40, 40 }; CheckCollisionRecs(playerRect, treeCollider)) {
                player.setPosition(oldPos);
            }

            camera.target = player.getPosition();

            if (cityTitleTimer > 0) cityTitleTimer -= GetFrameTime();

            if (IsKeyPressed(KEY_MINUS)) {
                health -= 10;
                if (health <= 0) { health = 0; currentState = GAMEOVER; }
            }
            break;
        }
        case GAMEOVER: {
            if (const UI::Button resetBtn({ screenWidth / 2.0f - 100, screenHeight / 2.0f, 200, 50 }, "RESET"); resetBtn.isClicked() || IsKeyPressed(KEY_R)) {
                resetGame();
                currentState = GAMEPLAY;
            }
            break;
        }
    }
}

//draw
void Game::draw() const {
    BeginDrawing();
    ClearBackground(BLACK);

    switch (currentState) {
        case TITLE: {
            UI::Label titleLabel = { "Echo Rift", 40, GOLD };
            titleLabel.draw(screenWidth, screenHeight / 2 - 100);

            UI::Button startBtn({ screenWidth / 2.0f - 100, screenHeight / 2.0f, 200, 50 }, "START");
            startBtn.draw();
            break;
        }
        case GAMEPLAY: {
            BeginMode2D(camera);
            DrawRectangleRec(treeCollider, GREEN);
            player.draw();
            EndMode2D();

            DrawRectangle(20, 20, 200, 25, DARKGRAY);
            DrawRectangle(20, 20, (int)((health / 100.0f) * 200.0f), 25, RED);

            if (cityTitleTimer > 0) {
                constexpr UI::Label cityLabel = { "Echo City", 30, VIOLET };
                cityLabel.draw(screenWidth, 100);
            }
            break;
        }
        case GAMEOVER: {
            constexpr UI::Label goLabel = { "GAME OVER", 40, RED };
            goLabel.draw(screenWidth, screenHeight / 2 - 100);

            const UI::Button resetBtn({ screenWidth / 2.0f - 100, screenHeight / 2.0f, 200, 50 }, "RESET");
            resetBtn.draw();
            break;
        }
    }
    EndDrawing();
}
