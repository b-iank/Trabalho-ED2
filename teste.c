#include "arvoreB/arvore_bp.h"
#include <stdio.h>

int main () {
    FILE *fp = fopen("Files/ibprimary.txt", "r+");
    //PAGE pag = le_pagina(0, fp);
    int teste = busca_registro(0, fp, "MMM11");
}