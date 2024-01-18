#include "structs.h"
#include "settings.h"


/************\
*  COSTANTI  *
\************/

// Numero di microsecondi tra un update e l'altro.
#define FRAME_UPDATE 33.333 * 1000

// Costanti per stampare a video le coordinate di debug.
#define DEBUG_COLUMNS COLUMNS_PER_MAP+5
#define DEBUG_TOP 11

#define FROG_DEBUG true
#define ENEMIES_DEBUG true
#define FROG_PROJECTILES_DEBUG true
#define ENEMIES_PROJECTILES_DEBUG true

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
void frogHandler(int frogToMain[], int mainToFrog[], int frogToFPH[]);

// Processo che gestisce i proiettili generati dalla rana.
void frogProjectilesHandler(int frogToFPH[], int PHToMain[], int mainToFPH[], short speed);

// Processo che verifica le collisioni e stampa l'interfaccia.
void mainManager(GameRules *rules, GameUpdates *gUpdt, int frogToMain[], int mainToFrog[], int crocToMain[], int mainToRivH[], int PHToMain[], int mainToFPH[], int enHToMain[], int mainToEnH[]);

// Processo che gestisce lo spawn dei coccodrilli.
void riverHandler(int crocToMain[], int mainToRivH[], GameRules *rules);

// Processo che gestisce un singolo coccodrillo.
void singleCrocodileHandler(int crocToMain[], Crocodile me);

// Processo che gestisce lo spawn (e la generazione) dei nemici.
void enemiesHandler(int enHToMain[], int mainToEnH[], int SEPHToMain[], short speed);

// Processo che gestisce un singolo nemico.
void singleEnemyHandler(Enemy myself, int SEPHToMain[], short speed);

// Processo che gestisce un singolo proiettile nemico.
void singleEnemyProjectileHandler(int SEPHToMain[], Projectile myself);


/*********************\
*  FUNZIONI UTILITA'  *
\*********************/

// Funzione per killare correttamente un processo (kill() & waitpid()).
int easyKill(pid_t PID);

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

// Funzione per generare una nuova scena (posizioni coccodrilli) all'inizio di una manche.
void newCrocodileScene(int crocToMain[], short directions[], short speeds[], short spawnTimers[], GameRules *rules);