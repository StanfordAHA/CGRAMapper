CXX ?= g++
ifeq ($(COREIRCONFIG),g++-4.9)
CXX = g++-4.9
endif
CXXFLAGS = -std=c++11  -Wall  -fPIC -g



INCS = -I$(COREIR)/include
LPATH = -L$(COREIR)/lib
LIBS = -Wl,-rpath,$(COREIR)/lib -lcoreir-cgralib -lcoreir

all: build/map
	cp build/map ../bin/.

clean:
	rm -rf build/*

build/%: %.cpp
	$(CXX) $(CXXFLAGS) $(INCS) -o $@ $< $(LPATH) $(LIBS)
