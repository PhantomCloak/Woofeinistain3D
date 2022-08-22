build:
	gcc -std=c99 ./src/*.c -lm -lSDL2 -o duum3d;
run:
	./duum3d;
clean:
	rm duum3d;
