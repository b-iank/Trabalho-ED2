# Coleção de filmes B+
Neste projeto está um programa em linguagem C responsável por controlar o arquivo
de filmes de um colecionador. O progrma interage com o usuário pelo terminal de execução
para tarefas como:
- Inserção de novo filme
- Remoção de um filme
- Modificar nota de um filme
- Listagem do catálogo
- Pesquisa de filmes por chave ou título
- Compactar arquivo de filmes

O objetivo deste projeto é aprimorar o projeto anterior, implementando uma árvore B+ para armazenar
os índices primários dos filmes.

## Árvore B+

Uma árvore B+ é uma estrutura de dados em árvore usada para armazenar e recuperar informações de forma
eficiente. No projeto, foi utilizada uma árvore B+ para armazenar os índices primários do arquivo de filmes.

### Estrutura:
- Os registros são armazendos no arquivo "Files/ibtree.idx";
- A ordem escolhida para esse projeto foi 8, e cada registro tem um total de 73 bytes (caracteres);

### Registro

Os registros armazenam cada página da nossa árvore B+, e são estruturados da seguinte forma:
![](Files\Registro.png)
- Cada campo é separado pelos separadores "@" e os valores dentro do campo são separados por "#";
- O primeiro valor indica o que é tal registro (1 = folha, 0, página interna, 2 = página apagada);
- Os valores no segundo campos são as chaves primárias dos filmes;
- Os valores no terceiro campo são os RRNs tanto para as páginas filhas (quando a página for interna), 
quanto para os RRNs no arquivo de dados (quando a página for folha), note que nas folhas o último RRN 
também é preenchido, indicando o apontamento para a próxima folha;
- Por fim o último campo se trata do RRN da página pai;
- Os '*' representam um valor em branco.

## Árvore AVL
Foi utilizado algoritmos de árvore AVL, abordados na matéria de Estrutura de Dados I, 
para armazenar em memória os valores dos índices secundários. Ao final da execução do 
programa, a árvore é escrita no arquivo "Files/itilte.idx".

## Lista dinâmica
Assim como a árvore binária, a estrutura de lista dinâmica também foi ensinada nas aulas de Algoritmo e Técnicas de Programação II
e utilizada neste projeto para armazenar as chaves primárias ná árvore AVL, uma vez que filmes
podem ter o mesmo nome (chave secundária) porém ‘IDs’ diferentes.

## Observações
1. Para garantir o correto funcionamento do programa, é necessário incluir os arquivos, caso existam, na pasta "Files". Além disso, 
é importante evitar o uso de sinais de acentuação nos valores do filme, pois o programa só pode ler em UTF-8 (sem caracteres especiais).
2. Quando o programa é executado em um terminal desatualizado, podem aparecer símbolos e números que podem dificultar a leitura do programa. 
Esses símbolos e números são resultados das formatações utilizadas para tornar a saída do programa mais legível e atraente para o usuário.

## Execução

Para compilar e executar o programa, siga as etapas abaixo:

1. Abra o terminal e navegue até o diretório do projeto.
2. Execute o comando `gcc -o nome_do_programa main.c filmes.c arvore_avl/avl.c arvore_bm/arvore_bm.c` para compilar o programa.
3. Em seguida, execute o comando `./nome_do_programa` para executar o programa.

Certifique-se de ter as dependências necessárias instaladas e de atender aos requisitos mínimos do sistema antes de executar o programa.
