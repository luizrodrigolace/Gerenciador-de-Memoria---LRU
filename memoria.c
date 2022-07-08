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

// Estrutura para a tabela de processos/paginas
typedef struct{
    int posMP;
    int isInMP;
    int isInMS;
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

//metodo para verificar se uma dada pagina já se encontra na MS
bool verificaPaginaNaMS(Pagina pagina,Tabela matriz[QUANT_THREADS][QUANT_PAGINAS]){
    if(matriz[pagina.idProcesso][pagina.idPagina].isInMS == 1){
        return true;
    }
    return false;
}

// varrendo o vetor da MP e adicionando uma unidade de tempo em todas as paginas
void adicionaTempo(int pos, Pagina *MP){
    for (int i = 0; i<pos;i++ ){
        MP[i].tempoNaMemoria += 1;
    }
}

int indiceDaPaginaMaisAntiga(int processo, Tabela tabela[QUANT_THREADS][QUANT_PAGINAS], Pagina MP[TAMANHO_MEMORIA_PRIMARIA]){
    int indiceNaMp;
    int tempo = 0;
    for(int page=0; page<50; page++){
        //caso a pagina esteja na MP
        if(tabela[processo][page].isInMP){
            //verificando qual delas possui o maior tempo
            if (MP[tabela[processo][page].posMP].tempoNaMemoria > tempo){
                indiceNaMp = tabela[processo][page].posMP;
                tempo = MP[tabela[processo][page].posMP].tempoNaMemoria;
            }
        }
    }
    return indiceNaMp;
}

int indiceDaPaginaMaisAntigaTodas(Pagina MP[TAMANHO_MEMORIA_PRIMARIA]){
    int indiceNaMp;
    int tempo = 0;
    for(int i =0; i<TAMANHO_MEMORIA_PRIMARIA ;i++){
        //verificando qual delas possui o maior tempo
        if (MP[i].tempoNaMemoria > tempo){
            indiceNaMp = i;
            tempo = MP[i].tempoNaMemoria;
        }
    }
    return indiceNaMp;
}


// Vetor das memorias principal e secundaria
Pagina memoria_principal[TAMANHO_MEMORIA_PRIMARIA];
Tabela tabela_de_paginas[QUANT_THREADS][QUANT_PAGINAS];


int main( int argc, char *argv[ ] ){
    //srand(time(NULL));

    int posMP = 0;
    int workingSetLimit[20];
    for (int i = 0 ; i < TAMANHO_MEMORIA_PRIMARIA ; i++)
    {
        memoria_principal[i].idPagina = -1;
        memoria_principal[i].idProcesso = -1;
        memoria_principal[i].tempoNaMemoria = -1;
    }
    for (int i = 0 ; i < QUANT_THREADS ; i++)
    {
        for (int j = 0 ; j < QUANT_PAGINAS ; j++)
        {
            tabela_de_paginas[i][j].posMP = -1;
            tabela_de_paginas[i][j].isInMP = 0;
            tabela_de_paginas[i][j].isInMS = 0;
        }
    }
    // zerando working set
    for (int i=0;i<20;i++){
        workingSetLimit[i] = 0;
    };

    //gerando processos de 0 até 19
    for (int i = 0; i<QUANT_THREADS;i++){
        //gerando paginas dos i processos existentes até então
        printf(" Processo %d criado\n",i);
        for (int j = 0; j<=i; j++){
            Pagina pagina = criaPaginaAleatoria(j);

            //verificando se a pagina se encontra na MP
            if(verificaPaginaNaMP(pagina,tabela_de_paginas)){
                memoria_principal[tabela_de_paginas[pagina.idProcesso][pagina.idPagina].posMP].tempoNaMemoria = 0;
                printf("O processo %d pediu para alocar a pagina %d, que já estava na MP. Tempo atualizado\n",j,pagina.idPagina);
                continue; 
            }

            // verificando se pode colocar na memoria devido ao working set
            if(workingSetLimit[j]<4){
                memoria_principal[posMP] = pagina;
                //marcando na tabela qual pagina de qual processo está na MP
                tabela_de_paginas[j][memoria_principal[posMP].idPagina].isInMP = 1;
                tabela_de_paginas[j][memoria_principal[posMP].idPagina].posMP = posMP;
                workingSetLimit[j] += 1;

                //adicionando unidade de tempo nas paginas presentes na MP
                printf("Colocando a pagina %d do processo %d na posição %d da MP\n",memoria_principal[posMP].idPagina, memoria_principal[posMP].idProcesso, posMP);

                posMP++;

                //debugando memoria quando lota
                printf("PosMP: %d\n",posMP);
                if (posMP == TAMANHO_MEMORIA_PRIMARIA){
                    printf("Processos: %d / %d\n",j,i);
                    for (int i = 0 ; i < TAMANHO_MEMORIA_PRIMARIA ; i++){
                        printf("%d ", memoria_principal[i].idProcesso);
                    }
                    printf("\n");
                    break;
                }
            }

            // fazendo LRU
            else{
                //printf("O processo %d já possui 4 paginas na MP, é necessario fazer LRU\n",j);

                // procurando onde na MP está a pagina mais antiga do processo j
                int indiceNaMp;
                int tempo = 0;
                for(int page=0; page<50; page++){
                    //caso a pagina esteja na MP
                    if(tabela_de_paginas[j][page].isInMP){
                        //verificando qual delas possui o maior tempo
                        printf("%d  Processo %d, Pagina: %d, Tempo: %d\n",j, memoria_principal[tabela_de_paginas[j][page].posMP].idProcesso,memoria_principal[tabela_de_paginas[j][page].posMP].idPagina,memoria_principal[tabela_de_paginas[j][page].posMP].tempoNaMemoria);
                        
                        if (memoria_principal[tabela_de_paginas[j][page].posMP].tempoNaMemoria > tempo){
                            tempo = memoria_principal[tabela_de_paginas[j][page].posMP].tempoNaMemoria;
                            indiceNaMp = tabela_de_paginas[j][page].posMP;
                        }
                    }
                }

                //printf("Pagina: %d, Processo: %d\n",memoria_principal[indiceNaMp].idPagina,memoria_principal[indiceNaMp].idProcesso);
                //printf("Pagina: %d, Processo: %d\n",memoria_principal[indiceDaPaginaMaisAntiga(j,tabela_de_paginas,memoria_principal)].idPagina,memoria_principal[indiceDaPaginaMaisAntiga(j,tabela_de_paginas,memoria_principal)].idProcesso);


                // pega pagina na MP a ser substituida
                // coloca pagina nova nessa posicao
                printf("LRU: Pagina %d; Processo: %d -> ", memoria_principal[indiceNaMp].idPagina, memoria_principal[indiceNaMp].idProcesso);

                // pagina antiga  MP = 0 , MS = 1 na tabela
                tabela_de_paginas[memoria_principal[indiceNaMp].idProcesso][memoria_principal[indiceNaMp].idPagina].isInMP = 0;
                tabela_de_paginas[memoria_principal[indiceNaMp].idProcesso][memoria_principal[indiceNaMp].idPagina].isInMS = 1;
                tabela_de_paginas[memoria_principal[indiceNaMp].idProcesso][memoria_principal[indiceNaMp].idPagina].posMP = -1;

                // pagina nova MP = 1 na tabela
                bool estavaNaMS = verificaPaginaNaMS(pagina,tabela_de_paginas);
                memoria_principal[indiceNaMp] = pagina;
                tabela_de_paginas[pagina.idProcesso][pagina.idPagina].isInMP = 1;
                tabela_de_paginas[pagina.idProcesso][pagina.idPagina].isInMS = 0;
                tabela_de_paginas[pagina.idProcesso][pagina.idPagina].posMP = indiceNaMp;
                printf(" Pagina: %d; Processo %d \n",memoria_principal[indiceNaMp].idPagina,memoria_principal[indiceNaMp].idProcesso);

                if(estavaNaMS){
                    printf("(A pagina %d do processo %d estava na MS e voltou para a MP)\n", pagina.idPagina,pagina.idProcesso);
                };
            }
        }

        for(int i = 0; i<posMP; i++){
            printf("Pagina %d do processo %d com tempo %d\n",memoria_principal[i].idPagina,memoria_principal[i].idProcesso,memoria_principal[i].tempoNaMemoria);
        }

        if (posMP == TAMANHO_MEMORIA_PRIMARIA){
            break;
        }
        adicionaTempo(posMP,memoria_principal);
        printaMatriz(tabela_de_paginas);
        printf("\n\n\n");

        //sleep(3);
        //system("clear");
        printf("\n");
    }

    for (int i = 0 ; i < QUANT_THREADS ; i++)
    {
        printf("%d ", workingSetLimit[i]);
    }

    // instante de tempo quando a memoria encheu
    // continuando após memoria encher, precisa tratar 16 e 17
     for(int i=16; i<18;i++){
        Pagina pagina = criaPaginaAleatoria(i);

        //verificando se a pagina se encontra na MP
        if(verificaPaginaNaMP(pagina,tabela_de_paginas)){
            memoria_principal[tabela_de_paginas[pagina.idProcesso][pagina.idPagina].posMP].tempoNaMemoria = 0;
            printf("O processo %d pediu para alocar a pagina %d, que já estava na MP. Tempo atualizado\n",i,pagina.idPagina);
            continue; 
        }

        // verificando se pode colocar na memoria devido ao working set
        if(workingSetLimit[i]<4){
            int maisAntigaNaMP = indiceDaPaginaMaisAntigaTodas(memoria_principal);

            workingSetLimit[memoria_principal[maisAntigaNaMP].idProcesso]--;
            int processoAntigo = memoria_principal[maisAntigaNaMP].idProcesso;
            int paginaAntiga = memoria_principal[maisAntigaNaMP].idPagina;
            tabela_de_paginas[memoria_principal[maisAntigaNaMP].idProcesso][memoria_principal[maisAntigaNaMP].idPagina].isInMP = 0;
            tabela_de_paginas[memoria_principal[maisAntigaNaMP].idProcesso][memoria_principal[maisAntigaNaMP].idPagina].isInMS = 1;
            tabela_de_paginas[memoria_principal[maisAntigaNaMP].idProcesso][memoria_principal[maisAntigaNaMP].idPagina].posMP = -1;

            memoria_principal[maisAntigaNaMP] = pagina;
            //marcando na tabela qual pagina de qual processo está na MP
            tabela_de_paginas[i][memoria_principal[maisAntigaNaMP].idPagina].isInMP = 1;
            tabela_de_paginas[i][memoria_principal[maisAntigaNaMP].idPagina].isInMS = 0;
            tabela_de_paginas[i][memoria_principal[maisAntigaNaMP].idPagina].posMP = maisAntigaNaMP;
            workingSetLimit[i] += 1;
            //adicionando unidade de tempo nas paginas presentes na MP
            printf("Colocando a pagina %d do processo %d na posição %d da MP\n",memoria_principal[maisAntigaNaMP].idPagina, memoria_principal[maisAntigaNaMP].idProcesso, maisAntigaNaMP);
        }
        
    }
    adicionaTempo(64,memoria_principal);

    // criar processos 18 e 19
    for (int i = 18; i<QUANT_THREADS;i++){
        //gerando paginas dos i processos existentes até então
        printf(" Processo %d criado\n",i);
        for (int j = 0; j<=i; j++){
            Pagina pagina = criaPaginaAleatoria(j);

            //verificando se a pagina se encontra na MP
            if(verificaPaginaNaMP(pagina,tabela_de_paginas)){
                memoria_principal[tabela_de_paginas[pagina.idProcesso][pagina.idPagina].posMP].tempoNaMemoria = 0;
                printf("O processo %d pediu para alocar a pagina %d, que já estava na MP. Tempo atualizado\n",j,pagina.idPagina);
                continue; 
            }

            // verificando se pode colocar na memoria devido ao working set
            if(workingSetLimit[j]<4){
                int maisAntigaNaMP = indiceDaPaginaMaisAntigaTodas(memoria_principal);

                workingSetLimit[memoria_principal[maisAntigaNaMP].idProcesso]--;
                int processoAntigo = memoria_principal[maisAntigaNaMP].idProcesso;
                int paginaAntiga = memoria_principal[maisAntigaNaMP].idPagina;
                tabela_de_paginas[memoria_principal[maisAntigaNaMP].idProcesso][memoria_principal[maisAntigaNaMP].idPagina].isInMP = 0;
                tabela_de_paginas[memoria_principal[maisAntigaNaMP].idProcesso][memoria_principal[maisAntigaNaMP].idPagina].isInMS = 1;
                tabela_de_paginas[memoria_principal[maisAntigaNaMP].idProcesso][memoria_principal[maisAntigaNaMP].idPagina].posMP = -1;

                memoria_principal[maisAntigaNaMP] = pagina;
                //marcando na tabela qual pagina de qual processo está na MP
                tabela_de_paginas[j][memoria_principal[maisAntigaNaMP].idPagina].isInMP = 1;
                tabela_de_paginas[j][memoria_principal[maisAntigaNaMP].idPagina].isInMS = 0;
                tabela_de_paginas[j][memoria_principal[maisAntigaNaMP].idPagina].posMP = maisAntigaNaMP;
                workingSetLimit[j] += 1;
                printf("Pagina %d do processo %d saiu\n", paginaAntiga, processoAntigo);
                //adicionando unidade de tempo nas paginas presentes na MP
                printf("Colocando a pagina %d do processo %d na posição %d da MP\n",memoria_principal[maisAntigaNaMP].idPagina, memoria_principal[maisAntigaNaMP].idProcesso, maisAntigaNaMP);

                
            }

            // fazendo LRU
            else{
                //printf("O processo %d já possui 4 paginas na MP, é necessario fazer LRU\n",j);
                int indiceNaMp = indiceDaPaginaMaisAntiga(j, tabela_de_paginas, memoria_principal);

                //printf("Pagina: %d, Processo: %d\n",memoria_principal[indiceNaMp].idPagina,memoria_principal[indiceNaMp].idProcesso);
                //printf("Pagina: %d, Processo: %d\n",memoria_principal[indiceDaPaginaMaisAntiga(j,tabela_de_paginas,memoria_principal)].idPagina,memoria_principal[indiceDaPaginaMaisAntiga(j,tabela_de_paginas,memoria_principal)].idProcesso);


                // pega pagina na MP a ser substituida
                // coloca pagina nova nessa posicao
                printf("LRU: Pagina %d; Processo: %d -> ", memoria_principal[indiceNaMp].idPagina, memoria_principal[indiceNaMp].idProcesso);

                // pagina antiga  MP = 0 , MS = 1 na tabela
                tabela_de_paginas[memoria_principal[indiceNaMp].idProcesso][memoria_principal[indiceNaMp].idPagina].isInMP = 0;
                tabela_de_paginas[memoria_principal[indiceNaMp].idProcesso][memoria_principal[indiceNaMp].idPagina].isInMS = 1;
                tabela_de_paginas[memoria_principal[indiceNaMp].idProcesso][memoria_principal[indiceNaMp].idPagina].posMP = -1;

                // pagina nova MP = 1 na tabela
                bool estavaNaMS = verificaPaginaNaMS(pagina,tabela_de_paginas);
                memoria_principal[indiceNaMp] = pagina;
                tabela_de_paginas[pagina.idProcesso][pagina.idPagina].isInMP = 1;
                tabela_de_paginas[pagina.idProcesso][pagina.idPagina].isInMS = 0;
                tabela_de_paginas[pagina.idProcesso][pagina.idPagina].posMP = indiceNaMp;
                printf(" Pagina: %d; Processo %d \n",memoria_principal[indiceNaMp].idPagina,memoria_principal[indiceNaMp].idProcesso);

                if(estavaNaMS){
                    printf("(A pagina %d do processo %d estava na MS e voltou para a MP)\n", pagina.idPagina,pagina.idProcesso);
                };
            }
        }

        for(int i = 0; i<64; i++){
            printf("Pagina %d do processo %d com tempo %d\n",memoria_principal[i].idPagina,memoria_principal[i].idProcesso,memoria_principal[i].tempoNaMemoria);
        }
        adicionaTempo(64,memoria_principal);
        printaMatriz(tabela_de_paginas);
        printf("\n\n\n");

        //sleep(3);
        //system("clear");
        printf("\n");
        for (int i = 0 ; i < QUANT_THREADS ; i++){
            printf("%d ", workingSetLimit[i]);
        }
        printf("\n");
    }

    // LRU infinito
    printf("Começando o LRU infinito\n");
    while(true){
        for (int j = 0; j<20; j++){
            Pagina pagina = criaPaginaAleatoria(j);

            //verificando se a pagina se encontra na MP
            if(verificaPaginaNaMP(pagina,tabela_de_paginas)){
                memoria_principal[tabela_de_paginas[pagina.idProcesso][pagina.idPagina].posMP].tempoNaMemoria = 0;
                printf("O processo %d pediu para alocar a pagina %d, que já estava na MP. Tempo atualizado\n",j,pagina.idPagina);
                continue; 
            }

            // verificando se pode colocar na memoria devido ao working set
            if(workingSetLimit[j]<4){
                int maisAntigaNaMP = indiceDaPaginaMaisAntigaTodas(memoria_principal);

                workingSetLimit[memoria_principal[maisAntigaNaMP].idProcesso]--;
                int processoAntigo = memoria_principal[maisAntigaNaMP].idProcesso;
                int paginaAntiga = memoria_principal[maisAntigaNaMP].idPagina;
                tabela_de_paginas[memoria_principal[maisAntigaNaMP].idProcesso][memoria_principal[maisAntigaNaMP].idPagina].isInMP = 0;
                tabela_de_paginas[memoria_principal[maisAntigaNaMP].idProcesso][memoria_principal[maisAntigaNaMP].idPagina].isInMS = 1;
                tabela_de_paginas[memoria_principal[maisAntigaNaMP].idProcesso][memoria_principal[maisAntigaNaMP].idPagina].posMP = -1;

                memoria_principal[maisAntigaNaMP] = pagina;
                //marcando na tabela qual pagina de qual processo está na MP
                tabela_de_paginas[j][memoria_principal[maisAntigaNaMP].idPagina].isInMP = 1;
                tabela_de_paginas[j][memoria_principal[maisAntigaNaMP].idPagina].isInMS = 0;
                tabela_de_paginas[j][memoria_principal[maisAntigaNaMP].idPagina].posMP = maisAntigaNaMP;
                workingSetLimit[j] += 1;
                printf("Pagina %d do processo %d saiu\n", paginaAntiga, processoAntigo);
                //adicionando unidade de tempo nas paginas presentes na MP
                printf("Colocando a pagina %d do processo %d na posição %d da MP\n",memoria_principal[maisAntigaNaMP].idPagina, memoria_principal[maisAntigaNaMP].idProcesso, maisAntigaNaMP);

                
            }

            // fazendo LRU
            else{
                //printf("O processo %d já possui 4 paginas na MP, é necessario fazer LRU\n",j);
                int indiceNaMp = indiceDaPaginaMaisAntiga(j, tabela_de_paginas, memoria_principal);

                //printf("Pagina: %d, Processo: %d\n",memoria_principal[indiceNaMp].idPagina,memoria_principal[indiceNaMp].idProcesso);
                //printf("Pagina: %d, Processo: %d\n",memoria_principal[indiceDaPaginaMaisAntiga(j,tabela_de_paginas,memoria_principal)].idPagina,memoria_principal[indiceDaPaginaMaisAntiga(j,tabela_de_paginas,memoria_principal)].idProcesso);


                // pega pagina na MP a ser substituida
                // coloca pagina nova nessa posicao
                printf("LRU: Pagina %d; Processo: %d -> ", memoria_principal[indiceNaMp].idPagina, memoria_principal[indiceNaMp].idProcesso);

                // pagina antiga  MP = 0 , MS = 1 na tabela
                tabela_de_paginas[memoria_principal[indiceNaMp].idProcesso][memoria_principal[indiceNaMp].idPagina].isInMP = 0;
                tabela_de_paginas[memoria_principal[indiceNaMp].idProcesso][memoria_principal[indiceNaMp].idPagina].isInMS = 1;
                tabela_de_paginas[memoria_principal[indiceNaMp].idProcesso][memoria_principal[indiceNaMp].idPagina].posMP = -1;

                // pagina nova MP = 1 na tabela
                bool estavaNaMS = verificaPaginaNaMS(pagina,tabela_de_paginas);
                memoria_principal[indiceNaMp] = pagina;
                tabela_de_paginas[pagina.idProcesso][pagina.idPagina].isInMP = 1;
                tabela_de_paginas[pagina.idProcesso][pagina.idPagina].isInMS = 0;
                tabela_de_paginas[pagina.idProcesso][pagina.idPagina].posMP = indiceNaMp;
                printf(" Pagina: %d; Processo %d \n",memoria_principal[indiceNaMp].idPagina,memoria_principal[indiceNaMp].idProcesso);

                if(estavaNaMS){
                    printf("(A pagina %d do processo %d estava na MS e voltou para a MP)\n", pagina.idPagina,pagina.idProcesso);
                };
            }
        }
        for(int i = 0; i<64; i++){
            printf("Pagina %d do processo %d com tempo %d\n",memoria_principal[i].idPagina,memoria_principal[i].idProcesso,memoria_principal[i].tempoNaMemoria);
        }
        adicionaTempo(64,memoria_principal);
        printaMatriz(tabela_de_paginas);
        printf("\n\n\n");

        sleep(3);
        system("clear");
        printf("\n");
        for (int i = 0 ; i < QUANT_THREADS ; i++){
            printf("%d ", workingSetLimit[i]);
        }
        printf("\n");
    }
    return 0;
}