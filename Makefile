CC=gcc
YUVConverter: main.o yuv.o
	$(CC) -o YUVConverter main.o yuv.o

main.o: main.c yuv.h
	$(CC) -c -O3 -std=c99 main.c

yuv.o: yuv.c yuv.h
	$(CC) -c -O3 -std=c99 yuv.c

clean:
	rm YUVConverter main.o yuv.o