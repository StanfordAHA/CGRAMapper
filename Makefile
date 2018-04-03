UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S), Linux)
TARGET = so
prefix?=/usr
endif
ifeq ($(UNAME_S), Darwin)
TARGET = dylib
prefix?=/usr/local
endif

BIN=bin/cgra-mapper

CXX?=g++
ifeq ($(COREIRCONFIG),g++-4.9)
CXX=g++-4.9
endif
CXXFLAGS+=-std=c++11 -Wall -fPIC -g -Wfatal-errors

TEST_FILES=$(wildcard examples/[^_]*.json)
MAPPED_FILES=$(patsubst examples/%, mapped/%, $(TEST_FILES))

export CXX
export CFLAGS
export CXXFLAGS


all: $(BIN)

#This Will copy cgra-mapper into bin/
.PHONY: bin/cgra-mapper
$(BIN): 
	$(MAKE) -C src

.PHONY: test
test: $(MAPPED_FILES)


$(MAPPED_FILES): $(BIN)
	$(BIN) examples/$(@F) $(@) || exit 1

.PHONY: pytest
pytest:
	pytest -s --libs cgralib --files $(MAPPED_FILES) -- tests

.PHONY: travis
travis:
	$(MAKE) clean
	$(MAKE) all
	$(MAKE) test
	. env/bin/activate && source .travis/install_coreir.sh && $(MAKE) pytest

.PHONY: clean
clean:
	$(MAKE) -C src clean
	-rm -rf bin/*
	-rm -f mapped/*
	-rm -f _*.json

.PHONY: install
install: $(BIN)
	install $< $(prefix)/bin
	install lib/libcoreir-cgra* $(prefix)/lib
	install -d $(prefix)/include/coreir/libs
	install include/coreir/libs/cgra*.h $(prefix)/include/coreir/libs

.PHONY: uninstall
uninstall:
	-rm $(prefix)/bin/cgra-mapper
	-rm $(prefix)/lib/libcoreir-cgra*
	-rm $(prefix)/include/coreir/libs/cgra*



