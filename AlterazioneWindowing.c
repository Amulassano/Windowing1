#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "AlterazioneBuffer.h"
#include <time.h>
#define file "final5000-1.csv"

int slide = 5; //scelgo un slide per le finestre
int width = 5000; //scelgo una larghezza per le finestre
int tprev = 0; //previous time

typedef struct{ //Data interface type
    char **e;
    int ts;
}data;

link newNode( int o,int c){ // mi crea un nuovo nodo nella lista
    link p = malloc(sizeof *p);
    p->w.c = c; //c = chiusura della finestra
    p->w.o = o; //o = apertura della finestra
    p->nc = 0; //indica il numero di contenuti all'interno della finestra considerata
    p->c = NULL; //al momento della creazione di un nodo il contenuto non è esistente
    return p;
}

void evictWindow(int ts,int l){ //elimina la finestra
    FILE *ff;
    ff = fopen("evict.txt","a");
    fprintf(ff,"Evicting [ %d, %d ), ts= %d\n",x->head[l]->w.o,x->head[l]->w.c,ts);
    if (x->head[l]->nc>0) { //se è presente del content all'interno vado a liberare lo spazio allocatogli precedentemente
        for (int i=0;i<x->head[l]->nc;i++) {
            for (int f = 0; f < Num_String; f++)
                free(x->head[l]->c[i].e[f]);
            free(x->head[l]->c[i].e);
        }

        free(x->head[l]->c);
        x->head[l]->c=NULL;
        x->head[l]->nc=0;
    }
    x->M++; //diminuisco nel Buffer il totale delle finestre presenti
    if (x->M >=MM)
        x->M -= MM;
    fclose(ff);
}

int giveMeLast(){ //mi ritorna la posizione dell'ultima finestra aperta se la tabella fosse infinita
    if (x->count==0)
        return 0;
    return (int)ceil((double)(tprev-x->c)/(double)slide);
}

void TakeAllOff(int pos){
    x->M =pos%MM;
    x->N =(pos)%MM;
    free(x->head);
    x->head = malloc(MM*(sizeof (x->head)));
}

int chooseIfSkip(int ts){
    int l = (int)ceil((double)(ts-x->c)/(double)slide);
    int last = giveMeLast();
    if (l-last<MM)
        return 1;
    return 0;
}

int scope(int ts){ //funzione scope, ritorna posizione esatta di dove inserire (e,ts)
    int c = x->c;
    int o = c - width; //calcolo l'apertura della prima finestra
    int i = 0;
    int l;
    l = (int)ceil((double)(ts-c)/(double)slide); //hash
    int last = giveMeLast();
    if (chooseIfSkip(ts)) {
        l= l%MM; //avendo MM celle, la posizione della mia finestra sarà sempre al suo interno
        if (x->count == 0) {
            printf("Calculating the Windows to Open. First one opens at [ %d ] and closes at [ %d ],ts=%d\n", o, c, ts);
            do { //ciclo che calcola, dalla prima finestra, tutte le finestre fino al mio timestamp attuale
                printf("Computing window [ %d , %d ) if absent\n", o, o + width);
                x->N++;
                x->count++;
                if (x->N >= MM) //se l'ultima casella supera il l'ampiezza della tabella, si ritorna da zero
                    x->N = x->N - MM;
                x->head[i] = newNode(o, o + width);
                o += slide;
                i++;
            } while (o <= ts);
        } else {
            printf("Calculating the Windows to Open. First one opens at [ %d ] and closes at [ %d ],ts=%d\n",
                   x->head[x->M]->w.o, x->head[x->M]->w.c, ts);
            //calcola le nuove finestre e le aggiunge a quelle già presenti

            if (x->N == 0) {
                o = x->head[MM - 1]->w.o + slide;
                i = 0;
            } else {
                i = x->N;
                o = x->head[x->N - 1]->w.o + slide;
            }
            if (o <= ts) {
                while (o <= ts) {
                    printf("Computing window [ %d , %d ) if absent\n", o, o + width);
                    x->N++;
                    x->count++;
                    if (x->N >= MM) //se l'ultima casella supera il l'ampiezza della tabella, si ritorna da zero
                        x->N = x->N - MM;
                    x->head[i] = newNode(o, o + width);
                    o += slide;
                    i++;
                    if (i >= MM) //il contatore riparte dalla prima casella
                        i -= MM;
                }
            }
        }
    }
    else{
        last = last%MM;
        int ch = x->head[last]->w.c;
        while (ch<ts)
            ch += width;
        TakeAllOff(l);
        o = ch-width;
        i = l%MM;
        printf("Calculating the Windows to Open. First one opens at [ %d ] and closes at [ %d ],ts=%d\n", o, ch, ts);
        do { //ciclo che calcola, dalla prima finestra, tutte le finestre fino al mio timestamp attuale
            printf("Computing window [ %d , %d ) if absent\n", o, o + width);
            x->N++;
            x->count++;
            if (x->N >= MM) //se l'ultima casella supera il l'ampiezza della tabella, si ritorna da zero
                x->N = x->N - MM;
            x->head[i] = newNode(o, o + width);
            o += slide;
            i++;
            i = i%MM;
        } while (o <= ts);
    }
    return l;
}

int tick(int tau, int ts){ //funzione tick: ritorna 1 ( ovvero TRUE ) se il timestamp attuale è maggiore o uguale a quello precedente ( tau ), altrimenti 0 ( FALSE )
    if (ts>=tau)
        return 1;
    return 0;
}

void extractData(link h,data *cont){ //mi estrae un vettore di content (e,ts)
    if (h->nc!=0) {
        for (int i = 0; i < h->nc;i++){
            for (int l=0;l<Num_String;l++)
                for (int t=0;t<String_Lenght;t++)
                    cont[i].e[l][t] = h->c[i].e[l][t]; //estraggo l'elemento
            cont[i].ts = h->c[i].ts; //estraggo il relativo timestamp
        }
    }
    else
        return ;
}

int active(window w, int ts){ //controllo se la finestra è attiva o meno: se l'attuale timestamp è compreso tra gli estremi della finestra allora essa è attiva e restituisce 1, altrimenti no e restituisce zero
    if (w.o<=ts && ts<= w.c)
        return 1;
    return 0;
}

int report(window w, int ts){ //report: window_close ( restituisce ( TRUE ) se timestamp = chiusura della finestra )
    if (w.c==ts)
        return 1;
    return 0;
}

void compute(link h, data *content){ //stampa il content
    FILE *fp;
    fp = fopen("log.txt","a");
    if (h->nc>0) { //se è presente del content
        fprintf(fp,"%d, %d, %d",tprev,h->w.o,h->w.c);

        for (int i = 0; i < h->nc; i++) {
            for (int l = 0; l < Num_String; l++) {
                for (int p = 0; p < String_Lenght; p++) {
                    if (l == 0 && p==0)
                        fprintf(fp, ", < %c", content[i].e[l][p]);
                    else
                        fprintf(fp, "%c", content[i].e[l][p]);
                }
            }
            fprintf(fp, ", %d >", content[i].ts);
        }
        fprintf(fp,"\n");
    }
    fclose(fp);
}

void windowing(char e[Num_String][String_Lenght], int ts){
    int l;
    double tot = 0.0;

    FILE *f = fopen(file,"a");

    double time_spent = 0.0;
    double time_spent2 = 0.0;
    data *cont = NULL;
    fprintf(f,",%d",ts);

    clock_t beg;
    clock_t dne;

    clock_t begin = clock();

    allocaBuffer(ts); //alloco il buffer
    clock_t end = clock();
    time_spent += (double)(end - begin) / CLOCKS_PER_SEC;tot+= time_spent;

    fprintf(f,",%d", (int)(time_spent*1000));

    if (ts>=tprev) {
        time_spent=0.0;
        begin = clock();

        l = scope(ts); //trovo l'indice della finestra in cui mettere il content

        end = clock();
        time_spent += (double)(end - begin) / CLOCKS_PER_SEC;tot+= time_spent;
        fprintf(f,",%d",(int) (time_spent*1000));
        time_spent=0.0;
        begin = clock();

        addToBuffer(e, ts, l); //aggiungo il content

        end = clock();
        time_spent += (double)(end - begin) / CLOCKS_PER_SEC;tot+= time_spent;
        fprintf(f,",%d",(int)( time_spent*1000));
        time_spent=0.0;
        begin = clock();

        if (tick(tprev, ts) && chooseIfSkip(ts)==1) {
            end = clock();
            time_spent += (double)(end - begin) / CLOCKS_PER_SEC;tot+= time_spent;
            fprintf(f,",%d",(int) (time_spent*1000));
            time_spent=0.0;

            int k = x->N;
            if (x->N > x->M) { //due casi: N>M -> posso iterare nella tabella tranquillamente, N<M significa che la prima finestra ha sede in M e l'ultima in N e dunque per iterare la tabella servono due diversi cicli: il primo fino all'ampiezza massima e il secondo che parte da zero e arriva ad N
                for (int i = x->M; i < k; i++) {
                    if (active(x->head[i]->w, ts)) {
                        cont=malloc(x->head[i]->nc * (sizeof(data)));
                        for (int l=0;l<x->head[i]->nc;l++) {
                            cont[l].e = malloc(Num_String * (sizeof(char *)));
                            for (int y = 0; y < Num_String; y++)
                                cont[l].e[y] = malloc(String_Lenght * (sizeof(char)));
                        }
                        begin = clock();
                        extractData(x->head[i], cont);
                        end = clock();
                        time_spent += (double)(end - begin) / CLOCKS_PER_SEC;
                        if (report(x->head[i]->w, ts)) {
                            compute(x->head[i], cont);
                        }
                        for (int l=0;l<x->head[i]->nc;l++) {
                            for (int y = 0; y < Num_String; y++)
                                free(cont[l].e[y]);
                            free(cont[l].e);
                        }
                        free(cont);
                    }

                }
                fprintf(f,",%d",(int) (time_spent*1000));tot+= time_spent;
                time_spent=0.0;
                begin = clock();
                for (int i = x->M; i < k; i++) {
                    if (x->head[i]->w.c <= ts) {
                        evictWindow(ts, i);
                    }
                    else
                        break;
                }
                end = clock();
                time_spent += (double)(end - begin) / CLOCKS_PER_SEC;tot+= time_spent;
                fprintf(f,",%d",(int) (time_spent*1000));
            } else {
                for (int i = x->M; i < MM; i++) { //primo ciclo
                    if (active(x->head[i]->w, ts)) {
                        cont=malloc(x->head[i]->nc * (sizeof(data)));
                        for (int l=0;l<x->head[i]->nc;l++) {
                            cont[l].e = malloc(Num_String * (sizeof(char *)));
                            for (int y = 0; y < Num_String; y++)
                                cont[l].e[y] = malloc(String_Lenght * (sizeof(char)));
                        }
                        begin = clock();
                        extractData(x->head[i], cont);
                        end = clock();
                        time_spent += (double)(end - begin) / CLOCKS_PER_SEC;
                        if (report(x->head[i]->w, ts)) {
                            compute(x->head[i], cont);
                        }
                        for (int l=0;l<x->head[i]->nc;l++) {
                            for (int y = 0; y < Num_String; y++)
                                free(cont[l].e[y]);
                            free(cont[l].e);
                        }
                        free(cont);
                    }
                }
                for (int i = x->M; i < MM; i++) {
                    if (x->head[i]->w.c <= ts) {
                        beg = clock();
                        evictWindow(ts, i);
                        dne = clock();
                        time_spent2+= (double)(dne - beg) / CLOCKS_PER_SEC;
                    } else
                        break;
                }
                if (k != 0) { //secondo ciclo
                    for (int i = 0; i < k; i++) {
                        if (active(x->head[i]->w, ts)) {
                            cont=malloc(x->head[i]->nc * (sizeof(data)));
                            for (int l=0;l<x->head[i]->nc;l++) {
                                cont[l].e = malloc(Num_String * (sizeof(char *)));
                                for (int y = 0; y < Num_String; y++)
                                    cont[l].e[y] = malloc(String_Lenght * (sizeof(char)));
                            }
                            begin = clock();
                            extractData(x->head[i], cont);
                            end = clock();
                            time_spent += (double)(end - begin) / CLOCKS_PER_SEC;
                            if (report(x->head[i]->w, ts)) {
                                compute(x->head[i], cont);
                            }
                            for (int l=0;l<x->head[i]->nc;l++) {
                                for (int y = 0; y < Num_String; y++)
                                    free(cont[l].e[y]);
                                free(cont[l].e);
                            }
                            free(cont);
                        }
                    }
                    fprintf(f,",%d",(int) (time_spent*1000));tot+= time_spent;
                    for (int i = 0; i < k; i++) {
                        if (x->head[i]->w.c <= ts) {
                            beg = clock();
                            evictWindow(ts, i);
                            dne = clock();
                            time_spent2+= (double)(dne - beg) / CLOCKS_PER_SEC;
                        }
                        else
                            break;
                    }
                    fprintf(f,",%d", (int)(time_spent2*1000));tot+= time_spent2;
                }
            }
        }
        tprev = ts;
    }
    fprintf(f,",%d\n",(int)(tot*1000));
    fclose(f);
}