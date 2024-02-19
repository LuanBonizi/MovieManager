#ifndef IDXPRIMARIO_H
#define IDXPRIMARIO_H

#include <stdio.h>
#include <stdlib.h>

#define ordem 4
//Struct utilizada para representar a estrutura de árvore B+ no índice primário
typedef struct node{
    int RRN;
    int isLeaf;
    char keys[ordem][6];
    int dataRRN[ordem];
    int children[ordem + 1];
    int numKeys;
    int parent;
    int next_node;
}Node;

//Struct que representa o header do arquivo de índice primário
typedef struct header{
    int root;
    int numNodes;
    int order;
}Header;

//Escopos das funções relacionadas aos índices primários
Node cria_node(int order, int isLeaf, int *qtdNos);

void insert_at_leaf(Node *leaf, char *key, int dataRRN);

int insert_in_parent(Node old_node, char *key_promovida, Node new_node, int *root, int *qtdNos,int order, FILE *idxPrimario);

Node search(int *root, char *chave, FILE *idxPrimario);

int insert(int *root, char *chave, int dataRRN, FILE *idxPrimario, int *qtdNos, int order);

int busca_RRN(FILE *idxPrimario, char *chave, int *root);

void lista_arquivo(FILE *idxPrimario);

int verifica_chave(int *root, char *chave, FILE *idxPrimario);

Node menor_no(int *root, FILE *idxPrimario);
#endif