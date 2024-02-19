//Importando as bibliotecas
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../includes/dados.h"
#include "../includes/idxSecundario.h"
#include "../includes/idxPrimario.h"


//Variáveis flag para verificar se mudou a raiz e flagMudanca para verificar se o arquivo de filmes foi alterado
//Variávies qtdFilmes e qtdNos para verificar a quantidade de filmes e nós, respectivamente
//Variável root para armazenar o valor da raiz
int flag,flagMudanca = 1, qtdFilmes, root,order, qtdNos = 0;

int main()
{   
    int op, RRN; //variáveis de opção do menu e RRN dos registros do arquivo de dados
    char chave[6], chaveSecundaria[50]; //Variáveis para guardar as chaves primárias e secundárias dos registros
    Header header; //Header do arquivo da árvore B+
    IDXSecundario *headSecundario = NULL, *indSecundario;//cabeça da lista de índices secundários e variável auxiliar para inserir na lista
    Filme filme; //Variável que representa o registro de dilme
    FILE *movies, *idxPrimario, *idxSecundario;//Arquivos

    //Tenta abrir os arquivos para leitura e escrita
    movies = fopen("movies.dat", "r+");
    idxPrimario = fopen("ibtree.idx", "rb+");
    idxSecundario = fopen("ititle.idx", "r+");

    //Verifica se arquivo de dados existe, senão, cria o arquivo
    if (movies == NULL)
        movies = fopen("movies.dat", "w+");
    //Busca a quantidade de registros do arquivo
    fseek(movies, 0, SEEK_END);
    qtdFilmes = ftell(movies) / 192;

    //verifica se o arquivo de índices primários existe, senão, cria o arquivo
    if (idxPrimario == NULL)
    {
        idxPrimario = fopen("ibtree.idx", "ab+");
        //Define os valores iniciais do header
        header.numNodes = 0;
        header.root = 0;
        header.order = 4;

        //Cria o primeiro nó da árvore B+ e o salva no arquivo
        Node new_node = cria_node(header.order,1,&header.numNodes);

        header.numNodes++;

        fwrite(&header,sizeof(struct header),1,idxPrimario);

        fwrite(&new_node,sizeof(struct node),1,idxPrimario);
        fflush(idxPrimario);
    }
    //Acessa o header para verificar a quantidade de nós e a raiz e salva nas respectivas variáveis
    fseek(idxPrimario,0,SEEK_SET);
    fread(&header,sizeof(struct header),1,idxPrimario);

    qtdNos = header.numNodes;
    root = header.root;

    //Verifica se o arquivo de índices secundários existe, senão realiza o mesmo processo que o arquivo de índices primários
    if (idxSecundario == NULL){
        idxSecundario = fopen("ititle.idx", "w+");
        fprintf(idxSecundario, "flag=0\n");
        atualiza_arquivoSecundario(movies, &headSecundario, qtdFilmes);
        fflush(idxSecundario);
    }else{//Verifica valor da flag
        fseek(idxSecundario, 5, SEEK_SET);
        fscanf(idxSecundario, "%d", &flag);
        if (flag == 0)
        {//Se flag = 0, atualiza o arquivo
            atualiza_arquivoSecundario(movies, &headSecundario, qtdFilmes);
        }
        else
        {//Senão, simplesmente lê o arquivo
            flag = 1;
            leitura_indicesSecundarios(&headSecundario, idxSecundario);
        }
    }

    //Menu do programa
    printf("------------Bem Vindo!------------\n\n");

    do
    {
        printf("\n\nEscolha uma opção: \n\n");
        printf("|1|- Inserir novo filme no arquivo\n");
        printf("|2|- Listar informações de um filme\n");
        printf("|3|- Listar arquivo\n");
        printf("|4|- Atualizar nota de um filme\n");
        printf("|5|- Listar filmes do arquivo\n");
        printf("|6|- Sair\n\n");

        printf("Opção: ");

        scanf("%d", &op);

        switch (op)
        {

        case 1://Inserção de novo filme
            fclose(idxPrimario);
            idxPrimario = fopen("ibtree.idx", "rb+");
            system("clear");
            printf("---------------Cadastro de novo filme---------------\n\n");
            printf("Não utilize acentos, por favor.\n\n");
            flag = insere_filme(movies, filme, idxPrimario, &root, &headSecundario, indSecundario, qtdFilmes, &qtdNos, header.order);
            flagMudanca = 0;
            
            //Caso flag = 0, significa que a raiz mudou, ou seja, atualiza o header do arquivo de índice primário
            if(flag == 0){
                fseek(idxPrimario,0,SEEK_SET);
                header.root = root;
                header.numNodes = qtdNos;
                fwrite(&header,sizeof(struct header),1,idxPrimario);
            }

            //Atualiza a flag do arquivo de índice secundário
            fseek(idxSecundario, 5, SEEK_SET);
            fputc('0', idxSecundario);
            fflush(idxPrimario);
            fflush(idxSecundario);

            //Aumenta a quantidade de filmes
            qtdFilmes++;
            pausa();
        
            break;

        case 2://Listar infos de um filme
            system("clear");
            printf("---------------Listar informações de filmes---------------\n\n");

            printf("Escolha um método de busca:\n\n");
            printf("\n|1|- Procurar por chave primária\n");
            printf("|2|- Procurar por título em português\n");
            printf("Opção: ");
            scanf("%d",&op);

            if(op == 1){//Opção de busca por chave primária
                printf("Digite a chave do filme: ");
                scanf(" %s", chave);

                RRN = busca_RRN(idxPrimario,chave,&root);//busca o RRN do registro
                if (RRN == -1){//Caso em que o filme não existe
                    printf("Esse filme não está cadastrado!\n");
                    pausa();
                }
                else{//Se o filme existir, imprime as infos na tela
                    busca_filme(movies, RRN);
                    pausa();
                }
            }else if(op == 2){//Opção de busca por chave secundária
                printf("Digite a chave secundária: ");
                scanf(" %[^\n]s", chaveSecundaria);
                //Imprime todos os filmes que possuem o mesmo título
                busca_filmes_por_chave_secundaria(movies,idxPrimario, headSecundario, chaveSecundaria, &root);
                pausa();
            }
            break;


        case 3:
            system("clear");
            //Lista todos os nós da árvore B+
            lista_arquivo(idxPrimario);
            pausa();
            break;


        case 4:
            system("clear");
            printf("---------------Atualizar nota de um filme---------------\n\n");
            printf("Digite a chave do filme: ");
            scanf(" %s", chave);

            //Busca o RRN do filme desejado
            RRN = busca_RRN(idxPrimario,chave,&root);

            if (RRN == -1){//Caso em que o filme não existe
                printf("Esse filme não está cadastrado!\n");
                pausa();
            }
            else
            {   //Se o filme existir, atualiza a nota dele
                atualiza_nota(movies, RRN, filme);
                printf("Nota atualizada com sucesso!\n");
                printf("\nNovos dados do filme:\n");
                //Mostra para o usuário as infos do filme com a nota atualizada
                busca_filme(movies,RRN);
                pausa();
            }

            break;

        case 5:
            system("clear");
            printf("---------------Listar filmes do arquivo---------------\n\n");
            printf("Escolha um método de listagem:\n\n");
            printf("\n|1|- Listar todos os filmes\n");
            printf("|2|- Listar por chave maior que um valor específico\n");
            printf("Opção: ");
            scanf("%d",&op);

            if (op == 1){//Procura a menor chave da árvore B+ e imprime todas as outras chaves a partir da lista ligada
                lista_filmes(movies,idxPrimario,&root);
                pausa();
            }
            else if(op == 2)
            {   printf("Digite a chave: ");
                scanf(" %s", chave);
                //Lista apenas os filmes com chaves maiores que a fornecida
                lista_maiores(movies,idxPrimario,&root,chave);
                pausa();
            }

            break;


        default:
            break;
        }

    } while (op != 6 && op >= 1 && op <= 6);

    if (flagMudanca != 1)
    { //Se houver mudanças, imprime os índices atualizados nos respectivos arquivos
        imprime_indiceSecundario(headSecundario,idxSecundario);
    }

    //Atualiza o header antes de fehcar o programa
    header.root = root;
    header.numNodes = qtdNos;
    fseek(idxPrimario,0,SEEK_SET);
    fwrite(&header,sizeof(struct header),1,idxPrimario);


    //Fecha os arquivos
    fclose(movies);
    fclose(idxPrimario);
    fclose(idxSecundario);
    //Libera a memória utilizada pelas listas
    libera_memoria_secundaria(&headSecundario);
    return 0;
}
