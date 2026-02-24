#include "../include/Game.h"
#include "../include/Player.h"
#include "components/UiComponents.h"

Game::Game() : player({ 400.0f, 300.0f}, 300.0f) {
    InitWindow(screenWidth, screenHeight, "Echo Rift");
    SetTargetFPS(60);

    // Initialize the procedural/map-based world data
    initWorld();

    // Configure the 2D camera viewport settings
    camera.target = { 400.0f, 300.0f};
    camera.offset = { screenWidth / 2.0f, screenHeight / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    currentState = TITLE;
    health = 100;
}

Game::~Game() {
    // Explicit heap cleanup to prevent memory leaks before context destruction
    if (activeItem != nullptr) {
        delete activeItem;
        activeItem = nullptr;
    }
    CloseWindow();
}

void Game::run() {
    // Standard game loop: Process logic then render frames
    while (!WindowShouldClose()) {
        update();
        draw();
    }
}

void Game::resetGame() {
    // Revert game state variables and player transform to initial values
    health = 100;
    roomTitleTimer = 5.0f;
    player.setPosition({ 400.0f, 300.0f});
    roomX = 0;
    roomY = 0;

    // Clean up any dynamic entities currently in memory
    if (activeItem != nullptr) {
        delete activeItem;
        activeItem = nullptr;
    }
}

void Game::initWorld() {
    // Populate the worldMap with room data using coordinate keys {X, Y}

    // Room [0,0]: Primary starting area
    LevelRoom grass;
    grass.name = "Echo Grasslands";
    grass.groundColor = Color{ 34, 139, 34, 255 };
    grass.solids.push_back({ 100.0f, 100.0f, 50.0f, 50.0f }); // Tree collider
    worldMap[{0, 0}] = grass;

    // Room [1,0]: Urban environment located East
    LevelRoom city;
    city.name = "Beton District";
    city.groundColor = GRAY;
    city.solids.push_back({ 200, 200, 300, 400 });
    city.solids.push_back({ 700, 100, 200, 200 });
    city.solids.push_back({ 1200, 600, 400, 300 });
    worldMap[{1, 0}] = city;

    // Room [0,1]: Dense forest environment located South
    LevelRoom forrest;
    forrest.name = "Forrest";
    forrest.groundColor = Color{ 20, 80, 20, 255 };
    forrest.solids.push_back({ 400, 400, 300, 200 });
    forrest.solids.push_back({ 0, 0, 100, 1000 });    // Western boundary wall
    forrest.solids.push_back({ 800, 200, 60, 60 });
    forrest.solids.push_back({ 900, 700, 60, 60 });
    worldMap[{0, 1}] = forrest;
}

void Game::update() {
    switch (currentState) {
        case TITLE: {
            // Handle Title Screen interactions
            if (const UI::Button startBtn({ screenWidth / 2.0f - 100, screenHeight / 2.0f, 200, 50 }, "START");
                startBtn.isClicked() || IsKeyPressed(KEY_ENTER)) {
                currentState = GAMEPLAY;
            }
            break;
        }
        case GAMEPLAY: {
            const Vector2 oldPos = player.getPosition();
            player.update(GetFrameTime());
            Vector2 currentPos = player.getPosition();

            // --- Spatial Transition Logic (Room Switching) ---

            // Trigger Eastward transition: Grasslands -> City
            if (roomX == 0 && roomY == 0 && currentPos.x > worldSize) {
                roomX = 1;
                currentPos.x = 50.0f; // Reset position to room entrance
                player.setPosition(currentPos);
                roomTitleTimer = 5.0f;
            }
            // Trigger Southward transition: Grasslands -> Forest
            else if (roomX == 0 && roomY == 0 && currentPos.y > worldSize) {
                roomY = 1;
                currentPos.y = 50.0f;
                player.setPosition(currentPos);
                roomTitleTimer = 5.0f;
            }
            // Trigger Northward transition: Forest -> Grasslands
            else if (roomX == 0 && roomY == 1 && currentPos.y < 0) {
                roomY = 0;
                currentPos.y = worldSize - 50.0f;
                player.setPosition(currentPos);
                roomTitleTimer = 5.0f;
            }

            // Define player collision bounds for the current frame
            const Rectangle playerRect = { player.getPosition().x - 20, player.getPosition().y - 20, 40, 40 };

            // Dynamic Collision Check: Iterates through solids in the current active room
            LevelRoom& current = worldMap[{roomX, roomY}];
            for (const auto& block : current.solids) {
                if (CheckCollisionRecs(block, playerRect)) {
                    player.setPosition(oldPos); // Revert movement on collision
                    break;
                }
            }

            // --- Movement Clamping (Global boundaries for unmapped edges) ---
            currentPos = player.getPosition();

            if (currentPos.y < 0 && roomY == 0) currentPos.y = 0; // World Top cap
            if (currentPos.x < 0) currentPos.x = 0;             // World Left cap

            // Barrier logic for edges where no room transition is defined
            if (roomX == 1 && roomY == 0 && currentPos.x > worldSize) currentPos.x = worldSize;
            if (roomX == 0 && roomY == 1 && currentPos.x > worldSize) currentPos.x = worldSize;
            if (roomX == 1 && roomY == 0 && currentPos.y > worldSize) currentPos.y = worldSize;
            if (roomX == 0 && roomY == 1 && currentPos.y > worldSize) currentPos.y = worldSize;

            player.setPosition(currentPos);
            camera.target = player.getPosition(); // Maintain camera centering on player

            // Tick down UI notification timers
            if (roomTitleTimer > 0) roomTitleTimer -= GetFrameTime();

            // Debug Input: HP Reduction check
            if (IsKeyPressed(KEY_MINUS)) {
                health -= 10;
                if (health <= 0) { health = 0; currentState = GAMEOVER; }
            }

            // Prototype: Interactive entity instantiation (Heap exercise)
            if (IsKeyPressed(KEY_I) && activeItem == nullptr) {
                activeItem = new Rectangle{ 600, 400, 40, 40 };
            }

            // Interaction logic for the dynamically spawned item
            if (activeItem != nullptr) {
                if (CheckCollisionRecs(playerRect, *activeItem)) {
                    delete activeItem; // Properly deallocate memory on pickup
                    activeItem = nullptr;
                    health = 100; // Recovery effect
                }
            }
            break;
        }
        case GAMEOVER: {
            // Check for Reset triggers to restart gameplay loop
            if (const UI::Button resetBtn({ screenWidth / 2.0f - 100, screenHeight / 2.0f, 200, 50 }, "RESET");
                resetBtn.isClicked() || IsKeyPressed(KEY_R)) {
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
            const LevelRoom& current = worldMap.at({roomX, roomY});

            BeginMode2D(camera); // Begin world-space rendering
                ClearBackground(current.groundColor);

                // --- Ground Visual Pass ---
                if (roomX == 0) { // Procedural Checkerboard for outdoor zones
                    for (int x = 0; x < 32; x++) {
                        for (int y = 0; y < 32; y++) {
                            if ((x + y) % 2 == 0) {
                                DrawRectangle(x * 64, y * 64, 64, 64, Color{ 50, 205, 50, 255 });
                            }
                        }
                    }
                }
                else if (roomX == 1) { // Wireframe grid for urban zones
                    for (int x = 0; x < 32; x++) {
                        for (int y = 0; y < 32; y++)
                            DrawRectangleLines(x * 64, y * 64, 64, 64, Fade(LIGHTGRAY, 0.3f));
                    }
                }

                // --- Static Geometry Pass ---
                for (const auto& block : current.solids) {
                    DrawRectangleRec(block, (roomX == 0 ? DARKGREEN : DARKBLUE));
                    DrawRectangleLinesEx(block, 3, (roomX == 0 ? GREEN : SKYBLUE));
                }

                // Render dynamic item if valid
                if (activeItem != nullptr) DrawRectangleRec(*activeItem, GOLD);

                player.draw();
            EndMode2D(); // End world-space rendering

            // --- HUD / Overlay Pass (Screen-space UI) ---
            DrawRectangle(10, 10, 280, 100, Fade(BLACK, 0.5f));
            DrawText(TextFormat("Area: %s", current.name), 20, 20, 20, RAYWHITE);
            DrawText(TextFormat("X: %.0f Y: %.0f", player.getPosition().x, player.getPosition().y), 20, 45, 20, GREEN);

            // Screen-to-World coordinate projection helper
            Vector2 mouseWorld = GetScreenToWorld2D(GetMousePosition(), camera);
            DrawText(TextFormat("Mouse World: %.0f %.0f", mouseWorld.x, mouseWorld.y), 20, 70, 20, YELLOW);

            // Resource HUD (Health bar)
            DrawRectangle(screenWidth - 220, 20, 200, 25, DARKGRAY);
            DrawRectangle(screenWidth - 220, 20, (int)((health / 100.0f) * 200.0f), 25, RED);

            // Passive Screen Notifications
            if (roomTitleTimer > 0) {
                UI::Label cityLabel = { current.name, 30, VIOLET };
                cityLabel.draw(screenWidth, 150);
            }
            break;
        }
        case GAMEOVER: {
            UI::Label goLabel = { "GAME OVER!", 40, RED };
            goLabel.draw(screenWidth, screenHeight / 2 - 100);
            UI::Button resetBtn({ screenWidth / 2.0f - 100, screenHeight / 2.0f, 200, 50 }, "RESET");
            resetBtn.draw();
            break;
        }
    }
    EndDrawing();
}