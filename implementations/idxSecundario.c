#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../includes/idxSecundario.h"
#include "../includes/idxPrimario.h"
#include "../includes/dados.h"

//Função que retorna um novo elemento do tipo índice secundários
IDXSecundario *novo_indiceSecundario(char *titulo, char *chave){
    IDXSecundario *aux;
    aux = malloc(sizeof(IDXSecundario));//Aloca memória para o elemento
    strcpy(aux->titulo,titulo);//Realiza a gravação das informações
    strcpy(aux->chave,chave);
    aux->prox = NULL;
    return aux;//retorna o elemento
}

//Função que insere um elemento na lista de índices secundários
void insere_indiceSecundario(IDXSecundario **head, IDXSecundario *indice){
    IDXSecundario *aux, *antes;

    if(*head == NULL){//Caso em que a lista está vazia
        *head = indice;
        return;
    }
    
    aux = antes = *head;

    //procura a melhor posição de inserção para manter a lista ordenada
    while(aux->prox && strcmp(indice->titulo,aux->titulo) > 0){
        antes = aux;
        aux = aux->prox;
    }

    //caso em que a inserção pode envolver a cabeça da lista
    if(aux == *head){
        if(strcmp(indice->titulo,aux->titulo) > 0){//Elemento a ser inserido é 'maior' que a cabeça
            //Elemento é inserido na frente da cabeça
            indice->prox = aux->prox;
            (*head)->prox = indice;
            return;
        }else if(strcmp(indice->titulo,aux->titulo) <= 0){//Elemento é menor ou igual a cabeça
            //Elemento se torna a nova cabeça
            indice->prox = aux;
            (*head) = indice;
            return;
        }
    }
    if(strcmp(indice->titulo,aux->titulo) > 0){//Elemento é maior que o elemento apontado pelo ponteiro auxiliar
        //Elemento é inserido após o auxiliar
        aux->prox = indice;
        return;
    }else if(strcmp(indice->titulo,aux->titulo) <= 0){//Elemento é menor que o elemento apontado pelo ponteiro auxiliar
        //Elemento é inserido antes do ponteiro auxiliar
        antes->prox = indice;
        indice->prox = aux;
        return;
    }

    return;
}

//Função que remove um elemento da lista baseado na chave fornecida
void remove_indiceSecundario(IDXSecundario **head, char *chave){
    IDXSecundario *aux, *antes;
    if(*head == NULL)//Caso de lista vazia, apenas retorna a função
        return;
    aux = antes = *head;

    //Busca o elemento
    while(aux && strcmp(aux->chave,chave) != 0){
        antes = aux;
        aux = aux->prox;
    }

    if(aux == *head){//Se a cabeça precisar ser removida, o elemento após ela se torna a nova cabeça
        *head = aux->prox;
        free(aux);//Libera a memória
        return;
    }else if(aux == NULL){//Caso em que o elemento não foi encontrado
        printf("Esse filme não está cadastrado!");
        return;
    }else{//Em ultimo caso, o elemento é removido, e seu anterior passa a ocupar seu lugar
        antes->prox = aux->prox;
        free(aux);
        return;
    }
}

//Função que lista os filmes com mesmo título
void busca_filmes_por_chave_secundaria(FILE *movies,FILE *idxPrimario,IDXSecundario *headSecundaria, char *chave, int *root){
    IDXSecundario *aux;
    int RRN;

    //Lista vazia
    if(headSecundaria == NULL){
        printf("\nNão há filmes cadastrados!\n");
        return;
    }
    aux = headSecundaria;
    
    //Busca o primeiro filme da lista com a mesma chave secundária
    while(aux && strcmp(aux->titulo,chave) != 0)
        aux = aux->prox;
    
    //Caso em que não foi encontrado o filme
    if(aux == NULL){
        printf("\nEsse filme não está cadastrado!\n");
        return;
    }

    //para cada filme com o mesmo título, é realizada uma busca no arquivo de dados, e as infos
    //são impressas na tela
    while(aux && strcmp(aux->titulo, chave) == 0){
        RRN = busca_RRN(idxPrimario,aux->chave,root);//Busca o RRN, dando como chave a chave primária que está no elemento
        busca_filme(movies,RRN);//Busca o filme e imprime as infos na tela
        aux = aux->prox; //pula para o próximo elemento da lista
    }

    return;
}

//Função que percorre toda a lista, liberando a memória de cada elemento
void libera_memoria_secundaria(IDXSecundario **head){
        IDXSecundario *antes;
        if(*head == NULL)
            return;
        antes = *head;
        while(*head){
            (*head) = (*head)->prox;
            free(antes);
            antes = *head;
        }
        return;
    }

//Funções relacionadas ao arquivo de índices secundários

//Função que monta a lista de índices secundários baseado no arquivo ititle.idx
void leitura_indicesSecundarios(IDXSecundario **head, FILE *idxSecundario){
    char *titulo = NULL, chave[5];
    int c;
    IDXSecundario *aux;
    //Fseek para pular o header com a flag
    fseek(idxSecundario,7,SEEK_SET);
    //Para cada iteração, é lido o título e a chave primária dos registros do arquivo, logo em
    //seguida é montado um elemento com essas informações, sendo este inserido na lista de índices secundários
    while((c = fgetc(idxSecundario)) != EOF){
        titulo = (char *)malloc(sizeof(char) * 50);
        ungetc(c,idxSecundario);
        fscanf(idxSecundario, "%[^@]@%[^#]##", titulo, chave);
        aux = novo_indiceSecundario(titulo,chave);
        insere_indiceSecundario(head,aux);
        free(titulo);
    }
}

//Função que imprime a lista no arquivo de índices secundários
void imprime_indiceSecundario(IDXSecundario *head, FILE *idxSecundario){
    idxSecundario = freopen("ititle.idx","w+",stdout);//reabre o arquivo para escrita e leitura
    fprintf(idxSecundario,"flag=1\n");

    if(head == NULL)//Lista vazia
        return; 

    while(head != NULL){
        //Percorre a lista, printando no arquivo cada elemento no formato título@chave##
        fprintf(idxSecundario,"%s@%s##",head->titulo,head->chave);
        head = head->prox;
    }
}

//Função que monta a lista de índices secundários baseado no arquivo de dados
void atualiza_arquivoSecundario(FILE *movies, IDXSecundario **head, int qtdFilmes){
    char chave[6], titulo[50];
    IDXSecundario *aux;
    for(int i = 0; i<qtdFilmes; i++){
        //percorre todo o arquivo de dados, lendo a chave e título de cada registro, e, logo em
        //seguida, criando um novo elemento e o inserindo na lista
        fseek(movies, i * 192, SEEK_SET);
        fscanf(movies, "%[^@]@%[^@]@", chave, titulo);

        if(chave[0] != '*' && chave[1] != '|'){
            aux = novo_indiceSecundario(titulo, chave);
            insere_indiceSecundario(head, aux);
        }
    }
}
