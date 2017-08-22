all: travis 

.PHONY: test
test: install
	./bin/map examples/caleb_example2.json _mapped.json
	./bin/map examples/conv.json _conv_mapped.json
	./bin/map examples/caleb_simplemem.json _simplemem.json
	pytest --libs cgralib --files _mapped.json _conv_mapped.json _simplemem.json -- tests

.PHONY: install
install:
	$(MAKE) -C src 

.PHONY: clean
clean:
	$(MAKE) -C src clean
	-rm -f bin/*
	-rm -f _*.json

.PHONY: travis
travis:
	$(MAKE) clean
	$(MAKE) test
