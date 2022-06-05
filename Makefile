CC=gcc -fPIC -g -w


all: libmyfs.so libmylibc.so testA testB

libmyfs.so: myfs.o
	$(CC) --shared -fPIC -g -pthread myfs.o -o libmyfs.so

libmylibc.so: myfs.o mystdio.o
	$(CC) --shared -fPIC -g -pthread myfs.o mystdio.o -o libmylibc.so

myfs.o: sources/myfs.c
	$(CC) sources/myfs.c -c 

mystdio.o: sources/mystdio.c
	$(CC) sources/mystdio.c -c 

testA: libmyfs.so testA.o
	$(CC) testA.o ./libmyfs.so -o testA

testA.o: testA.c
	$(CC) testA.c -c

testB: libmylibc.so testB.o
	$(CC) testB.o ./libmylibc.so -o testB

testB.o: testB.c
	$(CC) testB.c -c

clean: 
	rm -f *.so *.o testA a.out testB testfile.txt