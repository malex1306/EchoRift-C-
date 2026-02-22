#ifndef ECHORIFT_PLAYER_H
#define ECHORIFT_PLAYER_H
#include "raylib.h"


class Player {
private:
    Vector2 position;
    float speed;

public:
    Player(Vector2 startPos, float startSpeed);

    void update (float dt);

    void draw() const;

    [[nodiscard]] Vector2 getPosition() const {
        return position;
    }

    void setPosition(const Vector2 newPos) {
        position = newPos;
    }

};

#endif //ECHORIFT_PLAYER_H