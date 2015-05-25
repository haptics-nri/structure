OPENNIDIR = ../openni2
PNGDIR = ./png++

CXXFLAGS = -I$(OPENNIDIR)/Include -I$(PNGDIR)/include
LDFLAGS = -L. -Wl,-rpath ./ -lOpenNI2 `libpng-config --ldflags`

all: main.cpp
	g++ -o logger $^ $(CXXFLAGS) $(LDFLAGS)

clean:
	rm logger

