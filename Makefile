all: travis 

TESTS = $(wildcard [^_]*.json)

.PHONY: test
test: install
	$(MAKE)

mapped/%.json: examples/%.json
	./bin/map $< mapped/$*_mapped.json

.PHONY: install
install:
	$(MAKE) -C src 

.PHONY: clean
clean:
	$(MAKE) -C src clean
	-rm bin/*
	-rm _*.json

.PHONY: travis
travis:
	$(MAKE) clean
	$(MAKE) test


