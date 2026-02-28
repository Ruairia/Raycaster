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
 int screenHeight = 450;
constexpr short moveSpeed = 2; //squares per second
constexpr short turnSpeed = 2; //radians per second

void drawWall(int side, int screenX, double perpDistance, int hit);

int main(){
    InitWindow(screenWidth, screenHeight, "Raycaster");
    screenWidth = GetMonitorWidth(0);
    screenHeight = GetMonitorHeight(0);
    SetWindowSize(screenWidth, screenHeight);
    ToggleFullscreen();

    Shader shader = LoadShader(nullptr, "raycaster_shader.glsl");
    int resLocResolution = GetShaderLocation(shader, "resolution");
    float resolution[2] = {static_cast<float>(screenWidth),static_cast<float>(screenHeight)};
    SetShaderValue(shader, resLocResolution, resolution, SHADER_UNIFORM_VEC2);

    SetTargetFPS(120);
    auto player = Player({3.5,2.5},{0,-1},{1.32,0});

    int resLocPlayerPosition = GetShaderLocation(shader, "playerPosition");
    int resLocPlayerDirection = GetShaderLocation(shader, "playerDirection");
    int resLocCameraPlane = GetShaderLocation(shader, "cameraPlane");

    double previousTime=GetTime();
    double currentTime {0};
    double seconds_elapsed {0};

    while (!WindowShouldClose()) //Game loop
    {
        currentTime = GetTime();
        seconds_elapsed = currentTime - previousTime;
        previousTime = currentTime;

        player.handleMovement(moveSpeed, turnSpeed, seconds_elapsed);

        float playerPos[2] = {(float)player.position.x, (float)player.position.y};
        float playerDir[2] = {(float)player.direction.x, (float)player.direction.y};
        float camPlane[2]  = {(float)player.cameraPlane.x, (float)player.cameraPlane.y};
        SetShaderValue(shader, resLocPlayerPosition, playerPos, SHADER_UNIFORM_VEC2);
        SetShaderValue(shader, resLocPlayerDirection, playerDir, SHADER_UNIFORM_VEC2);
        SetShaderValue(shader, resLocCameraPlane, camPlane, SHADER_UNIFORM_VEC2);

        BeginDrawing();
        BeginShaderMode(shader);
        DrawRectangle(0, 0, screenWidth, screenHeight, WHITE);
        EndShaderMode();
        std::string fps_counter = "FPS: "+std::to_string(GetFPS());
        DrawText(fps_counter.c_str(), 20, 10, 20, RAYWHITE);
        EndDrawing();
    }


    CloseWindow();
    return 0;
}
