#include "hero.h"
#include <cmath>
#include <algorithm>

// Constructor to initialize hero starting/destination coordinates and movement parameters
hero::hero(int sX, int sY, int dX, int dY):startX(sX),startY(sY),destX(dX),destY(dY), worldX(sX), worldY(sY){
    pathIndex = 0;
    isMoving = false;
    speed = 0.1f;
};

int hero::getStartX() const{
    return startX;
}

int hero::getStartY() const{
    return startY;
}

int hero::getDestX() const{
    return destX;
}

int hero::getDestY() const{
    return destY;
}

float hero::getWorldX() const{
    return worldX;
}

float hero::getWorldY() const{
    return worldY;
}

void hero::setStart(sf::Vector2i s){
    startX = s.x;
    startY = s.y;

    worldX = s.x;
    worldY = s.y;
    isMoving = false;
}

void hero::setDest(sf::Vector2i d){
    destX = d.x;
    destY = d.y;
}

std::vector<sf::Vector2i> hero::getPath() const{
    return path;
}

// Heuristic distance calculation (Manhattan distance) for A* pathfinding
float hero::distance(Node *a, Node *b){
    return abs(a->x - b->x) + abs(a->y - b->y);
}

// Core A* pathfinding algorithm considering terrain heights and maximum climb limits
void hero::heroFindPath(float **gridheights, int squares){
    path.clear();

    if (startX < 0 || startX >= squares || startY < 0 || startY >= squares
        || destX < 0 || destX >= squares || destY < 0 || destY >= squares){
            return;
    }

    float maxClimb = 10.0f;

    std::vector<std::vector<Node>> allnodes(squares, std::vector<Node>(squares));
    for(int x = 0; x < squares; x++){
        for(int y = 0; y < squares; y++){
            allnodes[x][y].x = x;
            allnodes[x][y].y = y;
            allnodes[x][y].height = gridheights[x][y];  
        }
    }

    std::vector<Node*> toVisit;
    std::vector<std::vector<bool>> visited(squares, std::vector<bool>(squares, false));

    Node *start = &allnodes[startX][startY];
    Node *end = &allnodes[destX][destY];

    toVisit.push_back(start);

    // A* loop exploring neighboring nodes based on lowest f cost
    while(!toVisit.empty()){
        Node *current = toVisit[0];
        int currentIndex = 0;

        for(int i = 1; i < toVisit.size(); i++){
            if(current->f > toVisit[i]->f){
                current = toVisit[i];
                currentIndex = i;
            }
        }

        toVisit.erase(toVisit.begin() + currentIndex);
        visited[current->x][current->y] = true;

        // If destination is reached, reconstruct and reverse the path vector
        if(current->x == end->x && current->y == end->y){
            while(current != nullptr){
                path.push_back(sf::Vector2i(current->x, current->y));
                current = current->parent;
            }
            std::reverse(path.begin(), path.end());

            return;
        }

        int dx[] = {1, -1, 0, 0};
        int dy[] = {0, 0, 1, -1};

        for(int i = 0; i < 4; i++){
            int neighborX = current->x + dx[i];
            int neighborY = current->y + dy[i];

            if((neighborX >= 0 && neighborX < squares) && (neighborY >= 0 && neighborY < squares)){
                if(visited[neighborX][neighborY]){
                    continue;
                }

                Node* neighbor = &allnodes[neighborX][neighborY];

                // Check if height difference exceeds climbable threshold
                float heightDiff = abs(neighbor->height - current->height);
                if(heightDiff > maxClimb){
                    continue;
                }

                int moveCost = current->g + (int)heightDiff + 10;

                bool inToVisit = false;
                for(int j = 0; j < toVisit.size(); j++){
                    if(neighbor->x == toVisit[j]->x && neighbor->y == toVisit[j]->y){
                        inToVisit = true;
                    }
                }

                if(!inToVisit || moveCost < neighbor->g){
                    neighbor->g = moveCost;
                    neighbor->h = distance(neighbor, end) * 10;
                    neighbor->f = neighbor->g + neighbor->h;
                    neighbor->parent = current;

                    if(!inToVisit){
                        toVisit.push_back(neighbor);
                    }
                }
            }
        }
    }
}

// Trigger hero movement along the calculated path
void hero::startMoving(){
    if (!isMoving && path.size() > 0) {
        isMoving = true;
        pathIndex = 0;
    }
}

// Smoothly interpolate hero world coordinates step-by-step along path nodes
void hero::updateMovement(){
    if(!isMoving || path.empty() || pathIndex >= path.size()){
        isMoving = false;
        return;
    }

    float targetX = path[pathIndex].x;
    float targetY = path[pathIndex].y;

    float dx = targetX - worldX;
    float dy = targetY - worldY;

    float distance = std::sqrt(dx * dx + dy * dy);

    if(distance <= speed){
        worldX = targetX;
        worldY = targetY;
        pathIndex++;

        if (pathIndex >= path.size()) {
            isMoving = false;
            startX = destX;
            startY = destY;
            path.clear();
        }
    }
    else{
        worldX += (dx / distance) * speed;
        worldY += (dy / distance) * speed;
    }
}