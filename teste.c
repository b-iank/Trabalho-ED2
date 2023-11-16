#include "arvore_bm/arvore_bm.h"

int main () {
    FILE *fp;

    remove("Files/ibtree.idx");
    FILE * filmes = fopen("Files/movies.dat", "r+");
    if ((fp = fopen("Files/ibtree.idx", "r+")) == NULL) {
        fp = fopen("Files/ibtree.idx", "w+");
        escreve_header(1, fp, 0);
        escreve_pagina_vazia(fp);
    }

    int raiz = le_header(fp, filmes);

    printf("RAIZ: %d\n", raiz);
    int busca = busca_registro(raiz, fp, "NOL14");
    printf("BUSCA: %d\n", busca);

    printf("EM ORDEM:");
    em_ordem(raiz, fp);


    escreve_header(raiz, fp, 1);
    fclose(fp);

    getchar();

    return 0;
}