#include "rubro-negra.h"

/**
 * Aloca um novo nó na memória e adiciona seus valores
 * 
 * @param elem (int) elemento do nó
 * @param raiz (NO *) no pai do novo nó
 * @return Novo nó criado
*/
NO *novo_no(char ordem[71], char chave[6], char titulo[71], int rrn, NO *raiz) {
   NO *novo = malloc(sizeof(NO));
   if (novo == NULL) {
      printf(ERRO "Sem espaço de memória.\n" LIMPA);
      return NULL;
   }

   strcpy(novo->ordem, ordem);

   if (titulo && strcmp(titulo, "") != 0) {
       strcpy(novo->chave, "");
       strcpy(novo->titulo, titulo);
       novo->lista = NULL;
       insere_lista(&novo, chave);
   } else {
       strcpy(novo->chave, chave);
       novo->lista == NULL;
   }
   novo->rrn = rrn;

   novo->pai = raiz;
   novo->dir = novo->esq = NULL;
   novo->cor = VERMELHO;
   return novo;
}

/**
 * Verifica se um nó é folha ou não
 * 
 * @param no (NO *) nó a ser analisado
 * @return TRUE caso seja um nó folha ou FALSE c.c.
*/
boolean no_folha(NO *no) {
   return !(no->esq) && !(no->dir) ? TRUE : FALSE;
}

/**
 * Rotação simples a esquerda. Salvamos os dados do no em duas structs auxiliares (pai e avo) e em
 * seguida fazemos a rotação.
 * 
 * @param no (NO *) Nó a ser rotacionado
 * @return No após a rotação
*/
NO *rotacao_esq(NO *no) {
   NO *pai = no->pai;
   NO *avo = pai->pai;

   pai->dir = no->esq;

   if (no->esq)
      no->esq->pai = pai;

   no->pai = avo; // Nó pai agora é o avô (antigo nó assume a posição do pai (central))
   pai->pai = no; // O pai do pai (novo avô) é o antigo nó
   no->esq = pai; // Como nó era maior que o pai (estava a sua direita), po pai antigo será seu novo filho esquerdo

   if (avo) {                      // Caso haja um avô, devemos fazer ele apontar para o seu novo filho (nó)
      if (avo->dir == pai) // Se ele apontava para o antigo pai pela direita
         avo->dir = no;     // Aponta para nó pelo seu ponteiro direito
      else
         avo->esq = no; // Mesma coisa, mas com a esquerda
   }
   return no;
}

/**
 * Rotação simples a direita. Salvamos os dados do no em duas structs auxiliares (pai e avo) e em
 * seguida fazemos a rotação.
 * 
 * @param no (NO *) Nó a ser rotacionado
 * @return No após a rotação
*/
NO *rotacao_dir(NO *no) {
   NO *pai = no->pai;
   NO *avo = pai->pai;

   pai->esq = no->dir;

   if (no->dir)
      no->dir->pai = pai; // Se no->dir for diferente de NULL, temos que fazer no->esq devolver o apontamento para seu novo pai

   no->pai = avo; // O pai do nó passa a ser o avô (nó assume o lugar de seu antigo pai)
   pai->pai = no; // O no vira pai de seu antigo pai
   no->dir = pai; // Como seu antigo pai era maior que no, no apontará a ele pela direita

   if (avo) {                      // Se o avo existir, temos que fazer ele devolver o apontamento para nó
      if (avo->dir == pai) // Se ele apontava para o antigo pai pela direita
         avo->dir = no;     // Aponta para nó pelo seu ponteiro direito
      else
         avo->esq = no; // Mesma coisa, mas com a esquerda
   }
   return no;
}

/**
 * Corrige a coloração e balanceia a árvore após a inserção de um novo nó.
 * Verificamos os casos e colorimos os nós de acordo com as propriedades da árvore rubro-negra
 * 
 * @param no (NO *) nó a ser verificado
*/
void checa_no_insercao(NO *no) {
   // Caso em que não há nó ou o nó é a raiz
   if (!no || !no->pai)
      return; // Apenas retorna, não há nada para ser feito

   NO *filho = no; // Apenas guarda os dados da struct do nó recebido de parêmetro
   NO *pai = no->pai;  // Guarda os dados do pai do nó recebido de parâmetro
   NO *avo = pai->pai; // Guarda os dados da struct do avô do nó (pai->pai)


   if (no->cor == PRETO || no->pai->cor == PRETO)
      return;

   // Caso 1) o pai do nó inserido é a raiz
   if (!avo) {
      pai->cor = PRETO; // Raiz deve ser preta
      return;
   }

   // Caso 2) Ambos os filhos do avô são vermelhos
   if (avo->dir && avo->esq && avo->dir->cor == VERMELHO && avo->esq->cor == VERMELHO) {
      avo->cor = VERMELHO;
      avo->esq->cor = PRETO;
      avo->dir->cor = PRETO;
      return;
   }

   // Agora só há a opção de rotação
   NO *bisavo = avo->pai; // Salva os dados do bisavô do nó

   if (avo->dir == pai) {
      if (pai->dir == no) {                        // Temos uma diagonal principal (rotação esquerda)
         avo->dir = pai->esq;   // Esse elemento está à direita do avô, porntanto é maior que ele ou é NULL.
         if (pai->esq)          // Caso o elemnto exista (não é NULL)
            pai->esq->pai = avo; // Devolve o seu apontamento para o avô

         pai->esq = avo; // Agora, o pai sobe um nível da árvore
         avo->pai = pai;
         pai->pai = bisavo; // Une o nó com a árvore

         if (bisavo) {
            if (bisavo->esq && bisavo->esq == avo)
               bisavo->esq = pai;
            else
               bisavo->dir = pai;
         }

         //Corrigindo as cores:
         pai->cor = PRETO;
         avo->cor = VERMELHO;
      } else {

         // Caso de rotação direita esquerda (dupla esquerda)
         pai->esq = filho->dir; // Como nó é menor que o pai nesse caso (dupla esquerda), qualquer filho seu é menor que seu pai
         if (filho->dir)
            filho->dir->pai = pai; // Devolve o apontamento se existir
         filho->dir = pai;        // O nó filho troca de posição com o pai
         pai->pai = filho;

         // Inicializa o segundo caso da rotação (rotação esquerda)
         avo->dir = filho->esq; // Como nesse caso, o avô era menor que o antigo pai, ele automaticamente é menor que qualquqer filho do no

         if (filho->esq) // Caso filho→esq exista devolve o apontamento para seu novo pai
            filho->esq->pai = avo;
         filho->esq = avo;
         avo->pai = filho;

         // Para o nó ser inserido na árvore, devemos conectar um pai para filho
         filho->pai = bisavo;
         if (bisavo) {
            if (bisavo && bisavo->esq == avo)
               bisavo->esq = filho;
            else
               bisavo->dir = filho;
         }

         //Corrigindo as cores:
         filho->cor = PRETO;
         avo->cor = VERMELHO;
      }
   } else { // Pai está a esquerda do avô
      // Caso esquerdo (diagonal secundária) -> Rotação simples esquerda
      if (pai->esq == no) {                        // Nó é menor que seu pai
         avo->esq = pai->dir;   // O pai subirá de nível, seu nó direito não pode ser perdido

         if (pai->dir)          // Caso esse nó exista -> Devolve o apontamento
            pai->dir->pai = avo;
         pai->dir = avo;        // Avô é maior que o pai, então vai para sua direita
         avo->pai = pai;

         // Agora devemos conectá-lo à árvore
         pai->pai = bisavo;
         if (bisavo) {
            if (bisavo->esq && bisavo->esq == avo)
               bisavo->esq = pai;
            else
               bisavo->dir = pai;
         }

         //Corrigindo as cores:
         pai->cor = PRETO;
         avo->cor = VERMELHO;
      } else {
         // Caso esquerdo direito (rotação dupla direita)
         // Primeira parte da rotação (para a rotação esquerda)
         pai->dir = filho->esq; // filho é maior que o pai, logo qualquer filho do filho é maior que pai
         if (filho->esq)        // Devolver o apontamento
            filho->esq->pai = pai;
         filho->esq = pai; // filho é maior que seu antigo pai
         pai->pai = filho;

         // Rotação com o avô
         avo->esq = filho->dir; // Agora é a rotação direita, ou seja, o avô é maior que o filho
         if (filho->dir)
            filho->dir->pai = avo;
         filho->dir = avo;
         avo->pai = filho;

         // Conectá-lo à árvore
         filho->pai = bisavo;
         if (bisavo) {
            if (bisavo->esq && bisavo->esq == avo)
               bisavo->esq = filho;
            else
               bisavo->dir = filho;
         }

         //Corrigindo as cores:
         filho->cor = PRETO;
         avo->cor = VERMELHO;
      }
   }
}

/**
 * Verifica se o nó adicionado é uma raiz para corrigir o apontamento do ponteiro
 * 
 * @param raiz (NO **) ponteiro da raiz principal da árvore
 * @param verifica (NO **) ponteiro do no que acabamos de adicionar
*/
void verifica_raiz(NO **raiz, NO **verifca) {
   if (!(*verifca)->pai)
      *raiz = *verifca;

}

/**
 * Calcula recursivamente a altura da árvore
 * 
 * @param no (NO *) no raiz da arvore/subarvore
 * @return Altura da árvore
*/
int calcula_altura(NO *no) {
   if (no == NULL)
      return 0;


   int altura_esq = calcula_altura(no->esq);
   int altura_dir = calcula_altura(no->dir);

   // Retorna a altura máxima entre as subárvores
   return 1 + ((altura_esq > altura_dir) ? altura_esq : altura_dir);
}

/**
 * Calcula recursivamente a quantidade de nós da árvore
 * 
 * @param no (NO *) no raiz da arvore/subarvore
 * @return Número total de nós da árvore
*/
int num_total_nos(NO *no) {
   if (no == NULL)
      return 0;

   // Calcula o total de nós nas subárvores
   int no_esq = num_total_nos(no->esq);
   int no_dir = num_total_nos(no->dir);

   // Retorna o número total de nós
   return 1 + no_esq + no_dir;
}

/**
 * Printa a árvore binária com a técnica pre ordem
 * 
 * @param raiz (NO *) raiz da árvore
*/
void pre_ordem(NO *raiz) {
   if (!raiz)
      return;
   printa_no(raiz, " ");
   pre_ordem(raiz->esq);
   pre_ordem(raiz->dir);
}

/**
 * Printa a árvore binária com a técnica em ordem
 * 
 * @param raiz (NO *) raiz da árvore
*/
void em_ordem(NO *raiz) {
   if (!raiz)
      return;
   em_ordem(raiz->esq);
   printa_no(raiz, " ");
   em_ordem(raiz->dir);
}

/**
 * Printa a árvore binária com a técnica pos ordem
 * 
 * @param raiz (NO *) raiz da árvore
*/
void pos_ordem(NO *raiz) {
   if (!raiz)
      return;

   pos_ordem(raiz->esq);
   pos_ordem(raiz->dir);
   printa_no(raiz, " ");
}

/**
 * Remove da árvore rubro negra um nó em específico. Lembrando de manter as propriedades.
 * 
 * @param raiz (NO **) raiz da árvore
 * @param elem (int) elemento chave do nó a ser removido
 * 
 * @return TRUE caso o nó tenha sido encontrado e removido e FALSE c.c.
*/
boolean deleta_no(NO **raiz, char ordem[71], char chave[6], char titulo[71], int rrn) {
   NO *aux = *raiz;

   while (TRUE) { // Busca o elemento na árvore
      if (strcmp(ordem, aux->ordem) == 0)
         break; // Nó encontrado

      if (strcmp(ordem, aux->ordem) > 0) {
         if (aux->dir)
            aux = aux->dir;
         else
            return FALSE; // Nó não encontrado
      } else {
         if (aux->esq)
            aux = aux->esq;
         else
            return FALSE; // Nó não encontrado
      }
   }

   NO *removido = aux;

   if (removido->esq) { // Busca o maior dos menores
      removido = removido->esq;
      while (removido->dir)
         removido = removido->dir;
   } else if (removido->dir) { // Ou busca o menor dos maiores
      removido = removido->dir;
      while (removido->esq)
         removido = removido->esq;
   }

   if (removido == *raiz) { // Remove a raíz
      *raiz = NULL;
      return TRUE;
   }


   strcpy(aux->ordem, removido->ordem);
   strcpy(aux->chave, removido->chave);
   strcpy(aux->titulo, removido->titulo);
   aux->rrn = removido->rrn;

   strcpy(removido->ordem, ordem);
   strcpy(removido->chave, chave);
   strcpy(removido->titulo, titulo);
   removido->rrn = rrn;

   if (verifica_caso1(removido)) {
      if (!removido->esq && !removido->dir) { // Removido é folha
         if (removido->pai->esq == removido) {
            removido->pai->esq = NULL; // REMOVEU
            return TRUE;
         } else {
            removido->pai->dir = NULL; // REMOVEU
            return TRUE;
         }
      } else { // Filho deve ser vermelho
         if (removido->esq) {
            removido->pai->dir = removido->esq;
            removido->esq->pai = removido->pai;
            removido->esq->cor = VERMELHO; // Mudamos a cor para vermelho
         } else {
            removido->pai->esq = removido->dir;
            removido->dir->pai = removido->pai;
            removido->dir->cor = VERMELHO; // Mudamos a cor para vermelho
         }
      }

      free(removido); // Desaloca da memória
      return TRUE;
   }

   verifica_caso2(removido, TRUE, (removido->pai->dir == removido), raiz);
   return TRUE;
}

/**
 * Verifica se o no que estamos removendo da árvore contém as propriedades do caso 1
 * 
 * @param removido (NO *) no a ser removido
 * 
 * @return TRUE caso o nó se enquadre no caso 1 e FALSE c.c.
*/
boolean verifica_caso1(NO *removido) {
   if (removido->cor == VERMELHO || (removido->esq && removido->esq->cor == VERMELHO) ||
       (removido->dir && removido->dir->cor == VERMELHO))
      return TRUE;
   else
      return FALSE;
}

/**
 * Verifica se o no que estamos removendo da árvore contém as propriedades do caso 2.
 * Caso positivo ajustamos suas cores de acordo com as propriedades
 * 
 * @param removido (NO *) no a ser removido
 * @param remover (boolean) se estamos removendo o elemento da árvore
 * @param direita (boolean) se devemos olhar para direita ou esquerda do no
 * @param raiz (NO **) raiz original da árvore
*/
void verifica_caso2(NO *removido, boolean remover, boolean direita, NO **raiz) {
   if (removido == (*raiz)) {
      (*raiz)->cor = PRETO;
      return;
   }

   if (!remover && removido->cor == VERMELHO) {
      if (!direita) {
         if (removido->dir)
            removido->dir->cor = VERMELHO;
      } else {
         if (removido->esq)
            removido->esq->cor = VERMELHO;
      }

      removido->cor = 0;
      return;
   }

   NO *irmao;
   NO *pai = removido->pai;
   boolean removidoDir = FALSE; // TRUE quando o nó a ser removido está na direita do seu pai, FALSE c.c.
   if (pai->dir == removido) {
      irmao = pai->esq;
      removidoDir = TRUE;
   } else
      irmao = pai->dir;

   if ((irmao->dir && irmao->dir->cor == VERMELHO) ||
       (irmao->esq && irmao->esq->cor == VERMELHO)) {

      int corPai = pai->cor; // Precisamos salvar esse valor pois o nó pai ira mudar

      if (irmao->dir && irmao->dir->cor == VERMELHO) {
         if (removidoDir) { // irmão ta na esquerda e filho na direita -> Rotação esquerda direita

            // Primeiro passo: rotação esquerda do irmão
            irmao = rotacao_esq(irmao->dir);

            // Segundo passo: rotação direita do irmão
            pai = rotacao_dir(irmao);

            // Verifica se a raíz foi rotacionada
            if (!pai->pai)
               *raiz = pai;

            // Terceiro passo: atualizar as cores
            pai->cor = corPai;
            pai->esq->cor = PRETO;
            pai->dir->cor = PRETO;

            // Remover o nó "pai->dir->dir"
            if (remover) {
               if (removido->esq)
                  removido->esq->pai = pai->dir;

               pai->dir->dir = removido->esq;
               free(removido);
            }
         } else { // irmão e filho estão na esquerda -> Rotação esquerda esquerda

            // Primeiro passo: rotação esquerda do irmão
            pai = rotacao_esq(irmao);

            // Verifica se a raíz foi rotacionada
            if (!pai->pai)
               *raiz = pai;

            // Segundo passo: atualiza cores
            pai->cor = corPai;
            pai->esq->cor = PRETO;
            pai->dir->cor = PRETO;

            if (remover) {
               if (removido->dir)
                  removido->dir->pai = pai->esq;

               pai->esq->esq = removido->esq;
               free(removido);
            }
         }
      } else {

         if (!removidoDir) { // irmão ta na direita e filho na esquerda -> Rotação direita esquerda

            // Primeiro passo: rotação direita no irmão
            irmao = rotacao_dir(irmao->esq);

            // Segundo passo: rotação esquerda no irmão
            pai = rotacao_esq(irmao);

            // Verifica se a raíz foi rotacionada
            if (!pai->pai)
               *raiz = pai;

            // Terceiro passo: atualizar as cores
            pai->cor = corPai;
            pai->esq->cor = PRETO;
            pai->dir->cor = PRETO;

            // Remover o nó "pai->esq->esq"
            if (remover) {
               if (removido->dir)
                  removido->dir->pai = pai->esq;

               pai->esq->esq = removido->dir;
               free(removido);
            }
         } else { // irmão e filho estão na esquerda -> Rotação direita direita

            // Primeiro passo: rotação direita no irmão;
            pai = rotacao_dir(irmao);

            // Verifica se a raíz foi rotacionada
            if (!pai->pai)
               *raiz = pai;

            // Segundo passo: atualizar as cores
            pai->cor = corPai;
            pai->esq->cor = PRETO;
            pai->dir->cor = PRETO;

            // Remover o nó "pai->dir->dir"
            if (remover) {
               if (removido->esq)
                  removido->esq->pai = pai->dir;

               pai->dir->dir = removido->esq;
               free(removido);
            }
         }
      }
   } else if (irmao->cor == PRETO) { // Caso irmão for vermelho

      // Recolore o irmão
      irmao->cor = VERMELHO;

      if (remover) { // Remover quando necessário
         if (removidoDir) {
            removido->pai->dir = removido->esq;
            if (removido->esq)
               removido->esq->pai = removido->pai;
         } else {
            removido->pai->esq = removido->dir;
            if (removido->dir)
               removido->dir->pai = removido->pai;
         }
      }

      verifica_caso2(pai, FALSE, removidoDir, raiz);
   } else {
      if (removidoDir) {
         removido->pai->dir = removido->esq;
         if (removido->esq)
            removido->esq->pai = removido->esq;

         pai = rotacao_dir(irmao);

         // Verifica se a raíz foi rotacionada
         if (!pai->pai)
            *raiz = pai;

         pai->cor = PRETO;
         pai->dir->cor = VERMELHO;

         verifica_caso2(pai->dir, FALSE, TRUE, raiz);
      } else {
         removido->pai->esq = removido->dir;
         if (removido->dir)
            removido->dir->pai = removido->pai;

         pai = rotacao_esq(irmao);

         // Verifica se a raíz foi rotacionada
         if (!pai->pai)
            *raiz = pai;

         pai->cor = PRETO;
         pai->esq->cor = VERMELHO;
         verifica_caso2(pai->esq, FALSE, FALSE, raiz);
      }
   }
}

/**
 * Função principal de inserção de nós na árvore. Lembrando de manter suas propriedades.
 * 
 * @param raiz (NO **) raiz principal da árvore 
 * @param elem (int) elemento chave a ser inserido
 * @return TRUE caso foi possível inserir na árvore ou FALSE c.c.
*/
boolean insere_no(NO **raiz, char ordem[71], char chave[6], char titulo[71], int rrn) {

    if ((*raiz) == NULL) {
        *raiz = novo_no(ordem, chave, titulo, rrn, *raiz);
        return TRUE;
    }

   NO *busca = *raiz;
   while (busca) {
      if (strcmp(busca->ordem, ordem) > 0) {
         if (busca->esq)
            busca = busca->esq;
         else {
            NO *novo = novo_no(ordem, chave, titulo, rrn, busca);
            busca->esq = novo;
            busca = novo;

            break;
         }
      } else if (strcmp(busca->ordem, ordem) < 0) {
         if (busca->dir)
            busca = busca->dir;
         else {
            NO *novo = novo_no(ordem, chave, titulo, rrn, busca);
            busca->dir = novo;
            busca = novo;

            break;
         }
      } else {
         return FALSE; // No ja existe
      }
   }

   // Arruma a árvore
   while (busca != *raiz) {
      checa_no_insercao(busca); //Chama a função de corrigir as propriedades da árvore
      if (!busca->pai) {
         *raiz = busca;
         break;
      }

      busca = busca->pai;
      if (busca == *raiz)
         busca->cor = PRETO;
   }

   return TRUE;
}

/**
 * Busca de forma recursiva um nó em específico, usando as propriedades da árvore binária de busca.
 * 
 * @param raiz (NO *) raiz da arvore/subarvore
 * @param elem (int) elemento chave a ser procurado
 * @return O no encontrado ou retorna NULL c.c.
*/
NO *busca_binaria(NO *raiz, char busca[66]) {
   if (!raiz || strcmp(raiz->ordem, busca) == 0)
      return raiz;

   if (strcmp(raiz->ordem, busca) > 0)
      return busca_binaria(raiz->esq, busca);
   else
      return busca_binaria(raiz->dir, busca);
}

/**
 * Função que desaloca completamante a árvore da memória
 * 
 * @param raiz (NO *) raiz da arvore/subarvore
 * @return NULL no final quando toda árvore tenha sido expurgada
*/
NO *expurgar_arvore(NO *raiz, boolean lista) {
    if (raiz == NULL)
        return NULL;

    else {
        raiz->esq = expurgar_arvore(raiz->esq, lista);
        raiz->dir = expurgar_arvore(raiz->dir, lista);
        if (lista)
            expurgar_lista(raiz->lista);
        free(raiz);
        return NULL;
    }
}

/**
 * Função que desaloca completamente a lísta da memória
 * @param lista (LISTA *) cabeça da lista
 */
void expurgar_lista(LISTA *lista) {
    LISTA *atual = lista, *prox;
    while (atual != NULL) {
        prox = atual->prox;
        free(atual);
        atual = prox;
    }
}


/**
 * Imprime horizontalmente a árvore rubro negra
 * 
 * @param no (NO *) no a ser impresso
 * @param nivel (int) nivel que estamos na árvore
*/
void imprime_arvore(NO *no, int nivel) {
   if (no == NULL) {
      return;
   }

   imprime_arvore(no->dir, nivel + 1);

   for (int i = 0; i < nivel; i++) {
      printf("    ");
   }

   printa_no(no, "\n");

   imprime_arvore(no->esq, nivel + 1);
}

/**
 * Função auxiliar que printa no terminal um nó colorido de acordo com sua cor
 * 
 * @param no (NO *) no a ser impresso
 * @param espacamento (char *) string adicional após a impressão (mandamos um " " ou "\\n")
*/
void printa_no(NO *no, char *espacamento) {
   if (no->cor == VERMELHO)
      printf(NO_VERMELHO "%s%s" LIMPA, no->ordem, espacamento);
   else
      printf(NO_PRETO "%s%s" LIMPA, no->ordem, espacamento);
}

/**
 * Passa recursivamente pela árvore contando todos os nós pretos
 * 
 * @param no (NO *) no a ser verificado
 * @return A quantidade de nós pretos
*/
int conta_nos_pretos(NO *no) {
   if (no == NULL) {
      return 0;
   }

   int contagem = (no->cor == PRETO) ? 1 : 0;  // Incrementa a contagem se o nó for preto

   contagem += conta_nos_pretos(no->esq);  // Recursivamente conta os nós pretos à esquerda
   contagem += conta_nos_pretos(no->dir);  // Recursivamente conta os nós pretos à direita

   return contagem;
}

void insere_lista(NO **no, char chave[6]) {
    LISTA *novo = malloc(sizeof (LISTA)), *anterior = NULL, *atual = (*no)->lista;
    novo->prox = NULL;
    strcpy(novo->chave, chave);

    // Percorre a lista até encontrar a posição correta de inserção
    while (atual != NULL && strcmp(atual->chave, chave) < 0) {
        anterior = atual;
        atual = atual->prox;
    }

    // Insere o novo elemento na lista
    if (anterior == NULL) {
        // Insere no início da lista
        novo->prox = (*no)->lista;
        (*no)->lista = novo;
    } else {
        // Insere no meio ou no final da lista
        anterior->prox = novo;
        novo->prox = atual;
    }
}

void remove_lista(NO **no, char chave[6]) {
    LISTA *anterior = NULL, *atual = (*no)->lista;

    // Percorre a lista até encontrar o elemento com a chave desejada
    while (atual != NULL && strcmp(atual->chave, chave) != 0) {
        anterior = atual;
        atual = atual->prox;
    }

    // Remove o elemento da lista
    if (atual != NULL) {
        if (anterior == NULL) {
            // Remove o primeiro elemento da lista
            (*no)->lista = atual->prox;
        } else {
            // Remove um elemento do meio ou do final da lista
            anterior->prox = atual->prox;
        }
        free(atual);
    }
}


