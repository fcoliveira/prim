/*
Prim's algorithm
Author: fcoliveira
*/

#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <time.h>

//acucar sintatico para facilitar o entendimento do codigo:

    //marcador de inicializacao da matriz de adjacencia 
    #define ARESTA_INVALIDA -1

    //marcador de inicializacao do array de vertices adicionados a AGM
    #define NO_INVALIDO -2

    //status possiveis dos vertices
    #define ADICIONADO 0
    #define NAO_ADICIONADO -1

int qtd_vertices;
int qtd_arestas;

//matriz de adjacencia p/ representacao do grafo
double **matriz_adj;

//arrays de custos e de antecessores
double *custo;
int *ant;

//array de vertices adicionados a AGM e seu contador de controle
int *S;
int nos_adicionados;

//recebe como parâmetro um ponteiro de ponteiros 
//e devolve a matriz de adjacencia (lin x col) alocada e inicializada
double** aloca_matriz(double **matriz, int lin, int col){

    int aux;

    matriz = malloc(lin * sizeof(double *));
    for(aux = 0; aux < lin; aux++){
        matriz[aux] = malloc(col * sizeof(double));
    }
    for(int i  = 0; i < lin; i++){
        for(int j = 0; j < col; j++){
            matriz[i][j] = ARESTA_INVALIDA;
        }
    }

    return matriz;
}

//carrega os dados do grafo(V, A) para uma matriz de adjacencia V*V
double** carrega_matriz(FILE *input){

    if(fscanf(input, "%d %d", &qtd_vertices, &qtd_arestas) == 2){
        
        if(qtd_vertices > 0 && qtd_arestas >= 0){

            int origem, destino;
            double custo;

            matriz_adj = aloca_matriz(matriz_adj, qtd_vertices, qtd_vertices);

            while(fscanf(input, "%d %d %lf", &origem, &destino, &custo) == 3){
                    
                matriz_adj[origem][destino] = custo; 
                //matriz simetrica
                matriz_adj[destino][origem] = custo;
            
            }

            return matriz_adj;
        }
        
    }

    //caso os dois primeiros parametros do arquivo sejam invalidos para um grafo
    return NULL;
}

//grava resultados no arquivo de saida especificado por *file_name
int escreve_saida(char *file_name){

    FILE *fpout = fopen(file_name, "w");

    double custo_AGM = 0;

    for(int i = 0; i< qtd_vertices; i++){
        custo_AGM += custo[i];
    }

    fprintf(fpout, "%.2f\n", custo_AGM);

    for(int i = 1; i < qtd_vertices; i++){
        fprintf(fpout, "%d %d\n",  ant[S[i]], S[i]);
    }

    fclose(fpout);

    return 1;
}

//verifica se um dado vertice u ja foi adicionado a AGM
int adicionado(int u){

    for(int i = 0; i < qtd_vertices; i++){
        if(S[i]==u) return ADICIONADO;
    }

    return NAO_ADICIONADO;
}

//libera a memoria alocada
void free_all(){

    free(matriz_adj);
    free(custo);
    free(ant);
    free(S);

}

//verifica os nos alcancaveis a partir dos ja adicionados a AGM e encontra a aresta de menor custo
int v_menor_custo(){

    double custo_minimo = (double)INT_MAX;
    int destino_min = NO_INVALIDO;

    int i = 0;
    int origem = 0;
    int origem_min = NO_INVALIDO;
    
    while(S[i] != NO_INVALIDO){
        origem = S[i];
  
        for(int destino = 0; destino < qtd_vertices; destino++){
            
            if(adicionado(destino) == NAO_ADICIONADO){
 
                if(matriz_adj[origem][destino] != ARESTA_INVALIDA){
                    if(custo_minimo > matriz_adj[origem][destino]){
                        custo_minimo = matriz_adj[origem][destino];
                        origem_min = origem;
                        destino_min = destino;
                    }
                }
            }

            //inversao necessaria por ser um grafo nao-direcionado (matriz simetrica)
            if(adicionado(origem) == NAO_ADICIONADO){

                if(matriz_adj[destino][origem] != ARESTA_INVALIDA){
                    if(custo_minimo > matriz_adj[destino][origem]){
                        custo_minimo = matriz_adj[destino][origem];
                        origem_min = destino;
                        destino_min = origem;
                    }
                }

            }

        }
       
        i++;
    }
    //previne a propagacao de possiveis problemas na leitura da matriz
    if(destino_min == NO_INVALIDO){
        printf("ERRO AO PERCORRER GRAFO\n");
        return NO_INVALIDO;
    }

    S[nos_adicionados] = destino_min;
    ant[destino_min] = origem_min;
    custo[destino_min] = custo_minimo;
    nos_adicionados++;

    return destino_min;
}

int main (int argc, char *argv[]){

    //arquivo de entrada
    FILE *input = fopen(argv[1], "r");

    matriz_adj = carrega_matriz(input);
    //previne arquivo invalido
    if(matriz_adj == NULL){
        fclose(input);
        return -1;
    }

    //alocacao
    custo = malloc((qtd_vertices) * sizeof(double));
    ant = malloc((qtd_vertices) * sizeof(int));
    S = malloc((qtd_vertices) * sizeof(int));

    //inicializa os arrays de suporte
    for(int i = 0; i < qtd_vertices; i++){            
        custo[i] = (double)INT_MAX;
        ant[i] = NO_INVALIDO;
        S[i] = NO_INVALIDO;
    }
       
    //escolha aleatoria do primeiro vertice para construcao da AGM
    srand(time(NULL));
    int u = rand() % (qtd_vertices);    
    S[0] = u;
    custo[u] = 0;
    
    nos_adicionados = 1; //var incrementada dentro da funcao v_menor_custo()
	
    //Enquanto nao percorrer todos os vertices (S == V)
	while(nos_adicionados != qtd_vertices){
        
        v_menor_custo();
		
    }

    printf("\n------------------Resultados------------------\n");
    printf("Arestas:\n");
  
    int custoAGM = 0;
    for(int i = 1; i < qtd_vertices; i++){
        printf("%d %d %.2f\n", ant[S[i]], S[i], custo[S[i]]);
        custoAGM += custo[S[i]];
    }
    printf("Custo AGM: %d\n", custoAGM);

    escreve_saida(argv[2]);
    fclose(input);
    free_all();

    return 0;
}