#include "arvore_bp.h"


void escreve_pagina_vazia(FILE *fp) {
    fprintf(fp, "%s", "1@"); //Escreve folha;
    int i;
    for (i = 0; i < ORDEM-1; i++)
        fprintf(fp, "%s", "*****#");
    putc('@', fp);
    for (i = 0; i < ORDEM; i++)
        fprintf(fp, "%s", "**#");
    putc('@', fp);
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

    return pagina; //Retorna a página lida
}

PAGE busca_folha(int raiz, FILE *fp, char chave[6]) {

    PAGE pagina = le_pagina(raiz, fp);
    int rrn = 0;

    int i;
    while (!pagina.folha) {
        i = 0;
        while (i < pagina.quantidade_chaves) {
            if (strcmp(chave, pagina.chaves[i]) >= 0)
                i++;
            else
                break;
        }
        rrn = pagina.rrn[i];
        pagina = le_pagina(rrn, fp);
    }
    pagina.rrn_pagina = rrn;
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

int insere_chave(int raiz, FILE *fp, char chave[6], int rrn) {

    PAGE pagina = busca_folha(raiz, fp, chave);

    if (pagina.quantidade_chaves == ORDEM - 1) { //Árvore cheia -> split
        //raiz = split(raiz, fp, chave, rrn);
    } else {
        strcpy(pagina.chaves[pagina.quantidade_chaves], chave);
        pagina.rrn[pagina.quantidade_chaves] = rrn;
        pagina.quantidade_chaves++;
        escreve_pagina(fp, pagina);
    }

    return raiz;
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
}