#version 330


out vec4 fragColor;
uniform float value;
uniform vec2 resolution;
uniform vec2 playerDirection;
uniform vec2 playerPosition;
uniform vec2 cameraPlane;

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
    if (gl_FragCoord.y>resolution.y/2) fragColor = vec4(0.4, 0.75, 1.0, 1.0);
    else {
        float linearFactor = gl_FragCoord.y/resolution.y;
        float factor = 0.475*log(1-linearFactor);
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
        sideDist.y = (playerPosition.y + 1 - rayMapPosition.y) * pathDistanceForGridStep.y;
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

    if (abs(gl_FragCoord.y-resolution.y/2)>wallHeight/2 || hit==0) thisPixelIsWall=false;
    else thisPixelIsWall=true;

    vec4 material;
    if (thisPixelIsWall){
        switch (hit){
            case 1: material = vec4(1.0,1.0,1.0,1.0); break;
            case 2: material = vec4(1.0,0.0,0.0,1.0); break;
            case 3: material = vec4(0.0,1.0,0.0,1.0); break;
            case 4: material = vec4(0.0,0.0,1.0,1.0); break;
            default: material = vec4(0.4,0.4,0.4,1.0); break;
        }
        fragColor=material;
    }

}

