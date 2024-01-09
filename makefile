all: main.o colors.o sprite.o functionsP.o settings.o
	gcc -g main.o sprite.o colors.o functionsP.o settings.o -o game -lncursesw

main.o: main.c
	gcc -c main.c 

functionsP.o: functionsP.c
	gcc -c functionsP.c

settings.o: settings.c
	gcc -c settings.c

sprite.o: sprite.c
	gcc -c sprite.c

colors.o: colors.c
	gcc -c colors.c 

clean:
	rm -f *.o *.gch