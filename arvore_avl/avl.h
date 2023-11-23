#include <stdio.h>

// Estrutura da lista contida no no secundario
typedef struct lista {
    char chaveP[6];
    struct lista* prox;
} Lista;

// Estrutura que contem info do indice secundario
struct indiceS {
    char nome[66];
    Lista* chaves;
};

typedef struct indiceS IndiceS;

// No que contem informacoes do indice primario e secundario
typedef struct no {
    char chave[66];
    IndiceS idxSecundario;

    int fb;
    struct no *esq;
    struct no *dir;
} no;

typedef no* tree;

int altura(no *raiz);

int FB (no *no);

void insereLista(no **no, char chave[6]);

void removeLista(no **no, char chave[6]);

void escreve_no_Secundario(FILE* idxS, tree no);

void em_ordem_EscreveS(FILE* idxS, tree arvS);

no* rotacao_esq(no *raiz);

no* rotacao_dir(no *raiz);

no* rotacao_DuplaEsq (no *raiz);

no* rotacao_DuplaDir (no *raiz);

no* balancear(no* raiz);

no* inserir(no* raiz, no *x);

no* remover_avl(no** raiz, no* removido);

no* buscaNo(no *raiz, char* chave);

no *expurgar(no *raiz);