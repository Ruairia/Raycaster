//
// Created by ruairi on 03/08/2025.
//

#ifndef MAP_H
#define MAP_H

namespace raycaster {

class Map {
public:
static constexpr int map[10][10] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 2, 0, 1, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 1, 0, 3, 2, 0, 1},
    {1, 0, 0, 0, 1, 0, 0, 0, 0, 1},
    {1, 4, 3, 0, 1, 1, 0, 0, 0, 1},
    {1, 0, 5, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 4, 0, 1},
    {1, 2, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

static int getSquare(int x, int y)
{
    return map[y][x];
    }

    static bool hasSquare(int x, int y) {
        bool returnValue;
        if (x>=0 && x<=9 && y>=0 && y<=9)
        {
            returnValue = true;
        }
        else returnValue = false;
        return returnValue;
    }
};

} // raycaster

#endif //MAP_H
