#include "arvore/rubro-negra.h"
#include "perfumaria.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

/**
 * Header do arquivo "filmes.c"
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
boolean inserirFilme(FILE *, NO **, NO **);
boolean validaNome(char []);
boolean validaAno(char []);
boolean validaDuplicidade(char *, NO *);
int calculaRRN(FILE *);
boolean removerFilme(FILE *, NO **, NO **);
void modificarNota(FILE *, NO *);
void buscarChavePrimaria(FILE *, NO *, char *);
void buscarChaveSecundaria(FILE *, NO *, NO *, char *);
void listarFilmes(FILE *, NO *, NO *);
void imprimeFilme(FILE **, int);
boolean compactarArquivo(FILE *, NO **);
void escreverIndiceP(FILE *, NO *);
void escreverIndiceS(FILE *, NO *);
void alteraFlag(FILE *, FILE *, int *);