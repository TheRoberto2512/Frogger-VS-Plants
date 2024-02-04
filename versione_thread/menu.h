/************\
*  COSTANTI  *
\************/

// Altezza di un carrattere della scritta FROGGER in righe.
#define FROGGER_ROWS 10 

// Larghezza di un carrattere della scritta FROGGER in colonne.
#define FROGGER_COLUMNS 12 

// Spazio tra un lettera e l'altra in colonne della scritta FROGGER.
#define FROGGER_SPACE 3

// Altezza di un carrattere  della scritta VS in righe.
#define VS_ROWS 6

// Larghezza di un carrattere  della scritta VS in colonne.
#define VS_COLUMNS 20

// Altezza di un carrattere della scritta PLANTS in colonne.
#define PLANTS_ROWS 6

// Larghezza di un carrattere della scritta PLANTS in colonne.
#define PLANTS_COLUMNS 10

// Spazio tra un lettera e l'altra in colonne della scritta PLANTS.
#define PLANTS_SPACE 2

// Altezza del cursore nei menu in righe.
#define CURSOR_ROWS 8

// Larghezza del cursore nei menu in righe.
#define CURSOR_COLUMNS 16

// Altezza di un carrattere nell'interfaccia in righe.
#define INTERFACE_ROWS 8

// Larghezza di un carrattere nell'interfaccia in righe.
#define INTERFACE_COLUMNS 9

// Dimensione massima dei numeri presenti nello score.
#define POINT_NUM 4

// Altezza di un numero per lo SCORE in righe.
#define SCORE_ROWS 7

// Larghezza di un numero per lo SCORE in righe.
#define SCORE_COLUMNS 9

// Spazio tra un numero e l'altro in righe delle serie di numeri per lo score.
#define LOW_SPACE 3


/********\
*  MENU  *
\********/

// Funzione per utilizzare il menu
short Menu();

// Stampa il background del menu.
void printBackMenu(short x, short y, short topGrass, short river,short bottomGrass);

// Stampa il menu iniziale.
void printMainMenu(short x, short y, short pointerY);

//Stampa il menu delle difficoltà.
void printDifficultyMenu(short x, short y,short pointerY);

//Stampa il menu degli score salvati.
void printScoreMenu(short x, short y);

//Stampa le scritte di selezione del menu iniziale(PLAY,SCORE,EXIT).
void printInterfaceMenu(short x, short y,short z);


/***********\
*  Utilità  *
\***********/

void printCursor(short x, short y);

// Preleva dal file gli score delle vecchie partite.
void GetScore(char easy[],char medium[], char hard[]);

// Carica nel file gli score.
void SetScore(char easy[],char medium[], char hard[]);

// Converte un array char in short.
short charToShort(char array[]);


/******************\
*  STAMPA SCRITTE  *
\******************/

// Stampa la scritta FROGGER in stile titolo.
void printFROGGER(short x, short y);

// Stampa la scritta FROGGER in stile Sottotitolo.
void printPLANTS(short x, short y);

// Stampa la scritta VS tra titolo e sottotitolo.
void printVS(short x, short y);

// Stampa la scritta SCORE in stile titolo.
void printSCORE(short x, short y);

// Stampa lo socre di una difficoltà;
void printPoint(short x, short y, char P[],short z);

//Stampa le scritte delle varie difficoltà(EASY,MEDIUM,HARD).
void printDifficultyInterface(short x, short y);


/********************\
*  STAMPE CARATTERI  *
\********************/

// Stampa la lettera F in stile titolo.
void printF(short x, short y);

// Stampa la lettera R in stile titolo.
void printR(short x, short y);

// Stampa la lettera O in stile titolo.
void printO(short x, short y);

// Stampa la lettera G(più grande) in stile titolo.
void printBigG(short x, short y);

// Stampa la lettera G in stile titolo.
void printG(short x, short y);

// Stampa la lettera E in stile titolo.
void printE(short x, short y);

// Stampa la lettera C in stile titolo.
void printC(short x, short y);

// Stampa la lettere S in stile titolo.
void printS(short x, short y);

// Stampa la lettere P in stile sottotitolo.
void printPlatsP(short x, short y);

// Stampa la lettere L in stile sottotitolo.
void printPlatsL(short x, short y);

// Stampa la lettere A in stile sottotitolo.
void printPlatsA(short x, short y);

// Stampa la lettere N in stile sottotitolo.
void printPlatsN(short x, short y);

// Stampa la lettere T in stile sottotitolo.
void printPlatsT(short x, short y);

// Stampa la lettere S in stile sottotitolo.
void printPlatsS(short x, short y);

// Stampa la lettere P con lo stile per le interfacce.
void printInterfacciaP(short x, short y);

// Stampa la lettere L con lo stile per le interfacce.
void printInterfacciaL(short x, short y);

// Stampa la lettere A con lo stile per le interfacce.
void printInterfacciaA(short x, short y,short colore);

// Stampa la lettere Y con lo stile per le interfacce.
void printInterfacciaY(short x, short y,short colore);

// Stampa la lettere S con lo stile per le interfacce.
void printInterfacciaS(short x, short y,short colore);

// Stampa la lettere C con lo stile per le interfacce.
void printInterfacciaC(short x, short y);

// Stampa la lettere O con lo stile per le interfacce.
void printInterfacciaO(short x, short y);

// Stampa la lettere R con lo stile per le interfacce.
void printInterfacciaR(short x, short y,short colore);

// Stampa la lettere E con lo stile per le interfacce.
void printInterfacciaE(short x, short y,short colore);

// Stampa la lettere X con lo stile per le interfacce.
void printInterfacciaX(short x, short y);

// Stampa la lettere I con lo stile per le interfacce.
void printInterfacciaI(short x, short y,short colore);

// Stampa la lettere T con lo stile per le interfacce.
void printInterfacciaT(short x, short y);

// Stampa la lettere M con lo stile per le interfacce.
void printInterfacciaM(short x, short y);

// Stampa la lettere D con lo stile per le interfacce.
void printInterfacciaD(short x, short y,short colore);

// Stampa la lettere U con lo stile per le interfacce.
void printInterfacciaU(short x, short y);

// Stampa la lettere H con lo stile per le interfacce.
void printInterfacciaH(short x, short y);

// Stampa il nuemero 0 con lo stile per dello score.
void printScore0(short x, short y,short colore);

// Stampa il nuemero 1 con lo stile per dello score.
void printScore1(short x, short y,short colore);

// Stampa il nuemero 2 con lo stile per dello score.
void printScore2(short x, short y,short colore);

// Stampa il nuemero 3 con lo stile per dello score.
void printScore3(short x, short y,short colore);

// Stampa il nuemero 4 con lo stile per dello score.
void printScore4(short x, short y,short colore);

// Stampa il nuemero 5 con lo stile per dello score.
void printScore5(short x, short y,short colore);

// Stampa il nuemero 6 con lo stile per dello score.
void printScore6(short x, short y,short colore);

// Stampa il nuemero 7 con lo stile per dello score.
void printScore7(short x, short y,short colore);

// Stampa il nuemero 8 con lo stile per dello score.
void printScore8(short x, short y,short colore);

// Stampa il nuemero 9 con lo stile per dello score.
void printScore9(short x, short y,short colore);
