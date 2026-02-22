//
// Created by alexa on 21.02.2026.
//

#ifndef ECHORIFT_GAME_H
#define ECHORIFT_GAME_H
#include "Player.h"
#include "raylib.h"

enum GameState {
    TITLE, GAMEPLAY, GAMEOVER
};

class Game {
public:
    Game(); //constructor
    ~Game();//destructure

    void run(); //mainloop

private:
    void update(); //logic

    void draw(); //graphics

    void resetGame(); // reset

    const int screenWidth = 1980;
    const int screenHeight = 1080;

    float cityTitleTimer = 5.0f;
    int health;
    int maxHealth = 100;
    Rectangle treeCollider = { 100.0f, 100.0f, 50.0f, 50.0f};
    Player player;
    Camera2D camera;
    GameState currentState;
};

#endif //ECHORIFT_GAME_H