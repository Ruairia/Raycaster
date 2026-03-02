#include "Vector2D.h"
#include <iostream>
#include <raylib.h>
#include "rlgl.h"
#include "Player.h"

using namespace raycaster;
 int screenWidth = 800;
 int screenHeight = 450;



constexpr short moveSpeed = 2; //squares per second
constexpr float turnSpeed = 0.005; //radians per mouse delta


void bindTextureSlot(
    const Shader shader, const unsigned textureID, const int texLoc, const int slot)
{
    rlActiveTextureSlot(slot);
    rlEnableTexture(textureID);
    SetShaderValue(shader, texLoc, &slot, SHADER_UNIFORM_INT);
}

struct TextureSlot
{
    int slot;
    Texture2D texture;
    int texLoc;
    const char* path;
    const char* uniformName;
};

static Texture2D crosshair;

static TextureSlot textures[] = {
    {1, {}, 0, "../Assets/bricks.png",       "brickTexture"},
  {2, {}, 0, "../Assets/dirt.png",          "dirtTexture"},
  {3, {}, 0, "../Assets/dark_oak_log.png",  "dark_oak_logTexture"},
  {4, {}, 0, "../Assets/spruce_planks.png", "spruce_planksTexture"},
  {5, {}, 0, "../Assets/stone.png",         "stoneTexture"},
  {6, {}, 0, "../Assets/grass.png",         "grassTexture"}
};

struct ShaderLocations
{
    int playerPosition;
    int playerDirection;
    int cameraPlane;
    int pitch;
};

ShaderLocations getShaderLocations(Shader shader) {
    return {
        GetShaderLocation(shader, "playerPosition"),
        GetShaderLocation(shader, "playerDirection"),
        GetShaderLocation(shader, "cameraPlane"),
        GetShaderLocation(shader, "pitch")
    };
}

void loadTextures(Shader shader)
{
    for (auto& slot : textures) {
        slot.texture = LoadTexture(slot.path);
        slot.texLoc  = GetShaderLocation(shader, slot.uniformName);
        SetTextureFilter(slot.texture, TEXTURE_FILTER_POINT);
        rlActiveTextureSlot(slot.slot);
        rlEnableTexture(slot.texture.id);
        SetShaderValue(shader, slot.texLoc, &slot.slot, SHADER_UNIFORM_INT);
    }
    crosshair = LoadTexture("../Assets/crosshair.png");
}



void updateUniforms(Shader shader,ShaderLocations shaderLocations, Player player)
{
    float playerPos[2] = {(float)player.position.x, (float)player.position.y};
    float playerDir[2] = {(float)player.direction.x, (float)player.direction.y};
    float camPlane[2]  = {(float)player.cameraPlane.x, (float)player.cameraPlane.y};
    float pitch = player.pitch;
    SetShaderValue(shader, shaderLocations.playerPosition, playerPos, SHADER_UNIFORM_VEC2);
    SetShaderValue(shader, shaderLocations.playerDirection, playerDir, SHADER_UNIFORM_VEC2);
    SetShaderValue(shader, shaderLocations.cameraPlane, camPlane, SHADER_UNIFORM_VEC2);
    SetShaderValue(shader, shaderLocations.pitch, &pitch, SHADER_UNIFORM_FLOAT);
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

    SetTargetFPS(120);
    DisableCursor();

    auto player = Player({1.5,1.5},{0,-1},{1.32,0}, GetScreenHeight()/2);

    ShaderLocations shaderLocations = getShaderLocations(shader);


    double previousTime=GetTime();
    double currentTime {0};
    double seconds_elapsed {0};


    for (TextureSlot texture_slot : textures)
    {
        bindTextureSlot(shader, texture_slot.texture.id, texture_slot.texLoc, texture_slot.slot);
    }



    while (!WindowShouldClose()) //Game loop
    {
        currentTime = GetTime();
        seconds_elapsed = currentTime - previousTime;
        previousTime = currentTime;

        player.handleMovement(moveSpeed, turnSpeed, seconds_elapsed);


        updateUniforms(shader, shaderLocations, player);



        BeginDrawing();
        BeginShaderMode(shader);
        DrawRectangle(0,0,GetScreenWidth(),GetScreenHeight(),WHITE);
        EndShaderMode();

        const Vector2 crosshairPosition = {
            static_cast<float>(GetScreenWidth()) / 2 - 32,
            static_cast<float>(GetScreenHeight()) / 2 - 32
        };

        DrawTextureEx(crosshair, crosshairPosition, 0, 4, WHITE);

        std::string fps_counter = "FPS: "+std::to_string(GetFPS());
        DrawText(fps_counter.c_str(), 20, 10, 20, RAYWHITE);
        EndDrawing();
    }


    CloseWindow();
    return 0;
}
