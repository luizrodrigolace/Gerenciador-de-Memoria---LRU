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

typedef struct{
    int posMP;
    bool isInMP;
}Tabela;

// Vetor das memorias principal e secundaria
Pagina memoria_principal[TAMANHO_MEMORIA_PRIMARIA];
Tabela tabela_de_paginas[QUANT_THREADS][QUANT_PAGINAS];

int main( int argc, char *argv[ ] ){
    //int workingSet[20];
    int posMP = 0;

    //gerando processos
    for (int i = 0; i<QUANT_THREADS;i++){
        //gerando paginas dos processos existentes até então
        for (int j = 0; j<=i; j++){
            Pagina pagina;
            pagina.idProcesso = j;
            pagina.idPagina = rand() % 50;
            pagina.tempoNaMemoria = 0;
            memoria_principal[posMP] = pagina;
            printf("Colocando a pagina %d do processo %d na posição %d da MP\n",memoria_principal[posMP].idPagina,memoria_principal[posMP].idProcesso,posMP);
            posMP++;
        }
        sleep(3);
        printf("\n");
    }
    // continuando após a criação de todos os processos
    for(int i = 0; i<64;i++){
            Pagina pagina;
            pagina.idProcesso = i;
            pagina.idPagina = rand() % 50;
            pagina.tempoNaMemoria = 0;
            memoria_principal[posMP] = pagina;
            printf("Colocando a pagina %d do processo %d na posição %d da MP\n",memoria_principal[posMP].idPagina,memoria_principal[posMP].idProcesso,posMP);
            posMP++;
            sleep(3);
    }



    // gera um processo a cada 3s, cada processo gera uma pagina aleatoria a cada 3s - ok
    // randomiza uma pagina (0-49) do processo e coloca na MP - ok
    // atualiza as paginas de cada processo na MP (LRU)
    // Faz isso até a MP ficar cheia 
    // Quando a MP ficar cheia,
    // tira a pagina do processo mais antigo da MP e coloca na MV
    // ??????????????????
    // criterio de parada: ??

    return 0;
}

