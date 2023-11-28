/**
 *
 * @author Bianca Lançoni de Oliveira Garcia
 * @author Lucas Furriel Rodrigues
 */

#include <stdio.h>
#include "filmes.h"
#include "perfumaria.h"

int main() {
    char pathFilmes[80] = "Files/movies.dat"; // Caminho do arquivo filmes
    char pathPrimary[80] = "Files/ibtree.idx"; // Caminho do arquivo de índices primário
    char pathTitle[80] = "Files/ititle.idx"; // Caminho do arquivo de índices secundário

    FILE *filmes = arquivoFilme(pathFilmes); // Abre arquivo de filmes
    FILE *fileIndiceP = arquivoIndicePrimario(pathPrimary); // Abre arquivo de índice primário
    FILE *fileIndiceS = arquivoIndiceSecundario(pathTitle); // Abre arquivo de índice secundário

    int indiceP = le_header(fileIndiceP, filmes); // Carrega os índices primários em árvore rubro-negra
    no *indiceS = carregarIndiceS(filmes, fileIndiceS); // Carrega os índices secundários em árvore rubro-negra

    // Variáveis auxiliares
    int op, flag;
    char temp[71];
    PAGE folha;

    // Volta os ponteiros dos arquivos para o início
    rewind(filmes);
    rewind(fileIndiceP);
    rewind(fileIndiceS);

    fscanf(fileIndiceP, "%d", &flag); // Lê a flag do arquivo de índice primário

    do {
        op = menu(); // Retorna a opção selecionada pelo usuário
        switch (op) {
            case 0: // Exit
                printf(SUCESSO NEGRITO "Ate mais!\n" LIMPA);
                break;
            case 1: // Inserir novo filme
                if (inserirFilme(filmes, fileIndiceP, &indiceP, &indiceS) &&
                    flag == 1) // Lê do usuário um novo filme e o inclui no arquivo de filmes && flag == 1)
                    alteraFlag(fileIndiceP, fileIndiceS, &flag);
                break;
            case 2: // Remover um filme
                if (removerFilme(filmes, fileIndiceP, &indiceP, &indiceS) && flag == 1)
                    alteraFlag(fileIndiceP, fileIndiceS, &flag);
                if (indiceP == -1) {
                    fseek(fileIndiceP, 0, SEEK_SET);
                    ftruncate(fileno(fileIndiceP), 2 * TAM_REGISTRO);
                    escreve_pagina_vazia(fileIndiceP);
                    indiceP = 1;
                }
                break;
            case 3: // Modificar nota de um filme
                modificarNota(filmes, fileIndiceP, indiceP);
                break;
            case 4: // Buscar por ID
                printf(ITALICO "\nDigite o valor da chave primaria: " LIMPA);
                scanf(" %6[^\n]s", temp);
                while (getchar() != '\n');
                buscarChavePrimaria(filmes, fileIndiceP, indiceP, temp); // Busca um filme pela chave primária e imprime no terminal
                printf("---------------------------------\n");
                break;
            case 5: // Busca um filme pela chave secundária (nome) e imprime no terminal
                printf(ITALICO "\nDigite o titulo em portugues: " LIMPA);
                scanf(" %66[^\n]s", temp);
                while (getchar() != '\n');
                buscarChaveSecundaria(filmes, fileIndiceP, indiceP, indiceS, temp);
                printf("---------------------------------\n");
                break;
            case 6: // Listar todos os filmes
                if (indiceS == NULL)
                    printf(ERRO NEGRITO"Seu arquivo nao tem filmes!\n" LIMPA);
                else {
                    folha = busca_primeira_folha(indiceP, fileIndiceP);
                    listarFilmes(filmes, fileIndiceP, folha, folha.chaves[0]); // Lista todos os filmes
                    printf("---------------------------------\n");
                    printf(LARANJA ITALICO "Total de filmes: " LIMPA "%d\n", conta_chaves(fileIndiceP, indiceP));
                }
                break;
            case 7: // Listar range
                if (indiceS == NULL)
                    printf(ERRO NEGRITO"Seu arquivo nao tem filmes!\n" LIMPA);
                else {
                    printf(ITALICO "\nDigite o valor da chave primaria: " LIMPA);
                    scanf(" %6[^\n]s", temp);
                    while (getchar() != '\n');
                    for (int i = 0; i < 5; i++)
                        temp[i] = toupper(temp[i]);
                    folha = busca_folha(indiceP, fileIndiceP, temp);
                    listarFilmes(filmes, fileIndiceP, folha, temp); // Lista filmes partindo de uma chave
                    printf("---------------------------------\n");
                }
                break;
            case 8: // Remove do arquivo de filmes aqueles que foram deletados
                if (compactarArquivo(filmes, fileIndiceP, &indiceP) && flag == 1)
                    alteraFlag(fileIndiceP, fileIndiceS, &flag);
                break;
            default: // Case ‘default’ implica em algum erro não tratado
                printf(ERRO NEGRITO"ERRO!\n" LIMPA);
                break;
        }
    } while (op != 0);


    fclose(filmes); // Salva e fecha o arquivo de filmes

    if (flag == 0) { // Sobreescreve o arquivo de índice primário com as informações da árvore
        escreve_header(indiceP, fileIndiceP, 1);

        // Sobreescreve o arquivo de índice secundário com as informações da árvore
        fileIndiceS = fopen(pathTitle, "w");
        fputc('0', fileIndiceS);
        escreverIndiceS(fileIndiceS, indiceS);
        rewind(fileIndiceS);
        fputc('1', fileIndiceS);

        printf(SUCESSO NEGRITO"Arquivos de indices reescritos!\n" LIMPA);
    }

    fclose(fileIndiceP);
    fclose(fileIndiceS);

    expurgar(indiceS);
    return 0;
}