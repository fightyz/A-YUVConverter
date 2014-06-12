CC=gcc
YUVConverter: main.o yuv.o
	$(CC) -o YUVConverter main.o yuv.o

main.o: main.c yuv.h
	$(CC) -c -pthread -std=c99 -O3 main.c

yuv.o: yuv.c yuv.h
	$(CC) -c -pthread -std=c99 -O3 yuv.c

clean:
	rm YUVConverter main.o yuv.o