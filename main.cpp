#include "main.h"
#include <iostream>
#include <string>
#include <vector>
#include <raylib.h>
#include "Vector2D.h"
#include "rlgl.h"
#include "Player.h"

#define SPRITE_DATA_LENGTH 5

using namespace raycaster;
 int screenWidth = 800;
 int screenHeight = 450;

constexpr short moveSpeed = 2; //squares per second
constexpr float turnSpeed = 0.005; //radians per mouse delta

int main(){
    InitWindow(screenWidth, screenHeight, "Raycaster");
    screenWidth = GetMonitorWidth(0);
    screenHeight = GetMonitorHeight(0);
    SetWindowSize(screenWidth, screenHeight);
    ToggleFullscreen();
    SetTargetFPS(120);
    DisableCursor();

    Shader shader = LoadShader(nullptr, "raycaster_shader.glsl");
    passResolutionToShader(shader);
    loadTextures(shader);

    float fovInDegrees = 66;
    float cameraPlaneLength = 2 * tan(fovInDegrees * 0.5f * PI / 180);
    auto player = Player ({1.5,1.5},{0,-1},{cameraPlaneLength,0});
    float verticalFactor = calculateVerticalFactor(player);
    std::cout <<  "vertical factor: " <<verticalFactor;
    int verticalFactorLoc = GetShaderLocation(shader, "verticalFactor");
    SetShaderValue(shader, verticalFactorLoc, &verticalFactor, SHADER_UNIFORM_FLOAT);

    ShaderLocations shaderLocations = getShaderLocations(shader);

    double previousTime=GetTime();
    double currentTime {0};
    double seconds_elapsed {0};

    for (TextureSlot texture_slot : textures)
    {
        bindTextureSlot(shader, texture_slot.texture.id, texture_slot.texLoc, texture_slot.slot);
    }
    setupSpriteData(shader);

    while (!WindowShouldClose()) //Game loop
    {
        currentTime = GetTime();
        seconds_elapsed = currentTime - previousTime;
        previousTime = currentTime;

        player.handleMovement(moveSpeed, turnSpeed, seconds_elapsed);

        updateUniforms(shader, shaderLocations, player, verticalFactor);

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

void loadTextures(Shader shader)
{
    for (auto& slot : textures) {
        slot.texture = LoadTexture(slot.path);
        slot.texLoc  = GetShaderLocation(shader, slot.uniformName);
        SetTextureFilter(slot.texture, TEXTURE_FILTER_POINT);
        bindTextureSlot(shader, slot.texture.id, slot.texLoc, slot.slot);
    }
    crosshair = LoadTexture("../Assets/crosshair.png");
}



void updateUniforms(Shader shader,ShaderLocations shaderLocations, Player player, float verticalFactor)
{
    float playerPos[2] = {(float)player.position.x, (float)player.position.y};
    float playerDir[2] = {(float)player.direction.x, (float)player.direction.y};
    float camPlane[2]  = {(float)player.cameraPlane.x, (float)player.cameraPlane.y};
    float pixelOffset = verticalFactor * tan(player.pitch);
    SetShaderValue(shader, shaderLocations.pixelOffset, &pixelOffset, SHADER_UNIFORM_FLOAT);
    SetShaderValue(shader, shaderLocations.playerPosition, playerPos, SHADER_UNIFORM_VEC2);
    SetShaderValue(shader, shaderLocations.playerDirection, playerDir, SHADER_UNIFORM_VEC2);
    SetShaderValue(shader, shaderLocations.cameraPlane, camPlane, SHADER_UNIFORM_VEC2);
    SetShaderValue(shader, shaderLocations.verticalFactor, &verticalFactor, SHADER_UNIFORM_FLOAT);
}

float calculateVerticalFactor(Player player)
{
    // Compute vertical factor (focal length in pixels)
    float camPlaneLen = sqrt(player.cameraPlane.x * player.cameraPlane.x +
        player.cameraPlane.y * player.cameraPlane.y);
    float hfov = 2.0f * atan(camPlaneLen);                    // horizontal FOV
    float aspect = (float)screenHeight / (float)screenWidth;
    float vfov = 2.0f * atan(tan(hfov * 0.5f) * aspect);      // vertical FOV
    float verticalFactor = (screenHeight / 2.0f) / tan(vfov * 0.5f);
    return verticalFactor;
}

void passResolutionToShader(Shader shader)
{
    int resLocResolution = GetShaderLocation(shader, "resolution");
    float resolution[2] = {static_cast<float>(screenWidth),static_cast<float>(screenHeight)};
    SetShaderValue(shader, resLocResolution, resolution, SHADER_UNIFORM_VEC2);
}


void bindTextureSlot(
    const Shader shader, const unsigned textureID, const int texLoc, const int slot)
{
    rlActiveTextureSlot(slot);
    rlEnableTexture(textureID);
    SetShaderValue(shader, texLoc, &slot, SHADER_UNIFORM_INT);
}
ShaderLocations getShaderLocations(Shader shader) {
    return {
        GetShaderLocation(shader, "playerPosition"),
        GetShaderLocation(shader, "playerDirection"),
        GetShaderLocation(shader, "cameraPlane"),
        GetShaderLocation(shader, "pixelOffset"),
        GetShaderLocation(shader, "verticalFactor")
    };
}

void setupSpriteData(Shader shader) {
    int numberOfSprites = (int)sprites.size();
    float data[numberOfSprites * SPRITE_DATA_LENGTH];
    for (int i = 0; i < numberOfSprites*SPRITE_DATA_LENGTH; i+=SPRITE_DATA_LENGTH) {
        data[i + 0] = (float)sprites[i].pos.x;
        data[i + 1] = (float)sprites[i].pos.y;
        data[i + 2] = sprites[i].width;
        data[i + 3] = sprites[i].height;
        data[i + 4] = sprites[i].atlasIndex;
    }

    int loc = GetShaderLocation(shader, "spriteData");
    SetShaderValueV(shader, loc, data, SHADER_UNIFORM_FLOAT,numberOfSprites*SPRITE_DATA_LENGTH);

    loc = GetShaderLocation(shader, "numberOfSprites");
    SetShaderValue(shader, loc, &numberOfSprites, SHADER_UNIFORM_INT);
}