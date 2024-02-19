#ifndef IDXSECUNDARIO_H
#define IDXSECUNDARIO_H

#include <stdio.h>
#include <stdlib.h>
#include "idxPrimario.h"

//Struct utilizada para representar a estrutura de índice secundário
typedef struct idxSecundario{
    char titulo[50];//Título em português
    char chave[5];//Chave do filme
    struct idxSecundario *prox;
}IDXSecundario;

//Escopos das funções relacionadas aos índices secundários
IDXSecundario *novo_indiceSecundario(char *titulo, char *chave);

void insere_indiceSecundario(IDXSecundario **head, IDXSecundario *indice);

void remove_indiceSecundario(IDXSecundario **head, char *chave);

void leitura_indicesSecundarios(IDXSecundario **head, FILE *idxSecundario);

void imprime_indiceSecundario(IDXSecundario *head, FILE *idxSecundario);

void atualiza_arquivoSecundario(FILE *movies, IDXSecundario **head, int qtdFilmes);

void busca_filmes_por_chave_secundaria(FILE *movies,FILE *idxPrimario, IDXSecundario *headSecundaria, char *chave, int *root);

void libera_memoria_secundaria(IDXSecundario **head);

#endif