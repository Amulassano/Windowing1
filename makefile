AlterazioneWindowing: main.o AlterazioneWindowing.o AlterazioneBuffer.o
	gcc main.o -o AlterazioneWindowing

main.o: main.c AlterazioneWindowing.h
	gcc -c main.c

AlterazioneWindowing.o: AlterazioneWindowing.c AlterazioneBuffer.h AlterazioneWindowing.h
	gcc -c AlterazioneWindowing.c
	
AlterazioneBuffer.o: AlterazioneBuffer.c AlterazioneBuffer.h
	gcc -c AlterazioneBuffer.c
	
clean:
	rm *.o AlterazioneWindowing
