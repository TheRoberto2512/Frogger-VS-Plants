#include <pthread.h>
#include <semaphore.h>

#include "structs.h"

#ifndef BUFFER_GUARD
#define BUFFER_GUARD

# define BUFFER_SIZE 35

typedef union data
{
    Frog frogger;
    Crocodile croc;
    Projectile proj;
    Enemy enemy;
} Data;

typedef struct singleData
{
    char type;
    Data value;
} SingleData;

typedef struct buffer
{
    volatile SingleData data[BUFFER_SIZE];
    volatile int index;
    sem_t sem_free_space;
    pthread_mutex_t mutex;
} Buffer;
#endif