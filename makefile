all: main.o colors.o sprite.o
	gcc -g main.o sprite.o colors.o -o game -lncursesw

main.o: main.c
	gcc -c main.c 

sprite.o: sprite.c
	gcc -c sprite.c

colors.o: colors.c
	gcc -c colors.c 

clean:
	rm -f *.o *.gch game