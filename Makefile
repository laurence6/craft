build:
	g++ -std=c++14 -O2 -g -lglfw -lGL -o main main.cpp

clean:
	rm main
