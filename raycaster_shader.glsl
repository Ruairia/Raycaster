#version 330


out vec4 fragColor;
uniform vec2 resolution;
uniform vec2 playerDirection;
uniform vec2 playerPosition;
uniform vec2 cameraPlane;

uniform float horizon;

uniform sampler2D brickTexture;
uniform sampler2D dirtTexture;
uniform sampler2D stoneTexture;
uniform sampler2D spruce_planksTexture;
uniform sampler2D dark_oak_logTexture;
uniform sampler2D grassTexture;

const int MAP_W = 24;
const int MAP_H = 24;
const int map[576] = int[](
8,8,8,8,8,8,8,8,8,8,8,4,4,6,4,4,6,4,6,4,4,4,6,4,
8,0,0,0,0,0,0,0,0,0,8,4,0,0,0,0,0,0,0,0,0,0,0,4,
8,0,3,3,0,0,0,0,0,8,8,4,0,0,0,0,0,0,0,0,0,0,0,6,
8,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,
8,0,3,3,0,0,0,0,0,8,8,4,0,0,0,0,0,0,0,0,0,0,0,4,
8,0,0,0,0,0,0,0,0,0,8,4,0,0,0,0,0,6,6,6,0,6,4,6,
8,8,8,8,0,8,8,8,8,8,8,4,4,4,4,4,4,6,0,0,0,0,0,6,
7,7,7,7,0,7,7,7,7,0,8,0,8,0,8,0,8,4,0,4,0,6,0,6,
7,7,0,0,0,0,0,0,7,8,0,8,0,8,0,8,8,6,0,0,0,0,0,6,
7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,6,0,0,0,0,0,4,
7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,6,0,6,0,6,0,6,
7,7,0,0,0,0,0,0,7,8,0,8,0,8,0,8,8,6,4,6,0,6,6,6,
7,7,7,7,0,7,7,7,7,8,8,4,0,6,8,4,8,3,3,3,0,3,3,3,
2,2,2,2,0,2,2,2,2,4,6,4,0,0,6,0,6,3,0,0,0,0,0,3,
2,2,0,0,0,0,0,2,2,4,0,0,0,0,0,0,4,3,0,0,0,0,0,3,
2,0,0,0,0,0,0,0,2,4,0,0,0,0,0,0,4,3,0,0,0,0,0,3,
1,0,0,0,0,0,0,0,1,4,4,4,4,4,6,0,6,3,3,0,0,0,3,3,
2,0,0,0,0,0,0,0,2,2,2,1,2,2,2,6,6,0,0,5,0,5,0,5,
2,2,0,0,0,0,0,2,2,2,0,0,0,2,2,0,5,0,5,0,0,0,5,5,
2,0,0,0,0,0,0,0,2,0,0,0,0,0,2,5,0,5,0,5,0,5,0,5,
1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,
2,0,0,0,0,0,0,0,2,0,0,0,0,0,2,5,0,5,0,5,0,5,0,5,
2,2,0,0,0,0,0,2,2,2,0,0,0,2,2,0,5,0,5,0,0,0,5,5,
2,2,2,2,1,2,2,2,2,2,2,1,2,2,2,5,5,5,5,5,5,5,5,5
);


int getMap(int x, int y) {
    if (x < 0 || x >= MAP_W || y < 0 || y >= MAP_H) return 1;
    return map[(y * MAP_W) + x];
}

float bayer4[16] = float[](
0.0/16.0,  8.0/16.0,  2.0/16.0, 10.0/16.0,
12.0/16.0,  4.0/16.0, 14.0/16.0,  6.0/16.0,
3.0/16.0, 11.0/16.0,  1.0/16.0,  9.0/16.0,
15.0/16.0,  7.0/16.0, 13.0/16.0,  5.0/16.0
);

float getBayerValue(ivec2 pos) {
    int idx = (pos.x & 3) + (pos.y & 3) * 4;
    return bayer4[idx];
}



void main() {
     fragColor = vec4(0.4, 0.75, 1.0, 1.0);
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
    else {
        if (gl_FragCoord.y>horizon) {
            float linearFactor = (resolution.y-gl_FragCoord.y)/(horizon+1);
            float factor = log(linearFactor);
            fragColor = vec4(0.1, 0.3, 0.8, 1.0)+vec4(0.01*factor, 0.04*factor, 0.01*factor, 0.0);
        }
        else{
            float distanceFromHorizon = horizon - gl_FragCoord.y;
            float rowDist = (0.5 * resolution.y) / distanceFromHorizon;
            vec2 worldPos = playerPosition + (rowDist * rayDirection);
            ivec2 cell = ivec2(floor(worldPos));
            vec2 texCoord = fract(worldPos);
            fragColor = texture(grassTexture, texCoord);
        }
    }
    ivec2 screenPos = ivec2(gl_FragCoord.xy);
    float threshold = getBayerValue(screenPos);
    fragColor.rgb += threshold / 128.0; // or subtract, depending on desired effect
}



