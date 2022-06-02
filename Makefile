CC=gcc -fPIC -g 

all: libmyfs.so test

libmyfs.so: myfs.o
	$(CC) --shared -fPIC -g -pthread myfs.o -o libmyfs.so

myfs.o: myfs.c
	$(CC) myfs.c -c 

test: libmyfs.so test.o
	$(CC) test.o ./libmyfs.so -o test

test.o: test.c
	$(CC) test.c -c

clean: 
	rm -f *.so *.o test a.out