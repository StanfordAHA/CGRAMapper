INCS = -I$(COREIRHOME)/include
LPATH = -L$(COREIRHOME)/bin
LIBS = -Wl,-rpath,$(COREIRHOME)/bin -lcoreir-stdlib -lcoreir-passes -lcoreir
CXX ?= g++
CXXFLAGS = -std=c++11  -Wall  -fPIC 


all: ../build/main

clean:
	rm -rf ../build/*

../build/main: main.cpp
	$(CXX) $(CXXFLAGS) $(INCS) -o $@ $< $(LPATH) $(LIBS)
