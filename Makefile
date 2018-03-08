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

TEST_FILES=$(wildcard examples/[^_]*.json)
MAPPED_FILES=$(patsubst examples/%, mapped/%, $(TEST_FILES))

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
	. env/bin/activate && $(MAKE) pytest

.PHONY: clean
clean:
	$(MAKE) -C src clean
	-rm -f bin/*
	-rm -f mapped/*
	-rm -f _*.json

install: $(BIN)
	install $< $(prefix)/bin

