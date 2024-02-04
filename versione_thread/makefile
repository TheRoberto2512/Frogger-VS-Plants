CFLAGS = -g -c
LIBS = -lncursesw

all: main.c colors.o sprite.o functionsT.o settings.o collisionDetector.o menu.o
	gcc -g main.c sprite.o colors.o functionsT.o settings.o collisionDetector.o menu.o $(LIBS) -lpthread -o game 

functionsT.o: functionsT.c functionsT.h settings.o sprite.o structs.h collisionDetector.o
	gcc $(CFLAGS) functionsT.c functionsT.h -lpthread

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