all:
	$(MAKE) clean
	$(MAKE) install
	$(MAKE) test

test:
	./bin/map examples/caleb_example2.json _mapped.json

install:
	$(MAKE) -C src 

clean:
	rm _*.json
	$(MAKE) -C src clean
