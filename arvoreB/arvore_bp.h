#include "../perfumaria.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define boolean int

#define ORDEM 8
#define TAM_REGISTRO 70

typedef struct pagina PAGE;

struct pagina {
    int folha; // 1 = folha -- 0 = interno
    int quantidade_chaves;
    char chaves[ORDEM - 1][6];
    int rrn[ORDEM];
};


PAGE le_pagina(int, FILE *);
int busca_registro(int , FILE *, char[]);
