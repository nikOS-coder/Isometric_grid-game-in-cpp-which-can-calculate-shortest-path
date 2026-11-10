#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

// Node structure used for the A* pathfinding algorithm
typedef struct node{
    int x, y;
    float height;
    struct node* parent = nullptr;
    int g = 0, h = 0, f = 0;
}Node;

// Hero class managing character positioning, movement interpolation, and A* path calculation
class hero{
    private:
        int startX, startY;
        int destX, destY;

        std::vector<sf::Vector2i> path;

        float worldX, worldY;
        bool isMoving;
        int pathIndex;
        float speed;
    public:
        hero(int, int, int, int);
        int getStartX() const;
        int getStartY() const;
        int getDestX() const;
        int getDestY() const;
        float getWorldX() const;
        float getWorldY() const;

        void setStart(sf::Vector2i);
        void setDest(sf::Vector2i);

        void heroFindPath(float**, int);
        std::vector<sf::Vector2i> getPath() const;
        float distance(Node*, Node*);
        void startMoving();
        void updateMovement();
};