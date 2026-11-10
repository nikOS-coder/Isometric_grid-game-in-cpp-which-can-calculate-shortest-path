#pragma once
#include <SFML/Graphics.hpp>

// Class responsible for encapsulating the SFML RenderWindow and window dimensions
class mywindow{
    private:
        int x, y;                   // Window width and height resolution
        sf::RenderWindow window;    // SFML render window instance
    public:
        mywindow(int, int);
        ~mywindow();
        sf::RenderWindow& getWindow();      
};