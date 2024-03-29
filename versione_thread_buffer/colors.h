/************\
*  COSTANTI  *
\************/

// Moltiplicando un qualsiasi valore RGB [0 - 255] per questa costante lo si sposta nella scala [0 - 1000] (approssimata) richiesta da ncurses.
#define CONVERSION_CONSTANT 3.921

// Semplifica il cambio della coppia di colori attiva.
#define CHANGE_COLOR(n) attron(COLOR_PAIR(n))


/*************************\
*  PROTOTIPI DI FUNZIONE  *
\*************************/

// Inizializza il colore N con i valori rgb.
void inizializzaColore(int N, int r, int g, int b);

// Crea tutte le coppie di colori necessarie per il gioco.
void istanziaColori();


/******************\
*  SINGOLI COLORI  *
\******************/

// Colore per il fiume.
#define CELESTE 9

// Colore per il coccodrillo di background.
#define BLU 10

// Colore del prato.
#define ERBA 11

// Colore dei filetti d'erba e del bordo delle ninfee.
#define VERDE_SCURO 12

// Colore interno ninfea.
#define VERDE_CHIARO 13

// Colore nero (per le pupille).
#define NERO 14

// Colore per l'iride.
#define GIALLINO 15

// Colore del coccodrillo buono.
#define VERDE_CROCC 16

// Colore del coccodrillo cattivo.
#define DARK_CROCC 17

// Colore dei denti dei coccodrilli.
#define BIANCO 18

// Colore del gambo dei nemici.
#define GAMBO 19

// Colore del centro dei nemici.
#define CENTRO 20

// Colore dei petali dei nemici.
#define PETALI 21

// Colore occhi della rana.
#define EYES 22

// Colore pelle della rana.
#define COLORE_RANA 23

// Colore della scoreboard con poco tempo (<= 1/3).
#define LOW 24

// Colore della scoreboard con abbastanza tempo (<= 2/3 && > 1/3). 
#define MID 25

// Colore della scoreboard con molto tempo (<= 3/3 && > 2/3).
#define HIGH 26

// Colore di default per foreground.
#define DEFAULT_FG 27

// Colore di default per background.
#define DEFAULT_BG 28

// Colore per il proiettile della rana.
#define PURPLE_PROJ 29

// Colore bordo titolo
#define VIOLA_TIT 30

// Colore rosa del VS
#define ROSA 31

// Colore azzurro del VS
#define AZZURRO 32


/********************\
*  COPPIE DI COLORI  *
\********************/

// Coppia di colori da richiamare per il fiume.
#define FIUME 10

// Coppia di colori da richiamare per il prato.
#define PRATO 11

// Coppia di colori da richiamare per il prato che circonda l' acqua delle tane.
#define PRATO_E_ACQUA 12

// Coppia di colori da richiamare per l'acqua che circondare le tane.
#define ACQUA_E_BORDO 13

// Coppia di colori da richiamare per bordo e interno della ninfea.
#define NINFEA 14

// Coppia di colori da richiamare per fiume e coccodrillo.
#define ACQUA_E_CROCC 15 

// Coppia di colori da richiamare per la bocca del coccodrillo.
#define DENTI_CROCC 16 

// Coppia di colori da richiamare per la pupilla del coccodrillo.
#define PUPILLA_CROCC 17 

// Coppia di colori da richiamare per l'iride del coccodrillo.
#define IRIDE_CROCC 18 

// Coppia di colori da richiamare per fiume e coccodrillo.
#define ACQUA_E_DARK_CROCC 19 

// Coppia di colori da richiamare per la bocca del coccodrillo.
#define DENTI_DARK_CROCC 20 

// Coppia di colori da richiamare per la pupilla del coccodrillo.
#define PUPILLA_DARK_CROCC 21 

// Coppia di colori da richiamare per l'iride del coccodrillo.
#define IRIDE_DARK_CROCC 22 

// Coppia di colori da richiamare per petali e prato.
#define PETALI_E_PRATO 23

// Coppia di colori da richiamare per petali e centro.
#define PETALI_E_CENTRO 24

// Coppia di colori da richiamare per il centro.
#define FACCIA_CENTRO 25

// Coppia di colori da richiamare per i petali e il gambo.
#define PETALI_E_GAMBO 26

// Coppia di colori da richiamare per il gambo e il prato.
#define GAMBO_E_PRATO 27

// Coppia di colori per la rana.
#define RANA 28

// Colore della scoreboard con poco tempo (<= 1/3).
#define SC_LOW 29

// Colore della scoreboard con abbastanza tempo (<= 2/3 && > 1/3). 
#define SC_MID 30

// Colore della scoreboard con molto tempo (<= 3/3 && > 2/3).
#define SC_HIGH 31

// Colore di default (utile per i bordi).
#define DEFAULT 32

// Coppia di colori per i proiettili.
#define PROIETTILI 33

// Coppia di colori da richiamare per la scritta del menu (ViolaS, ViolaC).
#define TITOLOS_E_TITOLOC 34

// Coppia di colori bordo titolo e prato.
#define PRATO_E_TITOLO 35

// Coppia di colori fiume e rana.
#define FIUME_E_RANA 36

// Coppia di colori Rosa del VS e prato.
#define ROSA_E_PRATO 37

// Coppia di colori Rosa del VS e Azzurro del VS.
#define ROSA_E_AZZURRO 38

// Coppia di colori Celeste del VS e Azzurro del VS.
#define CELESTE_E_AZZURRO 39

// Coppia di colori occhio rana e Celeste del fiume.
#define EYES_E_CELESTE 40

// Coppia di colori da richiamare per le scritte dell'interfaccia Nero e Bianco.
#define NERO_E_BIANCO 41

// Coppia di colori da richiamare per le scritte dell'interfaccia Nero e Celeste.
#define NERO_E_CELESTE 42

// Coppia di colori da richiamare per la scritta easy della difficolta'.
#define NERO_E_VERDE 43

// Coppia di colori da richiamare per la scritta medium della difficolta'.
#define NERO_E_GIALLO 44

// Coppia di colori da richiamare per la scritta hard della difficolta'.
#define NERO_E_ROSSO 45

// Coppie di colori da richiamare per le schermate di debug.
#define RED_DEBUG 46
#define GREEN_DEBUG 47

// Coppie di colori da richiamare per il segnale di pericolo.
#define ROSSO_E_BIANCO 48
#define ROSSO_E_CELESTE 49