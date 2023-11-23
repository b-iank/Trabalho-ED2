#include "../perfumaria.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

#define ORDEM 8
#define TAM_REGISTRO 73

typedef struct pagina PAGE;

struct pagina {
    int rrn_pagina;             /* RRN da página*/
    int rrn_pai;                /* RRN da página*/
    int folha;                  /* 1 = folha || 0 = interno  || 2 = apagado */
    int quantidade_chaves;      /* Quantidade de chaves na página*/
    char chaves[ORDEM - 1][6];  /* Guarda as chaves da página */
    int rrn[ORDEM];             /* Guarda os RRNs do arquivo de dados (quando página for folha) e
                                    dos filhos (quando página for interna) */
};

// ÚTIL ------------------------------------

int le_header(FILE *, FILE *);
void escreve_header(int, FILE *, int);
void escreve_pagina_vazia(FILE *);
PAGE le_pagina(int, FILE *);
int calcula_rrn(FILE *);
void escreve_pagina(FILE *, PAGE);
int calcula_quant_minima(int);
void escreve_filho(FILE *, PAGE);
int conta_chaves(FILE *fp, int raiz);
// -----------------------------------------

// BUSCA -----------------------------------

PAGE busca_folha(int, FILE *, char []);
int busca_registro(int , FILE *, char[]);
PAGE busca_primeira_folha(int, FILE *);
//  -----------------------------------------

// INSERÇÃO --------------------------------

int insere_chave(int , FILE *, char [], int);
int insere_split_folha(int, FILE *, PAGE, char[], int);
int insere_pai(int, FILE *, char[], PAGE, PAGE);
int insere_nova_raiz(FILE *, char[], PAGE, PAGE);
int insere_split_interno(int, FILE *, PAGE, int, char[], PAGE);
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

void em_ordem_b(int, FILE *);
// -----------------------------------------