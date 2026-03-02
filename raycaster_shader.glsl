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

const float FOG_MAX = 0.7;
const float FOG_DISTANCE = 10.0;
const float SIDE_DARKENING = 0.2;
const int VIEW_DIST = 24;

const int WALL_STONE        = 1;
const int WALL_BRICK        = 2;
const int WALL_DIRT         = 3;
const int WALL_DARK_OAK_LOG = 4;

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

void main() {
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


    HitResult hitResult = performDDA(ray);



    int wallHeight = int(resolution.y/hitResult.perpDistance);
    int wallBottom = int(horizon - wallHeight/2);
    int wallTop = int(horizon + wallHeight/2);



    vec4 material;
    if (hitResult.wallType != 0 && gl_FragCoord.y >= wallBottom && gl_FragCoord.y <= wallTop) {
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

        textureY = (gl_FragCoord.y-wallBottom)/wallHeight;

        vec2 texCoords = 2*vec2(textureX,textureY);

        switch (hitResult.wallType) {
            case WALL_STONE:        material = texture(stoneTexture, texCoords);        break;
            case WALL_BRICK:        material = texture(brickTexture, texCoords);        break;
            case WALL_DIRT:         material = texture(dirtTexture, texCoords);         break;
            case WALL_DARK_OAK_LOG: material = texture(dark_oak_logTexture, texCoords); break;
            default:                material = texture(spruce_planksTexture, texCoords); break;
        }

        float darkening = max(0.0,FOG_MAX - FOG_DISTANCE / (hitResult.perpDistance*hitResult.perpDistance));

        if (hitResult.ySide) darkening+=SIDE_DARKENING;

        fragColor.rgb = material.rgb* (1-darkening);
        fragColor.a=1.0;
    }
    else {
        if (gl_FragCoord.y>horizon) {
            float linearFactor = (gl_FragCoord.y-horizon);
            float factor = log(linearFactor)*10;
            fragColor = vec4(0.1, 0.3, 0.8, 1.0)-vec4(0.01*factor, 0.04*factor, 0.01*factor, 0.0);
        }
        else{
            float distanceFromHorizon = horizon - gl_FragCoord.y;
            float rowDist = (0.5 * resolution.y) / distanceFromHorizon;
            vec2 worldPos = playerPosition + (rowDist * ray.direction);
            vec2 texCoord = 2*fract(worldPos);
            fragColor = texture(grassTexture, texCoord);
        }
    }
    ivec2 screenPos = ivec2(gl_FragCoord.xy);
    float threshold = getBayerValue(screenPos);
    fragColor.rgb += threshold / 64.0; // or subtract, depending on desired effect
}



