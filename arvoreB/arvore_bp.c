#include "arvore_bp.h"

//Função para ler uma página do arquivo de índice
PAGE le_pagina(int rrn, FILE *fp) {
    PAGE pag;
    int folha;
    char temp[3];

    fseek(fp, rrn * TAM_REGISTRO, SEEK_SET);

    // Lê se a página é uma folha
    fscanf(fp, "%d", &pag.folha);
    fgetc(fp);

    int qnt = 0;
    for (int i = 0; i < ORDEM - 1; i++) { //Laço para ler os campos de chaves e escrever na struct
        fscanf(fp, " %[^#]s", pag.chaves[i]);
        pag.chaves[i][5] = '\0';
        fgetc(fp);

        if (pag.chaves[i][0] != '*')
            qnt++;
    }

    pag.quantidade_chaves = qnt; //Quantiade de páginas lidas no laço anterior
    fgetc(fp);

    for (int i = 0; i < ORDEM; i++) { //Laço para ler os rrn dos filhos da página ou para o arquivo de dados
        fscanf(fp, "%[^#]s", temp);
        temp[2] = '\0';
        fgetc(fp);

        if (temp[0] != '*') //Caso RRN esteja preenchido
            pag.rrn[i] = atoi(temp); //Salva na struct seu número
        else
            pag.rrn[i] = -1; //Caso contrário, salva -1 para representar campo vazio
    }

    return pag;
}

int busca_registro(int rrn_raiz, FILE *fp, char chave[6]) {

    PAGE pag = le_pagina(rrn_raiz, fp);

    int i;
    while (!pag.folha) {
        i = 0;
        while (i < pag.quantidade_chaves) {
            if (strcmp(chave, pag.chaves[i]) >= 0)
                i++;
            else
                break;
        }
        pag = le_pagina(pag.rrn[i], fp);
    }

    for (i = 0; i < pag.quantidade_chaves; i++) {
        if (strcmp(pag.chaves[i], chave) == 0)
            return pag.rrn[i];
    }

    return -1;
}