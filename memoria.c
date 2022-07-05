#include	<stdio.h>
#include 	<stdlib.h>
#include 	<stdbool.h>
#include 	<time.h>

#define TAMANHO_MEMORIA_PRIMARIA 64
#define TAMANHO_MEMORIA_SECUNDARIA 16
#define QUANT_THREADS 20
#define QUANT_PAGINAS 50
#define MAX_WORKSET 4

// Estrutura de paginas para nas memórias
typedef struct{
    //bool memoriaPrincipal; //verifica se pagina esta na memoria principal ou na secundaria
    int idProcesso;
    int idPagina;
    int tempoNaMemoria;
}Pagina;

// Vetor das memorias principal e secundaria
struct Pagina memoria_principal[TAMANHO_MEMORIA_PRIMARIA];
struct Pagina memoria_secundaria[TAMANHO_MEMORIA_SECUNDARIA];


int main( int argc, char *argv[ ] ){
    int indiceProcessoDasPaginas[20];


    //gerando processos
    for (int i = 0; i<QUANT_THREADS;i++){
        Pagina pagina;
        pagina.idProcesso = i;
        pagina.idPagina = rand() % 50;
        pagina.tempoNaMemoria = 0;
        sleep(3);
    }

    return 0;
}

