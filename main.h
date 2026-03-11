//
// Created by ruairi on 05/03/2026.
//

#ifndef RAYCASTER_MAIN_H
#define RAYCASTER_MAIN_H
#include <raylib.h>
#include <vector>

#include "Player.h"

struct TextureSlot
{
    int slot;
    Texture2D texture;
    int texLoc;
    const char* path;
    const char* uniformName;
};

struct ShaderLocations
{
    int playerPosition;
    int playerDirection;
    int cameraPlane;
    int pixelOffset;
    int verticalFactor;
};

struct Sprite {
    raycaster::Vector2D pos;   // world position (x, y)
    float width;
    float height;
    int atlasIndex;
};

static std::vector<Sprite> allSprites = {
    {{1.5, 3.5}, 1.0, 1.0f, 0},
    {{2.5, 4.5}, 1.0, 1.0, 0},
    {{6.5, 4.5}, 1.0, 1.0, 0},
    {{5.5, 4.5}, 1.0, 1.0, 0},
    {{1.5, 1.5}, 1.0, 1.0, 0}
};

static TextureSlot textures[] = {
    {1, {}, 0, "../Assets/bricks.png",       "brickTexture"},
  {2, {}, 0, "../Assets/dirt.png",          "dirtTexture"},
  {3, {}, 0, "../Assets/dark_oak_log.png",  "dark_oak_logTexture"},
  {4, {}, 0, "../Assets/spruce_planks.png", "spruce_planksTexture"},
  {5, {}, 0, "../Assets/stone.png",         "stoneTexture"},
  {6, {}, 0, "../Assets/grass.png",         "grassTexture"},
    {7, {}, 0, "../Assets/spriteAtlas.png", "spriteAtlas"}
};

static Texture2D crosshair;

int main();
float calculateFocalLength(raycaster::Player player);
void updateUniforms(Shader shader, ShaderLocations shaderLocations, raycaster::Player player, float verticalFactor);
void passResolutionToShader(Shader shader);
void loadTextures(Shader shader);
void bindTextureSlot(const Shader shader, const unsigned textureID, const int texLoc, const int slot);
ShaderLocations getShaderLocations(const Shader shader);
void setupSpriteData(Shader shader);
void updateSprites(raycaster::Player player);


#endif //RAYCASTER_MAIN_H
