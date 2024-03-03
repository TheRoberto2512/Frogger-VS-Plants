#include "structs.h"
#include "settings.h"

#include <pthread.h>


/************\
*  COSTANTI  *
\************/

// Numero di microsecondi tra un update e l'altro.
#define FRAME_UPDATE 27.5 * 1000

// Costanti per stampare a video le coordinate di debug.
#define DEBUG_COLUMNS COLUMNS_PER_MAP+5
#define DEBUG_TOP 11

// Costante per attivare/disattivare le stampe di debug.
#define GENERAL_DEBUG true

// Costante per attivare/disattivare la stampa delle coordinate della rana.
#define FROG_DEBUG true

// Costante per attivare/disattivare la stampa delle coordinate dei proiettili della rana.
#define FROG_PROJECTILES_DEBUG true

// Costante per attivare/disattivare la stampa del numero di coccodrilli per ogni corsia.
#define RIVER_DEBUG true

// Costante per attivare/disattivare la stampa delle coordinate dei nemici.
#define ENEMIES_DEBUG true

// Costante per attivare/disattivare la stampa delle coordinate dei proiettili nemici.
#define ENEMIES_PROJECTILES_DEBUG true

// Costante per attivare/disattivare la stampa delle collisioni rilevate.
#define COLLISION_DEBUG true

// Costante per attivare/disattivare la stampa delle informazioni aggiuntive sulle collisioni rilevate.
#define COLLISION_DEBUG_INFO true

// Costanti per calcolare la distanza tra un coccodrillo e l'altro.
#define CROC_SPACE_MIN (COLUMNS_PER_BLOCK * 2)
#define CROC_SPACE_MAX (COLUMNS_PER_BLOCK * 4)

// Costante per definire il numero massimo di coccodrilli per riga.
#define MAX_CROCS 3


/*****************\
*  COSTANTI FLAG  *
\*****************/

// FLAG per identificare un coccodrillo come "buono".
#define GOOD_CROC_FLAG -10

#define ALIVE 0
#define MUST_KILL 1
#define KILLED -1
#define EMPTY -5
#define TURN_GOOD 2
 

/********************\
*  FUNZIONI THREAD  *
\********************/

// Thread che gestisce la rana.
void *frogHandler();

// Thread che gestisce un singolo proiettile nemico.
void *singleFrogProjectileHandler(void *arg);

// Thread che verifica le collisioni e stampa l'interfaccia.
void *mainManager();

// Thread che gestisce un singolo coccodrillo.
void *singleCrocodileHandler(void *arg1);

// Thread che gestisce un singolo nemico.
void *singleEnemyHandler(void *arg);

// Thread che gestisce un singolo proiettile nemico.
void *singleEnemyProjectileHandler(void *arg);


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
Crocodile buildCrocodile(short x, short y, short direction, short speed, short splashP, short row, short ID);

// Funzione per creare il thread coccodrillo.
void spawnCrocodile(Crocodile croc);

// Funzione per regolare la velocita' delle righe del fiume.
void riverSpeeds(short speeds[], short rulesSpeed);

// Funzione per impostare la velocità coccodrili pari alla loro corsia.
void setCrocSpeeds();

// Funzione per resettare le cordinate dei coccodrilli
void resetCrocCord();

// Funzione per generare randomicamente lo spazio tra due coccodrilli.
short crocodileSpace();

// Funzione per generare una nuova scena (posizioni coccodrilli) all'inizio di una manche.
void newCrocodileScene(short speeds[], short directions[], short spawnTimers[], GameRules *rules);

// Funzione per ottenere il numero della riga in base alla coordinata y dell'entita'.
short yToRowNumber(short y);

// Funzione che imposta a false tutti gli elementi di un array booleano.
void setToFalse(bool array[], short size);

// Funzione per generare nuove posizioni per tutti i nemici.
void newEnemiesScene(short rowsY[], Enemy allEnemies[]);

// Funzione per creare il thread nemico.
void spawnEnemy(short enemyID);

// Funzione per stampare dei messaggi su un file di testo.
void logChat(char *msg);
