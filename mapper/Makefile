INCS = -I$(COREIRHOME)/include
LPATH = -L$(COREIRHOME)/bin
LIBS = -lcoreir -lcoreir-stdlib -lcoreir-passes
CXX ?= g++
CXXFLAGS = -std=c++11  -Wall  -fPIC 


all: ../build/main

clean:
	rm -rf ../build/*

../build/main: main.cpp
	$(CXX) $(CXXFLAGS) $(INCS) $(LPATH) -o $@ $< $(LIBS)
