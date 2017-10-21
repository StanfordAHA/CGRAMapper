CXX?=g++
ifeq ($(COREIRCONFIG),g++-4.9)
CXX=g++-4.9
endif
CXXFLAGS+=-std=c++11 -Wall -fPIC -g

SRC=$(wildcard [^_]*.cpp)
OBJ=$(patsubst %.cpp, build/%.o, $(SRC))

LIBS= -lcoreir-cgralib -lcoreir-commonlib -lcoreir

all: build/mapper

build/mapper: build/mapper.o 
	$(CXX) $(LDFLAGS) -o $@ $< $(LIBS)
	cp build/mapper ../bin/.

build/%.o: %.cpp 
	$(CXX) $(CXXFLAGS) -c $^ -o $@

.PHONY: clean
clean:
	-rm -rf build/*
