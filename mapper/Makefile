INCS = -I$(COREIR)/include
LPATH = -L$(COREIR)/lib
LIBS = -Wl,-rpath,$(COREIR)/lib -lcoreir-cgralib -lcoreir-stdlib -lcoreir-passes -lcoreir
CXX ?= g++
CXXFLAGS = -std=c++11  -Wall  -fPIC -g

all: ../build/map

clean:
	rm -rf ../build/*

../build/%: %.cpp
	$(CXX) $(CXXFLAGS) $(INCS) -o $@ $< $(LPATH) $(LIBS)
