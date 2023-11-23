#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "avl.h"

// Estrutura de Dados 1: AVL

// Calcula altura
int altura (no *raiz) {
    if (!raiz)
        return 0;

    int esq = altura(raiz->esq);
    int dir = altura(raiz->dir);

    if (esq > dir)
        return esq+1;
    return dir+1;
}
// Calcula fator de balanceamento
int FB (no *no) {
    if (no)
        return (altura(no->dir) - altura(no->esq));
    return 0;
}

// Insere na lista de nomes do idxSecundario
void insereLista(no **no, char chave[6]) {
    Lista *novo = malloc(sizeof(Lista)), *anterior = NULL, *atual = (*no)->idxSecundario.chaves;
    novo->prox = NULL;
    strcpy(novo->chaveP, chave);

    while (atual) {
        anterior = atual;
        atual = atual->prox;
    }

    if (!anterior) {
        novo->prox = (*no)->idxSecundario.chaves;
        (*no)->idxSecundario.chaves = novo;
    } else {
        anterior->prox = novo;
        novo->prox = atual;
    }
}

// Remove um item da lista de nomes do idxSecundario
void removeLista(no **no, char chave[6]) {
    Lista *anterior = NULL, *atual = (*no)->idxSecundario.chaves;

    while (atual != NULL && strcmp(atual->chaveP, chave) != 0) {
        anterior = atual;
        atual = atual->prox;
    }

    if (atual != NULL) {
        if (anterior == NULL) {
            (*no)->idxSecundario.chaves = atual->prox;
        } else {
            anterior->prox = atual->prox;
        }
        free(atual);
    }
}

// Escreve no secundario no arquivo
void escreve_no_Secundario(FILE* idxS, tree no) {
    Lista* proximo = no->idxSecundario.chaves;
    while (proximo) {
        fprintf(idxS, "#%s@%s", no->idxSecundario.nome, proximo->chaveP);
        proximo = proximo->prox;
    }
}

// Funcao de escrever em-ordem
void em_ordem_EscreveS(FILE* idxS, tree arvS) {
    if (!arvS)
        return;
    em_ordem_EscreveS(idxS, arvS->esq);
    escreve_no_Secundario(idxS, arvS);
    em_ordem_EscreveS(idxS, arvS->dir);
}

// Faz a rotacao esquerda entre dois nos
no* rotacao_esq(no *raiz) {
    no *nova_raiz = raiz->dir, *filho = nova_raiz->esq;
    raiz->dir = filho;
    nova_raiz->esq = raiz;

    nova_raiz->fb = FB(nova_raiz);
    raiz->fb = FB(raiz);


    return nova_raiz;
}

// Faz a rotacao direita entre dois nos
no* rotacao_dir(no *raiz) {
    no *nova_raiz = raiz->esq, *filho = nova_raiz->dir;

    raiz->esq = filho;
    nova_raiz->dir = raiz;

    nova_raiz->fb = FB(nova_raiz);
    raiz->fb = FB(raiz);

    return nova_raiz;
}
// Faz a rotacao dupla esquerda
no* rotacao_DuplaEsq (no *raiz) {
    raiz->dir = rotacao_dir(raiz->dir);
    return rotacao_esq(raiz);
}

// Faz a rotacao dupla direita
no* rotacao_DuplaDir (no *raiz) {
    raiz->esq = rotacao_esq(raiz->esq);
    return rotacao_dir(raiz);
}

// Balanceia a arvore
no* balancear(no* raiz) {
    int fb = FB(raiz);

    if (fb > 1 && FB(raiz->dir) >= 0)
        raiz = rotacao_esq(raiz);
    else if (fb < -1 && FB(raiz->esq) <= 0)
        raiz = rotacao_dir(raiz);
    else if (fb < -1 && FB(raiz->esq) > 0)
        raiz = rotacao_DuplaDir(raiz);
    else if (fb > 1 && FB(raiz->dir) < 0)
        raiz = rotacao_DuplaEsq(raiz);
    return raiz;
}

// Insere no na arvore
no* inserir(no* raiz, no *x) {
    if (!raiz)
    {
        no *novono = x;
        novono->esq = NULL;
        novono->dir = NULL;
        novono->fb = 0;

        return novono;
    } else
    {
        if (strcmp(x->chave, raiz->chave) < 0)
            raiz->esq = inserir(raiz->esq, x);
        else if (strcmp(x->chave, raiz->chave) > 0)
            raiz->dir = inserir(raiz->dir, x);
    }

    raiz->fb = FB(raiz);
    return balancear(raiz);
}

// Remove um no de uma arvore
no* remover_avl(no** raiz, no* removido) {
    if (!raiz)
        return NULL;
    if (strcmp((*raiz)->chave, removido->chave) == 0) {
        if ((*raiz)->esq == NULL && (*raiz)->dir == NULL) { // folha
            free(*raiz);
            return NULL;
        }
        else if ((*raiz)->esq != NULL && (*raiz)->dir != NULL) // possui dois filhos
        {       no* aux = (*raiz)->esq;
            while (aux->dir)
                aux = aux->dir;

            strcpy((*raiz)->chave, aux->chave);
            (*raiz)->idxSecundario = aux->idxSecundario;

            strcpy(aux->chave, removido->chave);
            aux->idxSecundario = removido->idxSecundario;

            (*raiz)->esq = remover_avl(&(*raiz)->esq, removido);
            return *raiz;
        }
        else
        {
            no* aux;
            if ((*raiz)->esq)
                aux = (*raiz)->esq;
            else
                aux = (*raiz)->dir;
            free(*raiz);
            return aux;
        }
    }
    else
    {
        if (strcmp(removido->chave, (*raiz)->chave) < 0)
            (*raiz)->esq = remover_avl(&(*raiz)->esq, removido);
        else
            (*raiz)->dir = remover_avl(&(*raiz)->dir, removido);
    }

    (*raiz)->fb = FB(*raiz);
    *raiz = balancear(*raiz);

    return *raiz;
}
// Busca no de uma arvore
no* buscaNo(no *raiz, char* chave) {
    if (raiz == NULL || strcmp(raiz->chave, chave) == 0) {
        return raiz;
    }

    if (strcmp(raiz->chave, chave) > 0)
        return buscaNo(raiz->esq, chave);
    else
        return buscaNo(raiz->dir, chave);
}

// Free arvore
no *expurgar(no *raiz) {
    if (raiz == NULL)
        return NULL;

    Lista *aux, *prox;
    raiz->esq = expurgar(raiz->esq);
    raiz->dir = expurgar(raiz->dir);

    aux = raiz->idxSecundario.chaves;
    while (aux != NULL) {
        prox = aux->prox;
        free(aux);
        aux = prox;
    }

    free(raiz);
    return NULL;
}