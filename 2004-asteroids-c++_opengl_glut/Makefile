#!/usr/bin/make
all: asteroid

asteroid: main.o gameobj.o collision.o flame.o
	g++ -g -O2 -o asteroid main.o gameobj.o collision.o flame.o -lGLU -lglut -lGL 
	
%.o: %.cpp
	g++ -g -c -O2 -Wall $<

clean:
	rm -f asteroid *.o
