#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ---------------------------------------------------------------------------
// Definição da estrutura principal
// ---------------------------------------------------------------------------
typedef struct {
    char modelo[100];     // Nome do modelo da moto
    int cilindrada;      // Cilindrada (ex: 150, 600)
    float preco;         // Preço da moto
} Motocicleta;

// ---------------------------------------------------------------------------
// Protótipos das funções
// ---------------------------------------------------------------------------
void limpaBuffer();
int tamanho(FILE *arq);
void cadastrar(FILE *arq);
void consultar(FILE *arq);

// ---------------------------------------------------------------------------
// Função auxiliar: limpaBuffer()
// Limpa o buffer de entrada após o uso de scanf()
// ---------------------------------------------------------------------------
void limpaBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

// ---------------------------------------------------------------------------
// Função: tamanho()
// Retorna o número total de registros no arquivo
// ---------------------------------------------------------------------------
int tamanho(FILE *arq) {
    long bytes;
    fseek(arq, 0, SEEK_END);
    bytes = ftell(arq);
    rewind(arq);
    return (int)(bytes / sizeof(Motocicleta));
}

// ---------------------------------------------------------------------------
// Função: cadastrar()
// Lê os dados de uma nova motocicleta e grava no final do arquivo
// ---------------------------------------------------------------------------
void cadastrar(FILE *arq) {
    Motocicleta m;

    printf("\n=== Cadastro de Motocicleta ===\n");

    printf("Modelo: ");
    fgets(m.modelo, sizeof(m.modelo), stdin);
    m.modelo[strcspn(m.modelo, "\n")] = '\0'; // Remove o \n

    printf("Cilindrada (ex: 150, 600): ");
    scanf("%d", &m.cilindrada);
    limpaBuffer();

    printf("Preço (R$): ");
    scanf("%f", &m.preco);
    limpaBuffer();

    fseek(arq, 0, SEEK_END);
    fwrite(&m, sizeof(Motocicleta), 1, arq);
    fflush(arq);

    printf("\n✅ Motocicleta cadastrada com sucesso!\n");
}

// ---------------------------------------------------------------------------
// Função: consultar()
// Exibe os dados de uma motocicleta com base no índice informado
// ---------------------------------------------------------------------------
void consultar(FILE *arq) {
    int pos;
    Motocicleta m;
    int total = tamanho(arq);

    if (total == 0) {
        printf("\n⚠ Nenhum registro encontrado!\n");
        return;
    }

    printf("\nDigite o índice da motocicleta (0 até %d): ", total - 1);
    scanf("%d", &pos);
    limpaBuffer();

    if (pos < 0 || pos >= total) {
        printf("\n⚠ Índice inválido! Total de registros: %d\n", total);
        return;
    }

    fseek(arq, pos * sizeof(Motocicleta), SEEK_SET);
    fread(&m, sizeof(Motocicleta), 1, arq);

    printf("\n=== Motocicleta %d ===\n", pos);
    printf("Modelo: %s\n", m.modelo);
    printf("Cilindrada: %dcc\n", m.cilindrada);
    printf("Preço: R$ %.2f\n", m.preco);
}

// ---------------------------------------------------------------------------
// Função principal
// ---------------------------------------------------------------------------
int main() {
    FILE *arq;
    int opcao;

    // Tenta abrir o arquivo existente, ou cria um novo
    arq = fopen("motocicletas.dat", "r+b");
    if (arq == NULL) {
        arq = fopen("motocicletas.dat", "w+b");
        if (arq == NULL) {
            printf("Erro ao abrir ou criar o arquivo!\n");
            return 1;
        }
    }

    do {
        printf("\n===== MENU PRINCIPAL =====\n");
        printf("1 - Cadastrar motocicleta\n");
        printf("2 - Consultar motocicleta por índice\n");
        printf("3 - Mostrar quantidade de registros\n");
        printf("0 - Sair\n");
        printf("Escolha: ");
        scanf("%d", &opcao);
        limpaBuffer();

        switch (opcao) {
            case 1:
                cadastrar(arq);
                break;
            case 2:
                consultar(arq);
                break;
            case 3:
                printf("\n📦 Total de registros: %d\n", tamanho(arq));
                break;
            case 0:
                printf("\nEncerrando o programa...\n");
                break;
            default:
                printf("\n⚠ Opção inválida! Tente novamente.\n");
        }
    } while (opcao != 0);

    fclose(arq);
    return 0;
}
