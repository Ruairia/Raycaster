#include "Vector2D.h"
#include <cmath>
#include <iostream>
#include <ostream>
#include <raylib.h>
#include "rlgl.h"
#include "Player.h"
#include "Map.h"
#include "ray.h"

using namespace raycaster;
 int screenWidth = 800;
 int screenHeight = 450;

static Texture2D crosshairTexture;
static Texture2D brickTexture;
static Texture2D dirtTexture;
static Texture2D dark_oak_logTexture;
static Texture2D spruce_planksTexture;
static Texture2D stoneTexture;
static int texLocBrick;
static int texLocDirt;
static int texLocDarkOakLog;
static int texLocSprucePlanks;
static int texLocStone;

constexpr short moveSpeed = 2; //squares per second
constexpr float turnSpeed = 0.005; //radians per mouse delta

void drawWall(int side, int screenX, double perpDistance, int hit);

void loadShaderTexture(
    const Shader shader, const unsigned textureID, const int texLoc, const int slot)
{
    rlActiveTextureSlot(slot);
    rlEnableTexture(textureID);
    SetShaderValue(shader, texLoc, &slot, SHADER_UNIFORM_INT);
}

void loadTextures(Shader shader)
{
    brickTexture = LoadTexture("../Assets/bricks.png");
    texLocBrick = GetShaderLocation(shader, "brickTexture");
    SetShaderValueTexture(shader, texLocBrick, brickTexture);
    SetTextureFilter(brickTexture, TEXTURE_FILTER_POINT);

    dirtTexture = LoadTexture("../Assets/dirt.png");
    texLocDirt = GetShaderLocation(shader, "dirtTexture");
    SetShaderValueTexture(shader, texLocDirt, dirtTexture);
    SetTextureFilter(dirtTexture, TEXTURE_FILTER_POINT);

    dark_oak_logTexture = LoadTexture("../Assets/dark_oak_log.png");
    texLocDarkOakLog = GetShaderLocation(shader, "dark_oak_logTexture");
    SetShaderValueTexture(shader, texLocDarkOakLog, dark_oak_logTexture);
    SetTextureFilter(dark_oak_logTexture, TEXTURE_FILTER_POINT);

    spruce_planksTexture = LoadTexture("../Assets/spruce_planks.png");
    texLocSprucePlanks = GetShaderLocation(shader, "spruce_planksTexture");
    SetShaderValueTexture(shader, texLocSprucePlanks, spruce_planksTexture);
    SetTextureFilter(spruce_planksTexture, TEXTURE_FILTER_POINT);

    stoneTexture = LoadTexture("../Assets/stone.png");
    texLocStone = GetShaderLocation(shader, "stoneTexture");
    SetShaderValueTexture(shader, texLocStone, stoneTexture);
    SetTextureFilter(stoneTexture, TEXTURE_FILTER_POINT);

    crosshairTexture = LoadTexture("../Assets/crosshair.png");
}

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

    loadTextures(shader);

    SetTargetFPS(100);
    DisableCursor();

    auto player = Player({1.5,1.5},{0,-1},{1.32,0}, GetScreenHeight()/2);

    int resLocPlayerPosition = GetShaderLocation(shader, "playerPosition");
    int resLocPlayerDirection = GetShaderLocation(shader, "playerDirection");
    int resLocCameraPlane = GetShaderLocation(shader, "cameraPlane");
    int resLocHorizon = GetShaderLocation(shader, "horizon");

    double previousTime=GetTime();
    double currentTime {0};
    double seconds_elapsed {0};




    loadShaderTexture(shader, brickTexture.id, texLocBrick, 1);
    loadShaderTexture(shader, dirtTexture.id, texLocDirt, 2);
    loadShaderTexture(shader, dark_oak_logTexture.id, texLocDarkOakLog, 3);
    loadShaderTexture(shader, spruce_planksTexture.id, texLocSprucePlanks, 4);
    loadShaderTexture(shader, stoneTexture.id, texLocStone, 5);


    while (!WindowShouldClose()) //Game loop
    {
        currentTime = GetTime();
        seconds_elapsed = currentTime - previousTime;
        previousTime = currentTime;

        player.handleMovement(moveSpeed, turnSpeed, seconds_elapsed);

        float playerPos[2] = {(float)player.position.x, (float)player.position.y};
        float playerDir[2] = {(float)player.direction.x, (float)player.direction.y};
        float camPlane[2]  = {(float)player.cameraPlane.x, (float)player.cameraPlane.y};
        float horizon = player.horizon;
        SetShaderValue(shader, resLocPlayerPosition, playerPos, SHADER_UNIFORM_VEC2);
        SetShaderValue(shader, resLocPlayerDirection, playerDir, SHADER_UNIFORM_VEC2);
        SetShaderValue(shader, resLocCameraPlane, camPlane, SHADER_UNIFORM_VEC2);
        SetShaderValue(shader, resLocHorizon, &horizon, SHADER_UNIFORM_FLOAT);



        BeginDrawing();
        BeginShaderMode(shader);
        DrawRectangle(0,0,GetScreenWidth(),GetScreenHeight(),WHITE);
        EndShaderMode();

        const Vector2 crosshairPosition = {
            static_cast<float>(GetScreenWidth()) / 2 - 32,
            static_cast<float>(GetScreenHeight()) / 2 - 32
        };

        DrawTextureEx(crosshairTexture, crosshairPosition, 0, 4, WHITE);

        std::string fps_counter = "FPS: "+std::to_string(GetFPS());
        DrawText(fps_counter.c_str(), 20, 10, 20, RAYWHITE);
        EndDrawing();
    }


    CloseWindow();
    return 0;
}
