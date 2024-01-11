CFLAGS = -g -c

all: main.o colors.o sprite.o crocList.o functionsP.o settings.o
	gcc -g main.o sprite.o colors.o crocList.o functionsP.o settings.o -o game -lncursesw

main.o: main.c
	gcc $(CFLAGS) main.c 

functionsP.o: functionsP.c functionsP.h
	gcc $(CFLAGS) functionsP.c

crocList.o: crocList.c crocList.h 
	gcc $(CFLAGS) crocList.c

settings.o: settings.c settings.h
	gcc $(CFLAGS) settings.c

sprite.o: sprite.c sprite.h
	gcc $(CFLAGS) sprite.c

colors.o: colors.c colors.h
	gcc $(CFLAGS) colors.c 

clean:
	rm -f *.o *.gch game