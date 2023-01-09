#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "AlterazioneBuffer.h"
#include <time.h>

int slide = 2; //scelgo un slide per le finestre
int width = 2; //scelgo una larghezza per le finestre
int tprev = 0; //previous time
double tot = 0.0;

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

int chooseIfSkip(ts){
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
                o = x->head[MM - 1]->w.o + width;
                i = 0;
            } else {
                i = x->N;
                o = x->head[x->N - 1]->w.o + width;
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

void extractData(link h,int ***cont){ //mi estrae un vettore di content (e,ts)
    int **p;
    if (h->nc!=0) {
        p = malloc(h->nc * (sizeof(int *)));
        for (int i = 0; i < h->nc;i++){
            p[i] = malloc(2*(sizeof (int)));
            p[i][0] = h->c[i].e; //estraggo l'elemento
            p[i][1] = h->c[i].ts; //estraggo il relativo timestamp
        }
        *cont=p;
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

void compute(link h, int **content){ //stampa il content
    FILE *fp;
    fp = fopen("log.txt","a");
    if (h->nc>0) { //se è presente del content
        fprintf(fp,"%d, %d, %d",tprev,h->w.o,h->w.c);
        for (int i = 0; i < h->nc; i++)
            fprintf(fp,", < %d, %d >", content[i][0], content[i][1]);
        fprintf(fp,"\n");
    }
    fclose(fp);
}

void windowing(int e, int ts){
    int l;
    FILE *f = fopen("micro.txt","a");

    double time_spent = 0.0;
    double time_spent2 = 0.0;

    fprintf(f,"ts = %d\n",ts);

    clock_t beg;
    clock_t dne;

    clock_t begin = clock();

    allocaBuffer(ts); //alloco il buffer

    clock_t end = clock();

    // calcola il tempo trascorso trovando la differenza (end - begin) e
    // dividendo la differenza per CLOCKS_PER_SEC per convertire in secondi
    time_spent += (double)(end - begin) / CLOCKS_PER_SEC;

    fprintf(f,"The allocaBuffer time is %f seconds\n", time_spent);

    if (ts>=tprev) {
        time_spent=0.0;
        begin = clock();

        l = scope(ts); //trovo l'indice della finestra in cui mettere il content

        end = clock();
        tot+= time_spent += (double)(end - begin) / CLOCKS_PER_SEC;
        fprintf(f,"The scope time is %f seconds\n", time_spent);
        time_spent=0.0;
        begin = clock();

        addToBuffer(e, ts, l); //aggiungo il content
        end = clock();
        tot+= time_spent += (double)(end - begin) / CLOCKS_PER_SEC;
        fprintf(f,"The addToBuffer time is %f seconds\n", time_spent);
        time_spent=0.0;
        begin = clock();

        if (tick(tprev, ts) && chooseIfSkip(ts)==1) {
            end = clock();
            tot+= time_spent += (double)(end - begin) / CLOCKS_PER_SEC;
            fprintf(f,"The tick time is %f seconds\n", time_spent);
            time_spent=0.0;
            begin = clock();
            int k = x->N;
            if (x->N >
                x->M) { //due casi: N>M -> posso iterare nella tabella tranquillamente, N<M significa che la prima finestra ha sede in M e l'ultima in N e dunque per iterare la tabella servono due diversi cicli: il primo fino all'ampiezza massima e il secondo che parte da zero e arriva ad N

                for (int i = x->M; i < k; i++) {
                    if (active(x->head[i]->w, ts)) {
                        int **cont;
                        begin = clock();
                        extractData(x->head[i], &cont);
                        end = clock();
                        tot+=time_spent += (double)(end - begin) / CLOCKS_PER_SEC;

                        if (report(x->head[i]->w, ts)) {
                            compute(x->head[i], cont);
                        }
                    }
                }
                fprintf(f,"The extractData time is %f seconds\n", time_spent);
                time_spent=0.0;
                begin = clock();
                for (int i = x->M; i < k; i++) {
                    if (x->head[i]->w.c <= ts) {
                        evictWindow(ts, i);
                    }
                }
                end = clock();
                tot+=time_spent += (double)(end - begin) / CLOCKS_PER_SEC;
                fprintf(f,"The evictWindow time is %f seconds\n", time_spent);
            } else {

                for (int i = x->M; i < MM; i++) { //primo ciclo
                    if (active(x->head[i]->w, ts)) {
                        int **cont;
                        begin = clock();
                        extractData(x->head[i], &cont);
                        end = clock();
                        tot+=time_spent += (double)(end - begin) / CLOCKS_PER_SEC;
                        if (report(x->head[i]->w, ts)) {
                            compute(x->head[i], cont);
                        }
                    }
                }
                for (int i = x->M; i < MM; i++) {
                    if (x->head[i]->w.c <= ts) {
                        beg = clock();
                        evictWindow(ts, i);
                        dne = clock();
                        tot+=time_spent2+= (double)(end - begin) / CLOCKS_PER_SEC;
                    }
                }
                if (k != 0) { //secondo ciclo
                    for (int i = 0; i < k; i++) {
                        if (active(x->head[i]->w, ts)) {
                            int **cont;
                            begin = clock();
                            extractData(x->head[i], &cont);
                            end = clock();
                            tot+=time_spent += (double)(end - begin) / CLOCKS_PER_SEC;
                            if (report(x->head[i]->w, ts)) {
                                compute(x->head[i], cont);
                            }
                        }
                    }
                    fprintf(f,"The extractData time is %f seconds\n", time_spent);
                    for (int i = 0; i < k; i++) {
                        if (x->head[i]->w.c <= ts) {
                            beg = clock();
                            evictWindow(ts, i);
                            dne = clock();
                            tot+=time_spent2+= (double)(end - begin) / CLOCKS_PER_SEC;
                        }
                    }
                    fprintf(f,"The evictWindow time is %f seconds\n", time_spent2);
                }
            }
        }
        tprev = ts;
    }
    fprintf(f,"tot time: %f\n",tot);
    fclose(f);
}