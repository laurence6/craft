build:
	g++ -std=c++14 -Wall -Wextra -O2 -g -lglfw -lGLEW -lGLU -lGL -o main main.cpp

clean:
	rm main
