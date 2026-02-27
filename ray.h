//
// Created by ruairi on 03/08/2025.
//

#ifndef RAY_H
#define RAY_H
#include "Vector2D.h"

namespace raycaster
{
    class Ray {
    public:
        Ray() = default;
        Ray(const Vector2D& origin, const Vector2D& direction)
        {
            this->origin = origin;
            this->position = origin;
            this->direction = direction;
            this->direction.normalise();
            mapPosition = origin.floor();

            pathDistanceForGridStep.x = (direction.x == 0) ? 1e30 : std::abs(1/direction.x);
            pathDistanceForGridStep.y = (direction.y == 0) ? 1e30 : std::abs(1/direction.y);

            if (direction.x < 0)
            {
                step.x = -1;
                sideDist.x = (origin.x - mapPosition.x) * pathDistanceForGridStep.x;
            }
            else if (direction.x > 0)
            {
                step.x = 1;
                sideDist.x = (static_cast<float>(mapPosition.x) + 1.0 - origin.x) * pathDistanceForGridStep.x;
            }
            if (direction.y < 0)
            {
                step.y = -1;
                sideDist.y = (origin.y - static_cast<float>(mapPosition.y)) * pathDistanceForGridStep.y;
            }
            else if (direction.y > 0)
            {
                step.y = 1;
                sideDist.y = (static_cast<float>(mapPosition.y) + 1.0 - origin.y) * pathDistanceForGridStep.y;
            }

        }
    public:
        Vector2D  origin;
        Vector2D  direction;
        Vector2D  pathDistanceForGridStep {0,0};
        Vector2D  sideDist;
        Vector2D  step {0,0};
        Vector2D  mapPosition;
        Vector2D  position;
    };
}
#endif //RAY_H
