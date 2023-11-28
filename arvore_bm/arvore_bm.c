#include "arvore_bm.h"

// MÉTODOS ÚTIL ---------------------------------------------------------------------------------------------

/*Lê o cabeçalho da árvore B+ e retorna o RRN da raíz */
int le_header(FILE *arvore, FILE *filmes) {
    int raiz, flag, count = 0;
    char chave[6], aux[3];

    fseek(arvore, 0, SEEK_SET); // Posiciona o arquivo no começo
    fscanf(arvore, "%d", &flag); // Lê a flag de consistência

    fgetc(arvore);
    fscanf(arvore, "%[^#]s", aux); // Lê a raiz da árvore
    raiz = atoi(aux); // Transofrma em inteirp

    if (flag == 0) { // Verifica a consistência do arquivo, caso 0 a árvore será reescrita
        fseek(arvore, 0, SEEK_SET); // Posiciona o arquivo no começo
        ftruncate(fileno(arvore), 2 * TAM_REGISTRO); // Corta o fim do arquivo mantendo apenas a header e a primeira página
        escreve_pagina_vazia(arvore); // Escreve a primeira página que será a raiz
        raiz = 1; // Iguala o índice da raiz à 1
        rewind(filmes);

        while (1 == 1) { // Caminha pelo arquivo de filmes até que chegue no final
            fseek(filmes, count * 192, SEEK_SET); // Posiciona o ponteiro do arquivo no filme
            if (fscanf(filmes, "%[^@]s", chave) == EOF) //Lê a chave do arquivo de filmes
                break; // Sai do laço após o final do arquivo
            else if (chave[0] != '*' && chave[1] != '|') // Caso o filme não esteja excluído insere na árvore B+
                raiz = insere_chave(raiz, arvore, chave, count);
            count++; // Próximo RRN
        }
    }

    return raiz; // Retorna o RRN da raíz
}

/* Função para escrever a header da árvore B+ */
void escreve_header(int raiz, FILE *fp, int flag) {
    int len = 33;
    char header[TAM_REGISTRO + 1] = {'\0'};
    fseek(fp, 0, SEEK_SET);

    fprintf(fp, "%d#", flag); // Escreve a flag de consistência

    if (raiz == -1) // Se a raíz for -1 (árvore apagada) mudamos para 1
        raiz = 1;

    // Escreve ao RRN da raíz no arquivo
    if (raiz < 9)
        fprintf(fp, "0%d", raiz);
    else
        fprintf(fp, "%d", raiz);

    // Escreve o valor da ORDEM da árvore no arquivo
    if (ORDEM < 9)
        fprintf(fp, "#ORDEM:0%d", ORDEM);
    else
        fprintf(fp, "#ORDEM:%d", ORDEM);

    // Escreve o valor do tamanho do registro da árvore no arquivo
    fprintf(fp, "#TAMANHO_REGISTRO:%d", TAM_REGISTRO);

    // Preenche o espaço restante até o tamanho máximo de registro com caracteres "#"
    for (int i = len; i < TAM_REGISTRO; i++)
        strcat(header, "#");
    fprintf(fp, "%s", header);
}

/* Escreve uma primeira página vazia no arquivo */
void escreve_pagina_vazia(FILE *fp) {
    fseek(fp, TAM_REGISTRO, SEEK_SET); // Posiciona o ponteiro do arquivo após a header
    fprintf(fp, "%s", "1@"); // Escreve 1 para indicar folha;
    int i;
    for (i = 0; i < ORDEM - 1; i++)
        fprintf(fp, "*****#"); // Escreve ORDEM - 1 chaves com *****
    putc('@', fp);
    for (i = 0; i < ORDEM; i++)
        fprintf(fp, "**#"); // Escreve ORDEM RRNs com **
    fprintf(fp, "@**@"); // Escreve ** para indicar que página é raíz
}

/* Função para ler uma página do arquivo da árvore B+ */
PAGE le_pagina(int rrn, FILE *fp) {
    PAGE pagina;
    char temp[3];

    fseek(fp, rrn * TAM_REGISTRO, SEEK_SET); // Posiciona o ponteiro do arquivo no RRN de parâmetro

    // Lê se a página é uma folha
    fscanf(fp, "%d", &pagina.folha);
    fgetc(fp);

    int qnt = 0;
    for (int i = 0; i < ORDEM - 1; i++) { // Laço para ler os campos de chaves e escrever na struct
        fscanf(fp, " %[^#]s", pagina.chaves[i]);
        pagina.chaves[i][5] = '\0';
        fgetc(fp);

        if (pagina.chaves[i][0] != '*')
            qnt++;
    }

    pagina.quantidade_chaves = qnt; // Quantidade de páginas lidas no laço anterior
    fgetc(fp);

    for (int i = 0; i < ORDEM; i++) { // Laço para ler os RRNs dos filhos da página ou para o arquivo de dados
        fscanf(fp, "%[^#]s", temp);
        temp[2] = '\0';
        fgetc(fp);

        if (temp[0] != '*')
            pagina.rrn[i] = atoi(temp); // Caso RRN esteja preenchido salva na struct seu número
        else
            pagina.rrn[i] = -1; // Caso contrário, salva -1 para representar campo vazio
    }

    fgetc(fp);
    fscanf(fp, "%[^@]", temp); // Lê o RRN do pai
    temp[2] = '\0';

    if (temp[0] == '*')
        pagina.rrn_pai = -1; // Caso não esteja preenchido (**) salvamos -1
    else
        pagina.rrn_pai = atoi(temp); // Caso contrário salvamos o RRN do pai daquela página

    pagina.rrn_pagina = rrn; // Armazena o valor do RRN da página

    return pagina; // Retorna a página lida
}

/* Calcula o RRN da última posição do arquivo */
int calcula_rrn(FILE *fp) {
    fseek(fp, 0, SEEK_END); // Posiciona no fim do arquivo
    return ftell(fp) / TAM_REGISTRO; // Retorna a divisão offset da última posição pelo tamanho de registro
}

/* Escreve no arquivo uma página passada de parâmetro*/
void escreve_pagina(FILE *fp, PAGE pagina) {

    if (pagina.rrn_pagina != -1) { // Confirma que o RRN da página não é -1 (página apagada)
        fseek(fp, pagina.rrn_pagina * TAM_REGISTRO, SEEK_SET); // Posiciona o ponteiro do arquivo na posição da página

        fprintf(fp, "%d", pagina.folha); // Escreve se a página é folha (1), interna (0) ou apagada (2)
        fputc('@', fp);

        int i;
        for (i = 0; i < ORDEM - 1; i++)
            fprintf(fp, "%s#", pagina.chaves[i]); // Escreve as chaves armazenadas
        putc('@', fp);

        for (i = 0; i < ORDEM; i++) { // Escreve os RRNs armazenados
            if (pagina.rrn[i] > 9)
                fprintf(fp, "%d#", pagina.rrn[i]); // Quando > 9 escrevemos apenas o valor armazenado
            else if (pagina.rrn[i] == -1)
                fprintf(fp, "%s", "**#"); // Quando -1 escrevemos **
            else
                fprintf(fp, "0%d#", pagina.rrn[i]); // Quando <= 9 escrevemos 0 + o valor (preencher 2 bytes)
        }
        putc('@', fp);

        // Escreve o RRN do pai
        if (pagina.rrn_pai == -1)
            fprintf(fp, "**@"); // Quando -1 escrevemos **
        else if (pagina.rrn_pai > 9)
            fprintf(fp, "%d@", pagina.rrn_pai); // Quando > 9 escrevemos apenas o valor armazenado
        else
            fprintf(fp, "0%d@", pagina.rrn_pai); // Quando <= 9 escrevemos 0 + o valor (preencher 2 bytes)
    }
}

/* Calcula a quantidade mínima de chaves na folha */
int calcula_quant_minima(int tamanho) {
    if (tamanho % 2 == 0)
        return (tamanho) / 2 - 1;
    else
        return (int) ceil((double) tamanho / 2) - 1;
}

/* Caminha pelos filhos de uma página corrigindo o apontamento do 'rrn_pai' e as reescrevendo no arquivo */
void escreve_filho(FILE *fp, PAGE pagina) {
    PAGE filho;
    for (int i = 0; i <= pagina.quantidade_chaves; i++) { // Caminha pelos filhos
        filho = le_pagina(pagina.rrn[i], fp); // Lê a página filho
        filho.rrn_pai = pagina.rrn_pagina; // Aponta o 'rrn_pai' para a página pai
        escreve_pagina(fp, filho); // Escreve o filho no arquivo
    }
}

/* Conta a quantidade de chaves escritas na árvore B+ */
int conta_chaves(FILE *fp, int raiz) {
    PAGE pagina = busca_primeira_folha(raiz, fp); // Busca a folha mais a esquerda
    int rrn_folha = pagina.rrn_pagina, rrn, i, count = 0;

    while (rrn_folha != -1) { // Caminha por todas as páginas folhas
        i = 0;
        pagina = le_pagina(rrn_folha, fp); // Lê a página atual
        rrn = pagina.rrn[0];
        while (rrn != -1 && i < ORDEM - 1) { // Caminha pelos RRNs das chaves até chegar em -1 ou no fim
            count++; // Incrementa a contagem de chaves
            rrn = pagina.rrn[++i]; // Aponta para o próximo rrn
        }
        rrn_folha = pagina.rrn[ORDEM - 1]; // Quando chegamos no último RRN, apontamos para a próxima folha
    }

    return count; // Retorna a quantidade de chaves
}
// ----------------------------------------------------------------------------------------------------------

// MÉTODOS DE BUSCA -----------------------------------------------------------------------------------------

/* Busca a folha em que uma chave PODE estar */
PAGE busca_folha(int raiz, FILE *fp, char chave[6]) {
    PAGE pagina = le_pagina(raiz, fp); // Lê a primeira página (raiz)
    int rrn = raiz, i;
    while (pagina.folha == 0) { // Caminha pelas páginas enquanto não chegarmos em uma folha
        i = 0;
        while (strcmp(chave, pagina.chaves[i]) >= 0 && i < pagina.quantidade_chaves) // Caminha pelos índíces
            i++;
        pagina = le_pagina( pagina.rrn[i], fp); // Lê o filho da página atual
    }

    return pagina; // Retorna a folha encontrada
}

/* Busca na árvore B+ por uma chave e retorna o seu RRN no arquivo de dados */
int busca_registro(int rrn_raiz, FILE *fp, char chave[6]) {
    PAGE pagina = busca_folha(rrn_raiz, fp, chave); // Busca a folha em que a chave pode estar

    for (int i = 0; i < pagina.quantidade_chaves; i++) { // Busca pelas chaves da folha a chave do parâmetro
        if (strcmp(pagina.chaves[i], chave) == 0)
            return pagina.rrn[i]; // Retorna o RRN do arquivo de dados
    }

    return -1; // Retorna -1 quando o registro não foi encontrado
}

/* Caminha pela árvore B+ buscando a folha mais a esquerda */
PAGE busca_primeira_folha(int raiz, FILE *fp) {
    PAGE pagina = le_pagina(raiz, fp); // Lê a página da raiz
    while (pagina.folha == 0) // Enquanto 'pagina' for uma página interna seguimos no laço
        pagina = le_pagina(pagina.rrn[0], fp); // Lê o menor filho da página atual

    return pagina; // Retorna a primeira folha
}
// ----------------------------------------------------------------------------------------------------------

// MÉTODOS INSERÇÃO -----------------------------------------------------------------------------------------

/* Função de inserção na árvore B+ */
int insere_chave(int raiz, FILE *fp, char chave[6], int rrn_registro) {
    PAGE pagina = busca_folha(raiz, fp, chave); // Busca a filha onde deve ser inserido a nova chave
    pagina.folha = 1; // Garante que a pafina procura é uma folha

    int i, j;
    if (pagina.quantidade_chaves == ORDEM - 1) // Árvore cheia -> split
        raiz = insere_split_folha(raiz, fp, pagina, chave, rrn_registro);
    else { // Caso contrário insere ordenadamente na folha com espaço livre
        i = 0;
        while (strcmp(chave, pagina.chaves[i]) >= 0 && i < pagina.quantidade_chaves && i < ORDEM - 2)
            i++; // Busca o índice a ser inserido

        for (j = pagina.quantidade_chaves; j > i; j--) { // Desloca os valores para a dirita do índice
            strcpy(pagina.chaves[j], pagina.chaves[j - 1]);
            pagina.rrn[j] = pagina.rrn[j - 1];
        }

        // Insere a nova chave naquela página
        strcpy(pagina.chaves[i], chave);
        pagina.rrn[i] = rrn_registro;
        pagina.quantidade_chaves++;

        escreve_pagina(fp, pagina); // Escreve a página no arquivo
    }

    return raiz; // Retorna o RRN da raíz
}

/* Função que splita página folha e promove uma chave para o pai */
int insere_split_folha(int raiz, FILE *fp, PAGE folha, char chave[6], int rrn_registro) {
    PAGE nova_folha;
    nova_folha.folha = 1; // Inicializa nova folha como 1
    nova_folha.quantidade_chaves = 0; // Inicializa nova folha com quantidade de chaves = 0
    nova_folha.rrn_pai = folha.rrn_pai; // Inicializa nova folha apontando para o mesmo pai de 'folha'

    char aux_chave[ORDEM][6];
    int aux_rrn[ORDEM], split, indice, i, j;

    indice = 0; // Busca o índice onde será inserido a chave
    while (indice < ORDEM - 1 && strcmp(folha.chaves[indice], chave) < 0)
        indice++;

    // Copia para um vetor auxiliar as chaves e RRN até o índice
    for (i = 0, j = 0; i < folha.quantidade_chaves; i++, j++) {
        if (j == indice)
            j++;
        strcpy(aux_chave[j], folha.chaves[i]);
        aux_rrn[j] = folha.rrn[i];
    }

    // Insere a nova chave e novo RRN nos vetores auxiliares
    strcpy(aux_chave[indice], chave);
    aux_rrn[indice] = rrn_registro;

    // Calcula onde será realizado o split
    if (ORDEM % 2 == 0)
        split = (ORDEM - 1) / 2 + 1; // Caso ordem seja ímpar pegamos o índice do meio
    else
        split = (ORDEM - 1) / 2; // Caso ordem seja par pegamos o índice à direita do meio

    // Copia para a folha esquerda os RRNs e chaves até o split
    folha.quantidade_chaves = 0;
    for (i = 0; i < split; i++) {
        strcpy(folha.chaves[i], aux_chave[i]);
        folha.rrn[i] = aux_rrn[i];
        folha.quantidade_chaves++;
    }

    // Copia para a folha direita os RRNs e chaves do split ao fim
    for (i = split, j = 0; i < ORDEM; i++, j++) {
        strcpy(nova_folha.chaves[j], aux_chave[i]);
        nova_folha.rrn[j] = aux_rrn[i];
        nova_folha.quantidade_chaves++;
    }

    nova_folha.rrn[ORDEM - 1] = folha.rrn[ORDEM - 1]; // Folha direita aponta para onde a folha esquerda apontava
    nova_folha.rrn_pagina = calcula_rrn(fp); // Calcula o RRN que será inserido a nova folha
    folha.rrn[ORDEM - 1] = nova_folha.rrn_pagina; // Folha esquerda aponta para a folha direita (sua irmã)

    // Preenche com ***** e -1 as chaves e RRNs das posições vazias
    for (i = folha.quantidade_chaves; i < ORDEM - 1; i++) {
        strcpy(folha.chaves[i], "*****");
        folha.rrn[i] = -1;
    }

    // Preenche com ***** e -1 as chaves e RRNs das posições vazias
    for (i = nova_folha.quantidade_chaves; i < ORDEM - 1; i++) {
        strcpy(nova_folha.chaves[i], "*****");
        nova_folha.rrn[i] = -1;
    }

    // Escreve ambas páginas no arquivo
    escreve_pagina(fp, folha);
    escreve_pagina(fp, nova_folha);

    // Retorna a inserção da chave promovida no pai
    return insere_pai(raiz, fp, nova_folha.chaves[0], folha, nova_folha);
}

/* Função para inserção de uma chave na página pai */
int insere_pai(int raiz, FILE *fp, char chave[6], PAGE esquerda, PAGE direita) {
    if (esquerda.rrn_pai == -1) // Verifica se o filho era a raiz para inserção de uma nova raiz
        return insere_nova_raiz(fp, chave, esquerda, direita);

    PAGE pai = le_pagina(esquerda.rrn_pai, fp); // Busca a página do pai
    int indice_esquerdo = 0; // Busca o índice onde o filho esquerdo está inserido
    while (indice_esquerdo <= pai.quantidade_chaves && pai.rrn[indice_esquerdo] != esquerda.rrn_pagina)
        indice_esquerdo++;

    if (pai.quantidade_chaves < ORDEM - 1) { // Pai está com espaço livre, inserimos na página

        // Desloca os valores para direita de 'indice_esquerdo'
        for (int i = pai.quantidade_chaves; i > indice_esquerdo; i--) {
            strcpy(pai.chaves[i], pai.chaves[i - 1]);
            pai.rrn[i + 1] = pai.rrn[i];
        }
        // Insere a chave promovida e o RRN do filho direito na posição correta
        strcpy(pai.chaves[indice_esquerdo], chave);
        pai.rrn[indice_esquerdo + 1] = direita.rrn_pagina;
        pai.quantidade_chaves++;

        escreve_pagina(fp, pai); // Escreve a página no arquivo

        return raiz; // Retorna o RRN da raiz
    }

    // Caso contrário será a página interna será splitada e será promovida uma nova chave
    return insere_split_interno(raiz, fp, pai, indice_esquerdo, chave, direita);
}

/* Função para inserção de uma nova raiz da árvore B+ */
int insere_nova_raiz(FILE *fp, char chave[6], PAGE esquerda, PAGE direita) {
    PAGE raiz;
    int i;

    raiz.rrn_pagina = calcula_rrn(fp); // Calcula o RRN que será inserida a nova raiz
    raiz.folha = 0; // Inicializa nova raiz como 0 (página interna)

    // Inicializa as chaves como ***** e RRNs como -1
    for (i = 0; i < ORDEM - 1; i++) {
        strcpy(raiz.chaves[i], "*****");
        raiz.rrn[i] = -1;
    }
    raiz.rrn[i] = -1;

    strcpy(raiz.chaves[0], chave); // Insere a chave na posição 0
    raiz.rrn[0] = esquerda.rrn_pagina; // Insere o RRN do filho esquerdo no índice 0
    raiz.rrn[1] = direita.rrn_pagina;  // Insere o RRN do filho direito no índice 1

    raiz.quantidade_chaves = 1;
    raiz.rrn_pai = -1;  // Inicializa pai da raiz como -1 (indicativo que é raíz)

    // Aponta o pai de ambos os filho esquerdo e dirteito para nova raiz
    esquerda.rrn_pai = raiz.rrn_pagina;
    direita.rrn_pai = raiz.rrn_pagina;

    // Escreve as páginas no arquivo
    escreve_pagina(fp, raiz);
    escreve_pagina(fp, esquerda);
    escreve_pagina(fp, direita);

    return raiz.rrn_pagina; // Retorna o RRN da nova raiz
}

/* Função que splita página interna e promove uma chave para o pai */
int insere_split_interno(int raiz, FILE *fp, PAGE pai, int indice_esquerdo, char chave[6], PAGE direita) {
    int split, i, j, aux_rrn[ORDEM + 1];
    char aux_chave[ORDEM][6], chave_p[6];
    PAGE nova_pagina, filho;

    // Guarda os RRNs em um vetor auxiliar
    for (i = 0, j = 0; i < pai.quantidade_chaves + 1; i++, j++) {
        if (j == indice_esquerdo + 1)
            j++;
        aux_rrn[j] = pai.rrn[i];
    }

    // Guarda as chaves em um vetor auxiliar
    for (i = 0, j = 0; i < pai.quantidade_chaves; i++, j++) {
        if (j == indice_esquerdo)
            j++;
        strcpy(aux_chave[j], pai.chaves[i]);
    }

    // Posiciona a chave promovida e RRN nas posições corretas dos vetores auxioliares
    strcpy(aux_chave[indice_esquerdo], chave);
    aux_rrn[indice_esquerdo + 1] = direita.rrn_pagina;

    // Calcula onde será realizado o split
    if (ORDEM % 2 == 0)
        split = (ORDEM - 1) / 2 + 1; // Caso ordem seja ímpar pegamos o índice do meio
    else
        split = (ORDEM - 1) / 2; // Caso ordem seja par pegamos o índice à direita do meio


    nova_pagina.folha = 0; // Inicializa 'nova_pagina' como pagina interna
    nova_pagina.quantidade_chaves = 0; // Inicializa 'nova_pagina' com quantidade de chaves = 0

    // Copia para 'pai' (irmão da esquerda) os valores dos vetores auxiliares até o split
    pai.quantidade_chaves = 0;
    for (i = 0; i < split; i++) {
        strcpy(pai.chaves[i], aux_chave[i]);
        pai.rrn[i] = aux_rrn[i];
        pai.quantidade_chaves++;
    }
    pai.rrn[i] = aux_rrn[i];

    strcpy(chave_p, aux_chave[split]); // Pega a chave a ser promovida após split

    // Copia para a nova página (irmão da direita) os valores dos vetores auxiliares de split ao fim
    for (++i, j = 0; i < ORDEM; i++, j++) {
        strcpy(nova_pagina.chaves[j], aux_chave[i]);
        nova_pagina.rrn[j] = aux_rrn[i];
        nova_pagina.quantidade_chaves++;
    }
    nova_pagina.rrn[j] = aux_rrn[i];

    nova_pagina.rrn_pai = pai.rrn_pai; // Aponta o pai da nova página para o rrn de 'pai'
    nova_pagina.rrn_pagina = calcula_rrn(fp); // Calcula o RRN que será inserido a nova página

    // Preenche com ***** e -1 as chaves e RRNs das posições vazias
    for (i = pai.quantidade_chaves; i < ORDEM - 1; i++) {
        strcpy(pai.chaves[i], "*****");
        pai.rrn[i + 1] = -1;
    }

    // Preenche com ***** e -1 as chaves e RRNs das posições vazias
    for (i = nova_pagina.quantidade_chaves; i < ORDEM - 1; i++) {
        strcpy(nova_pagina.chaves[i], "*****");
        nova_pagina.rrn[i + 1] = -1;
    }

    escreve_filho(fp, nova_pagina); // Escreve os filhos da nova página

    escreve_pagina(fp, pai); // Escreve página 'pai' no arquivo
    escreve_pagina(fp, nova_pagina); // Escreve nova página no arquivo

    // Retorna a inserção da chave promovida no pai
    return insere_pai(raiz, fp, chave_p, pai, nova_pagina);
}
// ----------------------------------------------------------------------------------------------------------

// MÉTODOS DE REMOÇÃO ---------------------------------------------------------------------------------------

/* Primeira função de remover. Procura a página folha em que a chave está ou deveria estar */
int remover(int raiz, FILE *fp, char chave[6]) {
    PAGE folha = busca_folha(raiz, fp, chave); // Busca a folha que a chave será removida
    int rrn;

    // Caso a chave não seja encontrada, retorna a raiz normalmente
    if ((rrn = busca_registro(raiz, fp, chave)) == -1) {
        printf("Chave não encontrada!\n");
        return raiz;
    }

    // Remove a chave e faz os ajustes de acordo
    raiz = remove_chave(raiz, fp, folha, chave, rrn);
    return raiz;
}

/* Recebe a página em ocorrerá a remoção da chave, fazendo o tratamento da página com redistribuição, e caso impossibilitado,
 * concatenação, caso a quantidade de chaves fique menor que a quantidade mínima */
int remove_chave(int raiz, FILE *fp, PAGE pagina, char chave[6], int rrn) {
    int chaves_minimas;
    int irmao_esquerda, irmao_direita = 0, indice_p;
    PAGE pai, irmao_e, irmao_d;
    char chave_p[6];

    irmao_e.rrn_pai = irmao_d.rrn_pai = -1;
    pagina = remove_chave_no(fp, pagina, chave, rrn); // Remove a chave na página

    if (raiz == pagina.rrn_pagina)
        return ajusta_raiz(pagina, fp); // Remoção ocorre na raíz

    // Remoção de um nó não-raiz
    chaves_minimas = pagina.folha ? calcula_quant_minima(ORDEM - 1) : calcula_quant_minima(ORDEM);

    // Primeiro caso: não tem menos que a quantidade mínima de chaves
    if (pagina.quantidade_chaves >= chaves_minimas)
        return raiz; // Retorna sem nenhum tratamento

    // Segundo caso: quantidade de nós fica menor que o limite

    pai = le_pagina(pagina.rrn_pai, fp); // Lê o pai da página em questão
    for (irmao_esquerda = 0; irmao_esquerda <= pai.quantidade_chaves; irmao_esquerda++) { // Busca os índices dos irmãos
        if (pai.rrn[irmao_esquerda] == pagina.rrn_pagina) {
            irmao_direita = irmao_esquerda + 1;
            irmao_esquerda--;
            break;
        }
    }

    if (irmao_esquerda != -1)
        irmao_e = le_pagina(pai.rrn[irmao_esquerda], fp); // Lê o irmão esquerdo
    if (irmao_direita <= pai.quantidade_chaves)
        irmao_d = le_pagina(pai.rrn[irmao_direita], fp); // Lê o irmão direito

    // Redistribuicao
    if (irmao_d.rrn_pai != -1 && irmao_d.quantidade_chaves > chaves_minimas) // Damos prioridade para o irmâo direito
        return redistribuicao(raiz, fp, pai, pagina, irmao_d, irmao_direita - 1, 1);
    else if (irmao_e.rrn_pai != -1 && irmao_e.quantidade_chaves > chaves_minimas) // Caso não seja possível com o direito, testamos o esquerdo
        return redistribuicao(raiz, fp, pai, pagina, irmao_e, irmao_esquerda, 0);

    // De último caso aplicamos a concatenação
    indice_p = irmao_d.rrn_pai != -1 ? irmao_direita - 1 : irmao_esquerda; // Pegamos o índice do pai
    strcpy(chave_p, pai.chaves[indice_p]); // Pegamos a chave do pai
    return concatenar(raiz, fp, pagina, pai, irmao_e, irmao_d, indice_p, chave_p);
}

/* Procura a chave numa página e a remove, movendo os RRNs e chaves para continuarem ordenados corretamente */
PAGE remove_chave_no(FILE *fp, PAGE pagina, char chave[6], int rrn) {
    int i;

    // Procura a posição da chave no vetor de chaves da página e a remove
    i = 0;
    while (strcmp(pagina.chaves[i], chave) != 0)
        i++;
    for (++i; i <= pagina.quantidade_chaves; i++)
        strcpy(pagina.chaves[i - 1], pagina.chaves[i]);

    i = 0;
    while (pagina.rrn[i] != rrn)
        i++;

    // Move o restante das chaves
    if (pagina.folha) { // Se a página for folha, o vetor de RRNs possui a última posição apontando para o próximo nó folha
        for (++i; i < pagina.quantidade_chaves; i++)
            pagina.rrn[i - 1] = pagina.rrn[i];

        pagina.quantidade_chaves--;
        for (i = pagina.quantidade_chaves; i < ORDEM - 1; i++)
            pagina.rrn[i] = -1;
    } else { // Se a página não for folha, o vetor de RRNs possui apenas referências a outras chaves
        for (++i; i < pagina.quantidade_chaves + 1; i++)
            pagina.rrn[i - 1] = pagina.rrn[i];

        pagina.quantidade_chaves--;
        for (i = pagina.quantidade_chaves + 1; i < ORDEM; i++)
            pagina.rrn[i] = -1;
    }

    for (i = pagina.quantidade_chaves; i < ORDEM - 1; i++) { // Apaga as últimas posições do vetor de chaves
        strcpy(pagina.chaves[i], "*****");
    }
    if (pagina.quantidade_chaves == 0) // Apaga a página caso ela não tenha mais chaves
        pagina.folha = 2;

    escreve_pagina(fp, pagina); // Reescreve a página no arquivo
    return pagina;
}

/* Caso especial de remoção de chave da raiz */
int ajusta_raiz(PAGE raiz, FILE *fp) {
    PAGE nova_raiz = raiz;

    if (raiz.quantidade_chaves > 0) // Há chaves na raiz. Não necessita de mais reajustes
        return raiz.rrn_pagina;

    if (raiz.rrn[0] == -1) // Caso em que não há mais nenhuma página na árvore. A árvore inteira foi removida
        return -1;

    // Último caso: há uma concatenação entre as duas páginas filhas da raiz, em que uma está vazia
    nova_raiz = le_pagina(raiz.rrn[0], fp); // Lê a página e a transforma em raiz
    nova_raiz.rrn_pai = -1;
    escreve_pagina(fp, nova_raiz); // Reescreve no arquivo
    return nova_raiz.rrn_pagina; // Retorna o RRN dessa página como RRN da raiz
}

/* Função de redistribuiçõa dos nós de duas páginas. Recebe uma flag 'direita' para indicar se a redistribuição é com o
 * irmão da direita ou da esquerda */
int redistribuicao(int raiz, FILE *fp, PAGE pai, PAGE pagina, PAGE irmao, int indice_p, int direita) {
    char aux_chave[2 * ORDEM][6];
    int i, j, aux_rrn[2 * ORDEM], split, total, flag = 0;

    for (i = 0; i < 2 * ORDEM; i++) { // Inicializa os vetores auxiliares
        for (j = 0; j < 5; j++)
            aux_chave[i][j] = '*';
        aux_chave[i][5] = '\0';
        aux_rrn[i] = -1;
    }

    if (direita == 1) { // Página vem primeiro
        // Coloca as chaves e RRNs nos vetores auxiliares
        for (i = 0; i < pagina.quantidade_chaves; i++) {
            strcpy(aux_chave[i], pagina.chaves[i]);
            aux_rrn[i] = pagina.rrn[i];
        }

        if (irmao.folha == 0) // Se não for folha, o pai participa da redistribuição
            strcpy(aux_chave[i], pai.chaves[indice_p]);

        if (pagina.rrn[i] != -1) {
            aux_rrn[i] = pagina.rrn[i];
            i++;
        }

        for (j = 0; j < irmao.quantidade_chaves; i++, j++) {
            strcpy(aux_chave[i], irmao.chaves[j]);
            aux_rrn[i] = irmao.rrn[j];
        }

        aux_rrn[i] = irmao.rrn[j];
        pagina.folha = irmao.folha;

    } else { // Irmão vem primeiro
        // Coloca as chaves e RRNs nos vetores auxiliares
        for (i = 0; i < irmao.quantidade_chaves; i++) {
            strcpy(aux_chave[i], irmao.chaves[i]);
            aux_rrn[i] = irmao.rrn[i];
        }
        aux_rrn[i] = irmao.rrn[i];

        if (irmao.folha == 0) // Se não for folha, o pai participa da redistribuição
            strcpy(aux_chave[i], pai.chaves[indice_p]);

        i++;
        for (j = 0; j < pagina.quantidade_chaves; i++, j++) {
            strcpy(aux_chave[i], pagina.chaves[j]);
            aux_rrn[i] = pagina.rrn[j];
        }
        aux_rrn[i] = pagina.rrn[j];

        pagina.folha = irmao.folha;

        // Troca os RRNs para manter menor à esquerda e maior à direita
        i = irmao.rrn_pagina;
        irmao.rrn_pagina = pagina.rrn_pagina;
        pagina.rrn_pagina = i;

        if (pagina.folha == 1) { // Quando a página for folha, trocamos os apontamentos para próxima folha
            i = irmao.rrn[ORDEM - 1];
            irmao.rrn[ORDEM - 1] = pagina.rrn[ORDEM - 1];
            pagina.rrn[ORDEM - 1] = i;
        }

    }

    // Se for nó interno, terá uma chave a mais por copiar o pai
    total = flag == 0 ? pagina.quantidade_chaves + irmao.quantidade_chaves + 1 : pagina.quantidade_chaves +
                                                                                 irmao.quantidade_chaves;
    // Posição que vai ser promovida
    split = total / 2;

    // Reescreve valores da página
    pagina.quantidade_chaves = 0;
    for (i = 0; i < split; i++) {
        strcpy(pagina.chaves[i], aux_chave[i]);
        pagina.rrn[i] = aux_rrn[i];
        pagina.quantidade_chaves++;
    }
    if (!irmao.folha)
        pagina.rrn[i] = aux_rrn[i];

    // Preenche com ***** e -1 as chaves e RRNs das posições vazias
    j = irmao.folha == 1 ? pagina.quantidade_chaves : pagina.quantidade_chaves + 1;
    for (i = pagina.quantidade_chaves; i < ORDEM - 1; i++) {
        strcpy(pagina.chaves[i], "*****");
        pagina.rrn[j] = -1;
    }

    // Reescreve valores do irmão
    irmao.quantidade_chaves = 0;
    i = irmao.folha == 1 ? split : split + 1;
    for (j = 0; i < total; i++, j++) {
        strcpy(irmao.chaves[j], aux_chave[i]);
        irmao.rrn[j] = aux_rrn[i];
        if (irmao.chaves[j][0] != '*')
            irmao.quantidade_chaves++;
    }
    if (!irmao.folha)
        irmao.rrn[j] = aux_rrn[i];

    j = irmao.folha == 1 ? irmao.quantidade_chaves : irmao.quantidade_chaves + 1;
    for (i = irmao.quantidade_chaves; i < ORDEM - 1; i++, j++) {
        strcpy(irmao.chaves[i], "*****");
        irmao.rrn[j] = -1;
    }

    // Reescreve as páginas nos arquivos
    strcpy(pai.chaves[indice_p], aux_chave[split]);
    escreve_pagina(fp, pai);
    escreve_pagina(fp, pagina);
    escreve_pagina(fp, irmao);

    // Reescreve os filhos, arrumando o RRN do pai de cada uma delas
    if (pagina.folha == 0)
        escreve_filho(fp, pagina);
    if (irmao.folha == 0)
        escreve_filho(fp, irmao);

    return raiz;
}

/* Função de concatenação: caso a página irmã não tenha chaves o suficiente para dividir entre ambas, juntar as
 * duas páginas */
int concatenar(int raiz, FILE *fp, PAGE pagina, PAGE pai, PAGE irmao_e, PAGE irmao_d, int indice_p, char chave_p[6]) {
    int i, j, flag = 0;

    if ((irmao_e.rrn_pai != -1 && irmao_d.rrn_pai != -1 && irmao_e.quantidade_chaves < irmao_d.quantidade_chaves)
        || irmao_e.rrn_pai != -1) {
        flag = 1; // Confirmamos com qual irmão será feita a concatenação
    }

    if (flag) { // Concatena irmão esquerdo: página -> irmão
        // Primeiro passo: concatena as chaves
        if (!irmao_e.folha) { // Copiando o pai para o vetor, caso a página seja interna.
            strcpy(irmao_e.chaves[irmao_e.quantidade_chaves], chave_p);
            irmao_e.quantidade_chaves++;
            irmao_e.rrn[irmao_e.quantidade_chaves] = pagina.rrn[0];
        }
        // Copia as chaves e RRNs da página para as últimas posições dos vetores de chaves e de RRNs do irmão da esquerda.
        for (i = irmao_e.quantidade_chaves, j = 0; j < pagina.quantidade_chaves; i++, j++) {
            strcpy(irmao_e.chaves[i], pagina.chaves[j]);
            irmao_e.rrn[i] = pagina.rrn[j];
            irmao_e.quantidade_chaves++;
        }

        // Faz o irmão apontar para a próxima folha corretamente (a mesma que página apontava)
        irmao_e.rrn[ORDEM - 1] = pagina.rrn[ORDEM - 1];
        escreve_pagina(fp, irmao_e); // Reescreve o irmão esquerdo

        if (irmao_e.folha == 0) // Reescreve os filhos, arrumando a referência ao nó pai
            escreve_filho(fp, irmao_e);

        return remove_chave(raiz, fp, pai, chave_p, pagina.rrn_pagina); // Remove a chave separadora do nó pai.

    } else { // Concatena irmão direito: irmão -> página
        if (!irmao_d.folha) { // Copiando o pai (separador) para o vetor, caso a página seja interna
            strcpy(pagina.chaves[pagina.quantidade_chaves], chave_p);
            pagina.quantidade_chaves++;
            i = ORDEM - 1;
            while (irmao_d.rrn[i] == -1)
                i--;
            pagina.rrn[pagina.quantidade_chaves] = irmao_d.rrn[i--];
        }
        // Copia as chaves e RRNs do irmão da direita para as últimas posições dos vetores de chaves e de RRNs da página
        for (i = pagina.quantidade_chaves, j = 0; j <= irmao_d.quantidade_chaves; i++, j++) {
            strcpy(pagina.chaves[i], irmao_d.chaves[j]);
            pagina.rrn[i] = irmao_d.rrn[j];
            pagina.quantidade_chaves++;
        }

        // Caso a página tenha sido apagada, volta a ser o mesmo tipo que o irmão
        pagina.folha = irmao_d.folha;
        pagina.rrn[ORDEM - 1] = irmao_d.rrn[ORDEM - 1]; // Página aponta para a próxima página (a que antes irmão apontava)
        irmao_d.folha = 2; // Apaga irmão

        // Reescreve ambas no arquivo
        escreve_pagina(fp, pagina);
        escreve_pagina(fp, irmao_d);

        if (pagina.folha == 0) // Reescreve os filhos, arrumando a referência ao nó pai
            escreve_filho(fp, pagina);

        return remove_chave(raiz, fp, pai, chave_p, irmao_d.rrn_pagina); // Remove a chave separadora do nó pai.
    }
}
// ----------------------------------------------------------------------------------------------------------

// MÉTODOS IMPRESSÃO ----------------------------------------------------------------------------------------

/* Função para caminhar sequencialmente pelas folhas da árvore B+ */
void em_ordem_b(int raiz, FILE *fp) {
    PAGE pagina = busca_primeira_folha(raiz, fp); // Busca a primeira folha (mais a esquerda)
    int rrn_folha = pagina.rrn_pagina, rrn, i;

    while (rrn_folha != -1) { // Enquanto RRN da folha for != -1 continuamos o laço
        i = 0;
        pagina = le_pagina(rrn_folha, fp); // Lê a folha em 'rrn_folha'
        rrn = pagina.rrn[0]; // Pega o primeiro RRN
        while (rrn != -1 && i < ORDEM - 1) { // Caminhamos até 'rrn' ser -1 (sem apontamento) ou até o fim dos RRNs
            printf(" %d ", rrn); // Printa o RRN no terminal
            rrn = pagina.rrn[++i]; // Pega o próximo RRN
        }
        rrn_folha = pagina.rrn[ORDEM - 1]; // Ao fim do laço, pegamos o apontamento da próxima folha
    }
}
// ----------------------------------------------------------------------------------------------------------
