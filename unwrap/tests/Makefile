.PHONY: all clean

CXXFLAGS=-g -I../.. `pkg-config --cflags --libs opencv`

all: deserialise_test serialise_test

deserialise_test: deserialise/test.cc ../../common/opencv_unwrap_360.h
	$(CXX) -o $@ $< $(CXXFLAGS)

serialise_test: serialise/test.cc ../../common/opencv_unwrap_360.h
	$(CXX) -o $@ $< $(CXXFLAGS)

clean:
	rm -f deserialise_test serialise_test{,.yaml}
