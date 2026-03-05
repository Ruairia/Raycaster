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
    float size;                // world height (e.g., 1.0)
    // color will be hardcoded in shader for now
};

static std::vector<Sprite> sprites = {
    {{1.5, 3.5}, 1.0f}      // a single sprite
};

static TextureSlot textures[] = {
    {1, {}, 0, "../Assets/bricks.png",       "brickTexture"},
  {2, {}, 0, "../Assets/dirt.png",          "dirtTexture"},
  {3, {}, 0, "../Assets/dark_oak_log.png",  "dark_oak_logTexture"},
  {4, {}, 0, "../Assets/spruce_planks.png", "spruce_planksTexture"},
  {5, {}, 0, "../Assets/stone.png",         "stoneTexture"},
  {6, {}, 0, "../Assets/grass.png",         "grassTexture"},
};

static Texture2D crosshair;

int main();
float calculateVerticalFactor(raycaster::Player player);
void updateUniforms(Shader shader, ShaderLocations shaderLocations, raycaster::Player player, float verticalFactor);
void passResolutionToShader(Shader shader);
void loadTextures(Shader shader);
void bindTextureSlot(const Shader shader, const unsigned textureID, const int texLoc, const int slot);
ShaderLocations getShaderLocations(const Shader shader);
void setupSpriteData(Shader shader);


#endif //RAYCASTER_MAIN_H
