all: main.o colors.o sprite.o crocList.o functionsP.o settings.o
	gcc -g main.o sprite.o colors.o crocList.o functionsP.o settings.o -o game -lncursesw

main.o: main.c
	gcc -g -c main.c 

functionsP.o: functionsP.c functionsP.h
	gcc -g -c functionsP.c

crocList.o: crocList.c crocList.h 
	gcc -g -c crocList.c

settings.o: settings.c settings.h
	gcc -g -c settings.c

sprite.o: sprite.c sprite.h
	gcc -g -c sprite.c

colors.o: colors.c colors.h
	gcc -g -c colors.c 

clean:
	rm -f *.o *.gch game