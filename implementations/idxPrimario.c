#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../includes/dados.h"
#include "../includes/idxPrimario.h"

//Função que cria e incializa um nó da árvore B+
Node cria_node(int order, int isLeaf, int *qtdNos){
    Node new_node;
    //Inicializa os valores do nó
    new_node.RRN = *qtdNos;
    new_node.isLeaf = isLeaf;
    new_node.next_node = -1;
    new_node.numKeys = 0;
    new_node.parent =-1;

    //Inicializa os valores das chaves e data RRN
    for(int i = 0; i < order; i++){
        new_node.dataRRN[i] = -1;
        strcpy(new_node.keys[i],"");
    }
    //Inicializa os valores do filhos
    for(int i = 0; i < order + 1; i++)
        new_node.children[i] = -1;
    
    //Retorna o nó inicializado
    return new_node;
}

//Função para inserir em uma folha
void insert_at_leaf(Node *leaf, char *key, int dataRRN){
    if((*leaf).numKeys == 0){//Caso em que a folha está vazia, apenas insere na primeira posição
        strcpy((*leaf).keys[0], key);
        (*leaf).dataRRN[0] = dataRRN;
        (*leaf).numKeys++;
    }else{//Caso a folha não esteja vazia, busca a melhor posição para inserir a chave
        for(int i =0; i < (*leaf).numKeys; i++){
            if(strcmp((*leaf).keys[i],key) == 0){//Se a chave for encontrada, retorna que ela já existe
                printf("A chave a ser inserida já existe!");
                break;
            }else if(strcmp((*leaf).keys[i], key) > 0){//Chave é menor que a chave na posição verificada
                //Dessa forma, deslocamos todos os elementos a partir dessa chave maior uma posição para a direita
                for(int j = (*leaf).numKeys; j > i;j--){
                    strcpy((*leaf).keys[j],(*leaf).keys[j-1]);
                    (*leaf).dataRRN[j] =(*leaf).dataRRN[j-1];
                }
                //Depois inserimos a chave desejada na posição correta
                strcpy((*leaf).keys[i],key);
                (*leaf).dataRRN[i] = dataRRN;
                (*leaf).numKeys++;
                break;

            }else if(i == (*leaf).numKeys - 1){//Caso em que a chave é a maior que todas as outras chaves na folha
                //Insere a chave na última posição da folha
                strcpy((*leaf).keys[i+1],key);
                (*leaf).dataRRN[i + 1] = dataRRN;
                (*leaf).numKeys++;
                break;
            }
        }
    }
}
//Função para inserir no nó pai após o split
int insert_in_parent(Node old_node, char *key_promovida, Node new_node, int *root, int *qtdNos,int order, FILE *idxPrimario){
    int flag = 1;
    //Caso em que o nó que sofreu splir é a raiz
    if((*root) == old_node.RRN){
        //Cria uma nova raiz e copia a chave promovida para a primeira posição
        Node newRoot = cria_node(order,0,qtdNos);
        strcpy(newRoot.keys[0],key_promovida);

        //atualiza os ponteiros dos filhos da nova raiz
        newRoot.children[0] = old_node.RRN;
        newRoot.children[1] = new_node.RRN;

        newRoot.numKeys++;

        //Variável da raiz recebe a nova raiz
        (*root) = newRoot.RRN;

        //Atualiza os ponteiros de pai nos nós que sofreram split
        old_node.parent = newRoot.RRN;
        new_node.parent = newRoot.RRN;

        (*qtdNos)++;

        //Atualiza o old_node no arquivo
        fseek(idxPrimario,sizeof(struct node) * old_node.RRN + sizeof(struct header),SEEK_SET);
        fwrite(&old_node,sizeof(struct node),1,idxPrimario);

        //Atualiza o new_node no arquivo
        fseek(idxPrimario,sizeof(struct node) * new_node.RRN + sizeof(struct header),SEEK_SET);
        fwrite(&new_node,sizeof(struct node),1,idxPrimario);

        //Insere a nova raiz no arquivo
        fseek(idxPrimario,0,SEEK_END);
        fwrite(&newRoot,sizeof(struct node),1,idxPrimario);

        return 0;
    }
    
    //Carrega o nó pai do nó splitado
    Node parentNode = cria_node(order,0,qtdNos);
    fseek(idxPrimario,sizeof(struct node) * old_node.parent + sizeof(struct header), SEEK_SET);
    fread(&parentNode, sizeof(struct node), 1, idxPrimario);

    int parentNumKeys = parentNode.numKeys;

    //Verifica onde a key promovida deve ser inserida no nó pai
    for(int i = 0; i<parentNumKeys;i++){
        if(parentNode.children[i] == old_node.RRN){//Caso em que o nó splitado é filho do nó na posição i
            //key promovida é menor que a chave no índice i
            //realiza o deslocamento para a direita das outras chaves e seus respectivos filhos
            for(int j = parentNode.numKeys; j > i;j--){
                    strcpy(parentNode.keys[j],parentNode.keys[j-1]);
                    parentNode.children[j+1] = parentNode.children[j];
            }
            //Copia a chave na posição i
            strcpy(parentNode.keys[i],key_promovida);
            parentNode.children[i+1] = new_node.RRN;
            parentNode.numKeys++;
        }else if(i == parentNode.numKeys - 1){//Caso em que a chave é maior que todas as outras chaves
            //Insere a chave no fim do arquivo
            strcpy(parentNode.keys[i+1],key_promovida);
            parentNode.children[i+2] = new_node.RRN;
            parentNode.numKeys++;
        }


    }

        //Caso de overflow do nó pai
        if(parentNode.numKeys == order){
            //Cria um novo nó para fazer o split
            Node parentdash = cria_node(order,0,qtdNos);
            Node nodeAux;
            //Atualiza o ponteiro de pai do novo nó
            parentdash.parent = parentNode.parent;
            //Calcula a posição intermediária
            int mid = ((int)ceil(order/2)) - 1;

            int j = 0;
            //Copia todas as chaves de mid + 1 até o fim para o novo nó
            for(int i = mid + 1; i<parentNode.numKeys;i++){
                if(i != parentNode.numKeys - 1)
                    strcpy(parentdash.keys[j],parentNode.keys[i+1]);
                parentdash.children[j] = parentNode.children[i+1];
                parentdash.numKeys++;
                j++;
            }
            parentdash.numKeys--;//Desconsidera a chave promovida

            strcpy(key_promovida,parentNode.keys[mid + 1]);

            //Limpa as infos do nó que sofreu split
            int numKeys = parentNode.numKeys;
            for(int i = mid + 1; i < numKeys;i++){
                parentNode.children[i + 1] = -1;
                strcpy(parentNode.keys[i],"");
                parentNode.numKeys--;
            }

            //atualiza os ponteiros de pai para os filhos do novo nó
            for(int i = 0; i < parentdash.numKeys + 1; i++){
                fseek(idxPrimario,sizeof(struct node) * parentdash.children[i] + sizeof(struct header), SEEK_SET);
                fread(&nodeAux, sizeof(struct node),1,idxPrimario);

                nodeAux.parent = parentdash.RRN;

                fseek(idxPrimario,sizeof(struct node) * parentdash.children[i] + sizeof(struct header), SEEK_SET);
                fwrite(&nodeAux, sizeof(struct node),1,idxPrimario);
            }

            (*qtdNos)++;
            
            //Por fim, atualiza o nó splitado e salva o novo nó criado no fim do arquivo
            fseek(idxPrimario,sizeof(struct node) * parentNode.RRN + sizeof(struct header), SEEK_SET);
            fwrite(&parentNode, sizeof(struct node), 1, idxPrimario);

            fseek(idxPrimario,0,SEEK_END);
            fwrite(&parentdash,sizeof(struct node),1,idxPrimario);

            //Chama a função recursiva para inserir no nó pai
            flag = insert_in_parent(parentNode,key_promovida,parentdash,root, qtdNos,order, idxPrimario);
        }else{//Caso em que não há overflow, apenas atualiza o nó pai
            fseek(idxPrimario,sizeof(struct node) * parentNode.RRN + sizeof(struct header), SEEK_SET);
            fwrite(&parentNode, sizeof(struct node), 1, idxPrimario);

            fseek(idxPrimario,sizeof(struct node) * old_node.RRN + sizeof(struct header), SEEK_SET);
            fwrite(&old_node, sizeof(struct node), 1, idxPrimario);
        }
    
    return flag;
}

//Função que busca o nó folha onde a chave está ou deveria estar
Node search(int *root, char *chave, FILE *idxPrimario){

    //carrega o nó raiz
    fseek(idxPrimario,sizeof(struct node)*(*root) + sizeof(struct header),SEEK_SET);

    Node current_node;
    fread(&current_node,sizeof(struct node),1,idxPrimario);

    //busca pela árvore até chegar no nó raiz
    while(current_node.isLeaf == 0){
        //busca a posição da chave
        for(int i =0; i < current_node.numKeys;i++){

             if(strcmp(chave,current_node.keys[i]) < 0){
                //Caso a chave seja menor, vai para o filho à esquerda
                int filhoEsq = current_node.children[i];
                fseek(idxPrimario,sizeof(struct node)*filhoEsq + sizeof(struct header),SEEK_SET);
                fread(&current_node,sizeof(struct node),1,idxPrimario);
                break;
            }else if(strcmp(chave,current_node.keys[i]) >= 0 && i == current_node.numKeys - 1){
                
                // Caso a chave seja maior, vai para o filho à direita
                int filhoDir = current_node.children[i + 1];
                fseek(idxPrimario,sizeof(struct node)*filhoDir + sizeof(struct header),SEEK_SET);
                fread(&current_node,sizeof(struct node),1,idxPrimario);
                break;
            }
            
        }
    }

    return current_node;
}

//Função de inserção geral
int insert(int *root, char *chave, int dataRRN, FILE *idxPrimario, int *qtdNos, int order){
    int flag = 1;
    //Busca o nó folha onde a chave será inserida
    Node old_node = search(root,chave, idxPrimario);
    Node teste;

    //Insere no nó folha
    insert_at_leaf(&old_node,chave,dataRRN);

    //Caso de overflow no nó folha
    if(old_node.numKeys == order){
        //Cria um novo nó folha
        Node new_node = cria_node(order,1,qtdNos);
        //Atualiza o ponteiro de pai para o novo nó folha
        new_node.parent = old_node.parent;
        (*qtdNos)++;

        //calcula a posição média
        int mid = ((int)ceil(order/2)) - 1;

        int j = 0;
        //Copia as chaves e infos da posição mid + 1 até o fim para o novo nó folha
        for(int i = mid + 1; i<old_node.numKeys;i++){
            strcpy(new_node.keys[j],old_node.keys[i]);
            new_node.dataRRN[j] = old_node.dataRRN[i];
            new_node.numKeys++;
            j++;
        }
        
        //Limpa as infos do nó que sofreu split
        int oldNodeKeys = old_node.numKeys;
        for(int i = mid + 1; i < oldNodeKeys;i++){
                old_node.children[i] = -1;
                old_node.dataRRN[i] = -1;
                strcpy(old_node.keys[i],"");
                old_node.numKeys--;
            }

        //Atualiza os ponteiros "prox" que formam a lista ligada
        new_node.next_node = old_node.next_node;
        old_node.next_node = new_node.RRN;

        //Atualiza o nó que sofreu split no arquivo
        fseek(idxPrimario, sizeof(struct node) * old_node.RRN + sizeof(struct header),SEEK_SET);
        fwrite(&old_node,sizeof(struct node),1,idxPrimario);
        
        //Insere o novo nó folha no arquivo
        fseek(idxPrimario, 0,SEEK_END);
        fwrite(&new_node,sizeof(struct node),1,idxPrimario);

        fflush(idxPrimario);

        //Chama a função para inserir a chave promovida no nó pai
        flag = insert_in_parent(old_node,new_node.keys[0],new_node,root, qtdNos,order, idxPrimario);
    }else{//Caso não haja overflow, apenas atualiza o nó folha no arquivo
        fseek(idxPrimario,(sizeof(struct node) * old_node.RRN) + sizeof(struct header),SEEK_SET);
        fwrite(&old_node,sizeof(struct node),1,idxPrimario);
        fflush(idxPrimario);
    }

    return flag;
}

//Função que busca o RRN de um registro dada uma chave
int busca_RRN(FILE *idxPrimario, char *chave, int *root){
    //Carrega o nó folha onde a chave deveria estar
    Node new_node = search(root,chave, idxPrimario);
    //Procura a chave, se encontrar, retorna o RRN
    for(int i = 0; i <new_node.numKeys;i++){
        if(strcmp(new_node.keys[i],chave) == 0){
            return new_node.dataRRN[i];
        }
    }
    //Se não encontrar, retorna -1
    return -1;
}

//Função que lista todo o arquivo de índice primário
void lista_arquivo(FILE *idxPrimario){
    int c;
    Node teste;
    Header header;

    //Carrega e imprime o header
    fseek(idxPrimario, 0 ,SEEK_SET);
    fread(&header, sizeof(struct header),1,idxPrimario);
    printf("\n\n------------Header------------\n\n");
    printf("Raiz: %d\n",header.root);
    printf("Ordem: %d\n",header.order);
    printf("Número de nós: %d\n",header.numNodes);

    //Carrega e imprime todos os nós do arquivo de índice primário
    printf("\n\n------------Árvore B+------------\n\n");
    while((c = fgetc(idxPrimario)) != EOF){
        ungetc(c,idxPrimario);
        fread(&teste,sizeof(struct node),1,idxPrimario);
        printf("\n----------------------------------------\n");
        printf("RRN: %d\n",teste.RRN);
        printf("IsLeaf: %d\n",teste.isLeaf);
        printf("NumKeys: %d\n",teste.numKeys);
        printf("Parent: %d\n",teste.parent);
        printf("chaves: %s, %s, %s, %s\n",teste.keys[0], teste.keys[1], teste.keys[2], teste.keys[3]);
        printf("DataRRN: %d, %d, %d, %d\n",teste.dataRRN[0], teste.dataRRN[1], teste.dataRRN[2], teste.dataRRN[3]);
        printf("Filhos:%d, %d, %d, %d, %d\n",teste.children[0], teste.children[1], teste.children[2], teste.children[3], teste.children[4]);
        printf("Prox: %d\n",teste.next_node);
    }
}

//Função que verifica se uma chave existe
int verifica_chave(int *root, char *chave, FILE *idxPrimario){
    //Procura o nó folha onde a chave deveria estar
    Node leaf = search(root,chave,idxPrimario);
    int flag = 1;
    //Verifica se a chave está no nó folha carregado
    for(int i =0; i < leaf.numKeys; i++){
            if(strcmp(leaf.keys[i],chave) == 0){
                printf("\nA chave a ser inserida já existe!\n");
                flag = 0;
            }
        }
    return flag;
}

//Função que devolve o nó folha mais à esquerda
Node menor_no(int *root, FILE *idxPrimario){
    //carrega a raiz
    fseek(idxPrimario,sizeof(struct node)*(*root) + sizeof(struct header),SEEK_SET);

    Node current_node;
    fread(&current_node,sizeof(struct node),1,idxPrimario);
    //Percorre a árvore sempre carregando o filho à esquerda dos nós, até chegar em uma folha
    while(current_node.isLeaf == 0){

        int filhoEsq = current_node.children[0];
        fseek(idxPrimario,sizeof(struct node)*filhoEsq + sizeof(struct header),SEEK_SET);
        fread(&current_node,sizeof(struct node),1,idxPrimario);

            
    }
    return current_node;
}