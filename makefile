CFLAGS = -g -c
LIBS = -lncursesw

all: main.c colors.o sprite.o crocList.o functionsP.o settings.o collisionDetector.o menu.o
	gcc -g main.c sprite.o colors.o crocList.o functionsP.o settings.o collisionDetector.o menu.o $(LIBS) -o game 

functionsP.o: functionsP.c functionsP.h settings.o crocList.o sprite.o structs.h collisionDetector.o
	gcc $(CFLAGS) functionsP.c

crocList.o: crocList.c crocList.h sprite.o structs.h
	gcc $(CFLAGS) crocList.c

collisionDetector.o: collisionDetector.c collisionDetector.h sprite.o
	gcc $(CFLAGS) collisionDetector.c $(LIBS)

settings.o: settings.c settings.h
	gcc $(CFLAGS) settings.c $(LIBS)

menu.o: menu.c menu.h
	gcc $(CFLAGS) menu.c -lncursesw 

sprite.o: sprite.c sprite.h colors.o
	gcc $(CFLAGS) sprite.c $(LIBS)

colors.o: colors.c colors.h
	gcc $(CFLAGS) colors.c $(LIBS)

clean:
	rm -f *.o *.gch