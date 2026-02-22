#include "../include/Game.h"
#include "../include/Player.h"
#include "components/UiComponents.h"

Game::Game() : player({ 400.0f, 300.0f}, 300.0f) {
    InitWindow(screenWidth, screenHeight, "Echo Rift");
    SetTargetFPS(60);
    initCityLevel();
    camera.target = { 400.0f, 300.0f};
    camera.offset = { screenWidth / 2.0f, screenHeight / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
    currentState = TITLE;
    health = 100;
}

Game::~Game() {
    if (activeItem != nullptr) {
        delete activeItem;
        activeItem = nullptr;
    }
    CloseWindow();
}

void Game::run() {
    while (!WindowShouldClose()) {
        update();
        draw();
    }
}

//reset
void Game::resetGame() {
    health = 100;
    cityTitleTimer = 5.0f;
    player.setPosition({ 400.0f, 300.0f});

    if (activeItem != nullptr) {
        delete activeItem;
        activeItem = nullptr;
    }
}

void Game::initCityLevel() {
    cityBlocks.push_back({ 200, 200, 300, 400 });
    cityBlocks.push_back({ 700, 100, 200, 200 });
    cityBlocks.push_back({ 1200, 600, 400, 300 });
}

//update
void Game::update() {
    Vector2 mousePos = GetMousePosition();

    switch (currentState) {
        case TITLE: {
            if (const UI::Button startBtn({ screenWidth / 2.0f - 100, screenHeight / 2.0f, 200, 50 }, "START"); startBtn.isClicked() || IsKeyPressed(KEY_ENTER)) {
                currentState = GAMEPLAY;
            }
        }
        case GAMEPLAY: {
    const Vector2 oldPos = player.getPosition();
    player.update(GetFrameTime());
    Vector2 currentPos = player.getPosition();

    //change room logic
    if (roomX == 0 && currentPos.x > worldSize) {
        roomX = 1;
        currentPos.x = 50.0f;
        player.setPosition(currentPos);
    } else if (roomX == 1 && currentPos.x < 0) {
        roomX = 0;
        currentPos.x = worldSize - 50.0f;
        player.setPosition(currentPos);
    }

    if (roomX == 1) {
        const Rectangle playerRect = { player.getPosition().x - 20, player.getPosition().y - 20, 40, 40 };
        for (const auto& block : cityBlocks) {
            if (CheckCollisionRecs(playerRect, block)) {
                player.setPosition(oldPos);
                break;
            }
        }
    }

    currentPos = player.getPosition();
    if (currentPos.y < 0) currentPos.y = 0;
    if (currentPos.y > worldSize) currentPos.y = worldSize;
    if (roomX == 0 && currentPos.x < 0) currentPos.x = 0;
    if (roomX == 1 && currentPos.x > worldSize) currentPos.x = worldSize;
    player.setPosition(currentPos);

    if (roomX == 0) {
        const Rectangle playerRect = { player.getPosition().x - 20, player.getPosition().y - 20, 40, 40 };
        if (CheckCollisionRecs(playerRect, treeCollider)) {
            player.setPosition(oldPos);
        }
    }

    camera.target = player.getPosition();
    if (cityTitleTimer > 0) cityTitleTimer -= GetFrameTime();

    if (IsKeyPressed(KEY_MINUS)) {
        health -= 10;
        if (health <= 0) { health = 0; currentState = GAMEOVER; }
    }

    if (IsKeyPressed(KEY_I) && activeItem == nullptr) {
        activeItem = new Rectangle{ 600, 400, 40, 40 };
    }

    // pointer logic: take item
    if (activeItem != nullptr) {
        const Rectangle playerRect = { player.getPosition().x - 20, player.getPosition().y - 20, 40, 40 };
        if (CheckCollisionRecs(playerRect, *activeItem)) {
            delete activeItem;
            activeItem = nullptr;
            health = 100;
        }
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
            int tileSize = 64;

            for (int x = 0; x < 32; x++) {
                for (int y = 0; y < 32; y++) {
                    if (roomX == 0) {
                        const Color groundColor = ((x + y) % 2 == 0) ? Color{ 34, 139, 34, 255} : Color{ 50, 205, 50, 255 };
                        DrawRectangle(x * tileSize, y * tileSize, tileSize, tileSize, groundColor);
                    } else {
                        const Color betonColor = ((x + y) % 2 == 0) ? GRAY : DARKGRAY;
                        DrawRectangle(x * tileSize, y * tileSize, tileSize, tileSize, betonColor);
                        DrawRectangleLines(x * tileSize, y * tileSize, tileSize, tileSize,LIGHTGRAY);
                    }
                }
            }
            if (roomX == 0) {
                DrawRectangleRec(treeCollider, GREEN);
            }
            if (roomX == 1) {
                for (const auto& block : cityBlocks) {
                    DrawRectangleRec(block, DARKBLUE);
                    DrawRectangleLinesEx(block, 3, SKYBLUE);
                }
            }
            if (activeItem != nullptr) {
                DrawRectangleRec(*activeItem, GOLD);
            }
            player.draw();
            EndMode2D();
            DrawRectangle(10, 10, 250, 80, Fade(BLACK, 0.6f));
            DrawText(TextFormat("Raum: [%d, %d]", roomX, roomY), 20, 20, 20, WHITE);
            DrawText(TextFormat("Player X: %.0f Y: %.0f", player.getPosition().x, player.getPosition().y), 20, 45, 20, GREEN);
            DrawText(TextFormat("Mouse World: %.0f %.0f", GetScreenToWorld2D(GetMousePosition(), camera).x, GetScreenToWorld2D(GetMousePosition(), camera).y), 20, 70, 20, YELLOW);

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