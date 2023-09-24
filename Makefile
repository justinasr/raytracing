CC=gcc
CFLAGS=-Wall -Wextra -O2
LFLAGS=-lm

all: vec3.o ray.o
	$(CC) $(CFLAGS) -o raytrace main.c vec3.o ray.o $(LFLAGS)

test: vec3.o
	$(CC) $(CFLAGS) -o test test.c vec3.o $(LFLAGS)

vec3.o:
	$(CC) $(CFLAGS) -c vec3.c

ray.o: vec3.o
	$(CC) $(CFLAGS) -c ray.c

clean:
	rm -f *.o raytrace test
	rm -f *.ppm
