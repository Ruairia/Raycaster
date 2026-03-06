#version 330

#define MAX_SPRITES 64
#define SPRITE_DATA_LENGTH 4
uniform int numberOfSprites;
uniform vec4 spriteData[MAX_SPRITES];   // x, y, size, unused

out vec4 fragColour;
uniform vec2 resolution;
uniform vec2 playerDirection;
uniform vec2 playerPosition;
uniform vec2 cameraPlane;
uniform float pixelOffset;
uniform float verticalFactor;

uniform sampler2D brickTexture;
uniform sampler2D dirtTexture;
uniform sampler2D stoneTexture;
uniform sampler2D spruce_planksTexture;
uniform sampler2D dark_oak_logTexture;
uniform sampler2D grassTexture;




const float DARKEN_MAX = 0.8;
const float DARKEN_DISTANCE = 6.0;
const float SIDE_DARKENING = 0.2;
const int VIEW_DIST = 48;
const float PLAYER_HEIGHT = 0.55;

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


const int WALL_STONE        = 1;
const int WALL_BRICK        = 2;
const int WALL_DIRT         = 3;
const int WALL_DARK_OAK_LOG = 4;

struct Ray {
    vec2 direction;
    ivec2 mapPos;
    vec2 sideDist;
    vec2 pathDistanceForGridStep;
    ivec2 stepDir;
};

struct HitResult {
    int wallType;
    bool ySide;
    float perpDistance;
};

Ray setupRay(vec2 playerPosition, vec2 playerDirection, vec2 cameraPlane);
HitResult performDDA(Ray ray);
float calcDarkening(float distance);
vec2 getWallTextureCoordinates(HitResult hitResult, Ray ray, float wallBottom, float wallTop, float horizon);
vec3 sampleWallTexture(HitResult hitResult, vec2 texCoords);
vec3 calcSkyColour(float horizon);
vec3 calcGroundColour(float horizon, Ray ray);
float getBayerValue(ivec2 pos);

void main() {
    fragColour.a=1.0;

    float horizon = resolution.y/2 + pixelOffset;

    Ray ray = setupRay(playerPosition, playerDirection, cameraPlane);
    HitResult hitResult = performDDA(ray);

    float wallHeight = verticalFactor/hitResult.perpDistance;
    int wallBottom = int(horizon - wallHeight*(PLAYER_HEIGHT));
    int wallTop = int(horizon + 2*wallHeight* (1-PLAYER_HEIGHT));
    float euclDistance = (hitResult.wallType !=0) ? hitResult.perpDistance * length(ray.direction) : 1e10;

    float closestDist = hitResult.perpDistance;

    {
        vec4 material;
        if (hitResult.wallType != 0 && gl_FragCoord.y >= wallBottom && gl_FragCoord.y <= wallTop) {

            vec2 texCoords = getWallTextureCoordinates(hitResult, ray, wallBottom, wallTop, horizon);



            float darkening = calcDarkening(euclDistance);

            if (hitResult.ySide) darkening+=SIDE_DARKENING;

            fragColour.rgb = sampleWallTexture(hitResult, texCoords) * (1-darkening);
        }
        else if (gl_FragCoord.y>horizon) {
            fragColour.rgb=calcSkyColour(horizon);
            closestDist=1e10;
        }
        else {
            fragColour.rgb = calcGroundColour(horizon, ray);
            closestDist=1e10;
        }
    }

    for (int spriteIndex = 0; spriteIndex < numberOfSprites; spriteIndex++) {
        float thisSpriteData[SPRITE_DATA_LENGTH];
        for (int spriteDataIndex = 0; spriteDataIndex < SPRITE_DATA_LENGTH; spriteDataIndex++){
            thisSpriteData[spriteDataIndex] = spriteData[spriteIndex][spriteDataIndex];
        }
        vec2 spritePosition = vec2(thisSpriteData[0], thisSpriteData[1]);
        float spriteWidth = thisSpriteData[2];
        float spriteHeight = thisSpriteData[3];


        vec2 displacementToSprite = spritePosition - playerPosition;

        float cameraPlaneDeterminant = (cameraPlane.x * playerDirection.y - playerDirection.x * cameraPlane.y);
        float cameraSpaceLateral = 2* (playerDirection.y * displacementToSprite.x - playerDirection.x * displacementToSprite.y) / (cameraPlaneDeterminant);
        float cameraSpaceDepth = (-cameraPlane.y * displacementToSprite.x + cameraPlane.x * displacementToSprite.y) / cameraPlaneDeterminant;

        if (cameraSpaceDepth <= 0.1) continue;   // behind camera

        // Screen X position and size (in pixels)
        float spriteScreenX = (resolution.x / 2.0) * (1.0 + cameraSpaceLateral / cameraSpaceDepth);
        float spriteScaling = verticalFactor / cameraSpaceDepth;
        float spriteHeightOnScreen = spriteHeight * spriteScaling;
        float spriteWidthOnScreen = spriteWidth * spriteScaling;
        float left = spriteScreenX - 0.5*spriteWidthOnScreen;
        float right = spriteScreenX + 0.5*spriteWidthOnScreen;

        if (gl_FragCoord.x < left || gl_FragCoord.x > right) continue;

        float screenBottom = horizon - (PLAYER_HEIGHT) * verticalFactor / cameraSpaceDepth;
        float screenTop    = horizon + (spriteHeight - PLAYER_HEIGHT) * verticalFactor / cameraSpaceDepth;

        float yStart = screenBottom;
        float yEnd = screenTop;

        if (gl_FragCoord.y < yStart || gl_FragCoord.y > yEnd) continue;


        if ( cameraSpaceDepth < closestDist) fragColour.rgb = vec3(1.0, 0.0, 0.0) * (1-calcDarkening(length(displacementToSprite)));
        break;   // only one sprite, we can stop after first hit
    }


    ivec2 screenPos = ivec2(gl_FragCoord.xy);
    float dithering = getBayerValue(screenPos);
    fragColour.rgb += dithering / 64.0;
}



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





HitResult performDDA(Ray ray){
    HitResult hitResult;
    for (int i = 0; i<VIEW_DIST; i++){
        if (ray.sideDist.x<ray.sideDist.y){
            ray.sideDist.x += ray.pathDistanceForGridStep.x;
            ray.mapPos.x += ray.stepDir.x;
            hitResult.ySide=false;
        }
        else{
            ray.sideDist.y += ray.pathDistanceForGridStep.y;
            ray.mapPos.y += ray.stepDir.y;
            hitResult.ySide=true;
        }

        hitResult.wallType = getMap(int(ray.mapPos.x),int(ray.mapPos.y));
        if (hitResult.wallType!=0) break;
    }
    if (hitResult.ySide) hitResult.perpDistance = ray.sideDist.y - ray.pathDistanceForGridStep.y;
    else hitResult.perpDistance = ray.sideDist.x - ray.pathDistanceForGridStep.x;
    return hitResult;
}

Ray setupRay(vec2 playerPosition, vec2 playerDirection, vec2 cameraPlane){
    float cameraX = (-0.5 + gl_FragCoord.x/resolution.x);

    Ray ray;
    ray.direction = playerDirection + (cameraPlane*cameraX);
    ray.mapPos = ivec2(int(playerPosition.x),int(playerPosition.y));

    if (ray.direction.x==0) ray.pathDistanceForGridStep.x=1e30;
    else ray.pathDistanceForGridStep.x = abs(1/ray.direction.x);
    if (ray.direction.y==0) ray.pathDistanceForGridStep.y=1e30;
    else ray.pathDistanceForGridStep.y = abs(1/ray.direction.y);

    if (ray.direction.x<0) {
        ray.stepDir.x = -1;
        ray.sideDist.x = (playerPosition.x - ray.mapPos.x) * ray.pathDistanceForGridStep.x;
    }
    else {
        ray.stepDir.x=1;
        ray.sideDist.x = (ray.mapPos.x + 1 - playerPosition.x) * ray.pathDistanceForGridStep.x;
    }
    if (ray.direction.y<0){
        ray.stepDir.y=-1;
        ray.sideDist.y = (playerPosition.y - ray.mapPos.y) * ray.pathDistanceForGridStep.y;
    }
    else {
        ray.stepDir.y=1;
        ray.sideDist.y = (ray.mapPos.y + 1 - playerPosition.y) * ray.pathDistanceForGridStep.y;
    }
    return ray;
}

float calcDarkening(float distance){
    return max(0.0,DARKEN_MAX - DARKEN_DISTANCE / (distance*distance));
}

vec2 getWallTextureCoordinates(HitResult hitResult, Ray ray, float wallBottom, float wallTop, float horizon){
    float wallX;
    float textureX;
    float textureY;

    if (hitResult.ySide) {
        wallX = playerPosition.x + hitResult.perpDistance * ray.direction.x;
    }
    else{
        wallX = playerPosition.y + hitResult.perpDistance * ray.direction.y;
    }
    wallX = wallX - floor(wallX);

    textureX = wallX;
    if ((hitResult.ySide && ray.direction.y < 0) || (!hitResult.ySide && ray.direction.x > 0)) {
        textureX = 1.0 - textureX; // Flip horizontally
    }

    textureY = (gl_FragCoord.y-wallBottom)/(wallTop-wallBottom);

    return 2*vec2(textureX,textureY);
}

vec3 sampleWallTexture(HitResult hitResult, vec2 texCoords){
    vec3 material;
    switch (hitResult.wallType) {
        case WALL_STONE:        material = texture(stoneTexture, texCoords).rgb;        break;
        case WALL_BRICK:        material = texture(brickTexture, texCoords).rgb;        break;
        case WALL_DIRT:         material = texture(dirtTexture, texCoords).rgb;         break;
        case WALL_DARK_OAK_LOG: material = texture(dark_oak_logTexture, texCoords).rgb; break;
        default:                material = texture(spruce_planksTexture, texCoords).rgb; break;
    }
    return material;
}

vec3 calcSkyColour(float horizon){
    float linearFactor = (gl_FragCoord.y-horizon);
    float factor = log(linearFactor)*10;
    return vec3(0.1, 0.3, 0.8)-vec3(0.01*factor, 0.04*factor, 0.01*factor);
}

vec3 calcGroundColour(float horizon, Ray ray){
    float distanceFromHorizon = horizon - gl_FragCoord.y;
    float rowDist = (PLAYER_HEIGHT * verticalFactor) / distanceFromHorizon;
    vec2 worldPos = playerPosition + (rowDist * ray.direction);
    vec2 texCoord = 2*fract(worldPos);
    return texture(grassTexture, texCoord).rgb*(1-calcDarkening(rowDist * length(ray.direction)));
}




