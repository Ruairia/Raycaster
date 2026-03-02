//
// Created by ruairi on 03/08/2025.
//

#include "Player.h"
#include <algorithm>
#include <raylib.h>

#include "Map.h"

namespace raycaster {

    Player::Player(const Vector2D& position, const Vector2D& direction, const Vector2D& cameraPlane)
    : position(position), direction(direction), cameraPlane(cameraPlane){}

void Player::moveForward(const Vector2D& movement) {
        Vector2D next = position + movement;
        if (Map::getSquare(next.x, next.y)==0)
        {
            position = next;
        }
    }

    void Player::moveSideways(const Vector2D& movement) {
        Vector2D next = position + movement;
        if (Map::getSquare(next.x, next.y)==0)
        {
            position = next;
        }
    }

void Player::rotateYaw(const float angleInRadians) {
    direction.rotate(angleInRadians);
    cameraPlane.rotate(angleInRadians);
}

void Player::rotatePitch(const float angleInRadians)
{
    pitch += angleInRadians;
    if (pitch > 0.35*PI) pitch = 0.35*PI;
    if (pitch<-0.35*PI) pitch = -0.35*PI;
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
        moveSideways(moveSpeed * cameraPlane * secondsElapsed*0.5);
    }
    if (IsKeyDown(KEY_A))
    {
        moveSideways(-moveSpeed * cameraPlane * secondsElapsed*0.5);
    }
        float mouseDeltaX = GetMouseDelta().x;
        rotateYaw(turnSpeed * mouseDeltaX);
        float mouseDeltaY = GetMouseDelta().y;
        rotatePitch(0.75*turnSpeed*mouseDeltaY);
}

} // raycaster