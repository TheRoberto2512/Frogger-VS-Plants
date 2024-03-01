#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

#include "crocList.h"
#include "sprite.h"
#include "collisionDetector.h"

void KillR(CrocElement* elem, pthread_t crocPtid); // La si vuole "nascosta" e utilizzabile solo dalle funzioni di questo file.

void Update(CrocList* list, short lane, Crocodile croc, short updateTime)
{
    void UpdateR(CrocElement* list, Crocodile croc, short updateTime);
    if(list->lanes[lane] == NULL) // se non esiste lo crea
    {
        list->lanes[lane] = (CrocElement*) malloc(sizeof(CrocElement));
        list->lanes[lane]->croc = croc;
        list->lanes[lane]->next = NULL;
        list->lanes[lane]->lastUpdate = updateTime;
    }
    else if(list->lanes[lane]->croc.PTID == croc.PTID) // aggiorna il suo stato
    {
        list->lanes[lane]->lastUpdate = updateTime;
        list->lanes[lane]->croc = croc;
    }
    else // fa una ricerca piu' approfondita
    {
        UpdateR(list->lanes[lane], croc, updateTime);  
    }

    // aggiorna il numero di coccodrilli per ogni corsia
    CrocElement* c = list->lanes[lane]; short cont = 0;
    while(c != NULL)
    {
        cont++;
        c = c->next;
    }
    list->counts[lane] = cont;
}

void UpdateR(CrocElement* elem, Crocodile croc, short updateTime)
{
    if(elem->next == NULL) // se non esiste lo crea
    {
        elem->next = (CrocElement*) malloc(sizeof(CrocElement));
        elem->next->croc = croc; 
        elem->next->next = NULL; 
        elem->next->lastUpdate = updateTime; 
    }
    else if(elem->next->croc.PTID == croc.PTID) // aggiorna il suo stato
    {
        elem->next->croc = croc;
        elem->next->lastUpdate = updateTime;
    }
    else // continua la ricerca
    {
        UpdateR(elem->next, croc, updateTime);
    }
}

void Kill(CrocList* list, short lane, pthread_t crocPtid)
{
    if(list->lanes[lane] != NULL) // esiste almeno un elemento
    {
        if(list->lanes[lane]->croc.PTID == crocPtid) // controlliamo se e' lui che cerchiamo
        {
            CrocElement* x = list->lanes[lane]; // salviamo l'elemento da eliminare

            if(list->lanes[lane]->next != NULL) // se ha un successore
                list->lanes[lane] = list->lanes[lane]->next; // avanza
            else
                list->lanes[lane] = NULL; // altrimenti nulla

            pthread_cancel(crocPtid);
            pthread_join(crocPtid, NULL);

            free(x); // de-allochiamo la memoria allocata
        }
        else if(list->lanes[lane]->next->croc.PTID == crocPtid) // se il croc e' il successore
        {
            CrocElement* x = list->lanes[lane]->next;

            list->lanes[lane]->next = list->lanes[lane]->next->next;

            pthread_cancel(crocPtid);
            pthread_join(crocPtid, NULL);

            free(x); // de-allochiamo la memoria allocata
        }
        else if(list->lanes[lane]->next->next != NULL) // se esiste un terzo successore
        {
            KillR(list->lanes[lane]->next, crocPtid);
        }
    }
}

void KillR(CrocElement* elem, pthread_t crocPtid)
{
    if(elem->next != NULL)
    {
        if(elem->next->croc.PTID == crocPtid)
        {
            CrocElement* x = elem->next;

            elem->next = elem->next->next;

            pthread_cancel(crocPtid);
            pthread_join(crocPtid, NULL);

            free(x); // de-allochiamo la memoria allocata
        }
        else if(elem->next->next != NULL)
        {
            KillR(elem->next, crocPtid);
        }
    }
}

void killAll(CrocList* list)
{
    for(short i = 0; i < RIVER_ROWS; i++)
    {
        while(list->lanes[i] != NULL)
        {
            Kill(list, i, list->lanes[i]->croc.PTID);
        }
    }
}

void FixLane(CrocList* list, short lane)
{  
    CrocElement* c = list->lanes[lane];                         // pesca il primo
    while (c->croc.x >= RIGHT_LIMIT || c->croc.x <= LEFT_LIMIT) // se sgarra
    {
        pthread_t toKill = c->croc.PTID;                             // salva il pid
        c = c->next;                                            // va avanti al secondo
        Kill(list, lane, toKill);                               // uccide il primo
    }
    // una volta uscito da qui, c rispetta i limiti ma potrebbe avere successori che sgarrano
    do
    {
        if(c->next != NULL) // se ha un successore
        {
            if(c->next->croc.x >= RIGHT_LIMIT || c->next->croc.x <= LEFT_LIMIT) // controlliamo il successore
            {
                pthread_t toKill = c->next->croc.PTID; 
                KillR(c, toKill);
            }
            else
            {
                c = c->next;
            }
        }
    } while (c->next != NULL);
}

void printList(CrocList *list)
{
    void printListR(CrocElement *Elem);
    for(short i = 0; i < RIVER_ROWS; i++)
    {
        if(list->lanes[i] != NULL)
        {
            printListR(list->lanes[i]);
        }
    }
}

void printListR(CrocElement *Elem)
{
    if(Elem->croc.splash == -10)
    {
        printCrocodile(Elem->croc.x, Elem->croc.y, Elem->croc.direction);
    }
    else
    {
        printBadCrocodile(Elem->croc.x, Elem->croc.y, Elem->croc.direction);
    }

    if(Elem->next != NULL)
    {
        printListR(Elem->next);
    }
} 

void DeleteUnnecessary(CrocList *list, short updateTime)
{
    void DeleteUnnecessaryR(CrocElement *Elem, short updateTime);
    for(short i = 0; i < RIVER_ROWS; i++)
    {
        if(list->lanes[i] != NULL)
        {
            DeleteUnnecessaryR(list->lanes[i], updateTime);
            if(updateTime - list->lanes[i]->lastUpdate >= SKIPPED_UPDATES)
            {
                Kill(list, i, list->lanes[i]->croc.PTID);
            }
        }
    }
}

void DeleteUnnecessaryR(CrocElement *Elem, short updateTime)
{
    if(Elem->next != NULL)
    {
        DeleteUnnecessaryR(Elem->next , updateTime);
        if(updateTime - Elem->next->lastUpdate >= SKIPPED_UPDATES)
        {
            KillR(Elem, Elem->next->croc.PTID);
        }
    }
}