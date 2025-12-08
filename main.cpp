#include "Vector2D.h"
#include <cmath>
#include <iostream>
#include <ostream>
#include <raylib.h>

#include "Player.h"
#include "Map.h"
#include "ray.h"

using namespace raycaster;
 int screenWidth = 800;
 int screenHeight = 600;
constexpr short moveSpeed = 2; //squares per second
constexpr short turnSpeed = 2; //radians per second

void drawWall(int side, int screenX, double distance, int hit);

int main(){
    InitWindow(screenWidth, screenHeight, "Raycaster");
    screenWidth = GetMonitorWidth(0);
    screenHeight = GetMonitorHeight(0);
    SetWindowSize(screenWidth, screenHeight);
    ToggleFullscreen();

    SetTargetFPS(120);
    auto player = Player({3,3},{0,-1},{1.32,0});

    double previousTime=GetTime();
    double currentTime {0};
    double seconds_elapsed {0};

    while (!WindowShouldClose()) //Game loop
    {
        currentTime = GetTime();
        seconds_elapsed = currentTime - previousTime;
        previousTime = currentTime;

        player.handleMovement(moveSpeed, turnSpeed, seconds_elapsed);

        BeginDrawing();
        DrawRectangle(0, 0, screenWidth, screenHeight/2, SKYBLUE);
        for (float y = screenHeight/2; y < screenHeight; y++) {
            float linearFactor = y/screenHeight;
            float factor = 0.75*(std::log(linearFactor));

            Color floorColor = ColorBrightness(DARKGREEN, factor);
            DrawRectangle(0, y, screenWidth, 1, floorColor);
        }


        for (int screenX  {0}; screenX < screenWidth; screenX++)
        {
            const double  cameraX =
                (-0.5 + (static_cast<float>(screenX) / static_cast<float>(screenWidth)));
            Vector2D rayDirection = player.direction + player.cameraPlane * cameraX;
            auto ray = raycaster::Ray(player.position, rayDirection);


            int hit=0;
            int side=0;
            while (hit == 0 && Map::hasSquare(ray.mapPosition.x, ray.mapPosition.y))
            {
                if (ray.sideDist.x<ray.sideDist.y)
                {
                    ray.sideDist.x += ray.pathDistanceForGridStep.x;
                    ray.mapPosition.x += ray.step.x;
                    side=0;
                }

                else
                {
                    ray.sideDist.y += ray.pathDistanceForGridStep.y;
                    ray.mapPosition.y += ray.step.y;
                    side=1;
                }

                hit = Map::getSquare(ray.mapPosition.x, ray.mapPosition.y);

            }
            double perpendicularDistance = 0;
            if(side == 0) perpendicularDistance = (ray.sideDist.x - ray.pathDistanceForGridStep.x);
            else          perpendicularDistance = (ray.sideDist.y - ray.pathDistanceForGridStep.y);


            drawWall(side, screenX, perpendicularDistance, hit);

        }
        std::string fps_counter = "FPS: "+std::to_string(GetFPS());
        DrawText(fps_counter.c_str(), 20, 10, 20, RAYWHITE);
        EndDrawing();
    }


    CloseWindow();
    return 0;
}

void drawWall(int side, int screenX, double distance, int hit) {
    if (distance <= 0) return;  // Avoid division by zero

    Color material;
    switch (hit) {
        case 1: material = WHITE; break;
        case 2: material = RED; break;
        case 3: material = BLUE; break;
        case 4: material = GREEN; break;
        default: material = BLACK; break;
    }


    double darkening = - (0.7 - 20 * pow(distance,-2));
    if (darkening>=0) darkening=0;

    if (side == 0)
        material = ColorBrightness(material, darkening - 0.2);
    else
        material = ColorBrightness(material, darkening);

    int wallHeight = static_cast<int>(screenHeight / distance);
    // Clamp wall height to prevent excessive values
    wallHeight = std::min(wallHeight, screenHeight * 2);

    const int drawStart = std::max(0, (screenHeight - wallHeight) / 2);
    const int drawHeight = std::min(wallHeight, screenHeight - drawStart);

    DrawRectangle(screenX, drawStart, 1, drawHeight, material);
}