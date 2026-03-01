#version 330


out vec4 fragColor;
uniform vec2 resolution;
uniform vec2 playerDirection;
uniform vec2 playerPosition;
uniform vec2 cameraPlane;

uniform sampler2D brickTexture;
uniform sampler2D dirtTexture;
uniform sampler2D stoneTexture;
uniform sampler2D spruce_planksTexture;
uniform sampler2D dark_oak_logTexture;

const int MAP_W = 10;
const int MAP_H = 10;
const int map[100] = int[](
1,1,1,1,1,1,1,1,1,1,
1,0,2,0,1,0,0,0,0,1,
1,0,0,0,1,0,3,2,0,1,
1,0,0,0,1,0,0,0,0,1,
1,4,3,0,1,1,0,0,0,1,
1,0,5,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,4,0,1,
1,2,0,0,0,0,0,0,0,1,
1,1,1,1,1,1,1,1,1,1
);

int getMap(int x, int y) {
    if (x < 0 || x >= MAP_W || y < 0 || y >= MAP_H) return 1;
    return map[(y * MAP_W) + x];
}



void main() {
    float horizon = resolution.y/2;
    if (gl_FragCoord.y>horizon) fragColor = vec4(0.4, 0.75, 1.0, 1.0);
    else {
        float linearFactor = gl_FragCoord.y/horizon;
        float factor = 0.175*log(1-linearFactor);
        fragColor = vec4(0.1, 0.6, 0.05, 1.0)+vec4(factor, factor, factor, 0.0);
    }
    float cameraX = (-0.5 + gl_FragCoord.x/resolution.x);
    vec2 rayDirection = playerDirection + (cameraPlane*cameraX);
    ivec2 rayMapPosition = ivec2(int(playerPosition.x),int(playerPosition.y));
    vec2 pathDistanceForGridStep;
    ivec2 stepDir;
    vec2 sideDist;
    if (rayDirection.x==0) pathDistanceForGridStep.x=1e30;
    else pathDistanceForGridStep.x = abs(1/rayDirection.x);
    if (rayDirection.y==0) pathDistanceForGridStep.y=1e30;
    else pathDistanceForGridStep.y = abs(1/rayDirection.y);

    if (rayDirection.x<0) {
        stepDir.x = -1;
        sideDist.x = (playerPosition.x - rayMapPosition.x) * pathDistanceForGridStep.x;
    }
    else {
        stepDir.x=1;
        sideDist.x = (rayMapPosition.x + 1 - playerPosition.x) * pathDistanceForGridStep.x;
    }
    if (rayDirection.y<0){
        stepDir.y=-1;
        sideDist.y = (playerPosition.y - rayMapPosition.y) * pathDistanceForGridStep.y;
    }
    else {
        stepDir.y=1;
        sideDist.y = (rayMapPosition.y + 1 - playerPosition.y) * pathDistanceForGridStep.y;
    }

    int hit = 0;
    bool ySide = false;

    for (int i = 0; i<64; i++){
        if (sideDist.x<sideDist.y){
            sideDist.x += pathDistanceForGridStep.x;
            rayMapPosition.x += stepDir.x;
            ySide=false;
        }
        else{
            sideDist.y += pathDistanceForGridStep.y;
            rayMapPosition.y += stepDir.y;
            ySide=true;
        }

        hit = getMap(int(rayMapPosition.x),int(rayMapPosition.y));
        if (hit!=0) break;
    }
    float perpDistance = 0;
    if (ySide) perpDistance = sideDist.y - pathDistanceForGridStep.y;
    else perpDistance = sideDist.x - pathDistanceForGridStep.x;

    bool thisPixelIsWall;
    int wallHeight = int(resolution.y/perpDistance);
    int wallBottom = int(horizon - wallHeight/2);
    int wallTop = int(horizon + wallHeight/2);

    if (gl_FragCoord.y > wallTop || gl_FragCoord.y < wallBottom || hit==0) thisPixelIsWall=false;
    else thisPixelIsWall=true;

    vec4 material;
    if (thisPixelIsWall){
        float wallX;
        float textureX;
        float textureY;

        if (ySide) {
            wallX = playerPosition.x + perpDistance * rayDirection.x;
        }
        else{
        wallX = playerPosition.y + perpDistance * rayDirection.y;
        }
        wallX = wallX - floor(wallX);

        textureX = wallX;
        if ((ySide && rayDirection.y < 0) || (!ySide && rayDirection.x > 0)) {
            textureX = 1.0 - textureX; // Flip horizontally
        }

        textureY = (gl_FragCoord.y-wallBottom)/wallHeight;

        vec2 texCoords = vec2(textureX,textureY);

        switch (hit){
            case 1: material = texture(stoneTexture, texCoords); break;
            case 2: material = texture(brickTexture, texCoords); break;
            case 3: material = texture(dirtTexture, texCoords); break;
            case 4: material = texture(dark_oak_logTexture, texCoords); break;
            default: material = texture(spruce_planksTexture, texCoords); break;
        }



        float darkening = (0.7 - 10 / (perpDistance*perpDistance));
        if (darkening<0) darkening = 0;
        if (ySide) darkening+=0.2;

        fragColor.rgb = material.rgb* (1-darkening);
        fragColor.a=1.0;
    }

}



