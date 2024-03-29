#include "structs.h"
#include "settings.h"


/************\
*  COSTANTI  *
\************/

// Numero di microsecondi tra un update e l'altro.
#define FRAME_UPDATE 27.5 * 1000

// Costanti per stampare a video le coordinate di debug.
#define DEBUG_COLUMNS COLUMNS_PER_MAP+5
#define DEBUG_TOP 11

// Costante per attivare/disattivare le stampe di debug.
#define GENERAL_DEBUG false

// Costante per attivare/disattivare la stampa delle coordinate della rana.
#define FROG_DEBUG false

// Costante per attivare/disattivare la stampa delle coordinate dei proiettili della rana.
#define FROG_PROJECTILES_DEBUG false

// Costante per attivare/disattivare la stampa del numero di coccodrilli per ogni corsia.
#define RIVER_DEBUG false

// Costante per attivare/disattivare la stampa delle coordinate dei nemici.
#define ENEMIES_DEBUG false

// Costante per attivare/disattivare la stampa delle coordinate dei proiettili nemici.
#define ENEMIES_PROJECTILES_DEBUG false

// Costante per attivare/disattivare la stampa delle collisioni rilevate.
#define COLLISION_DEBUG false

// Costante per attivare/disattivare la stampa delle informazioni aggiuntive sulle collisioni rilevate.
#define COLLISION_DEBUG_INFO false

// Costanti per non confondere il comportamento delle pipe.
#define READ 0
#define WRITE 1

// Costanti per calcolare la distanza tra un coccodrillo e l'altro.
#define CROC_SPACE_MIN ((short)(COLUMNS_PER_BLOCK * 2))
#define CROC_SPACE_MAX ((short)(COLUMNS_PER_BLOCK * 4))


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
 

/*********************\
*  FUNZIONI PROCESSI  *
\*********************/

// Processo che gestisce la rana.
void frogHandler(int frogToMain[], int mainToFrog[], int frogToFPH[]);

// Processo che gestisce i proiettili generati dalla rana.
void frogProjectilesHandler(int frogToFPH[], int PHToMain[], int mainToFPH[]);

// Processo che verifica le collisioni e stampa l'interfaccia.
GameUpdates mainManager(short difficult, int frogToMain[], int mainToFrog[], int crocToMain[], int mainToRivH[], int PHToMain[], int mainToFPH[], int enHToMain[], int mainToEnH[]);

// Processo che gestisce lo spawn dei coccodrilli.
void riverHandler(int crocToMain[], int mainToRivH[]);

// Processo che gestisce un singolo coccodrillo.
void singleCrocodileHandler(int crocToMain[], Crocodile me);

// Processo che gestisce lo spawn (e la generazione) dei nemici.
void enemiesHandler(int enHToMain[], int mainToEnH[], int SEPHToMain[]);

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

// Funzione per ottenere il numero della riga in base alla coordinata y dell'entita'.
short yToRowNumber(short y);

// Funzione che imposta a false tutti gli elementi di un array booleano.
void setToFalse(bool array[], short size);

// Funzione per generare nuove posizioni per tutti i nemici.
void newEnemiesScene(short rowsY[], Enemy allEnemies[]);

// Funzione per creare il processo nemico.
void spawnEnemy(int enHToMain[], int PHToMain[], Enemy allEnemies[], short enemyID, short speed);

// Funzione per stampare dei messaggi su un file di testo.
void logChat(char *msg);