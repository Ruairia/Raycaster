//
// Created by ruairi on 03/08/2025.
//

#ifndef PLAYER_H
#define PLAYER_H

#include "Vector2D.h"

namespace raycaster {

class Player {
public:
    Vector2D position {};
    Vector2D direction {};
    Vector2D cameraPlane {};
    float horizon {0};

    void handleMovement(float moveSpeed, float turnSpeed, double secondsElapsed);
    void moveForward(const Vector2D& movement);
    void moveSideways(const Vector2D& movement);
    Player(const Vector2D& position, const Vector2D& direction, const Vector2D& cameraPlane, const float horizon);
    void rotate(double angleInRadians);
};

} // raycaster

#endif //PLAYER_H
