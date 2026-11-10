#include "mywindow.h"
#include "game.h"
#include "hero.h"

int main(void){
    mywindow window(1900, 900);
    game grid(30, 30.0f);
    grid.mainLoop(window);

    return 0;
}