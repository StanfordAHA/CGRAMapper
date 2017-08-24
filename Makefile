CXX?=g++
ifeq ($(COREIRCONFIG),g++-4.9)
CXX=g++-4.9
endif
CXXFLAGS=-std=c++11 -Wall -fPIC -g

SRC=$(wildcard src/*.cpp)
OBJ=$(patsubst src/%.cpp, build/%.o, $(SRC))
BIN=./bin/map

INCS=-I$(COREIR)/include
LPATH=-L$(COREIR)/lib
LIBS=-Wl,-rpath,$(COREIR)/lib -lcoreir-cgralib -lcoreir

TEST_FILES=$(wildcard examples/[^_]*.json)
MAPPED_FILES=$(patsubst examples/%, mapped/%, $(TEST_FILES))

all: $(BIN)

$(BIN): $(OBJ)
	mkdir -p bin
	$(CXX) $(CXXFLAGS) $(INCS) -o $@ $^ $(LPATH) $(LIBS)

build/%.o: src/%.cpp 
	mkdir -p build
	$(CXX) $(CXXFLAGS) $(INCS) -c $^ -o $@

.PHONY: test
test: $(MAPPED_FILES)

.PHONY: pytest
pytest: test
	pytest --libs cgralib --files $(MAPPED_FILES) -- tests

$(MAPPED_FILES): $(BIN)
	mkdir -p mapped
	$(BIN) examples/$(@F) $(@)

.PHONY: travis
travis:
	$(MAKE) clean
	$(MAKE) all
	$(MAKE) test
	$(MAKE) pytest

.PHONY: clean
clean:
	-rm -f bin/*
	-rm -f build/*
	-rm -f mapped/*

