OPENNIDIR = ../openni2

CXXFLAGS = -I$(OPENNIDIR)/Include
LDFLAGS = -L. -Wl,-rpath ./ -lOpenNI2

all: main.cpp
	g++ -o logger $^ $(CXXFLAGS) $(LDFLAGS)

clean:
	rm logger

