#include "structs.h"

// Numero di millisecondi tra un update e l'altro.
#define FRAME_UPDATE 33.333 * 1000

// Da attivare per fare il debug della rana.
#define FROG_DEBUG true

// Costanti per non confondere il comportamento delle pipe.
#define READ 0
#define WRITE 1

// Processo che gestisce la rana.
void frogHandler(int frogToMain[], int mainToFrog[], int FPHToMain[]);

// Processo che gestisce i proiettili generati dalla rana.
void frogProjectilesHandler(int frogProjPipe[], int killProjPipe[]);

// Processo che verifica le collisioni e stampa l'interfaccia.
void mainManager(short startTime, int frogToMain[], int mainToFrog[], int crocToMain[], int mainToRivH[]);

// Processo che gestisce lo spawn dei coccodrilli.
void riverHandler();

// Processo che gestisce un singolo coccodrillo.
void singleCrocodileHandler(int crocToMain[], int crocToRivH[], Crocodile me);
