//
// Created by ruairi on 03/08/2025.
//

#include "Player.h"

#include <raylib.h>

#include "Map.h"

namespace raycaster {

    Player::Player(const Vector2D& position, const Vector2D& direction, const Vector2D& cameraPlane)
    : position(position), direction(direction), cameraPlane(cameraPlane) {}

void Player::move(const Vector2D& movement) {
        Vector2D next = position + movement;
        if (Map::getSquare(next.x, next.y)==0)
        {
            position = next;
        }
    }

void Player::rotate(const double angleInRadians) {
    direction.rotate(angleInRadians);
    cameraPlane.rotate(angleInRadians);
}
    void Player::handleMovement(const float moveSpeed, const float turnSpeed, const double secondsElapsed)
{
    if (IsKeyDown(KEY_W))
    {
        move(moveSpeed * direction * secondsElapsed);
    }
    if (IsKeyDown(KEY_S))
    {
        move(-moveSpeed * direction * secondsElapsed);
    }
    if (IsKeyDown(KEY_D))
    {
        rotate(turnSpeed * secondsElapsed);
    }
    if (IsKeyDown(KEY_A))
    {
        rotate(-turnSpeed * secondsElapsed);
    }
}

} // raycaster