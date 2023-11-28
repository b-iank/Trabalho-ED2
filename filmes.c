/**
 * Arquivo com as funções principais do programa de coleção de filmes.
 * @author Bianca Lançoni de Oliveira Garcia
 * @author Lucas Furriel Rodrigues
 */


#include "filmes.h"

/*  Função responsável por ler do usuário uma dentre as 7 opções disponíveis do sitema. */
int menu() {

    int op = -1, ret;
    do {
        printf(LARANJA NEGRITO SUBLINHADO"\n------------ COLECAO DE FILMES ------------" LIMPA);
        printf("\n\n--------- Insira a opcao desejada:\n");
        printf("\t0- Encerrar programa\n");
        printf("\t1- Inserir novo filme\n");
        printf("\t2- Remover um filme\n");
        printf("\t3- Modificar nota de um filme\n");
        printf("\t4- Buscar um filme (por ID)\n");
        printf("\t5- Buscar um filme (por nome)\n");
        printf("\t6- Listar todos os filmes\n");
        printf("\t7- Listar os filmes apos uma chave (RANGE)\n");
        printf("\t8- Compactar o arquivo de filmes\n");
        printf(ITALICO "Opcao: " LIMPA);

        ret = scanf("%d", &op);

        if (ret != 1 || op < 0 || op > 8) {
            printf(ERRO NEGRITO "OPCAO INVALIDA!\n" LIMPA);
            while (getchar() != '\n'); //Limpa o buffer do teclado para evitar comportamentos inesperados
        }
    } while (ret != 1 || op < 0 || op > 8);

    return op;
}

/*  Abre o arquivo de filmes ("movies.dat"). */
FILE *arquivoFilme(char *pth) {
    FILE *fp;
    fp = fopen(pth, "r+a"); //Tenta a abertura por "r+a"

    if (fp == NULL)     //Caso o arquivo não exista, abre com "w+"
        fp = fopen(pth, "w+");

    return fp;
}

/*  Abre o arquivo de índice primário ("iprimary.idx"). */
FILE *arquivoIndicePrimario(char *pth) {
    FILE *fp;
    fp = fopen(pth, "r+a"); //Tenta a abertura por "r+a"

    if (fp == NULL) {   //Caso o arquivo não exista, abre com "w+" e insere flag 0 no cabeçalho
        fp = fopen(pth, "w+");
        fprintf(fp, "%s", "0#");
    }

    return fp;
}

/*  Abre o arquivo de índice secundário ("ititle.idx"). */
FILE *arquivoIndiceSecundario(char *pth) {
    FILE *fp;
    fp = fopen(pth, "r+a"); //Tenta a abertura por "r+a"

    if (fp == NULL) {   //Caso o arquivo não exista, abre com "w+" e insere flag 0 no cabeçalho
        fp = fopen(pth, "w+");
        fprintf(fp, "%s", "0#");
    }

    return fp;
}


/*  Monta a árvore rubro-negra com os arquivos de índice secundário ou, quando a flag está em 0, com o arquivo de filmes. */
no *carregarIndiceS(FILE *filmes, FILE *indiceS) {
    int flag, cont = 0;
    no *raiz = NULL, *novo, *aux;

    //Lê a flag do arquivo de índice secundário
    fscanf(indiceS, "%d", &flag);
    fgetc(indiceS);

    if (flag == 1) { //Caso a flag seja 1, carrega os dados do arquivo de índice secundário para a árvore rubro-negra
        while (!feof(indiceS)) {
            novo = malloc(sizeof(no));
            novo->idxSecundario.chaves = malloc(sizeof(Lista));
            fscanf(indiceS, "%[^@]s", novo->chave);
            fgetc(indiceS);
            fscanf(indiceS, "%[^#]s", novo->idxSecundario.chaves->chaveP);
            fgetc(indiceS);

            strcpy(novo->idxSecundario.nome, novo->chave);

            novo->idxSecundario.chaves->prox = NULL;
            if ((aux = buscaNo(raiz, novo->chave)) != NULL) {
                insereLista(&aux, novo->idxSecundario.chaves->chaveP);
            }
            else
                raiz = inserir(raiz, novo);
        }
        printf("\n");
    } else { //Caso contrário, carrega os dados do arquivo de filmes para a árvore rubro-negra
        rewind(filmes);

        while(!feof(filmes)) {
            novo = (no*) malloc(sizeof(no));
            novo->idxSecundario.chaves = malloc(sizeof(Lista));
            if (fscanf(filmes, "%[^@]s", novo->idxSecundario.chaves->chaveP) == EOF || fgetc(filmes) == EOF || fscanf(filmes, "%[^@]s", novo->chave) == EOF)
                break;

            strcpy(novo->idxSecundario.nome, novo->chave);

            novo->idxSecundario.chaves->prox = NULL;
            cont++;
            fseek(filmes, 192*cont, SEEK_SET);
            if (novo->idxSecundario.chaves->chaveP[0] != '*') {
                if ((aux = buscaNo(raiz, novo->idxSecundario.nome)) != NULL) {
                    insereLista(&aux, novo->idxSecundario.chaves->chaveP);
                }
                else
                    raiz = inserir(raiz, novo);
            }
        }
    }

    return raiz;
}


/*  Responsável por gerar a chave primária de um filme. Criada com as três primeiras letras do
    sobrenome do diretor e os últimos dois dígitos do ano de lançamento do filme. */
char *geraChavePrimaria(FILME *filme) {
    char *chave = malloc(6);
    for (int i = 0; i < 3; i++)
        chave[i] = toupper(filme->nomeDiretor[i]);
    chave[3] = filme->anoLancamento[2];
    chave[4] = filme->anoLancamento[3];
    chave[5] = '\0';

    return chave;
}

/*  Função responsável por inserir um novo filme no arquivo de filmes e nas árvores de índices primário e secundário. */
int inserirFilme(FILE *filmes, FILE *fileIndiceP, int *indiceP, no **indiceS) {
    int erro = 1;
    no *aux = NULL;
    FILME *novo = malloc(sizeof(FILME)); //Aloca memória para a estrutura FILME

    //Calcula o RRN (Relative Record Number) do novo registro no arquivo de filmes
    int rrn = calculaRRN(filmes);

    if (rrn >= 100) {
        printf(ALERTA "\nLimite de filmes atingido! Tente compactar o arquivo ou remover um filme." LIMPA);
        return 0;
    }
    printf(ALERTA "\nPara funcionamento correto do sistema, por favor nao inclua caracteres epeciais (acentos, cedilha, etc)\n" LIMPA);

    //Lê informações do novo filme do usuário
    printf(ITALICO "Digite o titulo em portugues do filme: " LIMPA);
    scanf(" %65[^\n]s", novo->tituloPortugues);
    while (getchar() != '\n');

    printf(ITALICO "Digite o titulo original do filme: " LIMPA);
    scanf(" %70[^\n]s", novo->tituloOriginal);
    while (getchar() != '\n');


    while (erro) {
        printf(ITALICO "Digite o nome do diretor <Sobrenome, Nome>: " LIMPA);
        scanf(" %20[^\n]s", novo->nomeDiretor);
        while (getchar() != '\n');
        erro = validaNome(novo->nomeDiretor);
        if (erro)
            printf(ERRO NEGRITO "O sobrenome precisa conter 3 caracteres\n" LIMPA);
    }

    erro = 1;
    while (erro) {
        printf(ITALICO "Digite o ano de lancamento: " LIMPA);
        scanf(" %4[^\n]s", novo->anoLancamento);
        while (getchar() != '\n');
        erro = validaAno(novo->anoLancamento);
        if (erro)
            printf(ERRO NEGRITO "Entrada invalida\n" LIMPA);
    }

    printf(ITALICO "Digite o pais do filme: " LIMPA);
    scanf(" %20[^\n]s", novo->pais);
    while (getchar() != '\n');

    do {
        erro = 0;
        printf(ITALICO "Digite sua nota para o filme: " LIMPA);
        scanf(" %c", &novo->nota);
        while (getchar() != '\n');
        if (isalpha(novo->nota)) {
            printf(ERRO NEGRITO "Nota e um valor numerico\n" LIMPA);
            erro = 1;
        }
    } while (erro);

    //Gera a chave primária para o novo filme
    strcpy(novo->chavePrimaria, geraChavePrimaria(novo));

    //Verifica se já existe um filme com a mesma chave primária no índice primário
    if (validaDuplicidade(fileIndiceP,novo->chavePrimaria, *indiceP)) {
        printf(ERRO NEGRITO "\nChave primaria %s ja existe na sua base de dados!" LIMPA, novo->chavePrimaria);
        return 0;
    }

    //Insere o novo filme nas árvores de índices primário e secundário
    *indiceP = insere_chave(*indiceP, fileIndiceP, novo->chavePrimaria, rrn);

    aux = buscaNo(*indiceS, novo->tituloPortugues);
    if (aux != NULL)
        insereLista(&aux, novo->chavePrimaria);
    else {
        aux = malloc(sizeof(no));
        aux->idxSecundario.chaves = malloc(sizeof(Lista));

        strcpy(aux->chave, (novo->tituloPortugues));

        strcpy(aux->idxSecundario.nome, novo->tituloPortugues);
        strcpy(aux->idxSecundario.chaves->chaveP, novo->chavePrimaria);

        aux->idxSecundario.chaves->prox = NULL;
        *indiceS = inserir(*indiceS, aux);
    }
    //Escreve as informações do novo filme no final do arquivo de filmes
    int len = 7;
    fprintf(filmes, "%s@", novo->chavePrimaria);
    len += (int) strlen(novo->chavePrimaria);
    fprintf(filmes, "%s@", novo->tituloPortugues);
    len += (int) strlen(novo->tituloPortugues);

    //Verifica se o título original é igual ao título em português e escreve "Idem" caso seja
    if (strcmp(novo->tituloOriginal, novo->tituloPortugues) == 0) {
        fprintf(filmes, "%s@", "Idem");
        len += 4;
    } else {
        fprintf(filmes, "%s@", novo->tituloOriginal);
        len += (int) strlen(novo->tituloOriginal);
    }

    //Escreve as informações restantes do novo filme no arquivo de filmes
    fprintf(filmes, "%s@", novo->nomeDiretor);
    len += (int) strlen(novo->nomeDiretor);
    fprintf(filmes, "%s@", novo->anoLancamento);
    len += (int) strlen(novo->anoLancamento);
    fprintf(filmes, "%s@", novo->pais);
    len += (int) strlen(novo->pais);
    fprintf(filmes, "%c@", novo->nota);
    len += 1;

    //Preenche o espaço restante até o tamanho máximo de registro (192 bytes) com caracteres "#"
    char temp[176] = "";
    for (int i = 192; i > len; i--)
        strcat(temp, "#");

    fprintf(filmes, "%s", temp);

    printf(SUCESSO "Filme inserido!\n" LIMPA);

    free(novo);

    return 1;
}

/*  Valida o nome do diretor digitado pelo usuário. O nome precisa conter ao menos 3 caracteres e eles não podem ser números.
    Retorna TRUE para inválido e FALSE para entrada permitida. */
int validaNome(char nome[21]) {
    if (strlen(nome) < 3)
        return 1;

    for (int i = 0; i < 3; i++) {
        if (nome[i] == ' ' || isdigit(nome[i]))
            return 1;
    }

    return 0;
}

/*  Valida o ano digitado pelo usuário. Precisa conter 4 caracteres, e todos devem ser digitos. Retorna TRUE para
    inválido e FALSE para entrada permitida  */
int validaAno(char ano[5]) {

    if (strlen(ano) < 4)
        return 1;

    for (int i = 0; i < 4; i++) {
        if (ano[i] == ' ' || isalpha(ano[i]))
            return 1;
    }

    int anoint = atoi(ano);
    if (anoint > 2023)
        return 1;

    return 0;
}

/*  Valida a existência de um filme com a mesma chave. */
int validaDuplicidade(FILE *fp, char *chave, int raiz) {
    int busca = busca_registro(raiz, fp, chave);
    return busca != -1 ? 1 : 0;
}

/*  Calcula o RRN da última posição do arquivo de filmes. */
int calculaRRN(FILE *fp) {
    fseek(fp, 0, SEEK_END);
    return ftell(fp) / 192;
}

/*  Lê do usuário uma chave primária e remove do arquivo de filmes aquele com a chave correspondente.
    A remoção é feita inserindo *| no início da chave (Ex: "RIB23" -> "*|B23"). */
int removerFilme(FILE *fp, FILE *fileIndiceP, int *indiceP, no **indiceS) {
    int busca;
    char temp[66], chave[6];
    no *buscaS = NULL;

    printf(ITALICO "Digite a chave primaria do filme que quer remover: " LIMPA);
    scanf(" %s", chave);
    for (int i = 0; i < 6; i++)
        chave[i] = toupper(chave[i]);

    if ((busca = busca_registro(*indiceP, fileIndiceP, chave)) == -1) {
        printf(ERRO NEGRITO "Filme nao encontrado!\n" LIMPA);
        return 0;
    }

    fseek(fp, busca * 192, SEEK_SET); //Posiciona o ponteiro do arquivo de filmes conforme o RRN do filme a ser removido
    fscanf(fp, "%[^@]", temp);
    fgetc(fp);
    fscanf(fp, "%[^@]", temp); //Pega a chave secundaria do filme para remover da árvore
    fseek(fp, busca * 192, SEEK_SET); //Volta ao início do registro
    fprintf(fp, "%s", "*|"); //Sobreescreve a chava primária com "*|"

    *indiceP = remover(*indiceP, fileIndiceP, chave);
    buscaS = buscaNo(*indiceS, temp);
    if (buscaS != NULL) {//Remove da árvore secundária
        if (buscaS->idxSecundario.chaves->prox != NULL)
            removeLista(&buscaS, chave);
        else
            *indiceS = remover_avl(indiceS, buscaS);
        printf(SUCESSO "Filme removido!\n" LIMPA);
    }
    else
        printf(ERRO NEGRITO "Erro ao deletar chave secundaria!\n" LIMPA);


    return 1;
}

/*  Lê do usuário uma chave primária e edita a nota no arquivo de filmes aquele com a chave correspondente.
    A edição é feita buscando o último valor do mesmo bloco, ou seja, caminhando por 6 separadores "@" e
    sobreescrevendo aquele valor. */
void modificarNota(FILE *fp, FILE *fileIndiceP, int indiceP) {
    int busca;
    char nota, chave[6] = {'\0'}, aux[100];
    int count = 0;

    printf(ITALICO "Digite a chave primaria do filme que quer editar: " LIMPA);
    scanf(" %s", chave);
    for (int i = 0; i < 6; i++)
        chave[i] = toupper(chave[i]);

    if ((busca = busca_registro(indiceP, fileIndiceP, chave)) == -1) {
        printf(ERRO NEGRITO "Filme nao encontrado!\n" LIMPA);
        return;
    }

    fseek(fp, busca * 192, SEEK_SET); //Posiciona o ponteiro do arquivo de filmes conforme o RRN do filme a ser editado

    printf(ITALICO "\nDigite a nova nota: " LIMPA);
    scanf(" %c", &nota);

    while (!feof(fp) && count != 6) { //Caminha pelas strings contando os separadores até a posição onde esta a nota
        fscanf(fp, "%[^@]s", aux);
        fgetc(fp);
        count++;
    }

    int pos = (int) ftell(fp);
    fseek(fp, pos, SEEK_SET); //Posiciona o ponteiro na posição da nota
    fprintf(fp, "%c", nota); //Sobreescereve editando a nota

    printf(SUCESSO "Nota alterada!\n" LIMPA);
}

/*  Busca por um filme pela sua chave primária e chama função de imprimir */
void buscarChavePrimaria(FILE *fp, FILE *fileIndiceP, int indiceP, char *chave) {
    int busca;
    for (int i = 0; i < 5; i++)
        chave[i] = toupper(chave[i]);

    if ((busca = busca_registro(indiceP, fileIndiceP, chave)) == -1) {
        printf(ERRO NEGRITO "Filme nao encontrado!\n" LIMPA);
        return;
    }

    imprimeFilme(&fp, busca);
}

/*  Busca por um filme pelo seu títlo em português e chama função para buscar pela sua chave primária */
void buscarChaveSecundaria(FILE *fp, FILE *fileIndiceP, int indiceP, no *indiceS, char *titulo) {
    no *busca = buscaNo(indiceS, titulo);
    if (busca == NULL) {
        printf(ERRO NEGRITO "Titulo nao encontrado!\n" LIMPA);
        return;
    }
    Lista *lista = busca->idxSecundario.chaves;
    while (lista != NULL) {
        buscarChavePrimaria(fp, fileIndiceP, indiceP, lista->chaveP);
        lista = lista->prox;
    }
}

/*  Lista os filmes do arquivo caminhando recursivamente pela árvore rubro-negra. A recursão funciona para caminhar em
    ordem pela árvore secundária e então caminha pela lista buscando na árvore primária para printar o filme no terminal. */
void listarFilmes(FILE *filmes, FILE *fileIndiceP, PAGE folha, char chave[6]) {
    int rrn_folha = folha.rrn_pagina, rrn, i;

    for (i = 0; i < 5; i++)
        chave[i] = toupper(chave[i]);

    i = 0;
    while (strcmp(chave, folha.chaves[i]) > 0)
        i++;
    while (rrn_folha != -1) {
        rrn = folha.rrn[i];
        while (rrn != -1 && i < ORDEM - 1) {
            imprimeFilme(&filmes, rrn);
            rrn = folha.rrn[++i];
        }
        rrn_folha = folha.rrn[ORDEM - 1];
        folha = le_pagina(rrn_folha, fileIndiceP);
        i = 0;
    }
}

/*  Imprime no terminal um filme em específico */
void imprimeFilme(FILE **f, int rrn) {
    FILE *fp = *f;
    char temp[71], aux[71];
    char nota;

    rewind(fp);
    fseek(fp, rrn * 192, SEEK_SET);

    printf("---------------------------------\n");
    fscanf(fp, "%[^@]s", temp);
    printf(LARANJA "Chave:\t\t\t" LIMPA  "%s\n", temp);
    fgetc(fp);

    fscanf(fp, "%[^@]s", temp);
    printf(LARANJA  "Titulo portugues:\t" LIMPA "%s\n", temp);
    fgetc(fp);

    fscanf(fp, "%[^@]s", aux);
    if (strcmp(aux, "Idem") == 0)
        printf(LARANJA "Titulo Original:\t" LIMPA "%s\n", temp);
    else
        printf(LARANJA "Titulo Original:\t" LIMPA "%s\n", aux);
    fgetc(fp);

    fscanf(fp, "%[^@]s", temp);
    printf(LARANJA "Diretor:\t\t" LIMPA "%s\n", temp);
    fgetc(fp);

    fscanf(fp, "%[^@]s", temp);
    printf(LARANJA "Ano de lancamento:\t" LIMPA "%s\n", temp);
    fgetc(fp);

    fscanf(fp, "%[^@]s", temp);
    printf(LARANJA "Pais:\t\t\t" LIMPA "%s\n", temp);
    fgetc(fp);

    fscanf(fp, "%c", &nota);
    printf(LARANJA "Nota:\t\t\t" LIMPA "%c\n", nota);
}

/*  Remove do arquivo de filmes aqueles deletados previamente. A função caminha pelo arquivo sobreescrevendo os registros
    de filmes que iniciam por *| contando canda filme removido. Para compactar o arquivo a função ainda aciona o método truncate.
    Por fim, a árvore de índice primário é expurgada e rescrita caminhando pelo arquivo agora compactado. */
int compactarArquivo(FILE *fp, FILE *fileIndiceP, int *indiceP) {
    int count = 0, aux, deletados = 0, total = calculaRRN(fp), rrn = 0;
    int continua = 1;
    char filme[193] = "", chave[6];

    while (count < total) {
        fseek(fp, rrn * 192, SEEK_SET);
        fscanf(fp, "%192[^\n]s", filme); //Pega um filme completo

        if (filme[0] == '*' && filme[1] == '|') { //Verifica se é um filme deletado
            deletados++;
            aux = rrn;
            continua = 1;
            while (continua) {
                fseek(fp, (aux + 1) * 192, SEEK_SET);
                fscanf(fp, "%192[^\n]s", filme);
                if (fgetc(fp) == EOF) //Verifica se chegou ao último filme
                    continua = 0;
                fseek(fp, aux * 192, SEEK_SET);
                fprintf(fp, "%s", filme);
                aux++;
            }
            rrn--;
        }
        count++;
        rrn++;
    }

    fseek(fp, 0, SEEK_END);
    if (deletados > 0) {
        ftruncate(fileno(fp), ftell(fp) - (deletados * 192)); //Truncate para remover do fim do arquivo os duplicados que ficam do laço anterior

        //Expurga a árvores de índice primário e a reescreve partindo do arquivo de filmes
        //É necessário para manter a integridade do RRN

        fseek(fileIndiceP, 0, SEEK_SET);
        ftruncate(fileno(fileIndiceP), 2 * TAM_REGISTRO);
        escreve_pagina_vazia(fileIndiceP);
        *indiceP = 1;
        rewind(fp);

        count = 0;
        while (1) {
            fseek(fp, count * 192, SEEK_SET);
            if (fscanf(fp, "%[^@]s", chave) == EOF) //Lê a chave do arquivo de filmes
                break;
            else if (chave[0] != '*' && chave[1] != '|')
                *indiceP = insere_chave(*indiceP, fileIndiceP, chave, count);  //Insere na árvore B+
            count++;
        }

        printf(SUCESSO "Arquivo compactado!\n" LIMPA);
        return 1;
    } else {
        printf(SUCESSO "Nada para compactar!\n" LIMPA);
        return 0;
    }
}

/*  Caminha de forma recursiva na árvore de índice secundário escrevendo seus valores no arquivo respectivo. */
void escreverIndiceS(FILE *fp, no *indiceS) {
    if (indiceS == NULL)
        return;
    escreverIndiceS(fp, indiceS->esq);
    Lista *lista = indiceS->idxSecundario.chaves;
    while (lista != NULL) {
        fprintf(fp, "#%s@%s", indiceS->idxSecundario.nome, lista->chaveP);
        lista = lista->prox;
    }
    escreverIndiceS(fp, indiceS->dir);
}

/*  Altera a flag dos arquivos de índice primário e secundário para 0 e salva o arquivo. */
void alteraFlag(FILE *fileIndiceP, FILE *fileIndiceS, int *flag) {
    rewind(fileIndiceP);
    rewind(fileIndiceS);

    fputc('0', fileIndiceP);
    fputc('0', fileIndiceS);

    fclose(fileIndiceS);

    *flag = 0;
}