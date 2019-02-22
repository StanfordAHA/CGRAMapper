HOME = ..
LPATH = -L$(HOME)/lib
INCS = -I$(HOME)/include -I.

SRC=$(wildcard [^_]*.cpp)
OBJ=$(patsubst %.cpp, build/%.o, $(SRC))

LIBS= -lcoreir-commonlib -lcoreir -lcoreir-cgralib -lcoreirsim

all: build/mapper

build/mapper: build/mapper.o
	$(MAKE) -C libs $(TARGET)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LPATH) $(LIBS)
	cp build/mapper ../bin/cgra-mapper

build/%.o: %.cpp 
	$(CXX) $(CXXFLAGS) $(INCS) -c $^ -o $@

.PHONY: clean
clean:
	-rm -rf build/*
	$(MAKE) -C libs clean
