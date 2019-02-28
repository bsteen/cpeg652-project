compile:
	g++ -o gltest.out main.cpp window.cpp -lsfml-window -lsfml-system -lGLEW -lGL -std=c++11
#http://www.sfml-dev.org/tutorials/2.1/start-linux.php
#-lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lsfml-network
#http://glew.sourceforge.net/install.html
#-lGLEW -lGLU -lGL

debug:
	g++ -g -o gltest.out main.cpp window.cpp -lsfml-window -lsfml-system -lGLEW -lGL -std=c++11

valgrind: debug
	clear
	 valgrind --leak-check=yes ./gltest.out

run:
	clear
	./gltest.out

compile-run: compile run

triangles:
	g++ -o triangles.out triangles.cpp -lsfml-window -lsfml-system -lGLEW -lGL -std=c++11



clean:
	rm *.out
