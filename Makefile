build:
	g++ -std=c++14 -Wall -Wextra -O2 -g `pkg-config --cflags --libs gl glew glfw3 glm` -o main main.cpp

clean:
	rm main
