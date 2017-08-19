all: travis 

.PHONY: test
test: install
	#./bin/map examples/caleb_example2.json _mapped.json
	#./bin/map examples/conv.json _conv_mapped.json
	./bin/map examples/add4.json _add4.json

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
