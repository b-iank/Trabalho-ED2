#include "arvore_bm/arvore_bm.h"

int main() {
    FILE *fp;

    //remove("Files/ibtree.idx");
    FILE *filmes = fopen("Files/movies.dat", "r+");
    if ((fp = fopen("Files/ibtree.idx", "r+")) == NULL) {
        fp = fopen("Files/ibtree.idx", "w+");
        escreve_header(1, fp, 0);
        escreve_pagina_vazia(fp);
    }

    int raiz = le_header(fp, filmes);

//    raiz = remover(raiz, fp, "AND99"); //ok
//    raiz = remover(raiz, fp, "CIM78"); //ok
//    raiz = remover(raiz, fp, "ROC64"); //ok
//    raiz = remover(raiz, fp, "PHI19"); //ok
//    raiz = remover(raiz, fp, "NOL08"); //ok
//    raiz = remover(raiz, fp, "SPI77"); //ok
//    raiz = remover(raiz, fp, "KUB60"); //ok
//    raiz = remover(raiz, fp, "KUB68"); //ok
//    raiz = remover(raiz, fp, "JOO19"); //ok
//    raiz = remover(raiz, fp, "BEN98"); //ok
//    raiz = remover(raiz, fp, "SPI82"); //ok
//    raiz = remover(raiz, fp, "ALL77"); //ok
//    raiz = remover(raiz, fp, "ARR00"); //ok
//    raiz = remover(raiz, fp, "COP72"); //ok
//    raiz = remover(raiz, fp, "LEO96"); //ok
//    raiz = remover(raiz, fp, "LUC77"); //ok
//    raiz = remover(raiz, fp, "AND07"); //ok
//    raiz = remover(raiz, fp, "MIY04"); //ok
//    raiz = remover(raiz, fp, "NOL14"); //ok
//    raiz = remover(raiz, fp, "RIO15"); //ok
//    raiz = remover(raiz, fp, "MIY01"); //ok
//    raiz = remover(raiz, fp, "KUB64"); //ok
//    raiz = remover(raiz, fp, "SCO04"); //ok
//    raiz = remover(raiz, fp, "DEM91"); //ok
//
//    raiz = remover(raiz, fp, "DOC01"); //ok
//    raiz = remover(raiz, fp, "COP79"); //ok
//    raiz = remover(raiz, fp, "DOC15"); //ok
//    raiz = remover(raiz, fp, "LES84"); //ok
//    raiz = remover(raiz, fp, "GER23"); //ok
//    raiz = remover(raiz, fp, "JAC03"); //ok
//    raiz = remover(raiz, fp, "ALT70"); //ok
//    raiz = remover(raiz, fp, "MEN99"); //ok
//    raiz = remover(raiz, fp, "NOL10"); //ok
//
//    raiz = remover(raiz, fp, "SPI83"); //ok
//    raiz = remover(raiz, fp, "SPI93"); //ok
//    raiz = remover(raiz, fp, "TAR03"); //ok
//    raiz = remover(raiz, fp, "TAR92"); //ok
//    raiz = remover(raiz, fp, "TAR94"); //ok
//    raiz = remover(raiz, fp, "ZEM94"); //ok
//    raiz = remover(raiz, fp, "LEA57"); //ok

    //raiz = insere_chave(raiz, fp, "NOL14", 19);

    printf("EM ORDEM:");

    em_ordem_b(raiz, fp);

    raiz = remover(raiz, fp, "NOL14");

    escreve_header(raiz, fp, 1);
    fclose(fp);

    getchar();

    return 0;
}