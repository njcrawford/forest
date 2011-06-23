all: forest-client
forest-client: forest-client.cpp apt-get.o yum.o wuaapi.o
	gcc -g forest-client.cpp apt-get.o yum.o wuaapi.o -o forest-client
apt-get.o: apt-get.cpp apt-get.h
	gcc -g -c apt-get.cpp -o apt-get.o
yum.o: yum.cpp yum.h
	gcc -g -c yum.cpp -o yum.o
wuaapi.o: wuaapi.cpp wuaapi.h
	gcc -g -c wuaapi.cpp -o wuaapi.o
clean:
	rm forest-client
