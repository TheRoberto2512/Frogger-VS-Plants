#include <unistd.h>
#include <stdlib.h>
#include <curses.h>
#include <locale.h>
#include <stddef.h>
#include <time.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>

#include "colors.h"
#include "sprite.h"
#include "structs.h"
#include "functionsT.h"
#include "settings.h"
#include "menu.h"

bool game();

int main()
{  
    setlocale(LC_ALL, ""); // attiva i caratteri speciali dell'UNICODE

    if(GENERAL_DEBUG)
        system("echo -e \"\\e[8;53;160t\""); // imposta la dimensione del terminale qualora sia troppo piccolo
    else
        system("echo -e \"\\e[8;53;108t\"");

    GameRules regole = getRules(MEDIUM);

    bool playAgain = game(&regole);

    if(playAgain)
        execl("./game", "./game", NULL);
    return 0;
}

bool game()
{
    bool playAgain = false;

    // . . .

    return playAgain;
}