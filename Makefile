build: tema3.o
	gcc -Wall -g tema3.c -lm -o bmp
	gcc -c tema3.c
run:
	./bmp
clean:
	rm bmp tema3.o
