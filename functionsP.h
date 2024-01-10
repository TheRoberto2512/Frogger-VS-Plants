#include "structs.h"
#include "settings.h"


/************\
*  COSTANTI  *
\************/

// Numero di microsecondi tra un update e l'altro.
#define FRAME_UPDATE 33.333 * 1000

// Da attivare per fare il debug della rana.
#define FROG_DEBUG true

// Costanti per non confondere il comportamento delle pipe.
#define READ 0
#define WRITE 1

// Costanti per calcolare la distanza tra un coccodrillo e l'altro.
#define CROC_SPACE_MIN ((short)(COLUMNS_PER_BLOCK * 2))
#define CROC_SPACE_MAX ((short)(COLUMNS_PER_BLOCK * 4))

/*********************\
*  FUNZIONI PROCESSI  *
\*********************/

// Processo che gestisce la rana.
void frogHandler(int frogToMain[], int mainToFrog[], int FPHToMain[]);

// Processo che gestisce i proiettili generati dalla rana.
void frogProjectilesHandler(int frogProjPipe[], int killProjPipe[]);

// Processo che verifica le collisioni e stampa l'interfaccia.
void mainManager(short startTime, int frogToMain[], int mainToFrog[], int crocToMain[], int mainToRivH[]);

// Processo che gestisce lo spawn dei coccodrilli.
void riverHandler(int crocToMain[], int mainToRivH[], GameRules rules);

// Processo che gestisce un singolo coccodrillo.
void singleCrocodileHandler(int crocToMain[], Crocodile me);


/*********************\
*  FUNZIONI UTILITA'  *
\*********************/

// Funzione per impostare un nuovo seed per la srand().
void randomSeed();

// Funzione per generare un numero random tra min e max (inclusi).
short randomNumber(short min, short max);

// Funzione per calcolare la Y di un coccodrillo in base al numero della corsia (0-7).
short computeY(short n);

// Funzione per calcolare la corsia di un coccodrillo in base alla sua altezza Y.
short reverseComputeY(short n);

// Funzione per assemblare facilmente una variabile Crocodile.
Crocodile buildCrocodile(short x, short y, short direction, short speed, short splash);

// Funzione per creare il processo coccodrillo.
short spawnCrocodile(int crocToMain[], Crocodile croc);

// Funzione per regolare la decisione delle velocita' del fiume.
void riverSpeeds(short speeds[], short rulesSpeed);

// Funzione per generare randomicamente lo spazio tra due coccodrilli.
short crocodileSpace();
