#include "mywindow.h"

// Constructor to initialize resolution dimensions and create the SFML render window
mywindow::mywindow(int resx, int resy):x(resx), y(resy){
    window.create(sf::VideoMode(x, y), "example_window");
};

// Accessor method to retrieve a reference to the main window for drawing operations
sf::RenderWindow& mywindow::getWindow(){
    return window;
}

// Destructor to safely close the window upon exit
mywindow::~mywindow(){
    window.close();
}