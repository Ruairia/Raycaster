#include <string>
#include <vector>
#include <raylib.h>
#include "Vector2D.h"
#include "rlgl.h"
#include "Player.h"

using namespace raycaster;
 int screenWidth = 800;
 int screenHeight = 450;

struct Sprite
{
    Vector2D position;
    float width;
    float height;
    float atlasIndex;
};

struct SpriteUniforms
{
    int countLoc;
    int positionsLoc;
    int atlasIndicesLoc;
    int atlasTextureLoc;
};

std::vector<Sprite> sprites = {
    { {.5f, 1.5f}, 1.0f, 2.0f, 0 },  // position, width, height, atlas index
};

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
  {6, {}, 0, "../Assets/grass.png",         "grassTexture"},
    {7,{}, 0, "../Assets/spriteAtlas.png", "spriteAtlas"}
};

struct ShaderLocations
{
    int playerPosition;
    int playerDirection;
    int cameraPlane;
    int pixelOffset;
    int verticalFactor;
    int spriteAtlasIndices;
    int spriteCount;
    int spritePositions;
};

ShaderLocations getShaderLocations(Shader shader) {
    return {
        GetShaderLocation(shader, "playerPosition"),
        GetShaderLocation(shader, "playerDirection"),
        GetShaderLocation(shader, "cameraPlane"),
        GetShaderLocation(shader, "pixelOffset"),
        GetShaderLocation(shader, "verticalFactor"),
        GetShaderLocation(shader, "spriteAtlasIndices"),
        GetShaderLocation(shader, "spriteCount"),
        GetShaderLocation(shader, "spritePositions")
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

void loadSprites(Shader shader)
{
    int spriteAtlasWidth = 494;
    int spriteAtlasHeight = 505;

    int atlasTileWidth = 494;
    int atlasTileHeight = 505;

    float tileW = (float)atlasTileWidth / spriteAtlasWidth;
    float tileH = (float)atlasTileHeight / spriteAtlasHeight;

    SetShaderValue(shader, GetShaderLocation(shader, "spriteTileWidth"), &tileW, SHADER_UNIFORM_FLOAT);
    SetShaderValue(shader, GetShaderLocation(shader, "spriteTileHeight"), &tileH, SHADER_UNIFORM_FLOAT);
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


    float focalDist = 1.32;
    auto player = Player ({1.5,1.5},{0,-1},{focalDist,0});

    float verticalFactor = calculateVerticalFactor(player);
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



    while (!WindowShouldClose()) //Game loop
    {
        currentTime = GetTime();
        seconds_elapsed = currentTime - previousTime;
        previousTime = currentTime;

        player.handleMovement(moveSpeed, turnSpeed, seconds_elapsed);

        int spriteCount = (int)sprites.size();
        SetShaderValue(shader, shaderLocations.spriteCount, &spriteCount, SHADER_UNIFORM_INT);

        // Prepare arrays for positions and indices
        std::vector<Vector2D> positions;
        std::vector<int> indices;
        for (const auto& s : sprites) {
            positions.push_back(s.position);
            indices.push_back((int)s.atlasIndex);
        }

        // Upload as arrays
        SetShaderValueV(shader, shaderLocations.spritePositions, positions.data(), SHADER_UNIFORM_VEC2, spriteCount);
        SetShaderValueV(shader, shaderLocations.spriteAtlasIndices, indices.data(), SHADER_UNIFORM_INT, spriteCount);

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
