#include "AlterazioneWindowing.h"
#include <stdio.h>
#include <time.h>
#define file "final5000-1.csv"

int main(){

    FILE *ff = fopen("log.txt","w");
    fclose(ff);
    ff = fopen("evict.txt","w");
    fclose(ff);



    // per memorizzare il tempo di esecuzione del codice
    double time_spent = 0.0;




    ff = fopen(file,"w");
   // fprintf(ff,"SIZE_BUFFER,SIZE_WINDOW, ts, allocaBuffer, scope, addToBuffer, tick, extractData, evictWindow, totem\n");
    fclose(ff);

    clock_t begin = clock();

    char h[1][20];
    for (int i=0;i<1;i++)
        for (int l=0;l<20;l++)
            h[i][l]='h';


    for (int i=1;i<15999;i++){
        FILE *ff = fopen(file,"a");
        fprintf(ff,"100000");fprintf(ff,",5000");
        fclose(ff);
        windowing(h,i);
    }


    clock_t end = clock();

    // calcola il tempo trascorso trovando la differenza (end - begin) e
    // dividendo la differenza per CLOCKS_PER_SEC per convertire in secondi
    time_spent += (double)(end - begin) / CLOCKS_PER_SEC;


    return 0;
}
