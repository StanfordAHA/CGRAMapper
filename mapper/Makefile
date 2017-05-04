INCS = -I$(COREIR)/include
LPATH = -L$(COREIR)/bin
LIBS = -Wl,-rpath,$(COREIR)/bin -lcoreir-cgralib -lcoreir-stdlib -lcoreir-passes -lcoreir
CXX ?= g++
CXXFLAGS = -std=c++11  -Wall  -fPIC -g

all: ../build/main ../build/regexample ../build/memexample

clean:
	rm -rf ../build/*

../build/%: %.cpp
	$(CXX) $(CXXFLAGS) $(INCS) -o $@ $< $(LPATH) $(LIBS)
