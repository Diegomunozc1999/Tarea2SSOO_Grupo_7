CXX = g++
CXXFLAGS = -std=c++11 -Wall `pkg-config --cflags opencv4`
LDFLAGS = `pkg-config --libs opencv4`

all: image_processing_sequential

image_processing_sequential: image_processing.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -f image_processing_sequential
