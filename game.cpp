#include "game.h"

// Destructor to clean up dynamically allocated 2D grid heights memory
game::~game(){
    for(int i = 0; i < squares; i++){
        delete[] gridHeights[i];
    }
    delete[] gridHeights;
}

// Constructor to initialize grid parameters, tile shapes, colors, and dynamic height matrix
game::game(int s, float b):squares(s), blockHeight(b), myhero(1, 1, 6, 6){
    upperTile.setPointCount(4);
    upperTile.setOutlineThickness(1.0f);
    upperTile.setFillColor(sf::Color(200, 200, 200));
    upperTile.setOutlineColor(sf::Color(50, 50, 50));

    leftTile.setPointCount(4);
    leftTile.setOutlineThickness(1.0f);
    leftTile.setFillColor(sf::Color(150, 150, 150));
    leftTile.setOutlineColor(sf::Color(50, 50, 50));

    rightTile.setPointCount(4);
    rightTile.setOutlineThickness(1.0f);
    rightTile.setFillColor(sf::Color(100, 100, 100));
    rightTile.setOutlineColor(sf::Color(50, 50, 50));

    gridHeights = new float*[squares];
    for(int i = 0; i < squares; i++){
        gridHeights[i] = new float[squares];
    }

    for(int i = 0; i < squares; i++){
        for(int j = 0; j < squares; j++){
            gridHeights[i][j] = 0;
        }
    }
};

// Main application loop managing events, camera zoom/pan, height editing, pathfinding inputs, and rendering
void game::mainLoop(mywindow& window){
    sf::View camera = window.getWindow().getDefaultView();
    window.getWindow().setFramerateLimit(60);
    while(window.getWindow().isOpen()){
        sf::Event event;
        while(window.getWindow().pollEvent(event)){
            if(event.type == sf::Event::Closed){
                window.getWindow().close();
            }

            sf::Vector2i pixelPos = sf::Mouse::getPosition(window.getWindow());
            sf::Vector2f beforeZoom = window.getWindow().mapPixelToCoords(pixelPos, camera);
        
            // Handle mouse wheel zoom with smooth cursor anchor tracking
            if(event.type == sf::Event::MouseWheelMoved){
                if(event.mouseWheel.delta > 0){
                    camera.zoom(0.9f);
                }
                else{
                    camera.zoom(1.1f);
                }

                sf::Vector2f afterZoom = window.getWindow().mapPixelToCoords(pixelPos, camera);
                camera.move(beforeZoom.x - afterZoom.x, beforeZoom.y - afterZoom.y);
            }

            // Handle arrow keys for camera panning
            float cameraSpeed = 10.0f;
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left))  camera.move(-cameraSpeed, 0);
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right))  camera.move(cameraSpeed, 0);
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up))  camera.move(0, -cameraSpeed);
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down))  camera.move(0, cameraSpeed);
        }

        // Trigger hero movement along path upon Space key press
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
                myhero.startMoving();
            }
        myhero.updateMovement();

        window.getWindow().clear(sf::Color::Black);

        window.getWindow().setView(camera);

        tileLength = 64.0f;
        tileWidth = 32.0f;

        // Setup isometric 2x2 projection matrix
        isoMat2x2[0][0] = 0.5 * tileLength;
        isoMat2x2[0][1] = -0.5 * tileLength;
        isoMat2x2[1][0] = 0.25 * tileWidth;
        isoMat2x2[1][1] = 0.25 * tileWidth;

        float reversedIsoMat2x2[2][2];
        reverseMat(isoMat2x2, reversedIsoMat2x2);

        sf::Vector2f p0 = mat2x2xv(isoMat2x2, sf::Vector2f(0.0f, 0.0f));
        sf::Vector2f p1 = mat2x2xv(isoMat2x2, sf::Vector2f(1.0f, 0.0f));
        sf::Vector2f p2 = mat2x2xv(isoMat2x2, sf::Vector2f(1.0f, 1.0f));
        sf::Vector2f p3 = mat2x2xv(isoMat2x2, sf::Vector2f(0.0f, 1.0f));

        float shiftX = tileLength / 2.0f;
        p0.x += shiftX; p1.x += shiftX; p2.x += shiftX; p3.x += shiftX;

        upperTile.setPoint(0, p0);
        upperTile.setPoint(1, p1);
        upperTile.setPoint(2, p2);
        upperTile.setPoint(3, p3);

        leftTile.setPoint(0, p3);
        leftTile.setPoint(1, p2);
        leftTile.setPoint(2, sf::Vector2f(p2.x, p2.y + blockHeight));
        leftTile.setPoint(3, sf::Vector2f(p3.x, p3.y + blockHeight));

        rightTile.setPoint(0, p2);
        rightTile.setPoint(1, p1);
        rightTile.setPoint(2, sf::Vector2f(p1.x, p1.y + blockHeight));
        rightTile.setPoint(3, sf::Vector2f(p2.x, p2.y + blockHeight));

        float startX = (window.getWindow().getSize().x / 2.0f - tileLength / 2.0f);
        float startY = window.getWindow().getSize().y / 4.0f;

        // Perform mouse picking to map screen coordinates to isometric grid space
        sf::Vector2i pixelPos = sf::Mouse::getPosition(window.getWindow());
        sf::Vector2f worldPos = window.getWindow().mapPixelToCoords(pixelPos, camera);

        float dx = (float)worldPos.x - (startX + tileLength / 2.0f);
        float dy = (float)worldPos.y - startY;

        sf::Vector2f shiftMousePos(dx, dy);
        shiftMousePos = mat2x2xv(reversedIsoMat2x2, shiftMousePos);
        shiftMousePos.x = std::floor(shiftMousePos.x); 
        shiftMousePos.y = std::floor(shiftMousePos.y);

        int hoverX = (int)shiftMousePos.x;
        int hoverY = (int)shiftMousePos.y;

        // Handle user interaction: Shift+Click for Start point, Ctrl+Click for Destination point, or Left/Right click for height modification
        if((hoverX >= 0 && hoverX < squares) && (hoverY >= 0 && hoverY < squares)){
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) && sf::Mouse::isButtonPressed(sf::Mouse::Left)){
                myhero.setStart(sf::Vector2i(hoverX, hoverY));
                myhero.heroFindPath(gridHeights, squares);
            }
            else if(sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) && sf::Mouse::isButtonPressed(sf::Mouse::Left)){
                myhero.setDest(sf::Vector2i(hoverX, hoverY));
                myhero.heroFindPath(gridHeights, squares);
            }
            else{
                if(!((hoverX == myhero.getStartX() && hoverY == myhero.getStartY()) || (hoverX == myhero.getDestX() && hoverY == myhero.getDestY()))){
                    if(sf::Mouse::isButtonPressed(sf::Mouse::Left)){
                        gridHeights[hoverX][hoverY] -= 2.0f;
                    }
                    if(sf::Mouse::isButtonPressed(sf::Mouse::Right)){
                        if(gridHeights[hoverX][hoverY] < 0.0f){
                            gridHeights[hoverX][hoverY] += 2.0f;
                        }
                    }
                }
            }
        }
        
        // Render all isometric grid tiles with height modifications and path coloring
        for(int y = 0; y < squares; y++){
            for(int x = 0; x < squares; x++){

                float screenX = startX + (x - y) * tileLength / 2.0f;
                float screenY = startY + (x + y) * tileWidth / 4.0f;

                float h = gridHeights[x][y];

                upperTile.setPoint(0, sf::Vector2f(p0.x, p0.y + h));
                upperTile.setPoint(1, sf::Vector2f(p1.x, p1.y + h));
                upperTile.setPoint(2, sf::Vector2f(p2.x, p2.y + h));
                upperTile.setPoint(3, sf::Vector2f(p3.x, p3.y + h));

                leftTile.setPoint(0, sf::Vector2f(p3.x, p3.y + h));
                leftTile.setPoint(1, sf::Vector2f(p2.x, p2.y + h));
                leftTile.setPoint(2, sf::Vector2f(p2.x, p2.y + blockHeight));
                leftTile.setPoint(3, sf::Vector2f(p3.x, p3.y + blockHeight));

                rightTile.setPoint(0, sf::Vector2f(p2.x, p2.y + h));
                rightTile.setPoint(1, sf::Vector2f(p1.x, p1.y + h));
                rightTile.setPoint(2, sf::Vector2f(p1.x, p1.y + blockHeight));
                rightTile.setPoint(3, sf::Vector2f(p2.x, p2.y + blockHeight));

                sf::Vector2f offset(screenX, screenY);
                upperTile.setPosition(offset);
                leftTile.setPosition(offset);
                rightTile.setPosition(offset);

                bool inPath = false;
                std::vector<sf::Vector2i> currentPath = myhero.getPath();
                for(int j = 0; j < currentPath.size(); j++){
                    if(currentPath[j].x == x && currentPath[j].y == y){
                        inPath = true;
                    }
                }

                // Apply tile colors based on hover state, start/destination points, path, and defaults
                if(x == shiftMousePos.x && y == shiftMousePos.y){
                    upperTile.setFillColor(sf::Color(255, 255, 100));
                    leftTile.setFillColor(sf::Color(255, 255, 100));
                    rightTile.setFillColor(sf::Color(255, 255, 100));
                }
                else if(x == myhero.getStartX() && y == myhero.getStartY()){
                    upperTile.setFillColor(sf::Color(100, 255, 100));
                    leftTile.setFillColor(sf::Color(70, 180, 70));
                    rightTile.setFillColor(sf::Color(40, 120, 40));
                }
                else if(x == myhero.getDestX() && y == myhero.getDestY()){
                    upperTile.setFillColor(sf::Color(255, 100, 100));
                    leftTile.setFillColor(sf::Color(180, 70, 70));
                    rightTile.setFillColor(sf::Color(120, 40, 40));
                }
                else if(inPath){
                    upperTile.setFillColor(sf::Color::Blue);
                    leftTile.setFillColor(sf::Color(0, 0, 200));
                    rightTile.setFillColor(sf::Color(0, 0, 150));
                }
                else{
                    upperTile.setFillColor(sf::Color(200, 200, 200));
                    leftTile.setFillColor(sf::Color(150, 150, 150));
                    rightTile.setFillColor(sf::Color(100, 100, 100));
                }

                window.getWindow().draw(leftTile);
                window.getWindow().draw(rightTile);
                window.getWindow().draw(upperTile);
            }
        }

        // Render the hero character agent entity on top of the grid
        float hx = myhero.getWorldX();
        float hy = myhero.getWorldY();

        int gridX = (int)(hx + 0.5f);
        int gridY = (int)(hy + 0.5f);

        if (gridX >= 0 && gridX < squares && gridY >= 0 && gridY < squares) {
            
            float heroScreenX = startX + (hx - hy) * tileLength / 2.0f;
            float heroScreenY = startY + (hx + hy) * tileWidth / 4.0f;

            float groundHeight = gridHeights[gridX][gridY];
            float tallness = 20.0f; 

            upperTile.setPoint(0, sf::Vector2f(p0.x, p0.y + groundHeight - tallness));
            upperTile.setPoint(1, sf::Vector2f(p1.x, p1.y + groundHeight - tallness));
            upperTile.setPoint(2, sf::Vector2f(p2.x, p2.y + groundHeight - tallness));
            upperTile.setPoint(3, sf::Vector2f(p3.x, p3.y + groundHeight - tallness));

            leftTile.setPoint(0, sf::Vector2f(p3.x, p3.y + groundHeight - tallness));
            leftTile.setPoint(1, sf::Vector2f(p2.x, p2.y + groundHeight - tallness));
            leftTile.setPoint(2, sf::Vector2f(p2.x, p2.y + groundHeight));
            leftTile.setPoint(3, sf::Vector2f(p3.x, p3.y + groundHeight));

            rightTile.setPoint(0, sf::Vector2f(p2.x, p2.y + groundHeight - tallness));
            rightTile.setPoint(1, sf::Vector2f(p1.x, p1.y + groundHeight - tallness));
            rightTile.setPoint(2, sf::Vector2f(p1.x, p1.y + groundHeight));
            rightTile.setPoint(3, sf::Vector2f(p2.x, p2.y + groundHeight));

            sf::Vector2f heroOffset(heroScreenX, heroScreenY);
            upperTile.setPosition(heroOffset);
            leftTile.setPosition(heroOffset);
            rightTile.setPosition(heroOffset);

            upperTile.setFillColor(sf::Color(200, 50, 200));
            leftTile.setFillColor(sf::Color(150, 30, 150));
            rightTile.setFillColor(sf::Color(100, 20, 100));

            window.getWindow().draw(leftTile);
            window.getWindow().draw(rightTile);
            window.getWindow().draw(upperTile);
        }
        window.getWindow().display();
    }
}

// Matrix vector multiplication helper
sf::Vector2f game::mat2x2xv(float m[2][2], sf::Vector2f v){
    sf::Vector2f temp = v;
    v.x = m[0][0] * temp.x + m[0][1] * temp.y;
    v.y = m[1][0] * temp.x + m[1][1] * temp.y;
    return v;
}

// Matrix inversion helper for mouse picking reverse mapping
void game::reverseMat(float m[2][2], float rm[2][2]){
    float horizon = 1 / (m[0][0] * m[1][1] - m[0][1] * m[1][0]);
    rm[0][0] = m[1][1] * horizon;
    rm[1][0] = -m[1][0] * horizon;
    rm[0][1] = -m[0][1] * horizon;
    rm[1][1] = m[0][0] * horizon;
}