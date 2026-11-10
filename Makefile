all:
	g++ main.cpp mywindow.cpp game.cpp hero.cpp -lsfml-graphics -lsfml-window -lsfml-system -o isometric_sim