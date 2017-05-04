INCS = -I$(COREIRHOME)/include
LPATH = -L$(COREIRHOME)/bin
LIBS = -Wl,-rpath,$(COREIRHOME)/bin -lcoreir-stdlib -lcoreir-passes -lcoreir
CXX ?= g++
CXXFLAGS = -std=c++11  -Wall  -fPIC -g

all: ../build/main ../build/regexample

clean:
	rm -rf ../build/*

../build/%: %.cpp
	$(CXX) $(CXXFLAGS) $(INCS) -o $@ $< $(LPATH) $(LIBS)
