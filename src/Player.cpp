//
// Created by alexa on 21.02.2026.
//

#include "../include/Player.h"

#include <cmath>

Player::Player(Vector2 startPos, float startSpeed) :
    position(startPos), speed(startSpeed){}

void Player::update(float dt) {
    Vector2 direction = { 0.0f, 0.0f };
    if (IsKeyDown(KEY_W)) direction.y -= 1.0f;
    if (IsKeyDown(KEY_S)) direction.y += 1.0f;
    if (IsKeyDown(KEY_A)) direction.x -= 1.0f;
    if (IsKeyDown(KEY_D)) direction.x += 1.0f;

    float length = sqrtf(direction.x * direction.x + direction.y * direction.y);

    if (length > 0.0f) {
        direction.x /= length;
        direction.y /= length;
    }
    position.x += direction.x * speed * dt;
    position.y += direction.y * speed * dt;


}

void Player::draw() {
    DrawCircleV(position, 20, VIOLET);
}