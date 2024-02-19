# Movie Manager
Programa feito em C que simula um gerenciador de filmes usando o algoritmo de Árvore B+ para armazenamento das informações

## Diretórios
- build/ : Onde ocorre a criação do executável e link das dependências
- include/ : Arquivos de cabeçalho .h
- implementations/ : Códigos fonte de implementações gerais do projeto
- src/ : Código fonte do arquivo main

## Execução do projeto
- Primeiro, entre no diretório build/
- Digite o comando `cmake .. ` para atualizar o arquivo MakeFile
- Depois, basta usar o comando `make` para criar o executável "movie_manager"
- Por último, execute o programa com o comando: `./movie_manager`

## Especificações
- Arquivos utilizados (localizados no diretório build/ após a execução do programa):
    - movies.dat: Arquivo que armazena as informações dos filmes
    - ibtree.idx: Arquivo binário responsável por armazenar os nós da Árvore B+, com um cabeçalho indicando a raiz e quantidade de nós da árvore
    - ititle.idx: Arquivo que armazena a chave e título dos filmes, este será usado como índice secundário
