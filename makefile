# Makefile

CXX = g++
FLAGS_SDL2 = -lSDL2 -lSDL2_ttf -g
CXXFLAGS = ${FLAGS_SDL2}

pong: main.cpp variables.h
	$(CXX) $(CXXFLAGS) -o $@ $<

pong2: main.cpp variables.h
		$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	rm pong
