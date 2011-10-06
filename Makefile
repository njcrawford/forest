all: forest-client

forest-client: 
	cd client; make

clean-forest-client:
	cd client; make clean

clean: clean-forest-client
