#include	<stdio.h>
#include 	<stdlib.h>
#include 	<stdbool.h>
#include 	<time.h>

#define TAMANHO_MEMORIA_PRIMARIA 64
#define QUANT_THREADS 20
#define QUANT_PAGINAS 50
#define MAX_WORKSET 4

// Estrutura de paginas para nas memórias
typedef struct{
    int idProcesso;
    int idPagina;
    int tempoNaMemoria;
}Pagina;

// Estrutura para a tabela de processos/paginas
typedef struct{
    int posMP;   // sua posicao na memoria principal
    int isInMP;  // se está na MP
    int isInMS;  // se está na MS
}Tabela;

//Metodo para a criação de paginas aleatoria de um determinado processo
Pagina criaPaginaAleatoria(int processo){
    Pagina pagina;
    pagina.idProcesso = processo;
    pagina.idPagina = rand() % QUANT_PAGINAS;
    pagina.tempoNaMemoria = 0;

    return pagina;
}

// imprimindo o conteúdo da matriz (tempo na MP de cada pagina)
void printaMatriz (Tabela matriz[QUANT_THREADS][QUANT_PAGINAS],Pagina MP[TAMANHO_MEMORIA_PRIMARIA]){
    for (int i = 0; i < QUANT_THREADS; i++) { //para as linhas
        for (int j = 0; j < QUANT_PAGINAS; j++) { //para as colunas
            if(matriz[i][j].isInMP){
                printf("\033[37m\033[42m%d\033[0m ", MP[matriz[i][j].posMP].tempoNaMemoria);
            }
            else{
                printf("%d ",matriz[i][j].isInMP);
            }
        }
        printf("\n");
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

// Verifica a pagina mais antiga na MP de um determinado processo
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

// verifica a pagina mais antiga da MP entre todos os processos
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


// Vetor da memoria principal
Pagina memoria_principal[TAMANHO_MEMORIA_PRIMARIA];

// tabela de processos x pagina
Tabela tabela_de_paginas[QUANT_THREADS][QUANT_PAGINAS];


int main( int argc, char *argv[ ] ){
    //inicializando as estruturas 
    int posMP = 0;
    int workingSetLimit[20];
    for (int i = 0 ; i < TAMANHO_MEMORIA_PRIMARIA ; i++){
        memoria_principal[i].idPagina = -1;
        memoria_principal[i].idProcesso = -1;
        memoria_principal[i].tempoNaMemoria = -1;
    }
    for (int i = 0 ; i < QUANT_THREADS ; i++){
        for (int j = 0 ; j < QUANT_PAGINAS ; j++){
            tabela_de_paginas[i][j].posMP = -1;
            tabela_de_paginas[i][j].isInMP = 0;
            tabela_de_paginas[i][j].isInMS = 0;
        }
    }
    // zerando working set
    for (int i=0;i<20;i++){
        workingSetLimit[i] = 0;
    };

    //gerando processos até a memoria encher 
    for (int i = 0; i<QUANT_THREADS;i++){
        //gerando paginas dos i processos existentes até então
        printf("Processo %d criado\n",i);
        for (int j = 0; j<=i; j++){
            Pagina pagina = criaPaginaAleatoria(j);

            //verificando se a pagina se encontra na MP
            if(verificaPaginaNaMP(pagina,tabela_de_paginas)){
                memoria_principal[tabela_de_paginas[pagina.idProcesso][pagina.idPagina].posMP].tempoNaMemoria = 0;
                printf("Processo %d: Pediu para alocar a pagina %d, que já estava na MP. Tempo atualizado.\n",j,pagina.idPagina);
                continue; 
            }

            // verificando se pode colocar na memoria devido ao working set
            if(workingSetLimit[j]<4){
                memoria_principal[posMP] = pagina;
                //marcando na tabela qual pagina de qual processo está na MP
                tabela_de_paginas[j][memoria_principal[posMP].idPagina].isInMP = 1;
                tabela_de_paginas[j][memoria_principal[posMP].idPagina].posMP = posMP;
                workingSetLimit[j] += 1;

                //log
                printf("Processo %d: Colocando a pagina %d na posição %d da MP\n",memoria_principal[posMP].idProcesso, memoria_principal[posMP].idPagina, posMP);

                //andando com a posicao da MP
                posMP++;

                // caso a memoria lote
                if (posMP == TAMANHO_MEMORIA_PRIMARIA){
                    printf("--MP CHEIA--\n");
                    break;
                }
            }

            // fazendo LRU
            else{
                //log
                printf("Processo %d: já possui 4 paginas na MP\n",j);

                // procurando onde na MP está a pagina mais antiga do processo j
                int indiceNaMp = indiceDaPaginaMaisAntiga(j,tabela_de_paginas,memoria_principal);
                
                //log
                printf("Pagina %d vai para MS -> ", memoria_principal[indiceNaMp].idPagina);

                // tirando a pagina antiga da MP 
                tabela_de_paginas[memoria_principal[indiceNaMp].idProcesso][memoria_principal[indiceNaMp].idPagina].isInMP = 0;
                tabela_de_paginas[memoria_principal[indiceNaMp].idProcesso][memoria_principal[indiceNaMp].idPagina].isInMS = 1;
                tabela_de_paginas[memoria_principal[indiceNaMp].idProcesso][memoria_principal[indiceNaMp].idPagina].posMP = -1;

                // colocando a pagina nova na MP
                bool estavaNaMS = verificaPaginaNaMS(pagina,tabela_de_paginas); // verificando se a pagina nova estava na MS
                memoria_principal[indiceNaMp] = pagina;
                tabela_de_paginas[pagina.idProcesso][pagina.idPagina].isInMP = 1;
                tabela_de_paginas[pagina.idProcesso][pagina.idPagina].isInMS = 0;
                tabela_de_paginas[pagina.idProcesso][pagina.idPagina].posMP = indiceNaMp;

                //log
                printf(" Pagina: %d entra na MP em seu lugar\n", memoria_principal[indiceNaMp].idPagina);

                if(estavaNaMS){
                    //log
                    printf("(A pagina %d do processo %d estava na MS e voltou para a MP)\n", pagina.idPagina, pagina.idProcesso);
                }
            }
        }
        // saindo do loop exterior caso a memoria lote
        if (posMP == TAMANHO_MEMORIA_PRIMARIA){
            break;
        }

        printf("\n");
        for(int i = 0; i<posMP; i++){
            printf("Pagina %d do processo %d com tempo %d\n",memoria_principal[i].idPagina, memoria_principal[i].idProcesso, memoria_principal[i].tempoNaMemoria);
        }

        printaMatriz(tabela_de_paginas,memoria_principal);
        adicionaTempo(posMP,memoria_principal);
        
        printf("\n\n\n");

        sleep(3);
        //system("clear");
        printf("\n");
    }

    // ---------------------------instante de tempo quando a memoria encheu---------------------------
    // -----------------------continuando após memoria encher, precisa tratar 16 e 17-----------------
     for(int i=16; i<18;i++){
        Pagina pagina = criaPaginaAleatoria(i);

        //verificando se a pagina se encontra na MP
        if(verificaPaginaNaMP(pagina,tabela_de_paginas)){
            memoria_principal[tabela_de_paginas[pagina.idProcesso][pagina.idPagina].posMP].tempoNaMemoria = 0;
            printf("Processo %d: pediu para alocar a pagina %d, que já estava na MP. Tempo atualizado.\n",i,pagina.idPagina);
            continue; 
        }

        // verificando se pode colocar na memoria devido ao working set
        if(workingSetLimit[i]<4){
            int maisAntigaNaMP = indiceDaPaginaMaisAntigaTodas(memoria_principal);

            // diminuindo o working set
            workingSetLimit[memoria_principal[maisAntigaNaMP].idProcesso]--;

            // variaveis auxiliares para o log
            int processoAntigo = memoria_principal[maisAntigaNaMP].idProcesso;
            int paginaAntiga = memoria_principal[maisAntigaNaMP].idPagina;

            // marcando na tabela que a pagina antiga foi para a MS
            tabela_de_paginas[memoria_principal[maisAntigaNaMP].idProcesso][memoria_principal[maisAntigaNaMP].idPagina].isInMP = 0;
            tabela_de_paginas[memoria_principal[maisAntigaNaMP].idProcesso][memoria_principal[maisAntigaNaMP].idPagina].isInMS = 1;
            tabela_de_paginas[memoria_principal[maisAntigaNaMP].idProcesso][memoria_principal[maisAntigaNaMP].idPagina].posMP = -1;

            // colocando a pagian nova no lugar da antiga
            memoria_principal[maisAntigaNaMP] = pagina;

            //marcando na tabela que a pagina nova está na MP
            tabela_de_paginas[i][memoria_principal[maisAntigaNaMP].idPagina].isInMP = 1;
            tabela_de_paginas[i][memoria_principal[maisAntigaNaMP].idPagina].isInMS = 0;
            tabela_de_paginas[i][memoria_principal[maisAntigaNaMP].idPagina].posMP = maisAntigaNaMP;
            workingSetLimit[i] += 1;

            //log
            printf("Processo %d: Pagina %d do processo %d foi para MS (swap)\n",i,paginaAntiga, processoAntigo);
            printf("Colocando a pagina %d do processo %d na posição %d da MP\n",memoria_principal[maisAntigaNaMP].idPagina, memoria_principal[maisAntigaNaMP].idProcesso, maisAntigaNaMP);
        }
    }
    printf("\n");
    for(int i = 0; i<posMP; i++){
        printf("Pagina %d do processo %d com tempo %d\n",memoria_principal[i].idPagina, memoria_principal[i].idProcesso, memoria_principal[i].tempoNaMemoria);
    }
    printaMatriz(tabela_de_paginas,memoria_principal);
    adicionaTempo(64,memoria_principal);
    printf("\n\n\n");


    //---------------------------------criar processos 18 e 19----------------------------------
    for (int i = 18; i<QUANT_THREADS;i++){
        //gerando paginas dos i processos existentes até então
        printf("Processo %d criado\n",i);
        for (int j = 0; j<=i; j++){
            Pagina pagina = criaPaginaAleatoria(j);

            //verificando se a pagina se encontra na MP
            if(verificaPaginaNaMP(pagina,tabela_de_paginas)){
                memoria_principal[tabela_de_paginas[pagina.idProcesso][pagina.idPagina].posMP].tempoNaMemoria = 0;
                printf("Processo %d: pediu para alocar a pagina %d, que já estava na MP. Tempo atualizado.\n",j,pagina.idPagina);
                continue; 
            }

            // verificando se pode colocar na memoria devido ao working set
            if(workingSetLimit[j]<4){
                int maisAntigaNaMP = indiceDaPaginaMaisAntigaTodas(memoria_principal);

                //diminuindo o working
                workingSetLimit[memoria_principal[maisAntigaNaMP].idProcesso]--;
                int processoAntigo = memoria_principal[maisAntigaNaMP].idProcesso;
                int paginaAntiga = memoria_principal[maisAntigaNaMP].idPagina;

                // desmarcando da tabela a pagina que vai sair da MP
                tabela_de_paginas[memoria_principal[maisAntigaNaMP].idProcesso][memoria_principal[maisAntigaNaMP].idPagina].isInMP = 0;
                tabela_de_paginas[memoria_principal[maisAntigaNaMP].idProcesso][memoria_principal[maisAntigaNaMP].idPagina].isInMS = 1;
                tabela_de_paginas[memoria_principal[maisAntigaNaMP].idProcesso][memoria_principal[maisAntigaNaMP].idPagina].posMP = -1;

                //colocando no lugar da mais antiga
                memoria_principal[maisAntigaNaMP] = pagina;
                //marcando na tabela qual pagina de qual processo tomou o lugar na MP
                tabela_de_paginas[j][memoria_principal[maisAntigaNaMP].idPagina].isInMP = 1;
                tabela_de_paginas[j][memoria_principal[maisAntigaNaMP].idPagina].isInMS = 0;
                tabela_de_paginas[j][memoria_principal[maisAntigaNaMP].idPagina].posMP = maisAntigaNaMP;
                workingSetLimit[j] += 1;
                
                printf("Processo %d: Pagina %d do processo %d foi para MS (swap)\n",j,paginaAntiga, processoAntigo);
                //adicionando unidade de tempo nas paginas presentes na MP
                printf("Colocando a pagina %d do processo %d na posição %d da MP\n",memoria_principal[maisAntigaNaMP].idPagina, memoria_principal[maisAntigaNaMP].idProcesso, maisAntigaNaMP);

            }

            // fazendo LRU
            else{
                printf("Processo %d: já possui 4 paginas na MP\n",j);
                int indiceNaMp = indiceDaPaginaMaisAntiga(j, tabela_de_paginas, memoria_principal);

                //log
                printf("Pagina %d vai para MS -> ", memoria_principal[indiceNaMp].idPagina);

                // tirando a pagina antiga da MP na tabela
                tabela_de_paginas[memoria_principal[indiceNaMp].idProcesso][memoria_principal[indiceNaMp].idPagina].isInMP = 0;
                tabela_de_paginas[memoria_principal[indiceNaMp].idProcesso][memoria_principal[indiceNaMp].idPagina].isInMS = 1;
                tabela_de_paginas[memoria_principal[indiceNaMp].idProcesso][memoria_principal[indiceNaMp].idPagina].posMP = -1;

                // auxiliar para log
                bool estavaNaMS = verificaPaginaNaMS(pagina,tabela_de_paginas);

                // colocando pagina nova no lugar da antiga
                memoria_principal[indiceNaMp] = pagina;

                // marcando na tabela que a pagina nova está na MP no lugar da pagina antiga
                tabela_de_paginas[pagina.idProcesso][pagina.idPagina].isInMP = 1;
                tabela_de_paginas[pagina.idProcesso][pagina.idPagina].isInMS = 0;
                tabela_de_paginas[pagina.idProcesso][pagina.idPagina].posMP = indiceNaMp;

                //log
                printf(" Pagina %d entra na MP em seu lugar.\n",memoria_principal[indiceNaMp].idPagina);

                if(estavaNaMS){
                    printf("(A pagina %d do processo %d estava na MS e voltou para a MP)\n", pagina.idPagina,pagina.idProcesso);
                };
            }
        }

        printf("\n");
        for(int i = 0; i<64; i++){
            printf("Pagina %d do processo %d com tempo %d\n",memoria_principal[i].idPagina,memoria_principal[i].idProcesso,memoria_principal[i].tempoNaMemoria);
        }
        printaMatriz(tabela_de_paginas,memoria_principal);
        adicionaTempo(64,memoria_principal);
        
        printf("\n\n\n");

        sleep(3);
        //system("clear");

        /*
        printf("\n");
        for (int i = 0 ; i < QUANT_THREADS ; i++){
            printf("%d ", workingSetLimit[i]);
        }*/
        printf("\n");
    }

    // ----------------------------------------LRU infinito-----------------------------------
    
    while(true){
        for (int j = 0; j<20; j++){
            Pagina pagina = criaPaginaAleatoria(j);

            //verificando se a pagina se encontra na MP
            if(verificaPaginaNaMP(pagina,tabela_de_paginas)){
                memoria_principal[tabela_de_paginas[pagina.idProcesso][pagina.idPagina].posMP].tempoNaMemoria = 0;
                printf("Processo %d: pediu para alocar a pagina %d, que já estava na MP. Tempo atualizado.\n",j,pagina.idPagina);
                continue; 
            }

            // verificando se pode colocar na memoria devido ao working set
            if(workingSetLimit[j]<4){
                int maisAntigaNaMP = indiceDaPaginaMaisAntigaTodas(memoria_principal);

                // diminuindo working set
                workingSetLimit[memoria_principal[maisAntigaNaMP].idProcesso]--;

                //variaveis axuliares para log
                int processoAntigo = memoria_principal[maisAntigaNaMP].idProcesso;
                int paginaAntiga = memoria_principal[maisAntigaNaMP].idPagina;

                // tirando a pagina antiga da MP na tabela
                tabela_de_paginas[memoria_principal[maisAntigaNaMP].idProcesso][memoria_principal[maisAntigaNaMP].idPagina].isInMP = 0;
                tabela_de_paginas[memoria_principal[maisAntigaNaMP].idProcesso][memoria_principal[maisAntigaNaMP].idPagina].isInMS = 1;
                tabela_de_paginas[memoria_principal[maisAntigaNaMP].idProcesso][memoria_principal[maisAntigaNaMP].idPagina].posMP = -1;

                // colocando a pagina nova no lugar da antiga
                memoria_principal[maisAntigaNaMP] = pagina;

                //marcando na tabela que a pagina nova está no lugar da antiga da MP
                tabela_de_paginas[j][memoria_principal[maisAntigaNaMP].idPagina].isInMP = 1;
                tabela_de_paginas[j][memoria_principal[maisAntigaNaMP].idPagina].isInMS = 0;
                tabela_de_paginas[j][memoria_principal[maisAntigaNaMP].idPagina].posMP = maisAntigaNaMP;
                workingSetLimit[j] += 1;

                //log
                printf("Processo %d: Pagina %d do processo %d foi para MS (swap)\n",j,paginaAntiga, processoAntigo);
                printf("Colocando a pagina %d do processo %d na posição %d da MP\n",memoria_principal[maisAntigaNaMP].idPagina, memoria_principal[maisAntigaNaMP].idProcesso, maisAntigaNaMP);                
            }

            // fazendo LRU
            else{
                printf("Processo %d: já possui 4 paginas na MP\n",j);
                int indiceNaMp = indiceDaPaginaMaisAntiga(j, tabela_de_paginas, memoria_principal);
                
                //log
                printf("Pagina %d vai para MS -> ", memoria_principal[indiceNaMp].idPagina);

                // tirando a pagina antiga da MP na tabela
                tabela_de_paginas[memoria_principal[indiceNaMp].idProcesso][memoria_principal[indiceNaMp].idPagina].isInMP = 0;
                tabela_de_paginas[memoria_principal[indiceNaMp].idProcesso][memoria_principal[indiceNaMp].idPagina].isInMS = 1;
                tabela_de_paginas[memoria_principal[indiceNaMp].idProcesso][memoria_principal[indiceNaMp].idPagina].posMP = -1;

                // auxiliar para log
                bool estavaNaMS = verificaPaginaNaMS(pagina,tabela_de_paginas);

                //colocando a pagina nova no lugar da antiga
                memoria_principal[indiceNaMp] = pagina;

                //marcando na tabela que a pagina nova está no lugar da antiga da MP
                tabela_de_paginas[pagina.idProcesso][pagina.idPagina].isInMP = 1;
                tabela_de_paginas[pagina.idProcesso][pagina.idPagina].isInMS = 0;
                tabela_de_paginas[pagina.idProcesso][pagina.idPagina].posMP = indiceNaMp;

                //log
                printf(" Pagina: %d entra em seu lugar na MP \n",memoria_principal[indiceNaMp].idPagina);

                if(estavaNaMS){
                    printf("(A pagina %d do processo %d estava na MS e voltou para a MP)\n", pagina.idPagina,pagina.idProcesso);
                };
            }
        }
        printf("\n");
        for(int i = 0; i<64; i++){
            printf("Pagina %d do processo %d com tempo %d\n",memoria_principal[i].idPagina,memoria_principal[i].idProcesso,memoria_principal[i].tempoNaMemoria);
        }

        adicionaTempo(64,memoria_principal);
        printaMatriz(tabela_de_paginas,memoria_principal);
        printf("\n\n\n");

        sleep(3);
        //system("clear");

        printf("\n");
        
        /*for (int i = 0 ; i < QUANT_THREADS ; i++){
            printf("%d ", workingSetLimit[i]);
        }*/
        printf("\n");
    }
    return 0;
}