#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct List *list;
typedef struct  node *link;

typedef struct wind{
    int o;
    int c;
}window;

struct node{
    link next;
    window w;
    int **content;
    int nc; //numero di oggetti content nella finestra
};
struct List{
    link head;
    int n;
};

int t0 = 0;
int slide = 2;
int width = 2;
int tprev = 0;
list x ;

link newNode(int *content, int o,int c, link next){ // mi crea una nuova lista
    link p = malloc(sizeof *p);
    if (p == NULL)
        return NULL;
    else {
        p->next = next;
        p->w.c = c;
        p->w.o = o;
        p->nc = 0;
        p->content = NULL;
        if (content != NULL) {
            p->nc++;
            p->content = malloc(sizeof (int*));
            p->content[0] = malloc(2*sizeof (int));
            p->content[0][0] = content[0];
            p->content[0][1] = content[0];
        }
    }
    return p;
}

link listInsTail(link h,int *content, int o,int c){ //inserisce in coda una lista: utile per lo scope
    link p;
    if (h==NULL)
        return newNode(content,o,c,NULL);
    for (p=h; p->next != NULL ; p= p->next);
    p->next = newNode(content,o,c,NULL);
    return h;
}

void contentInsert(int e,int ts, link h){ //inserisce il content: conosco già la finestra in cui metterlo
    link p = h;
    p->nc++;
    if (p->content==NULL) {
        p->content = malloc((p->nc) * (sizeof(int *)));
        p->content[p->nc - 1] = malloc(2 * sizeof(int));
    }
    else{
        p->content = realloc(p->content,(p->nc) * (sizeof(int *)));
        p->content[p->nc - 1] = malloc(2 * sizeof(int));
    }
    p->content[p->nc-1][0] = e;
    p->content[p->nc-1][1] = ts;
}

void allocaBuffer(){
    if (x==NULL) {
        x = malloc(sizeof *x);
        x->head = NULL;
        x->n = 0;
    }

}

link scope(int ts){
    int c = (int) ceil((double) abs(ts- t0) / (double) slide)*slide;
    int o = c - width;
    link p = x->head;
    if (p==NULL) {
        printf("Calculating the Windows to Open. First one opens at [ %d ] and closes at [ %d ]\n", o, c);

        do {
            printf("Computing window [ %d , %d ] if absent\n", o, o + width);
            x->n++;
            p = listInsTail(p, NULL, o, o + width);
            o += slide;

        } while (o <= ts);
    }
    else{
        link l=p;
        while (l->next!=NULL)
            l=l->next;
        if (l->w.c<=ts) {
            o = l->w.c;
            do {
                printf("Computing window [ %d , %d ] if absent\n", o, o + width);
                x->n++;
                l = listInsTail(l, NULL, o, o + width);
                o += slide;

            } while (o <= ts);
        }
    }
    return p;
}

void addToBuffer(int e, int ts){
    link p = x->head;
    for (int i=0;i<x->n;i++,p=p->next)
        if (p->w.o<=ts && ts<=p->w.c)
            contentInsert(e,ts,p);
}

int tick(int tau, int ts){
    if (ts>=tau)
        return 1;
    return 0;
}

void extractData(link h,int ***cont){ //mi estrae un vettore di content (e,ts)
    int **p;
    if (h->nc!=0) {
        p = malloc(h->nc * (sizeof(int *)));
        for (int i = 0; i < h->nc;i++){
            p[i] = malloc(2*(sizeof (int)));
            p[i][0] = h->content[i][0];
            p[i][1] = h->content[i][1];
        }
        *cont=p;
    }
    else
        return ;
}

int active(window w, int ts){ //controllo se la finestra è attiva o meno
    if (w.o<=ts && ts<= w.c)
        return 1;
    return 0;
}

int report(window w, int ts){ //report: window_close
    if (w.c==ts)
        return 1;
    return 0;
}

link evictWindow(){ //elimina la finestra
    link p = x->head;
    printf("Evicting [ %d, %d ]",p->w.o,p->w.c);
    if (x->head==NULL)
        return NULL;
    x->head = x->head->next;
    if (p->nc>0) {
        for (int i = 0; i < p->nc; i++)
            free(p->content[i]);
        free(p->content);
    }
    free(p);
    x->n--;
    return x->head;
}

void compute(int nc, int **content){ //stampa il content
    if (nc>0)
        for (int i=0;i<nc;i++)
            printf("Content: ( %d, %d )\n",content[i][0],content[i][1]);
}

void windowing(int e, int ts){

    allocaBuffer();

    if (ts>=tprev)
        x->head = scope(ts);

    addToBuffer(e,ts);

    if (tick(tprev,ts)) {
        link h=x->head;
        int l = x->n;
        for (int i=0;i<l;i++) {
            if (active(h->w, ts)) {
                int **cont;
                extractData(h, &cont);
                if (report(h->w, ts)) {
                    compute(h->nc, cont);

                }
            }
            h = h->next;            
        }
        h=x->head;
        for (int i=0;i<l;i++){
            if (h->w.c<=ts) {
                x->head = evictWindow();
                printf(", ts: %d\n",ts);
                h = x->head;
                }
        }
    }
    tprev= ts;
}

int main(){


    for(int i=0;i<10;i++)
        windowing(i,i);


    return 0;
};
