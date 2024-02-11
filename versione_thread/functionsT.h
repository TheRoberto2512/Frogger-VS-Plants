#include "structs.h"
#include "settings.h"

#include <pthread.h>


/************\
*  COSTANTI  *
\************/

// Numero di microsecondi tra un update e l'altro.
#define FRAME_UPDATE 33 * 1000

// Costanti per stampare a video le coordinate di debug.
#define DEBUG_COLUMNS COLUMNS_PER_MAP+5
#define DEBUG_TOP 11

#define GENERAL_DEBUG true
#define FROG_DEBUG true
#define FROG_PROJECTILES_DEBUG true
#define RIVER_DEBUG false
#define ENEMIES_DEBUG true
#define ENEMIES_PROJECTILES_DEBUG true
#define COLLISION_DEBUG false
#define COLLISION_DEBUG_INFO false

// Costanti per calcolare la distanza tra un coccodrillo e l'altro.
#define CROC_SPACE_MIN (COLUMNS_PER_BLOCK * 2)
#define CROC_SPACE_MAX (COLUMNS_PER_BLOCK * 4)


/*****************\
*  COSTANTI FLAG  *
\*****************/

// FLAG per identificare un coccodrillo come "buono".
#define GOOD_CROC_FLAG -10

// FLAG per indicare l'apocalisse dei coccodrilli.
#define CROCS_APOCALYPSE -12589

// FLAG per mettere in pausa la generazione di coccodrilli.
#define CROC_STOP -15000

// FLAG per richiedere una generazione istantanea di un coccodrillo.
#define CROC_NOW -13427

// FLAG per comunicare all'enemiesHandler di rigenerare tutti i nemici.
#define NEW_ENEMIES_FLAG -5000
 

/********************\
*  FUNZIONI THREAD  *
\********************/

// Thread che gestisce la rana.
void *frogHandler();

// Thread che gestisce i proiettili generati dalla rana.
void *frogProjectilesHandler();

// Thread che verifica le collisioni e stampa l'interfaccia.
void *mainManager();

// Thread che gestisce lo spawn dei coccodrilli.
void *riverHandler(void *args);

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

// Funzione per killare correttamente un thread.
void easyKill(pthread_t PTID);

// Funzione per calcolare la Y di un coccodrillo in base al numero della corsia (0-7).
short computeY(short n);

// Funzione per calcolare la corsia di un coccodrillo in base alla sua altezza Y.
short reverseComputeY(short n);

// Funzione per assemblare facilmente una variabile Crocodile.
Crocodile buildCrocodile(short x, short y, short direction, short speed, short splash);

// Funzione per creare il thread coccodrillo.
short spawnCrocodile(short lane, short x,short y );

// Funzione per regolare la velocita' delle righe del fiume.
void riverSpeeds();

// Funzione per regolare la direzione delle righe del fiume.
void riversDirections();

// Funzione per impostare la velocità coccodrili pari alla loro corsia.
void setCrocSpeeds();

// Funzione per resettare le cordinate dei coccodrilli
void resetCrocCord();

// Funzione per generare randomicamente lo spazio tra due coccodrilli.
short crocodileSpace();

// Funzione per generare una nuova scena (posizioni coccodrilli) all'inizio di una manche.
void newCrocodileScene(RiverRow rivRow[],short spawnTimers[]);

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
