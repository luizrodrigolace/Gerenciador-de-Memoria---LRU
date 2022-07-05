#include	<stdio.h>
#include 	<stdlib.h>
#include 	<stdbool.h>
#include 	<time.h>

#define TAMANHO_MEMORIA_PRIMARIA 16
#define TAMANHO_MEMORIA_SECUNDARIA 16
#define QUANT_THREADS 5
#define QUANT_PAGINAS 10
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
    int isInMP;
}Tabela;

Pagina criaPaginaAleatoria(int processo){
    Pagina pagina;
    pagina.idProcesso = processo;
    pagina.idPagina = rand() % QUANT_PAGINAS;
    pagina.tempoNaMemoria = 0;

    return pagina;
}

// imprimindo o conteúdo da matriz
void printaMatriz (Tabela matriz[QUANT_THREADS][QUANT_PAGINAS]){
    for (int i = 0; i < QUANT_THREADS; i++) { //para as linhas
        for (int j = 0; j < QUANT_PAGINAS; j++) { //para as colunas
            printf("%d ", matriz[i][j].isInMP);
        }
        printf("\n"); //salta uma linha
    }
}

//metodo para verificar se uma dada pagina já se encontra na MP
bool verificaPaginaNaMP(Pagina pagina,Tabela matriz[QUANT_THREADS][QUANT_PAGINAS]){
    if(matriz[pagina.idProcesso][pagina.idPagina].isInMP == 1){
        return true;
    }
    return false;
}

// Vetor das memorias principal e secundaria
Pagina memoria_principal[TAMANHO_MEMORIA_PRIMARIA];
Tabela tabela_de_paginas[QUANT_THREADS][QUANT_PAGINAS];

int main( int argc, char *argv[ ] ){
    //int workingSet[20];
    int posMP = 0;

    //gerando processos de 0 até 19
    for (int i = 0; i<QUANT_THREADS;i++){
        //gerando paginas dos processos existentes até então
        for (int j = 0; j<=i; j++){
            Pagina pagina = criaPaginaAleatoria(j);
            //verificando se a pagina se encontra na MP
            if(verificaPaginaNaMP(pagina,tabela_de_paginas)){
                memoria_principal[tabela_de_paginas[pagina.idProcesso][pagina.idPagina].posMP].tempoNaMemoria = 0;
                printf("O processo %d pediu para alocar a pagina %d, que já estava na MP. Tempo atualizado\n",j,pagina.idPagina);
                continue;
            }

            memoria_principal[posMP] = pagina;
            //marcando na tabela qual pagina de qual processo está na MP
            tabela_de_paginas[j][memoria_principal[posMP].idPagina].isInMP = 1;
            tabela_de_paginas[j][memoria_principal[posMP].idPagina].posMP = posMP;

            printf("Colocando a pagina %d do processo %d na posição %d da MP\n",memoria_principal[posMP].idPagina,memoria_principal[posMP].idProcesso,posMP);
            
            posMP++;
            if (posMP > TAMANHO_MEMORIA_PRIMARIA - 1){
                break;
            }
        }
        //printaMatriz(tabela_de_paginas);
        //sleep(3);
        //system("clear");
        printf("\n");
        if (posMP > TAMANHO_MEMORIA_PRIMARIA - 1 ){
                break;
        }
    }

    // continuando após a criação de todos os processos
    /*for(int i = 0; i<64;i++){
            Pagina pagina;
            pagina.idProcesso = i;
            pagina.idPagina = rand() % 50;
            pagina.tempoNaMemoria = 0;
            memoria_principal[posMP] = pagina;
            printf("Colocando a pagina %d do processo %d na posição %d da MP\n",memoria_principal[posMP].idPagina,memoria_principal[posMP].idProcesso,posMP);
            posMP++;
            sleep(3);
    }*/

    return 0;
}

