#define MM 3
typedef struct List *list;
typedef struct  node *link;

typedef struct wind{ // [o,c)
    int o; //opening
    int c; //closing -> opened
}window;

typedef struct content{
    int e;
    int ts;
}content;

struct node{
    window w; //finestra
    content *c; //content:(vettore di (e,ts)
    int nc; //numero di oggetti content nella finestra
};

struct List{ //Buffer
    link *head; //prima finestra
    int N; //nrappresenta la casella successiva all'ultimo arrivo
    int M; //indicatore della prima cella occupata
    int c; //prima chiusura
    int count; //numero caselle passate nel tempo
};

list x;


void allocaBuffer(int ts);
void addToBuffer(int e, int ts, int l);

