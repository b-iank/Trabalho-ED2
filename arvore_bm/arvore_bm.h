#include "../perfumaria.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ORDEM 4
#define TAM_REGISTRO 37

typedef struct pagina PAGE;

struct pagina {
    int rrn_pagina;
    int rrn_pai;
    int folha; // 1 = folha -- 0 = interno
    int quantidade_chaves;
    char chaves[ORDEM - 1][6];
    int rrn[ORDEM];
};

void escreve_pagina_vazia(FILE *);
PAGE le_pagina(int, FILE *);
PAGE busca_folha(int, FILE *, char []);
int busca_registro(int , FILE *, char[]);
int insere_chave(int , FILE *, char [], int);
void escreve_pagina(FILE *, PAGE);
int insere_split(int , FILE *, PAGE , char [], int);
int calcula_rrn(FILE *fp);
int insere_pai(int, FILE *, char[], PAGE, PAGE);
int insere_nova_raiz(FILE *, char[], PAGE, PAGE);
int insere_pagina(int, FILE *, PAGE, int, char[], PAGE);
int insere_pagina_split(int, FILE *, PAGE, int, char[], PAGE);