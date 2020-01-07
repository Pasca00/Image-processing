build: tema3.o
	@gcc -Wall -g tema3.c -lm -o bmp
	@gcc -c tema3.c
run:
	@./bmp
clean:
	@rm -f bmp tema3.o
	@for i in 0 1 2 3 4 5 6 7 8 9; do \
		rm -f test"$$i"_black_white.bmp; \
		rm -f test"$$i"_filter.bmp;  \
		done
