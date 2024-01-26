#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

#include "crocList.h"
#include "sprite.h"

// La si vuole "nascosta" e utilizzabile solo dalle funzioni di questo file.
short KillR(CrocElement* elem, pid_t crocPid);

void Update(CrocList* list, short lane, Crocodile croc, short updateTime)
{
    short UpdateR(CrocElement* list, Crocodile croc, short updateTime);
    if(list->lanes[lane] == NULL) // se non esiste lo crea
    {
        list->lanes[lane] = (CrocElement*) malloc(sizeof(CrocElement));
        list->lanes[lane]->croc = croc;
        list->lanes[lane]->next = NULL;
        list->lanes[lane]->lastUpdate = updateTime;
        list->counts[lane] = list->counts[lane] + 1;
    }
    else if(list->lanes[lane]->croc.PID == croc.PID) // aggiorna il suo stato
    {
        list->lanes[lane]->lastUpdate = updateTime;
        list->lanes[lane]->croc = croc;
    }
    else // fa una ricerca piu' approfondita
    {
        list->counts[lane] += UpdateR(list->lanes[lane], croc, updateTime);  
    }
}

short UpdateR(CrocElement* elem, Crocodile croc, short updateTime)
{
    if(elem->next == NULL) // se non esiste lo crea
    {
        elem->next = (CrocElement*) malloc(sizeof(CrocElement));
        elem->next->croc = croc; 
        elem->next->next = NULL; 
        elem->next->lastUpdate = updateTime; 
        return 1;
    }
    else if(elem->next->croc.PID == croc.PID) // aggiorna il suo stato
    {
        elem->next->croc = croc;
        elem->next->lastUpdate = updateTime;
        return 0;
    }
    else // continua la ricerca
    {
        return UpdateR(elem->next, croc, updateTime);
    }
}

void Kill(CrocList* list, short lane, pid_t crocPid)
{
    if(list->lanes[lane] != NULL) // esiste almeno un elemento
    {
        if(list->lanes[lane]->croc.PID == crocPid) // controlliamo se e' lui che cerchiamo
        {
            CrocElement* x = list->lanes[lane]; // salviamo l'elemento da eliminare

            if(list->lanes[lane]->next != NULL) // se ha un successore
                list->lanes[lane] = list->lanes[lane]->next; // avanza
            else
                list->lanes[lane] = NULL; // altrimenti nulla

            kill(crocPid, SIGTERM);           // uccidiamo il croc selezionato
            int status;
            waitpid(crocPid, &status, 0);     // eseguiamo la wait per liberare le risorse

            free(x); // de-allochiamo la memoria allocata
            list->counts[lane] = list->counts[lane] - 1;
        }
        else if(list->lanes[lane]->next->croc.PID == crocPid) // se il croc e' il successore
        {
            CrocElement* x = list->lanes[lane]->next;

            list->lanes[lane]->next = list->lanes[lane]->next->next;

            kill(crocPid, SIGTERM);           // uccidiamo il croc selezionato
            int status;
            waitpid(crocPid, &status, 0);     // eseguiamo la wait per liberare le risorse

            free(x); // de-allochiamo la memoria allocata
            list->counts[lane] = list->counts[lane] - 1;
        }
        else if(list->lanes[lane]->next->next != NULL) // se esiste un terzo successore
        {
            list->counts[lane] += KillR(list->lanes[lane]->next, crocPid);
        }
    }
}

short KillR(CrocElement* elem, pid_t crocPid)
{
    if(elem->next != NULL)
    {
        if(elem->next->croc.PID == crocPid)
        {
            CrocElement* x = elem->next;

            elem->next = elem->next->next;

            kill(crocPid, SIGTERM);           // uccidiamo il croc selezionato
            int status;
            waitpid(crocPid, &status, 0);     // eseguiamo la wait per liberare le risorse

            free(x); // de-allochiamo la memoria allocata
            return -1;
        }
        else if(elem->next->next != NULL)
        {
            return KillR(elem->next, crocPid);
        }
    }
}

void killAll(CrocList* list)
{
    for(short i = 0; i < RIVER_ROWS; i++)
    {
        while(list->lanes[i] != NULL)
        {
            Kill(list, i, list->lanes[i]->croc.PID);
        }
    }
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
    short DeleteUnnecessaryR(CrocElement *Elem, short updateTime);
    for(short i = 0; i < RIVER_ROWS; i++)
    {
        if(list->lanes[i] != NULL)
        {
            list->counts[i] += DeleteUnnecessaryR(list->lanes[i], updateTime);
            if(updateTime - list->lanes[i]->lastUpdate >= SKIPPED_UPDATES)
            {
                Kill(list, i, list->lanes[i]->croc.PID);
            }
        }
    }
}

short DeleteUnnecessaryR(CrocElement *Elem, short updateTime)
{
    short killed = 0;
    if(Elem->next != NULL)
    {
        killed += DeleteUnnecessaryR(Elem->next , updateTime);
        if(updateTime - Elem->next->lastUpdate >= SKIPPED_UPDATES)
        {
            killed += KillR(Elem, Elem->next->croc.PID);
        }
    }
    return killed;
}