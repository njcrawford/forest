all: forest-client
forest-client: forest-client.c apt-get.o yum.o lineList.o wuaapi.o
	gcc -g forest-client.c apt-get.o yum.o wuaapi.o lineList.o -o forest-client
apt-get.o: apt-get.c apt-get.h
	gcc -g -c apt-get.c -o apt-get.o
yum.o: yum.c yum.h
	gcc -g -c yum.c -o yum.o
wuaapi.o: wuaapi.c wuaapi.h
	gcc -g -c wuaapi.c -o wuaapi.o
lineList.o: lineList.c lineList.h
	gcc -g -c lineList.c -o lineList.o
clean:
	rm forest-client
