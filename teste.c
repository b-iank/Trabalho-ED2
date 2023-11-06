#include "arvoreB/arvore_bp.h"
#include <stdio.h>

int main () {
    FILE *fp;

    if ((fp = fopen("Files/ibtree.idx", "r+")) == NULL) {
        fp = fopen("Files/ibtree.idx", "w+");
        escreve_pagina_vazia(fp);
    }

    int raiz = 0;

    raiz = insere_chave(raiz, fp, "AAA11", 1);
    raiz = insere_chave(raiz, fp, "BBB11", 2);
    raiz = insere_chave(raiz, fp, "CCC11", 3);
    raiz = insere_chave(raiz, fp, "DDD11", 4);
    raiz = insere_chave(raiz, fp, "EEE11", 5);

    fclose(fp);
    printf("%d", raiz);
}