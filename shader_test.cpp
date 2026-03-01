#include <raylib.h>
#include <rlgl.h>

int main()
{
    const int screenWidth = 800;
    const int screenHeight = 600;
    const int res[2]= {screenWidth,screenHeight};
    InitWindow(screenWidth, screenHeight, "Texture Shader Test");

    // Load shader (default vertex, custom fragment)
    Shader shader = LoadShader(nullptr, "shader_test.glsl");

    // Load texture
    Texture2D texture = LoadTexture("../Assets/bricks.png");

    // Get sampler location
    int texLoc = GetShaderLocation(shader, "tex");
    int resLoc = GetShaderLocation(shader, "resolution");

    // IMPORTANT: set sampler to texture unit 0
    int texUnit = 0;
    SetShaderValue(shader, texLoc, &texUnit, SHADER_UNIFORM_INT);
    SetShaderValueTexture(shader, texLoc, texture);
    SetTextureFilter(texture, TEXTURE_FILTER_POINT);
    SetShaderValue(shader,resLoc, res, SHADER_UNIFORM_VEC2);

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);

        BeginShaderMode(shader);

        // Bind texture to slot 0
        rlActiveTextureSlot(0);
        rlEnableTexture(texture.id);

        // Draw fullscreen quad
        DrawRectangle(0, 0, screenWidth, screenHeight, WHITE);

        EndShaderMode();

        EndDrawing();
    }

    UnloadTexture(texture);
    UnloadShader(shader);
    CloseWindow();
    return 0;
}
