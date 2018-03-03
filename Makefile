build: minimax

minimax: minimax.c
	gcc -Wall minimax.c cerinta3.h -o minimax

clean:
	rm -fr minimax
