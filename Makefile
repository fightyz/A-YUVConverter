CC=gcc
YUVConverter: main.o yuv.o threads.o
	$(CC) -o YUVConverter threads.o yuv.o main.o

main.o: main.c yuv.h threads.h
	$(CC) -c -pthread -std=c99 -O3 main.c

yuv.o: yuv.c yuv.h
	$(CC) -c -pthread -std=c99 -O3 yuv.c

threads.o: threads.c threads.h yuv.h
	$(CC) -c -pthread -std=c99 -O3 threads.c

clean:
	rm YUVConverter main.o yuv.o threads.o
