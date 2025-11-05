/*
 * DESAFIO: Fila de Peças Futuras + Pilha de Reserva + Trocas
 *
 * Descrição: Este programa simula o gerenciamento de peças
 * de um jogo estilo Tetris, usando uma Fila Circular (FIFO)
 * e uma Pilha (LIFO).
 *
 * Requisitos:
 * - Fila Circular (Tamanho 5)
 * - Pilha Linear (Tamanho 3)
 * - Ações: Jogar, Reservar, Usar, Troca Simples, Troca Múltipla.
 * - Fila deve ser mantida sempre cheia (quando aplicável).
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

// Funções de Ação Estratégica (RF)
void jogarPeca();
void reservarPeca();
void usarPecaReservada();
void trocarPecaAtual();  // RF Novo
void trocaMultipla();    // RF Novo

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

        // (RF) Exibe o menu de opções (Exemplo de Saída)
        printf("Opcoes disponiveis:\n");
        printf("1. Jogar peca da frente da fila\n");
        printf("2. Enviar peca da fila para a pilha de reserva\n");
        printf("3. Usar peca da pilha de reserva\n");
        printf("4. Trocar peca da frente da fila com o topo da pilha\n");
        printf("5. Trocar os 3 primeiros da fila com as 3 pecas da pilha\n");
        printf("0. Sair\n");
        printf("--------------------------------------------------\n");
        printf("Opcao escolhida: ");
        
        scanf("%d", &opcao);
        limparBuffer(); // Limpa o '\n' do buffer

        switch (opcao) {
            case 1: jogarPeca(); break;
            case 2: reservarPeca(); break;
            case 3: usarPecaReservada(); break;
            case 4: trocarPecaAtual(); break;
            case 5: trocaMultipla(); break;
            case 0: printf("\nEncerrando o Tetris Stack...\n"); break;
            default:
                printf("\n[ERRO] Opcao invalida. Tente novamente.\n");
        }

        if(opcao != 0) pausarSistema();

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

    // 1. Exibir Fila Circular (Frente -> Fim)
    printf("Fila de pecas: ");
    if (filaEstaVazia()) {
        printf("[VAZIA]");
    } else {
        // Itera da posição 'fila_inicio' até dar 'fila_total' voltas
        int i = fila_inicio;
        for (int count = 0; count < fila_total; count++) {
            printf("[%c %d] ", filaDePecas[i].nome, filaDePecas[i].id);
            i = (i + 1) % TAM_FILA; // Avança o índice circularmente
        }
    }

    // 2. Exibir Pilha (Topo -> Base)
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

// --- 7. Implementação das Ações Estratégicas ---

/**
 * @brief (RF) Ação 1: Remove a peça da frente (dequeue)
 * e reabastece a fila (enqueue).
 */
void jogarPeca() {
    if (filaEstaVazia()) {
        printf("\n[AVISO] Fila vazia! Nenhuma peca para jogar.\n");
        return;
    }
    
    // 1. Remove da fila
    struct Peca pecaJogada = dequeue();
    printf("\n[ACAO] Peca jogada: [%c %d]\n", 
           pecaJogada.nome, pecaJogada.id);
    
    // 2. Adiciona nova peça (RF: Mantém fila cheia)
    enqueue(gerarPeca());
    printf("[SISTEMA] Nova peca [%c %d] entrou na fila.\n", 
           filaDePecas[(fila_fim - 1 + TAM_FILA) % TAM_FILA].nome,
           filaDePecas[(fila_fim - 1 + TAM_FILA) % TAM_FILA].id);
}

/**
 * @brief (RF) Ação 2: Move a peça da frente da fila
 * para o topo da pilha (dequeue -> push) e reabastece a fila.
 */
void reservarPeca() {
    if (pilhaEstaCheia()) {
        printf("\n[AVISO] Pilha de reserva cheia! Nao e possivel reservar.\n");
        return;
    }
    if (filaEstaVazia()) {
        printf("\n[AVISO] Fila vazia! Nenhuma peca para reservar.\n");
        return;
    }

    // 1. Remove da fila
    struct Peca pecaParaReservar = dequeue();
    
    // 2. Adiciona na pilha
    push(pecaParaReservar);
    printf("\n[ACAO] Peca reservada: [%c %d]\n", 
           pecaParaReservar.nome, pecaParaReservar.id);

    // 3. Adiciona nova peça na fila (RF: Mantém fila cheia)
    enqueue(gerarPeca());
    printf("[SISTEMA] Nova peca [%c %d] entrou na fila.\n",
           filaDePecas[(fila_fim - 1 + TAM_FILA) % TAM_FILA].nome,
           filaDePecas[(fila_fim - 1 + TAM_FILA) % TAM_FILA].id);
}

/**
 * @brief (RF) Ação 3: Remove a peça do topo da pilha (pop).
 * Esta ação NÃO reabastece a fila.
 */
void usarPecaReservada() {
    if (pilhaEstaVazia()) {
        printf("\n[AVISO] Pilha de reserva vazia! Nao ha o que usar.\n");
        return;
    }

    // 1. Remove da pilha (pop)
    struct Peca pecaUsada = pop();
    printf("\n[ACAO] Peca da reserva usada: [%c %d]\n", 
           pecaUsada.nome, pecaUsada.id);
}

/**
 * @brief (RF) Ação 4: Troca a peça da frente da fila
 * com a peça do topo da pilha, sem remover/adicionar.
 */
void trocarPecaAtual() {
    if (filaEstaVazia()) {
        printf("\n[AVISO] A fila esta vazia. Troca impossivel.\n");
        return;
    }
    if (pilhaEstaVazia()) {
        printf("\n[AVISO] A pilha esta vazia. Troca impossivel.\n");
        return;
    }

    // A troca ocorre diretamente nos arrays, sem usar push/pop/etc.
    // 'fila_inicio' aponta para a peça da frente da fila.
    // 'pilha_topo' aponta para a peça do topo da pilha.
    
    printf("\n[ACAO] Trocando [%c %d] (Frente Fila) com [%c %d] (Topo Pilha).\n",
           filaDePecas[fila_inicio].nome, filaDePecas[fila_inicio].id,
           pilhaDeReserva[pilha_topo].nome, pilhaDeReserva[pilha_topo].id);

    // Variável temporária para a troca (swap)
    struct Peca temp = filaDePecas[fila_inicio];
    filaDePecas[fila_inicio] = pilhaDeReserva[pilha_topo];
    pilhaDeReserva[pilha_topo] = temp;
}

/**
 * @brief (RF) Ação 5: Troca as 3 primeiras peças da fila
 * com as 3 peças do topo da pilha.
 */
void trocaMultipla() {
    // Validação de condição
    if (fila_total < 3) {
        printf("\n[AVISO] Troca multipla falhou: Fila precisa de no minimo 3 pecas.\n");
        return;
    }
    // (pilha_topo + 1) é o total de itens na pilha
    if ((pilha_topo + 1) < 3) {
        printf("\n[AVISO] Troca multipla falhou: Pilha precisa de no minimo 3 pecas.\n");
        return;
    }

    printf("\n[ACAO] Executando troca multipla...\n");

    // Mapeamento dos índices (NFR: Legibilidade)
    
    // Índices da Fila (Circular)
    int q_idx0 = fila_inicio; // 1ª peça (Frente)
    int q_idx1 = (fila_inicio + 1) % TAM_FILA; // 2ª peça
    int q_idx2 = (fila_inicio + 2) % TAM_FILA; // 3ª peça

    // Índices da Pilha (Linear)
    int s_idx0 = pilha_topo;     // 1ª peça (Topo)
    int s_idx1 = pilha_topo - 1; // 2ª peça
    int s_idx2 = pilha_topo - 2; // 3ª peça (mais próxima da base)

    // Variável temporária para a troca
    struct Peca temp;

    // Executa a troca (Swap) 1: (Frente da Fila <-> Topo da Pilha)
    temp = filaDePecas[q_idx0];
    filaDePecas[q_idx0] = pilhaDeReserva[s_idx0];
    pilhaDeReserva[s_idx0] = temp;

    // Executa a troca (Swap) 2: (2ª da Fila <-> 2ª da Pilha)
    temp = filaDePecas[q_idx1];
    filaDePecas[q_idx1] = pilhaDeReserva[s_idx1];
    pilhaDeReserva[s_idx1] = temp;

    // Executa a troca (Swap) 3: (3ª da Fila <-> 3ª da Pilha)
    temp = filaDePecas[q_idx2];
    filaDePecas[q_idx2] = pilhaDeReserva[s_idx2];
    pilhaDeReserva[s_idx2] = temp;
    
    printf("A troca foi realizada com sucesso.\n");
}


// --- 8. Funções de Estrutura de Dados (Fila) ---

/**
 * @brief Adiciona um item ao FIM da fila circular.
 */
void enqueue(struct Peca novaPeca) {
    if (filaEstaCheia()) { return; } // Segurança
    filaDePecas[fila_fim] = novaPeca;
    fila_fim = (fila_fim + 1) % TAM_FILA; // Avança o fim circularmente
    fila_total++;
}

/**
 * @brief Remove e retorna um item do INÍCIO da fila circular.
 */
struct Peca dequeue() {
    struct Peca pecaRemovida = filaDePecas[fila_inicio]; // Pega a peça
    fila_inicio = (fila_inicio + 1) % TAM_FILA; // Avança o início circularmente
    fila_total--;
    return pecaRemovida;
}

int filaEstaCheia() { return (fila_total == TAM_FILA); }
int filaEstaVazia() { return (fila_total == 0); }


// --- 9. Funções de Estrutura de Dados (Pilha) ---

/**
 * @brief Adiciona um item ao TOPO da pilha.
 */
void push(struct Peca pecaReservada) {
    if (pilhaEstaCheia()) { return; } // Segurança
    pilha_topo++; // Move o topo para cima
    pilhaDeReserva[pilha_topo] = pecaReservada;
}

/**
 * @brief Remove e retorna um item do TOPO da pilha.
 */
struct Peca pop() {
    struct Peca pecaRemovida = pilhaDeReserva[pilha_topo];
    pilha_topo--; // Move o topo para baixo
    return pecaRemovida;
}

int pilhaEstaCheia() { return (pilha_topo == TAM_PILHA - 1); }
int pilhaEstaVazia() { return (pilha_topo == -1); }


// --- 10. Funções Auxiliares (NFR) ---

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