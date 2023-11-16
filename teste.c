#include "arvore_bm/arvore_bm.h"

int main () {
    FILE *fp;

    //remove("Files/ibtree.idx");
    if ((fp = fopen("Files/ibtree.idx", "r+")) == NULL) {
        fp = fopen("Files/ibtree.idx", "w+");
        escreve_pagina_vazia(fp);
    }

    int raiz = 6;

//    raiz = insere_chave(raiz, fp, "AAA11", 1); //ok
//    raiz = insere_chave(raiz, fp, "BBB11", 2); //ok
//    raiz = insere_chave(raiz, fp, "CCC11", 3); //ok
//    raiz = insere_chave(raiz, fp, "DDD11", 4); //ok
//    raiz = insere_chave(raiz, fp, "EEE11", 5); //ok
//    raiz = insere_chave(raiz, fp, "FFF11", 6); //ok
//    raiz = insere_chave(raiz, fp, "GGG11", 7); //ok
//    raiz = insere_chave(raiz, fp, "HHH11", 8); //ok
//    raiz = insere_chave(raiz, fp, "III11", 9); //ok
//    raiz = insere_chave(raiz, fp, "JJJ11", 10); //ok

    //TODO: erro, concatenação parece funcionar porém ao propagar o erro, da problema no nó interno
    //tanto concatenação em seguida quanto redistribuição
    raiz = remover(raiz, fp, "AAA11");


    printf("RAIZ: %d\n", raiz);
    int busca = busca_registro(raiz, fp, "AAA13");
    printf("BUSCA: %d\n", busca);

    printf("EM ORDEM:");
    em_ordem(raiz, fp);

    fclose(fp);

    getchar();

    return 0;
}