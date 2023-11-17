#include "../perfumaria.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

#define ORDEM 4
#define TAM_REGISTRO 37

typedef struct pagina PAGE;

struct pagina {
    int rrn_pagina;
    int rrn_pai;
    int folha; /* 1 = folha || 0 = interno */
    int quantidade_chaves;
    char chaves[ORDEM - 1][6];
    int rrn[ORDEM];
};

// ÚTIL ------------------------------------
int le_header(FILE *, FILE *);
void escreve_header(int, FILE *, int);
void escreve_pagina_vazia(FILE *);
PAGE le_pagina(int, FILE *);
int calcula_rrn(FILE *);
void escreve_pagina(FILE *, PAGE);
PAGE busca_primeira_folha(int, FILE *);
int calcula_quant_minima(int);
void escreve_filho(FILE *, PAGE);
int conta_nos(FILE *, int);
// -----------------------------------------

// BUSCA -----------------------------------
PAGE busca_folha(int, FILE *, char []);
int busca_registro(int , FILE *, char[]);
//  -----------------------------------------

// INSERÇÃO --------------------------------
int insere_chave(int , FILE *, char [], int);
int insere_split(int , FILE *, PAGE , char [], int);
int insere_pai(int, FILE *, char[], PAGE, PAGE);
int insere_nova_raiz(FILE *, char[], PAGE, PAGE);
int insere_pagina(int, FILE *, PAGE, int, char[], PAGE);
int insere_pagina_split(int, FILE *, PAGE, int, char[], PAGE);
//  -----------------------------------------

// REMOÇÃO ----------------------------------
int remover(int, FILE *, char[]);
int remove_chave(int, FILE *, PAGE, char[], int);
PAGE remove_chave_no(FILE *, PAGE, char[], int);
int ajusta_raiz(PAGE, FILE *);
int redistribuicao(int, FILE *, PAGE, PAGE, PAGE, int, int);
int concatenar(int, FILE *, PAGE, PAGE, PAGE, PAGE, int, char[]);
//  -----------------------------------------

// IMPRESSÃO --------------------------------
void em_ordem_b(int raiz, FILE *fp);
// -----------------------------------------