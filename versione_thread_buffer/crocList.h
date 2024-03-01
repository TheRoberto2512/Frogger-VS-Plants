#include <unistd.h>
#include "structs.h"
#include "sprite.h"


/**************\
*  STRUTTURE   *
\**************/

typedef struct crocElement
{
    Crocodile croc;
    struct crocElement *next;
    short lastUpdate; 
} CrocElement;

typedef struct CrocList
{
    CrocElement *lanes[RIVER_ROWS];
    short counts[RIVER_ROWS];
} CrocList;


/************\
*  COSTANTI  *
\************/

// Costante per definire il limite destro.
#define RIGHT_LIMIT COLUMNS_PER_MAP + 1

// Costante per definire il limite sinistro.
#define LEFT_LIMIT 0 - CROCODILE_COLUMNS

// Costante per decidere dopo quanti update mancati eliminare un coccodrillo.
#define SKIPPED_UPDATES 5


/*************************\
*  PROTOTIPI DI FUNZIONE  *
\*************************/

// Aggiorna un coccodrillo esistente (se non esiste lo crea).
void Update(CrocList* list, short lane, Crocodile croc, short updateTime);

// Uccide un pid nella corsia specificata.
void Kill(CrocList* list, short lane, pthread_t crocPtid);

// Uccide tutti i pid di tutte le corsie.
void killAll(CrocList* list);

// Da richiamare quando in una corsia risultano un numero anomalo di coccodrilli.
void FixLane(CrocList* list, short lane);

// Stampa a schermo tutti i coccodrilli.
void printList(CrocList *list);

// Cancella gli elementi inattivi (quindi non piu' necessari).
void DeleteUnnecessary(CrocList *list, short updateTime);