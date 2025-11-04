/*
 * DESAFIO: Fila de Peças Futuras + Pilha de Reserva
 *
 * Descrição: Este programa simula o gerenciamento de peças
 * de um jogo estilo Tetris, usando uma Fila Circular (FIFO)
 * para peças futuras e uma Pilha (LIFO) para peças reservadas.
 *
 * Requisitos:
 * - Fila Circular (Tamanho 5)
 * - Pilha Linear (Tamanho 3)
 * - Ações: Jogar (Dequeue), Reservar (Dequeue -> Push), Usar (Pop)
 * - Fila deve ser mantida sempre cheia.
 */

// --- 1. Bibliotecas Necessárias ---
#include <stdio.h>    // Para entrada e saída (printf, scanf)
#include <stdlib.h>   // Para rand(), srand() e system()
#include <time.h>     // Para time() (semente do rand)

// --- 2. Definições Globais e Structs ---

// (RF) Capacidades das estruturas
#define TAM_FILA 5
#define TAM_PILHA 3

/**
 * @brief (RF Atributos)
 * Define a estrutura de uma peça do jogo.
 */
struct Peca {
    char nome; // Tipo da peça (ex: 'I', 'O', 'T', 'L')
    int id;    // Identificador único
};

// --- 3. Variáveis Globais (Estado do Jogo) ---

// Fila Circular de Peças Futuras
struct Peca filaDePecas[TAM_FILA];
int fila_inicio = 0;    // Índice de onde a peça SAIRÁ (dequeue)
int fila_fim = 0;       // Índice de onde a peça ENTRARÁ (enqueue)
int fila_total = 0;     // Contador de itens na fila

// Pilha de Peças Reservadas
struct Peca pilhaDeReserva[TAM_PILHA];
int pilha_topo = -1;    // Índice do topo da pilha (-1 = vazia)

// --- 4. Protótipos das Funções (Modularização) ---

// Geração de Peças
struct Peca gerarPeca();

// Funções da Fila Circular
void enqueue(struct Peca novaPeca);
struct Peca dequeue();
int filaEstaCheia();
int filaEstaVazia();

// Funções da Pilha Linear
void push(struct Peca pecaReservada);
struct Peca pop();
int pilhaEstaCheia();
int pilhaEstaVazia();

// Funções do Jogo e Interface
void inicializarSistema(); // (RF)
void exibirEstado();       // (RF)

// Funções Auxiliares (NFR Usabilidade)
void limparTela();
void pausarSistema();
void limparBuffer();

// --- 5. Função Principal (Fluxo do Jogo) ---

int main() {
    int opcao;
    // Inicializa o gerador de números aleatórios UMA VEZ
    srand(time(NULL));

    // (RF) Inicializa a fila com 5 peças
    inicializarSistema();

    // Loop principal do menu
    do {
        limparTela();
        
        // (RF) Exibe o estado atual ANTES de cada ação
        exibirEstado();

        // (RF) Exibe o menu de opções
        printf("Opcoes de Acao:\n");
        printf("1. Jogar peca\n");
        printf("2. Reservar peca\n");
        printf("3. Usar peca reservada\n");
        printf("0. Sair\n");
        printf("----------------------------------\n");
        printf("Opcao: ");
        
        scanf("%d", &opcao);
        limparBuffer(); // Limpa o '\n' do buffer

        switch (opcao) {
            case 1: { // Jogar peça (RF)
                // 1. Remove da fila
                struct Peca pecaJogada = dequeue();
                printf("\n[ACAO] Peca jogada: [%c %d]\n", 
                       pecaJogada.nome, pecaJogada.id);
                
                // 2. Adiciona nova peça (RF: Mantém fila cheia)
                enqueue(gerarPeca());
                pausarSistema();
                break;
            }

            case 2: { // Reservar peça (RF)
                // 1. Verifica se a pilha tem espaço
                if (pilhaEstaCheia()) {
                    printf("\n[AVISO] Pilha de reserva cheia! Nao e possivel reservar.\n");
                } else {
                    // 2. Remove da fila
                    struct Peca pecaParaReservar = dequeue();
                    
                    // 3. Adiciona na pilha
                    push(pecaParaReservar);
                    printf("\n[ACAO] Peca reservada: [%c %d]\n", 
                           pecaParaReservar.nome, pecaParaReservar.id);

                    // 4. Adiciona nova peça na fila (RF: Mantém fila cheia)
                    enqueue(gerarPeca());
                }
                pausarSistema();
                break;
            }

            case 3: { // Usar peça reservada (RF)
                // 1. Verifica se a pilha tem peças
                if (pilhaEstaVazia()) {
                    printf("\n[AVISO] Pilha de reserva vazia! Nao ha o que usar.\n");
                } else {
                    // 2. Remove da pilha (pop)
                    struct Peca pecaUsada = pop();
                    printf("\n[ACAO] Peca da reserva usada: [%c %d]\n", 
                           pecaUsada.nome, pecaUsada.id);
                    // (Nota: Esta ação não remove da fila, 
                    // portanto não adiciona uma nova peça)
                }
                pausarSistema();
                break;
            }

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

// --- 6. Implementação das Funções do Jogo ---

/**
 * @brief (RF) Gera uma nova peça automaticamente.
 * Usa um ID estático (static) para garantir que seja único
 * a cada chamada.
 */
struct Peca gerarPeca() {
    static int proximoId = 0; 
    char tiposDePeca[] = {'I', 'O', 'T', 'L', 'S', 'Z', 'J'};
    struct Peca nova;

    // Sorteia um tipo de peça (0 a 6)
    nova.nome = tiposDePeca[rand() % 7]; 
    // Atribui o ID único e incrementa o contador para a próxima
    nova.id = proximoId++;

    return nova;
}

/**
 * @brief (RF) Inicializa o sistema, enchendo a fila
 * com o número definido de peças (5).
 */
void inicializarSistema() {
    for (int i = 0; i < TAM_FILA; i++) {
        enqueue(gerarPeca());
    }
}

/**
 * @brief (RF) Exibe o estado atual da Fila e da Pilha.
 * (NFR Usabilidade / Exemplo de Saída)
 */
void exibirEstado() {
    printf("==================================================\n");
    printf("                  TETRIS STACK                    \n");
    printf("==================================================\n");
    printf("Estado atual:\n\n");

    // 1. Exibir Fila Circular
    printf("Fila de pecas: ");
    if (filaEstaVazia()) {
        printf("[VAZIA]");
    } else {
        // Itera da posição 'inicio' até dar 'total' de voltas
        int i = fila_inicio;
        for (int count = 0; count < fila_total; count++) {
            printf("[%c %d] ", filaDePecas[i].nome, filaDePecas[i].id);
            i = (i + 1) % TAM_FILA; // Avança o índice circularmente
        }
    }

    // 2. Exibir Pilha
    printf("\nPilha de reserva (Topo -> Base): ");
    if (pilhaEstaVazia()) {
        printf("[VAZIA]");
    } else {
        // Itera do topo ('pilha_topo') até a base (0)
        for (int i = pilha_topo; i >= 0; i--) {
            printf("[%c %d] ", pilhaDeReserva[i].nome, pilhaDeReserva[i].id);
        }
    }
    printf("\n--------------------------------------------------\n");
}


// --- 7. Funções de Estrutura de Dados (Fila) ---

/**
 * @brief Adiciona um item ao FIM da fila circular.
 */
void enqueue(struct Peca novaPeca) {
    if (filaEstaCheia()) {
        // Este cenário não deve ocorrer na lógica do jogo
        // (pois só damos enqueue após um dequeue), 
        // mas é uma boa prática de defesa.
        printf("[ERRO_FILA] Fila cheia, impossivel adicionar!\n");
        return;
    }
    filaDePecas[fila_fim] = novaPeca;
    fila_fim = (fila_fim + 1) % TAM_FILA; // Avança o fim circularmente
    fila_total++;
}

/**
 * @brief Remove e retorna um item do INÍCIO da fila circular.
 */
struct Peca dequeue() {
    if (filaEstaVazia()) {
        // Defesa contra erros
        struct Peca vazia = {'?', -1};
        printf("[ERRO_FILA] Fila vazia, impossivel remover!\n");
        return vazia;
    }
    struct Peca pecaRemovida = filaDePecas[fila_inicio];
    fila_inicio = (fila_inicio + 1) % TAM_FILA; // Avança o início circularmente
    fila_total--;
    return pecaRemovida;
}

int filaEstaCheia() { return (fila_total == TAM_FILA); }
int filaEstaVazia() { return (fila_total == 0); }


// --- 8. Funções de Estrutura de Dados (Pilha) ---

/**
 * @brief Adiciona um item ao TOPO da pilha.
 */
void push(struct Peca pecaReservada) {
    if (pilhaEstaCheia()) {
        printf("[ERRO_PILHA] Pilha cheia, impossivel adicionar!\n");
        return;
    }
    pilha_topo++; // Move o topo para cima
    pilhaDeReserva[pilha_topo] = pecaReservada;
}

/**
 * @brief Remove e retorna um item do TOPO da pilha.
 */
struct Peca pop() {
    if (pilhaEstaVazia()) {
        struct Peca vazia = {'?', -1};
        printf("[ERRO_PILHA] Pilha vazia, impossivel remover!\n");
        return vazia;
    }
    struct Peca pecaRemovida = pilhaDeReserva[pilha_topo];
    pilha_topo--; // Move o topo para baixo
    return pecaRemovida;
}

int pilhaEstaCheia() { return (pilha_topo == TAM_PILHA - 1); }
int pilhaEstaVazia() { return (pilha_topo == -1); }


// --- 9. Funções Auxiliares (NFR) ---

void limparTela() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void pausarSistema() {
    printf("\nPressione Enter para continuar...");
    limparBuffer();
}

void limparBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}