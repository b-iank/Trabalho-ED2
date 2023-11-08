#include "arvore_bm.h"

// MÉTODOS ÚTIL ---------------------------------------------------------------------------------------------

void escreve_pagina_vazia(FILE *fp) {
    fprintf(fp, "%s", "1@"); //Escreve folha;
    int i;
    for (i = 0; i < ORDEM - 1; i++)
        fprintf(fp, "*****#");
    putc('@', fp);
    for (i = 0; i < ORDEM; i++)
        fprintf(fp, "**#");
    fprintf(fp, "@**@");
}

//Função para ler uma página do arquivo de índice
PAGE le_pagina(int rrn, FILE *fp) {
    PAGE pagina;
    char temp[3];

    fseek(fp, rrn * TAM_REGISTRO, SEEK_SET);

    // Lê se a página é uma folha
    fscanf(fp, "%d", &pagina.folha);
    fgetc(fp);

    int qnt = 0;
    for (int i = 0; i < ORDEM - 1; i++) { //Laço para ler os campos de chaves e escrever na struct
        fscanf(fp, " %[^#]s", pagina.chaves[i]);
        pagina.chaves[i][5] = '\0';
        fgetc(fp);

        if (pagina.chaves[i][0] != '*')
            qnt++;
    }

    pagina.quantidade_chaves = qnt; //Quantiade de páginas lidas no laço anterior
    fgetc(fp);

    for (int i = 0; i < ORDEM; i++) { //Laço para ler os rrn dos filhos da página ou para o arquivo de dados
        fscanf(fp, "%[^#]s", temp);
        temp[2] = '\0';
        fgetc(fp);

        if (temp[0] != '*') //Caso RRN esteja preenchido
            pagina.rrn[i] = atoi(temp); //Salva na struct seu número
        else
            pagina.rrn[i] = -1; //Caso contrário, salva -1 para representar campo vazio
    }

    fgetc(fp);
    fscanf(fp, "%[^@]", temp);
    temp[2] = '\0';
    if (temp[0] == '*')
        pagina.rrn_pai = -1;
    else
        pagina.rrn_pai = atoi(temp);
    pagina.rrn_pagina = rrn;
    return pagina; //Retorna a página lida
}

int calcula_rrn(FILE *fp) {
    fseek(fp, 0, SEEK_END);
    return ftell(fp) / TAM_REGISTRO;
}

void escreve_pagina(FILE *fp, PAGE pagina) {
    fseek(fp, pagina.rrn_pagina * TAM_REGISTRO, SEEK_SET);

    fprintf(fp, "%d", pagina.folha);
    fputc('@', fp);

    int i;
    for (i = 0; i < ORDEM - 1; i++)
        fprintf(fp, "%s#", pagina.chaves[i]);
    putc('@', fp);

    for (i = 0; i < ORDEM; i++) {
        if (pagina.rrn[i] > 9)
            fprintf(fp, "%d#", pagina.rrn[i]);
        else if (pagina.rrn[i] == -1)
            fprintf(fp, "%s", "**#");
        else
            fprintf(fp, "0%d#", pagina.rrn[i]);
    }
    putc('@', fp);
    if (pagina.rrn_pai == -1)
        fprintf(fp, "**@");
    else if (pagina.rrn_pai > 9)
        fprintf(fp, "%d@", pagina.rrn_pai);
    else
        fprintf(fp, "0%d@", pagina.rrn_pai);
}

PAGE busca_primeira_folha(int raiz, FILE *fp) {
    PAGE pagina = le_pagina(raiz, fp);
    while (!pagina.folha)
        pagina = le_pagina(pagina.rrn[0], fp);

    return pagina;
}
// ----------------------------------------------------------------------------------------------------------

PAGE busca_folha(int raiz, FILE *fp, char chave[6]) {
    PAGE pagina = le_pagina(raiz, fp);
    int rrn = raiz, rrn_pai = -1;

    int i;
    while (!pagina.folha) {
        i = 0;
        while (i < pagina.quantidade_chaves) {
            if (strcmp(chave, pagina.chaves[i]) >= 0)
                i++;
            else
                break;
        }
        rrn_pai = rrn;
        rrn = pagina.rrn[i];
        pagina = le_pagina(rrn, fp);
    }
    pagina.rrn_pagina = rrn;
    pagina.rrn_pai = rrn_pai;
    return pagina;
}

int busca_registro(int rrn_raiz, FILE *fp, char chave[6]) {
    PAGE pagina = busca_folha(rrn_raiz, fp, chave);

    int i;
    for (i = 0; i < pagina.quantidade_chaves; i++) {
        if (strcmp(pagina.chaves[i], chave) == 0)
            return pagina.rrn[i];
    }

    return -1;
}

int insere_chave(int raiz, FILE *fp, char chave[6], int rrn_registro) {
    PAGE pagina = busca_folha(raiz, fp, chave);
    int i, j;
    if (pagina.quantidade_chaves == ORDEM - 1) { //Árvore cheia -> split
        raiz = insere_split(raiz, fp, pagina, chave, rrn_registro);
    } else {
        i = 0;
        while (i < pagina.quantidade_chaves && i < ORDEM - 2) {
            if (strcmp(chave, pagina.chaves[i]) < 0)
                break;
            i++;
        }

        for (j = pagina.quantidade_chaves; j > i; j--) {
            strcpy(pagina.chaves[j], pagina.chaves[j - 1]);
            pagina.rrn[j] = pagina.rrn[j - 1];
        }

        strcpy(pagina.chaves[i], chave);
        pagina.rrn[i] = rrn_registro;
        pagina.quantidade_chaves++;

        escreve_pagina(fp, pagina);
    }

    return raiz;
}

int insere_split(int raiz, FILE *fp, PAGE pagina_folha, char chave[6], int rrn_registro) {
    PAGE nova_pagina;
    nova_pagina.folha = 1;
    nova_pagina.quantidade_chaves = 0;
    char aux_chave[ORDEM][6], chave_p[6];
    int aux_rrn[ORDEM], split, indice, i, j;

    indice = 0;
    while (indice < ORDEM - 1 && strcmp(pagina_folha.chaves[indice], chave) < 0)
        indice++;

    for (i = 0, j = 0; i < pagina_folha.quantidade_chaves; i++, j++) {
        if (j == indice)
            j++;
        strcpy(aux_chave[j], pagina_folha.chaves[i]);
        aux_rrn[j] = pagina_folha.rrn[i];
    }

    strcpy(aux_chave[indice], chave);
    aux_rrn[indice] = rrn_registro;

    if ((ORDEM - 1) % 2 == 0)
        split = (ORDEM - 1) / 2;
    else
        split = (ORDEM - 1) / 2 + 1;

    pagina_folha.quantidade_chaves = 0;
    for (i = 0; i < split; i++) {
        strcpy(pagina_folha.chaves[i], aux_chave[i]);
        pagina_folha.rrn[i] = aux_rrn[i];
        pagina_folha.quantidade_chaves++;
    }

    for (i = split, j = 0; i < ORDEM; i++, j++) {
        strcpy(nova_pagina.chaves[j], aux_chave[i]);
        nova_pagina.rrn[j] = aux_rrn[i];
        nova_pagina.quantidade_chaves++;
    }

    nova_pagina.rrn[ORDEM - 1] = pagina_folha.rrn[ORDEM - 1];
    nova_pagina.rrn_pagina = calcula_rrn(fp);
    pagina_folha.rrn[ORDEM - 1] = nova_pagina.rrn_pagina;

    for (i = pagina_folha.quantidade_chaves; i < ORDEM - 1; i++) {
        strcpy(pagina_folha.chaves[i], "*****");
        pagina_folha.rrn[i] = -1;
    }

    for (i = nova_pagina.quantidade_chaves; i < ORDEM - 1; i++) {
        strcpy(nova_pagina.chaves[i], "*****");
        nova_pagina.rrn[i] = -1;
    }


    nova_pagina.rrn_pai = pagina_folha.rrn_pai;
    strcpy(chave_p, nova_pagina.chaves[0]);

    escreve_pagina(fp, pagina_folha);
    escreve_pagina(fp, nova_pagina);

    return insere_pai(raiz, fp, chave_p, pagina_folha, nova_pagina);
}

int insere_pai(int raiz, FILE *fp, char chave[6], PAGE esquerda, PAGE direita) {
    if (esquerda.rrn_pai == -1)
        return insere_nova_raiz(fp, chave, esquerda, direita);

    PAGE pai = le_pagina(esquerda.rrn_pai, fp);
    int indice_esquerdo = 0;
    while (indice_esquerdo <= pai.quantidade_chaves && pai.rrn[indice_esquerdo] != esquerda.rrn_pagina)
        indice_esquerdo++;

    if (pai.quantidade_chaves < ORDEM - 1)
        return insere_pagina(raiz, fp, pai, indice_esquerdo, chave, direita);

    return insere_pagina_split(raiz, fp, pai, indice_esquerdo, chave, direita);
}

int insere_nova_raiz(FILE *fp, char chave[6], PAGE esquerda, PAGE direita) {
    PAGE raiz;
    int i;

    raiz.rrn_pagina = calcula_rrn(fp);
    raiz.folha = 0;
    strcpy(raiz.chaves[0], chave);
    for (i = 1; i < ORDEM - 1; i++)
        strcpy(raiz.chaves[i], "*****");
    raiz.rrn[0] = esquerda.rrn_pagina;
    raiz.rrn[1] = direita.rrn_pagina;
    for (i = 2; i < ORDEM; i++)
        raiz.rrn[i] = -1;
    raiz.quantidade_chaves = 1;
    raiz.rrn_pai = -1;

    esquerda.rrn_pai = raiz.rrn_pagina;
    direita.rrn_pai = raiz.rrn_pagina;

    escreve_pagina(fp, raiz);
    escreve_pagina(fp, esquerda);
    escreve_pagina(fp, direita);

    return raiz.rrn_pagina;
}

int insere_pagina(int raiz, FILE *fp, PAGE pai, int indice_esquerdo, char chave[6], PAGE direita) {
    int i;
    for (i = pai.quantidade_chaves; i > indice_esquerdo; i--) {
        strcpy(pai.chaves[i], pai.chaves[i - 1]);
        pai.rrn[i + 1] = pai.rrn[i];
    }
    strcpy(pai.chaves[indice_esquerdo], chave);
    pai.rrn[indice_esquerdo + 1] = direita.rrn_pagina;
    pai.quantidade_chaves++;
    escreve_pagina(fp, pai);
    return raiz;
}

int insere_pagina_split(int raiz, FILE *fp, PAGE pai, int indice_esquerdo, char chave[6], PAGE direita) {
    int split, i, j, aux_rrn[ORDEM + 1];
    char aux_chave[ORDEM][6], chave_p[6];
    PAGE nova_pagina, filho;

    for (i = 0, j = 0; i < pai.quantidade_chaves + 1; i++, j++) {
        if (j == indice_esquerdo + 1)
            j++;
        aux_rrn[j] = pai.rrn[i];
    }

    for (i = 0, j = 0; i < pai.quantidade_chaves; i++, j++) {
        if (j == indice_esquerdo)
            j++;
        strcpy(aux_chave[j], pai.chaves[i]);
    }

    strcpy(aux_chave[indice_esquerdo], chave);
    aux_rrn[indice_esquerdo + 1] = direita.rrn_pagina;

    if ((ORDEM - 1) % 2 == 0)
        split = (ORDEM - 1) / 2;
    else
        split = (ORDEM - 1) / 2 + 1;

    nova_pagina.folha = 0;
    nova_pagina.quantidade_chaves = 0;

    pai.quantidade_chaves = 0;
    for (i = 0; i < split; i++) {
        strcpy(pai.chaves[i], aux_chave[i]);
        pai.rrn[i] = aux_rrn[i];
        pai.quantidade_chaves++;
    }
    pai.rrn[i] = aux_rrn[i];

    strcpy(chave_p, aux_chave[split]);
    for (++i, j = 0; i < ORDEM; i++, j++) {
        strcpy(nova_pagina.chaves[j], aux_chave[i]);
        nova_pagina.rrn[j] = aux_rrn[i];
        nova_pagina.quantidade_chaves++;
    }
    nova_pagina.rrn[j] = aux_rrn[i];
    nova_pagina.rrn_pai = pai.rrn_pai;
    nova_pagina.rrn_pagina = calcula_rrn(fp);

    for (i = pai.quantidade_chaves; i < ORDEM - 1; i++) {
        strcpy(pai.chaves[i], "*****");
        pai.rrn[i + 1] = -1;
    }
    for (i = nova_pagina.quantidade_chaves; i < ORDEM - 1; i++) {
        strcpy(nova_pagina.chaves[i], "*****");
        nova_pagina.rrn[i + 1] = -1;
    }


    for (i = 0; i <= nova_pagina.quantidade_chaves; i++) {
        filho = le_pagina(nova_pagina.rrn[i], fp);
        filho.rrn_pai = nova_pagina.rrn_pagina;
        escreve_pagina(fp, filho);
    }

    escreve_pagina(fp, pai);
    escreve_pagina(fp, nova_pagina);
    return insere_pai(raiz, fp, chave_p, pai, nova_pagina);
}

// MÉTODOS REMOÇÃO ------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------------

// MÉTODOS IMPRESSÃO ----------------------------------------------------------------------------------------
void em_ordem(int raiz, FILE *fp) {
    PAGE pagina = busca_primeira_folha(raiz, fp);
    int rrn_folha = pagina.rrn_pagina, rrn, i;

    while (rrn_folha != -1) {
        i = 0;
        pagina = le_pagina(rrn_folha, fp);
        rrn = pagina.rrn[0];
        while (rrn != -1 && i < ORDEM - 1) {
            printf(" %d ", rrn);
            rrn = pagina.rrn[++i];
        }
        rrn_folha = pagina.rrn[ORDEM - 1];
    }
}
// ----------------------------------------------------------------------------------------------------------
