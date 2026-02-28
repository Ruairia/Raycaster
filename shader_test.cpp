// main.cpp
#include <cmath>
#include <iostream>
#include <ostream>
#include <raylib.h>

int main() {
    InitWindow(1600, 900, "Shader Test");
    SetTargetFPS(60);

    Shader shader = LoadShader(nullptr, "shader_test.glsl");
    int resLocResolution = GetShaderLocation(shader, "resolution");
    float resolution[2] = {1600.0f,900.0f};
    while (!WindowShouldClose()) {
        BeginDrawing();
        BeginShaderMode(shader);
        SetShaderValue(shader, resLocResolution, resolution, SHADER_UNIFORM_VEC2);
        DrawRectangle(0, 0, 1600, 900, WHITE);
        EndShaderMode();
        EndDrawing();
    }

    UnloadShader(shader);
    CloseWindow();
}