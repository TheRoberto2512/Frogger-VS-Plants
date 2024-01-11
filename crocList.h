#include <unistd.h>
#include "structs.h"
#include "sprite.h"

#define SKIPPED_UPDATES 5

typedef struct crocElement
{
    Crocodile croc;
    struct crocElement *next;
    short lastUpdate; 
} CrocElement;

typedef struct CrocList
{
    CrocElement *lanes[RIVER_ROWS];
} CrocList;

// Aggiorna un coccodrillo esistente (se non esiste lo crea).
void Update(CrocList* list, short lane, Crocodile croc, short updateTime);

// Uccide un pid nella corsia specificata.
void Kill(CrocList* list, short lane, pid_t crocPid);

// Uccide tutti i pid di tutte le corsie.
void killAll(CrocList* list);

// Stampa a schermo tutti i coccodrilli.
void printList(CrocList *list);

// Cancella gli elementi inattivi (quindi non piu' necessari).
void DeleteUnnecessary(CrocList *list, short updateTime);