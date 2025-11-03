/*
 * Descrição: Este programa simula a fila de "próximas peças"
 * de um jogo estilo Tetris, usando uma Fila Circular.
 *
 * Conceitos Trabalhados:
 * - Fila Circular (FIFO - First-In, First-Out)
 * - Structs e Arrays
 * - Modularização (Funções)
 */

// --- Bibliotecas Necessárias ---
#include <stdio.h>    // Para entrada e saída (printf, scanf)
#include <stdlib.h>   // Para rand(), srand() e system()
#include <time.h>     // Para time() (semente do rand)

// --- Definições Globais e Structs ---

// Define o tamanho máximo da nossa fila de peças
#define TAM_FILA 5

/**
 * @brief (FR Atributos)
 * Define a estrutura de uma peça do jogo.
 */
struct Peca {
    char nome; // Tipo da peça (ex: 'I', 'O', 'T', 'L')
    int id;    // Identificador único
};

// --- Variáveis Globais da Fila Circular ---

// O array (vetor) que armazena as peças
struct Peca filaDePecas[TAM_FILA];

// Índices que controlam a fila circular
int inicio = 0; // Aponta para o PRIMEIRO item da fila (onde sai)
int fim = 0;    // Aponta para a PRÓXIMA POSIÇÃO LIVRE (onde entra)
int totalItens = 0; // Contador para verificar se está cheia ou vazia

// --- Protótipos das Funções (Modularização) ---

// Funções de Geração de Peças
struct Peca gerarPeca();

// Funções da Fila (Conceitos)
void enqueue(struct Peca novaPeca); // Inserir nova peça (FR)
struct Peca dequeue();             // Jogar peça (FR)
void exibirFila();                 // (FR)

// Funções de Verificação (Controle)
int estaVazia();
int estaCheia();

// Funções Auxiliares (NFR Usabilidade)
void limparTela();
void pausarSistema();
void limparBuffer();
void inicializarFila(); // (FR Inicializar)

// --- Função Principal (Fluxo do Jogo) ---

int main() {
    int opcao;

    // Inicializa o gerador de números aleatórios UMA VEZ
    srand(time(NULL));

    // (FR) Inicializa a fila com o número fixo de elementos
    inicializarFila();

    // Loop principal do menu
    do {
        limparTela();
        
        // (FR) Exibe o estado atual após cada ação
        exibirFila();

        // (FR) Exemplo de menu e comandos
        printf("\nOpcoes de acao:\n");
        printf("1. Jogar peca (dequeue)\n");
        printf("2. Inserir nova peca (enqueue)\n");
        printf("0. Sair\n");
        printf("----------------------------------\n");
        printf("Escolha uma acao: ");
        
        scanf("%d", &opcao);
        limparBuffer(); // Limpa o '\n' do buffer

        switch (opcao) {
            case 1: // Jogar peça (dequeue)
                if (!estaVazia()) {
                    struct Peca pecaJogada = dequeue();
                    printf("\n[ACAO] Peca jogada: [%c %d]\n", 
                           pecaJogada.nome, pecaJogada.id);
                } else {
                    printf("\n[AVISO] A fila esta vazia. Nenhuma peca para jogar.\n");
                }
                pausarSistema();
                break;

            case 2: // Inserir nova peça (enqueue)
                if (!estaCheia()) {
                    struct Peca novaPeca = gerarPeca();
                    enqueue(novaPeca);
                    printf("\n[ACAO] Nova peca inserida: [%c %d]\n", 
                           novaPeca.nome, novaPeca.id);
                } else {
                    printf("\n[AVISO] A fila esta cheia. Jogue uma peca primeiro.\n");
                }
                pausarSistema();
                break;

            case 0:
                printf("\nSaindo do Tetris Stack...\n");
                break;

            default:
                printf("\n[ERRO] Opcao invalida. Tente novamente.\n");
                pausarSistema();
        }

    } while (opcao != 0);

    return 0;
}

// --- Implementação das Funções ---

/**
 * @brief (FR) Gera uma nova peça automaticamente.
 * Usa um ID estático para garantir que seja único.
 * (Simplificação)
 */
struct Peca gerarPeca() {
    // 'static' faz com que 'proximoId' mantenha seu valor
    // entre as chamadas da função.
    static int proximoId = 0; 
    
    char tiposDePeca[] = {'I', 'O', 'T', 'L', 'S', 'Z', 'J'};
    struct Peca nova;

    // Sorteia um tipo de peça
    nova.nome = tiposDePeca[rand() % 7]; 
    // Atribui o ID único e incrementa o contador
    nova.id = proximoId;
    proximoId++;

    return nova;
}

/**
 * @brief (FR Inicializar)
 * Preenche a fila com 5 peças iniciais.
 */
void inicializarFila() {
    printf("Inicializando fila de pecas...\n");
    for (int i = 0; i < TAM_FILA; i++) {
        struct Peca p = gerarPeca();
        enqueue(p);
    }
}

/**
 * @brief (FR) Insere uma peça no FIM da fila (enqueue).
 * Utiliza lógica circular.
 */
void enqueue(struct Peca novaPeca) {
    if (estaCheia()) {
        return; // Segurança, embora o main() já verifique
    }

    // Adiciona a peça na posição 'fim'
    filaDePecas[fim] = novaPeca;
    
    // A "mágica" da fila circular:
    // Move o 'fim' para a próxima posição,
    // voltando a 0 se chegar ao final do vetor.
    fim = (fim + 1) % TAM_FILA; 
    
    totalItens++; // Incrementa o contador
}

/**
 * @brief (FR) Remove e retorna a peça do INÍCIO da fila (dequeue).
 * Utiliza lógica circular.
 */
struct Peca dequeue() {
    if (estaVazia()) {
        // Retorna uma peça "vazia" simbólica em caso de erro
        // (A lógica principal em main() evita isso)
        struct Peca vazia = {' ', -1};
        return vazia; 
    }

    // Pega a peça da posição 'inicio'
    struct Peca pecaRemovida = filaDePecas[inicio];

    // A "mágica" da fila circular:
    // Move o 'inicio' para a próxima posição,
    // voltando a 0 se chegar ao final do vetor.
    inicio = (inicio + 1) % TAM_FILA;
    
    totalItens--; // Decrementa o contador

    return pecaRemovida;
}

/**
 * @brief (FR) Exibe o estado atual da fila.
 * (NFR Usabilidade)
 */
void exibirFila() {
    printf("==================================\n");
    printf("      Fila de Pecas (Tetris Stack)  \n");
    printf("==================================\n");

    if (estaVazia()) {
        printf("Fila de pecas: [VAZIA]\n");
        return;
    }

    // (Exemplo de Saída)
    printf("Fila de pecas: ");
    
    // Para exibir uma fila circular, não podemos
    // simplesmente iterar de 0 a totalItens.
    // Devemos começar em 'inicio' e dar 'totalItens' voltas.
    int i = inicio;
    for (int count = 0; count < totalItens; count++) {
        printf("[%c %d] ", filaDePecas[i].nome, filaDePecas[i].id);
        
        // Avança o índice 'i' de forma circular
        i = (i + 1) % TAM_FILA;
    }
    printf("\n");
}

// --- Funções de Verificação (Controle) ---

/**
 * @brief Verifica se a fila está vazia.
 */
int estaVazia() {
    return (totalItens == 0);
}

/**
 * @brief Verifica se a fila está cheia.
 */
int estaCheia() {
    return (totalItens == TAM_FILA);
}


// --- Funções Auxiliares (NFR Usabilidade) ---

/**
 * @brief Limpa a tela do console (portátil).
 */
void limparTela() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

/**
 * @brief Pausa a execução até o usuário pressionar Enter.
 */
void pausarSistema() {
    printf("\nPressione Enter para continuar...");
    limparBuffer();
}

/**
 * @brief Limpa o buffer de entrada (stdin).
 * Necessário após scanf() para consumir o '\n' (Enter).
 */
void limparBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}