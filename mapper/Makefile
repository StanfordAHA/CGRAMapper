SRCDIR = ../../coreir/src
LIBSDIR = ../../coreir/libs

INCS = -I$(SRCDIR) -I$(LIBSDIR)
LIBS = -L$(SRCDIR) -L$(LIBSDIR)

CXX ?= g++
CXXFLAGS = -std=c++11  -Wall  -fPIC 


all: ../build/main

clean:
	rm -rf ../build/*

../build/main: main.cpp
	make -C ../../coreir/src
	$(CXX) $(CXXFLAGS) $(INCS) -o $@ $< ../../coreir/build/coreir.a
