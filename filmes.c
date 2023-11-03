/**
 * Arquivo com as funÃ§Ãµes principais do programa de coleÃ§Ã£o de filmes.
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
        printf("\t7- Compactar o arquivo de filmes\n");
        printf(ITALICO "Opcao: " LIMPA);

        ret = scanf("%d", &op);

        if (ret != 1 || op < 0 || op > 7) {
            printf(ERRO NEGRITO "OPCAO INVALIDA!\n" LIMPA);
            while (getchar() != '\n'); //Limpa o buffer do teclado para evitar comportamentos inesperados
        }
    } while (ret != 1 || op < 0 || op > 7);

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

/*  Monta a árvore rubro-negra com os arquivos de índice primário ou, quando a flag está em 0, com o arquivo de filmes. */
NO *carregarIndiceP(FILE *filmes, FILE *indiceP) {
    char chave[6];
    int rrn, flag, count = 0;
    NO *raiz = NULL;

    fscanf(indiceP, "%d", &flag); //Lê a flag do arquivo de índice primário
    fgetc(indiceP);

    if (flag == 1) { //Caso a flag seja 1, carrega os dados do arquivo de índice primário para a árvore rubro-negra
        while (!feof(indiceP)) {
            // Lê a chave e o RRN do arquivo de índice primário
            fscanf(indiceP, "%[^@]s", chave);
            fgetc(indiceP);
            fscanf(indiceP, "%d", &rrn);
            fgetc(indiceP);

            //Insere o nó na árvore rubro-negra
            insere_no(&raiz, chave, chave, "", rrn);
        }
    } else { //Caso contrário, carrega os dados do arquivo de filmes para a árvore rubro-negra
        rewind(filmes);
        while (TRUE) {
            fseek(filmes, count * 192, SEEK_SET);
            if (fscanf(filmes, "%[^@]s", chave) == EOF) //Lê a chave do arquivo de filmes
                break;
            else if (chave[0] != '*' && chave[1] != '|') {
                //Insere o nó na árvore rubro-negra
                insere_no(&raiz, chave, chave, "", count);
            }

            count++;
        }
    }

    return raiz;
}

/*  Monta a árvore rubro-negra com os arquivos de índice secundário ou, quando a flag está em 0, com o arquivo de filmes. */
NO *carregarIndiceS(FILE *filmes, FILE *indiceS) {
    char chave[6], titulo[71];
    int flag, count = 0;
    NO *raiz = NULL;

    //Lê a flag do arquivo de índice secundário
    fscanf(indiceS, "%d", &flag);
    fgetc(indiceS);

    if (flag == 1) { //Caso a flag seja 1, carrega os dados do arquivo de índice secundário para a árvore rubro-negra
        while (!feof(indiceS)) {
            //Lê o título e a chave do arquivo de índice secundário
            fscanf(indiceS, "%[^@]s", titulo);
            fgetc(indiceS);
            fscanf(indiceS, "%[^#]s", chave);
            fgetc(indiceS);

            NO *aux = busca_binaria(raiz, titulo);
            if (aux == NULL)
                insere_no(&raiz, titulo, chave, titulo, -1);
            else
                insere_lista(&aux, chave);
        }
    } else { //Caso contrário, carrega os dados do arquivo de filmes para a árvore rubro-negra
        rewind(filmes);
        while (TRUE) {
            fseek(filmes, count * 192, SEEK_SET);
            if (fscanf(filmes, "%[^@]s", chave) == EOF) //Lê a chave e o título do arquivo de filmes
                break;
            else if (chave[0] != '*' && chave[1] != '|') {
                fgetc(filmes);
                fscanf(filmes, "%[^@]s", titulo);

                NO *aux = busca_binaria(raiz, titulo);
                if (aux == NULL)
                    insere_no(&raiz, titulo, chave, titulo, -1);
                else
                    insere_lista(&aux, chave);
            }

            count++;
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
boolean inserirFilme(FILE *filmes, NO **indiceP, NO **indiceS) {
    boolean erro = TRUE;
    FILME *novo = malloc(sizeof(FILME)); //Aloca memória para a estrutura FILME
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

    erro = TRUE;
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
        erro = FALSE;
        printf(ITALICO "Digite sua nota para o filme: " LIMPA);
        scanf(" %c", &novo->nota);
        while (getchar() != '\n');
        if (isalpha(novo->nota)) {
            printf(ERRO NEGRITO "Nota e um valor numerico\n" LIMPA);
            erro = TRUE;
        }
    } while (erro);

    //Gera a chave primária para o novo filme
    strcpy(novo->chavePrimaria, geraChavePrimaria(novo));

    //Verifica se já existe um filme com a mesma chave primária no índice primário
    if (validaDuplicidade(novo->chavePrimaria, *indiceP)) {
        printf(ERRO NEGRITO "\nChave primaria %s ja existe na sua base de dados!" LIMPA, novo->chavePrimaria);
        return FALSE;
    }

    //Calcula o RRN (Relative Record Number) do novo registro no arquivo de filmes
    int rrn = calculaRRN(filmes);

    //Insere o novo filme nas árvores de índices primário e secundário
    insere_no(indiceP, novo->chavePrimaria, novo->chavePrimaria, NULL, rrn);
    NO *busca = busca_binaria(*indiceS, novo->tituloPortugues);
    if (busca == NULL)
        insere_no(indiceS, novo->tituloPortugues, novo->chavePrimaria, novo->tituloPortugues, -1);
    else
        insere_lista(&busca, novo->chavePrimaria);

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
    char aux[176] = "";
    for (int i = 192; i > len; i--)
        strcat(aux, "#");

    fprintf(filmes, "%s", aux);

    printf(SUCESSO "Filme inserido!\n" LIMPA);

    free(novo);

    return TRUE;
}

/*  Valida o nome do diretor digitado pelo usuário. O nome precisa conter ao menos 3 caracteres e eles não podem ser números.
    Retorna TRUE para inválido e FALSE para entrada permitida. */
boolean validaNome(char nome[21]) {
    if (strlen(nome) < 3)
        return TRUE;

    for (int i = 0; i < 3; i++) {
        if (nome[i] == ' ' || isdigit(nome[i]))
            return TRUE;
    }

    return FALSE;
}

/*  Valida o ano digitado pelo usuário. Precisa conter 4 caracteres, e todos devem ser digitos. Retorna TRUE para
    inválido e FALSE para entrada permitida  */
boolean validaAno(char ano[5]) {

    if (strlen(ano) < 4)
        return TRUE;

    for (int i = 0; i < 4; i++) {
        if (ano[i] == ' ' || isalpha(ano[i]))
            return TRUE;
    }

    int anoint = atoi(ano);
    if (anoint > 2023)
        return TRUE;

    return FALSE;
}

/*  Valida a existência de um filme com a mesma chave. */
boolean validaDuplicidade(char *chave, NO *indiceP) {
    NO *busca = busca_binaria(indiceP, chave);
    return busca != NULL ? TRUE : FALSE;
}

/*  Calcula o RRN da última posição do arquivo de filmes. */
int calculaRRN(FILE *fp) {
    fseek(fp, 0, SEEK_END);
    return ftell(fp) / 192;
}


/*  Lê do usuário uma chave primária e remove do arquivo de filmes aquele com a chave correspondente.
    A remoção é feita inserindo *| no início da chave (Ex: "RIB23" -> "*|B23"). */
boolean removerFilme(FILE *fp, NO **indiceP, NO **indiceS) {
    NO *busca;
    char temp[66], chave[6];

    printf(ITALICO "Digite a chave primaria do filme que quer remover: " LIMPA);
    scanf(" %s", chave);
    for (int i = 0; i < 6; i++)
        chave[i] = toupper(chave[i]);
    busca = busca_binaria(*indiceP, chave);
    if (busca == NULL) {
        printf(ERRO NEGRITO "Filme nao encontrado!\n" LIMPA);
        return FALSE;
    }

    fseek(fp, busca->rrn * 192, SEEK_SET); //Posiciona o ponteiro do arquivo de filmes conforme o RRN do filme a ser removido
    fscanf(fp, "%[^@]", temp);
    fgetc(fp);
    fscanf(fp, "%[^@]", temp); //Pega a chave secundaria do filme para remover da árvore
    fseek(fp, busca->rrn * 192, SEEK_SET); //Volta ao início do registro
    fprintf(fp, "%s", "*|"); //Sobreescreve a chava primária com "*|"

    if (deleta_no(indiceP, busca->ordem, busca->chave, busca->titulo, busca->rrn)) { //Remove da árvore primária
        busca = busca_binaria(*indiceS, temp);
        if (busca != NULL) {//Remove da árvore secundária
            if (busca->lista->prox == NULL) //Implica que há apenas um elemento
                deleta_no(indiceS, busca->ordem, busca->chave, busca->titulo, busca->rrn);
            else
                remove_lista(&busca, chave);
            printf(SUCESSO "Filme removido!\n" LIMPA);
        } else
            printf(ERRO NEGRITO "Erro ao deletar chave secundaria!\n" LIMPA);
    } else
        printf(ERRO NEGRITO "Erro ao deletar chave primaria!\n" LIMPA);

    return TRUE;
}

/*  Lê do usuário uma chave primária e edita a nota no arquivo de filmes aquele com a chave correspondente.
    A edição é feita buscando o último valor do mesmo bloco, ou seja, caminhando por 6 separadores "@" e
    sobreescrevendo aquele valor. */
void modificarNota(FILE *fp, NO *indiceP) {
    NO *busca;
    char nota, chave[6] = {'\0'}, aux[100];
    int count = 0;

    printf(ITALICO "Digite a chave primaria do filme que quer editar: " LIMPA);
    scanf(" %s", chave);
    for (int i = 0; i < 6; i++)
        chave[i] = toupper(chave[i]);
    busca = busca_binaria(indiceP, chave);
    if (busca == NULL) {
        printf(ERRO NEGRITO "Filme nao encontrado!\n" LIMPA);
        return;
    }

    fseek(fp, busca->rrn * 192, SEEK_SET); //Posiciona o ponteiro do arquivo de filmes conforme o RRN do filme a ser editado

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
void buscarChavePrimaria(FILE *fp, NO *indiceP, char *chave) {
    char chaveUpper[6] = {'\0'};
    for (int i = 0; i < 5; i++)
        chaveUpper[i] = toupper(chave[i]);

    NO *busca = busca_binaria(indiceP, chaveUpper);

    if (busca == NULL) {
        printf(ERRO NEGRITO "Filme nao encontrado!\n" LIMPA);
        return;
    }

    imprimeFilme(&fp, busca->rrn);
}

/*  Busca por um filme pelo seu títlo em português e chama função para buscar pela sua chave primária */
void buscarChaveSecundaria(FILE *fp, NO *indiceP, NO *indiceS, char *titulo) {

    NO *busca = busca_binaria(indiceS, titulo);
    if (busca == NULL) {
        printf(ERRO NEGRITO "Titulo nao encontrado!\n" LIMPA);
        return;
    }
    LISTA *lista = busca->lista;
    while (lista != NULL) {
        buscarChavePrimaria(fp, indiceP, lista->chave);
        lista = lista->prox;
    }
}

/*  Lista os filmes do arquivo caminhando recursivamente pela árvore rubro-negra. A recursão funciona para caminhar em
    ordem pela árvore secundária e então caminha pela lista buscando na árvore primária para printar o filme no terminal. */
void listarFilmes(FILE *fp, NO *indiceP, NO *indiceS) {
    if (indiceS == NULL)
        return;
    listarFilmes(fp, indiceP, indiceS->esq);
    LISTA *lista = indiceS->lista;
    NO *busca;
    while (lista) {
        busca = busca_binaria(indiceP, lista->chave);
        imprimeFilme(&fp, busca->rrn);
        lista = lista->prox;
    }
    listarFilmes(fp, indiceP, indiceS->dir);
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
boolean compactarArquivo(FILE *fp, NO **indiceP) {
    int count = 0, aux, deletados = 0, total = calculaRRN(fp), rrn = 0;
    boolean continua = TRUE;
    char filme[193] = "", chave[6];

    while (count < total) {
        fseek(fp, rrn * 192, SEEK_SET);
        fscanf(fp, "%192[^\n]s", filme); //Pega um filme completo

        if (filme[0] == '*' && filme[1] == '|') { //Verifica se é um filme deletado
            deletados++;
            aux = rrn;
            continua = TRUE;
            while (continua) {
                fseek(fp, (aux + 1) * 192, SEEK_SET);
                fscanf(fp, "%192[^\n]s", filme);
                if (fgetc(fp) == EOF) //Verifica se chegou ao último filme
                    continua = FALSE;
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
        expurgar_arvore(*indiceP, FALSE);
        *indiceP = NULL;

        total = calculaRRN(fp);
        rewind(fp);

        count = 0;
        while (count < total) {
            fseek(fp, count * 192, SEEK_SET);
            fscanf(fp, "%[^@]s", chave);
            insere_no(indiceP, chave, chave, "", count);
            count++;
        }
        printf(SUCESSO "Arquivo compactado!\n" LIMPA);
        return TRUE;
    } else {
        printf(SUCESSO "Nada para compactar!\n" LIMPA);
        return FALSE;
    }
}

/*  Caminha de forma recursiva na árvore de índice primário escrevendo seus valores no arquivo de índice respectivo. */
void escreverIndiceP(FILE *fp, NO *indiceP) {
    if (indiceP == NULL)
        return;
    escreverIndiceP(fp, indiceP->esq);
    fprintf(fp, "#%s@%d", indiceP->chave, indiceP->rrn);
    escreverIndiceP(fp, indiceP->dir);
}

/*  Caminha de forma recursiva na árvore de índice secundário escrevendo seus valores no arquivo respectivo. */
void escreverIndiceS(FILE *fp, NO *indiceS) {
    if (indiceS == NULL)
        return;
    escreverIndiceS(fp, indiceS->esq);
    LISTA *lista = indiceS->lista;
    while (lista != NULL) {
        fprintf(fp, "#%s@%s", indiceS->titulo, lista->chave);
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

    fclose(fileIndiceP);
    fclose(fileIndiceS);

    *flag = 0;
}