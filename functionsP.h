#include "structs.h"

// Numero di millisecondi tra un update e l'altro.
#define FRAME_UPDATE 33.333 * 1000

// Costanti per non confondere il comportamento delle pipe.
#define READ 0
#define WRITE 1

// Processo che gestisce la rana.
void frogHandler(int frogToMain[2], int mainToFrog[2], int FPHToMain[2]);

// Processo che gestisce i proiettili generati dalla rana.
void frogProjectilesHandler(int frogProjPipe[2], int killProjPipe[2]);

// Processo che verifica le collisioni e stampa l'interfaccia.
void mainManager(WINDOW *gameArea, WINDOW *scoreBoard, int frogToMain[2], int mainToFrog[2]);

// Processo che gestisce lo spawn dei coccodrilli.
void riverHandler();

// Processo che gestisce un singolo coccodrillo.
void singleCrocodileHandler();
