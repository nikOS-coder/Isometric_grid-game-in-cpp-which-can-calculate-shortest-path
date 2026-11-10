#pragma once
#include "mywindow.h"
#include "hero.h"
#include <cmath>
#include <vector>

// Core game class handling isometric grid rendering, heightmaps, camera controls, and mouse input
class game{
    private:
        sf::ConvexShape upperTile;      // Top face polygon of an isometric block
        sf::ConvexShape leftTile;       // Left side face polygon
        sf::ConvexShape rightTile;      // Right side face polygon
        float tileLength, tileWidth, blockHeight;
        float **gridHeights;            // 2D dynamic array storing height values for each grid coordinate
        int squares;                    // Grid dimensions (squares x squares)
        float isoMat2x2[2][2];          // Isometric transformation matrix

        hero myhero;                    // Pathfinding hero agent instance
    public:
        game(int, float);
        ~game();
        void mainLoop(mywindow&);
        sf::Vector2f mat2x2xv(float m[2][2], sf::Vector2f);
        void reverseMat(float m[2][2], float rm[2][2]);
};