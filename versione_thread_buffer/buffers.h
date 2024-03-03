#include <pthread.h>
#include <semaphore.h>

#include "structs.h"

#ifndef BUFFER_GUARD
#define BUFFER_GUARD

# define BUFFER_SIZE 35

typedef union data                          // in questo modo si raggruppano tutte le entita' in un solo tipo
{
    Frog frogger;
    Crocodile croc;
    Projectile proj;
    Enemy enemy;
} Data;

typedef struct singleData
{
    char type;                              // per capire il tipo dell'entita'
    Data value;                             // per leggere l'entita' (in base a type)
} SingleData;

typedef struct buffer
{
    volatile SingleData data[BUFFER_SIZE];  // buffer
    volatile int index;                     // indice prima cella libera
    sem_t sem_free_space;                   // semaforo per gestire il numero di spazi
    pthread_mutex_t mutex;                  // mutex per evitare modifiche simultanee
} Buffer;

#endif