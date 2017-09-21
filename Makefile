BIN=bin/mapper

TEST_FILES=$(wildcard examples/[^_]*.json)
MAPPED_FILES=$(patsubst examples/%, mapped/%, $(TEST_FILES))

all: $(BIN)

#This Will copy mapper into bin/
.PHONY: bin/mapper
$(BIN): 
	$(MAKE) -C src

.PHONY: test
test: $(MAPPED_FILES)

$(MAPPED_FILES): $(BIN)
	$(BIN) examples/$(@F) $(@)

.PHONY: pytest
pytest:
	pytest -s --libs cgralib --files $(MAPPED_FILES) -- tests

.PHONY: travis
travis:
	$(MAKE) clean
	$(MAKE) all
	$(MAKE) test
	$(MAKE) pytest

.PHONY: clean
clean:
	$(MAKE) -C src clean
	-rm -f bin/*
	-rm -f mapped/*
	-rm -f _*.json

