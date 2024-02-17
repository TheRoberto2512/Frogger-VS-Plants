#include <pthread.h>

#ifndef FROG_GUARD
#define FROG_GUARD
typedef struct frog
{
    short x;            // coordinata x
    short y;            // coordinata y
} Frog;
#endif

#ifndef CROC_GUARD
#define CROC_GUARD
typedef struct crocodile
{
    pthread_t PTID;     // per identificare il coccodrillo
    short x;            // coordinata x
    short y;            // coordinata y
    short direction;    // direzione
    short speed;        // velocita' di aggiornamento
    short splash;       // conto alla rovescia prima di affondare (-10 se coccodrillo buono)
} Crocodile;
#endif

#ifndef ENEMY_GUARD
#define ENEMY_GUARD
typedef struct enemy
{
    pthread_t PTID;     // threadID del processo nemico
    short ID;           // numero del nemico
    short x;            // coordinata x
    short y;            // coordinata y
    short shot;         // conto alla rovescia prima di sparare un proiettile
    short genTime;      // conto alla rovescia prima di generarsi completamente
} Enemy;
#endif

#ifndef PROJ_GUARD
#define PROJ_GUARD
typedef struct projectile
{
    pthread_t PTID;     // threadID del processo nemico
    short ID;           // identificatore 
    short x;            // coordinata x
    short y;            // coordinata y
    short speed;        // velocita' del proiettile
} Projectile;
#endif

#ifndef RIVER_GUARD
#define RIVER_GUARD
#define MAX_CROCODILE_PER_ROW 3
#define STOPPED_CROCODILE -100
typedef struct riverRow
{
    short direction;                        // direzione da seguire
    short speed;                            // velocita'
    Crocodile crocs[MAX_CROCODILE_PER_ROW]; // coccodrilli presenti
} RiverRow;
#endif

#ifndef CROC_POS
#define CROC_POS
typedef struct crocPos
{
    short row;          // riga del fiume
    short ID;           // ID del coccodrillo
} CrocPos;
#endif
