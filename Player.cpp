//
// Created by ruairi on 03/08/2025.
//

#include "Player.h"

#include <raylib.h>

#include "Map.h"

namespace raycaster {

    Player::Player(const Vector2D& position, const Vector2D& direction, const Vector2D& cameraPlane, const float horizon)
    : position(position), direction(direction), cameraPlane(cameraPlane), horizon(horizon) {}

void Player::moveForward(const Vector2D& movement) {
        Vector2D next = position + movement;
        if (Map::getSquare(next.x, next.y)==0)
        {
            position = next;
        }
    }

    void Player::moveSideways(const Vector2D& movement) {
        Vector2D next = position + movement*0.5;
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
        moveForward(moveSpeed * direction * secondsElapsed);
    }
    if (IsKeyDown(KEY_S))
    {
        moveForward(-moveSpeed * direction * secondsElapsed);
    }
    if (IsKeyDown(KEY_D))
    {
        moveSideways(moveSpeed * cameraPlane * secondsElapsed);
    }
    if (IsKeyDown(KEY_A))
    {
        moveSideways(-moveSpeed * cameraPlane * secondsElapsed);
    }
        float mouseDeltaX = GetMouseDelta().x;
        rotate(turnSpeed * mouseDeltaX);
        float mouseDeltaY = GetMouseDelta().y;
        horizon += 2*mouseDeltaY;
        if (horizon < 0) horizon = 0;
        if (horizon > GetScreenHeight()) horizon = GetScreenHeight();
}

} // raycaster