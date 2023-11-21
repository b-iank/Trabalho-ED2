#include "arvore_rn/rubro-negra.h"
#include "arvore_bm/arvore_bm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

/**
 * Header do arquivo "filmes.c"
 * @author Bianca Lançoni de Oliveira Garcia
 * @author Lucas Furriel Rodrigues
 */

typedef struct filmes FILME;

struct filmes { /*  Struct responsável por armazenar um filme. */
    char chavePrimaria[6];
    char tituloPortugues[66];
    char tituloOriginal[71];
    char nomeDiretor[21];
    char anoLancamento[5];
    char pais[21];
    char nota;
};

/*  Protótipos de função. */
int menu();
FILE * arquivoFilme(char *);
FILE * arquivoIndicePrimario(char *);
FILE * arquivoIndiceSecundario(char *);
NO * carregarIndiceP(FILE *, FILE *);
NO * carregarIndiceS(FILE *, FILE *);
char * geraChavePrimaria(FILME *);
boolean inserirFilme(FILE *, FILE *, int *, NO **);
boolean validaNome(char []);
boolean validaAno(char []);
boolean validaDuplicidade(FILE *, char *, int);
int calculaRRN(FILE *);
boolean removerFilme(FILE *, FILE *, int *, NO **);
void modificarNota(FILE *, FILE *, int);
void buscarChavePrimaria(FILE *, FILE *, int, char *);
void buscarChaveSecundaria(FILE *, FILE *, int, NO *, char *);
void listarFilmes(FILE *, FILE *, PAGE, char []);
void imprimeFilme(FILE **, int);
boolean compactarArquivo(FILE *, FILE *, int *);
void escreverIndiceP(FILE *, NO *);
void escreverIndiceS(FILE *, NO *);
void alteraFlag(FILE *, FILE *, int *);