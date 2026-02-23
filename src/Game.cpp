#include "../include/Game.h"
#include "../include/Player.h"
#include "components/UiComponents.h"

Game::Game() : player({ 400.0f, 300.0f}, 300.0f) {
    InitWindow(screenWidth, screenHeight, "Echo Rift");
    SetTargetFPS(60);

    // Initialize world map data
    initWorld();

    // Setup camera properties
    camera.target = { 400.0f, 300.0f};
    camera.offset = { screenWidth / 2.0f, screenHeight / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    currentState = TITLE;
    health = 100;
}

Game::~Game() {
    // Clean up heap allocated memory to prevent memory leaks
    if (activeItem != nullptr) {
        delete activeItem;
        activeItem = nullptr;
    }
    CloseWindow();
}

void Game::run() {
    // Main game loop
    while (!WindowShouldClose()) {
        update();
        draw();
    }
}

void Game::resetGame() {
    // Reset player stats and position
    health = 100;
    roomTitleTimer = 5.0f;
    player.setPosition({ 400.0f, 300.0f});
    roomX = 0;
    roomY = 0;

    // Remove active item from heap if it exists during reset
    if (activeItem != nullptr) {
        delete activeItem;
        activeItem = nullptr;
    }
}

void Game::initWorld() {
    // Initialize Room [0,0]: Echo Grasslands
    LevelRoom grass;
    grass.name = "Echo Grasslands";
    grass.groundColor = Color{ 34, 139, 34, 255 };
    grass.solids.push_back({ 100.0f, 100.0f, 50.0f, 50.0f }); // Static obstacle (tree)
    worldMap[{0, 0}] = grass;

    // Initialize Room [1,0]: Beton District
    LevelRoom city;
    city.name = "Beton District";
    city.groundColor = GRAY;
    city.solids.push_back({ 200, 200, 300, 400 }); // Building 1
    city.solids.push_back({ 700, 100, 200, 200 }); // Building 2
    city.solids.push_back({ 1200, 600, 400, 300 }); // Building 3
    worldMap[{1, 0}] = city;
}

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
            Vector2 currentPos = player.getPosition();

            // Room transition logic (Teleport player to opposite side when crossing world boundaries)
            if (roomX == 0 && currentPos.x > worldSize) {
                roomX = 1;
                currentPos.x = 50.0f;
                player.setPosition(currentPos);
                roomTitleTimer = 5.0f;
            } else if (roomX == 1 && currentPos.x < 0) {
                roomX = 0;
                currentPos.x = worldSize - 50.0f;
                player.setPosition(currentPos);
                roomTitleTimer = 5.0f;
            }

            const Rectangle playerRect = { player.getPosition().x - 20, player.getPosition().y - 20, 40, 40 };

            // Dynamic collision check for the current room's static objects
            LevelRoom& current = worldMap[{roomX, roomY}];
            for (const auto& block : current.solids) {
                if (CheckCollisionRecs(block, playerRect)) {
                    player.setPosition(oldPos);
                    break;
                }
            }

            // Clamping: Prevent player from leaving world bounds in specific rooms
            currentPos = player.getPosition();
            if (currentPos.y < 0) currentPos.y = 0;
            if (currentPos.y > worldSize) currentPos.y = worldSize;
            if (roomX == 0 && currentPos.x < 0) currentPos.x = 0;
            if (roomX == 1 && currentPos.x > worldSize) currentPos.x = worldSize;
            player.setPosition(currentPos);

            camera.target = player.getPosition();

            // Handle UI timers and health deduction for testing
            if (roomTitleTimer > 0) roomTitleTimer -= GetFrameTime();
            if (IsKeyPressed(KEY_MINUS)) {
                health -= 10;
                if (health <= 0) { health = 0; currentState = GAMEOVER; }
            }

            // Heap Memory Exercise: Spawn a dynamic item (Pointer)
            if (IsKeyPressed(KEY_I) && activeItem == nullptr) {
                activeItem = new Rectangle{ 600, 400, 40, 40 };
            }

            // Handle item interaction and memory deallocation
            if (activeItem != nullptr) {
                if (CheckCollisionRecs(playerRect, *activeItem)) {
                    delete activeItem;
                    activeItem = nullptr;
                    health = 100; // Heal player as reward
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
            // Get current room data using const-safe .at()
            const LevelRoom& current = worldMap.at({roomX, roomY});

            BeginMode2D(camera);
                // Draw base ground color
                ClearBackground(current.groundColor);

                // --- Ground Design (Procedural/Grid based) ---
                if (roomX == 0) {
                    // Checkered grass pattern for Echo Grasslands
                    for (int x = 0; x < 32; x++) {
                        for (int y = 0; y < 32; y++) {
                            if ((x + y) % 2 == 0) {
                                DrawRectangle(x * 64, y * 64, 64, 64, Color{ 50, 205, 50, 255 });
                            }
                        }
                    }
                }
                else if (roomX == 1) {
                    // Industrial grid lines for Beton District
                    for (int x = 0; x < 32; x++) {
                        for (int y = 0; y < 32; y++)
                            DrawRectangleLines(x * 64, y * 64, 64, 64, Fade(LIGHTGRAY, 0.3f));
                    }
                }

                // --- Static Room Objects (Solids) ---
                for (const auto& block : current.solids) {
                    DrawRectangleRec(block, (roomX == 0 ? DARKGREEN : DARKBLUE));
                    DrawRectangleLinesEx(block, 3, (roomX == 0 ? GREEN : SKYBLUE));
                }

                // Draw heap-allocated item if it exists
                if (activeItem != nullptr) DrawRectangleRec(*activeItem, GOLD);

                player.draw();
            EndMode2D();

            // --- UI Overlay & Debug Information ---
            DrawRectangle(10, 10, 280, 100, Fade(BLACK, 0.5f));
            DrawText(TextFormat("Area: %s", current.name), 20, 20, 20, RAYWHITE);
            DrawText(TextFormat("X: %.0f Y: %.0f", player.getPosition().x, player.getPosition().y), 20, 45, 20, GREEN);

            // Mouse world position for level design assistance
            Vector2 mouseWorld = GetScreenToWorld2D(GetMousePosition(), camera);
            DrawText(TextFormat("Mouse World: %.0f %.0f", mouseWorld.x, mouseWorld.y), 20, 70, 20, YELLOW);

            // Simple Health HUD
            DrawRectangle(screenWidth - 220, 20, 200, 25, DARKGRAY);
            DrawRectangle(screenWidth - 220, 20, (int)((health / 100.0f) * 200.0f), 25, RED);

            // Floating location label
            if (roomTitleTimer > 0) {
                UI::Label cityLabel = { current.name, 30, VIOLET };
                cityLabel.draw(screenWidth, 150);
            }
            break;
        }
        case GAMEOVER: {
            UI::Label goLabel = { "GAME OVER", 40, RED };
            goLabel.draw(screenWidth, screenHeight / 2 - 100);
            UI::Button resetBtn({ screenWidth / 2.0f - 100, screenHeight / 2.0f, 200, 50 }, "RESET");
            resetBtn.draw();
            break;
        }
    }
    EndDrawing();
}