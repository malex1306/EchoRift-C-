//
// Created by alexa on 21.02.2026.
//

#include "../include/Player.h"

#include <cmath>

Player::Player(const Vector2 startPos, const float startSpeed) :
    position(startPos), speed(startSpeed){}

void Player::update(const float dt) {
    Vector2 direction = { 0.0f, 0.0f };
    if (IsKeyDown(KEY_W)) direction.y -= 1.0f;
    if (IsKeyDown(KEY_S)) direction.y += 1.0f;
    if (IsKeyDown(KEY_A)) direction.x -= 1.0f;
    if (IsKeyDown(KEY_D)) direction.x += 1.0f;

    if (const float length = sqrtf(direction.x * direction.x + direction.y * direction.y); length > 0.0f) {
        direction.x /= length;
        direction.y /= length;
    }
    position.x += direction.x * speed * dt;
    position.y += direction.y * speed * dt;


}

void Player::draw() const {
    DrawCircleV(position, 20, VIOLET);
}
