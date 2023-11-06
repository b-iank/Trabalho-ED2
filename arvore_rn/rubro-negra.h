#include "../perfumaria.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRUE 1
#define FALSE 0
#define boolean int

#define PRETO 0
#define VERMELHO 1
#define color int

typedef struct node NO;
typedef struct lista LISTA;

/**
 * Estrutura que representa um nó de uma árvore rubro-negra.
 */
struct node {
    char ordem[66];    /**< Elemento que representa o valor responsável por odenar a árvore.*/
    char chave[6];    /**< Elemento que representa a chave primária.*/
    int rrn;    /**< Elemento que representa o RRN.*/
    char titulo[61];    /**< Elemento que representa a chave secundária.*/
    LISTA *lista; /**< Lista que contempla todos as chaves primárias de filmes com mesmo nome.*/
    NO *pai;     /**< Ponteiro para o pai do nó.*/
    NO *dir;     /**< Ponteiro para o nó filho direito.*/
    NO *esq;     /**< Ponteiro para o nó filho esquerdo.*/
    color cor;   /**< Cor do nó (PRETO || VERMELHO).*/
};

/**
 * Estrutura que representa um a lista de chaves primárias de um filme de mesmo nome.
 */
struct lista {
    char chave[6];
    LISTA *prox;
};


//Auxiliares
int menu();
int calcula_altura(NO* no);
int num_total_nos(NO* no);
boolean no_folha(NO *raiz);
NO * busca_binaria(NO *raiz, char busca[61]);
NO * expurgar_arvore(NO *arvore, boolean lista);
void expurgar_lista(LISTA *lista);
void printa_no(NO *no, char *espacamento);
int conta_nos_pretos(NO* no);
// -----------------------------------------------------------------------------

//Inserção e remoção:

NO * novo_no(char ordem[61], char chave[6], char titulo[61], int rrn, NO *raiz);
boolean insere_no(NO **raiz, char ordem[61], char chave[6], char titulo[61], int rrn);
boolean deleta_no(NO **raiz, char ordem[61], char chave[6], char titulo[61], int rrn);
NO * rotacao_esq(NO *no);
NO * rotacao_dir(NO *no);
void checa_no_insercao(NO *no);
void verifica_raiz(NO **raiz, NO **verifca);
boolean verifica_caso1(NO *removido);
void verifica_caso2(NO *removido, boolean remover, boolean direita, NO **raiz);
void insere_lista(NO **no, char chave[6]);
void remove_lista(NO **no, char chave[6]);
// -----------------------------------------------------------------------------

//Imprimir árvore:

void pre_ordem (NO *raiz);
void em_ordem (NO *raiz);
void pos_ordem (NO *raiz);
void imprime_arvore(NO *no, int nivel);
// -----------------------------------------------------------------------------