#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ---------------------------------------------------------------------------
// Definição da estrutura principal
// ---------------------------------------------------------------------------
typedef struct {
    char modelo[100];     // Nome do modelo da moto
    int cilindrada;       // Cilindrada (ex: 150, 600)
    float preco;          // Preço da moto
} Motocicleta;

// ---------------------------------------------------------------------------
// Protótipos das funções
// ---------------------------------------------------------------------------
void limpaBuffer();
int tamanho(FILE *arq);
void cadastrar(FILE *arq);
void consultar(FILE *arq);
FILE *excluir(FILE *arq);               // retorna o ponteiro reaberto (ou NULL em erro)
void gerarRelatorio(FILE *arq);

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
    if (arq == NULL) return 0;
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

    if (arq == NULL) {
        printf("\nErro: arquivo não aberto para cadastro.\n");
        return;
    }

    printf("\n=== Cadastro de Motocicleta ===\n");

    printf("Modelo: ");
    fgets(m.modelo, sizeof(m.modelo), stdin);
    m.modelo[strcspn(m.modelo, "\n")] = '\0'; // Remove o \n

    printf("Cilindrada (ex: 150, 600): ");
    if (scanf("%d", &m.cilindrada) != 1) {
        printf("Entrada inválida para cilindrada.\n");
        limpaBuffer();
        return;
    }
    limpaBuffer();

    printf("Preço (R$): ");
    if (scanf("%f", &m.preco) != 1) {
        printf("Entrada inválida para preço.\n");
        limpaBuffer();
        return;
    }
    limpaBuffer();

    fseek(arq, 0, SEEK_END);
    if (fwrite(&m, sizeof(Motocicleta), 1, arq) != 1) {
        printf("Erro ao gravar no arquivo.\n");
    } else {
        fflush(arq);
        printf("\n✅ Motocicleta cadastrada com sucesso!\n");
    }
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
    if (scanf("%d", &pos) != 1) {
        printf("Entrada inválida.\n");
        limpaBuffer();
        return;
    }
    limpaBuffer();

    if (pos < 0 || pos >= total) {
        printf("\n⚠ Índice inválido! Total de registros: %d\n", total);
        return;
    }

    fseek(arq, pos * sizeof(Motocicleta), SEEK_SET);
    if (fread(&m, sizeof(Motocicleta), 1, arq) != 1) {
        printf("Erro ao ler o registro.\n");
        return;
    }

    printf("\n=== Motocicleta %d ===\n", pos);
    printf("Modelo: %s\n", m.modelo);
    printf("Cilindrada: %dcc\n", m.cilindrada);
    printf("Preço: R$ %.2f\n", m.preco);
}

// ---------------------------------------------------------------------------
// Função: excluir()
// ---------------------------------------------------------------------------
FILE *excluir(FILE *arq) {
    int pos, total;
    Motocicleta m;
    FILE *tmp;

    if (arq == NULL) {
        printf("\nErro: arquivo não aberto.\n");
        return NULL;
    }

    total = tamanho(arq);
    if (total == 0) {
        printf("\n⚠ Nenhum registro para excluir!\n");
        return arq;
    }

    printf("\nDigite o índice da motocicleta a excluir (0 até %d): ", total - 1);
    if (scanf("%d", &pos) != 1) {
        printf("Entrada inválida.\n");
        limpaBuffer();
        return arq;
    }
    limpaBuffer();

    if (pos < 0 || pos >= total) {
        printf("\n⚠ Índice inválido! Total de registros: %d\n", total);
        return arq;
    }

    // Abrir arquivo temporário
    tmp = fopen("temp.dat", "w+b");
    if (tmp == NULL) {
        printf("Erro ao criar arquivo temporário.\n");
        return arq;
    }

    // Copiar todos exceto o índice pos
    rewind(arq);
    for (int i = 0; i < total; i++) {
        if (fread(&m, sizeof(Motocicleta), 1, arq) != 1) {
            printf("Erro ao ler registro %d.\n", i);
            fclose(tmp);
            return arq;
        }
        if (i == pos) continue; // pula o registro a ser excluído
        if (fwrite(&m, sizeof(Motocicleta), 1, tmp) != 1) {
            printf("Erro ao escrever no arquivo temporário.\n");
            fclose(tmp);
            return arq;
        }
    }

    fclose(arq);
    fclose(tmp);

    // Apaga o arquivo original e renomeia o temporário
    if (remove("motocicletas.dat") != 0) {
        printf("Erro ao remover arquivo original.\n");
        // tenta reabrir o original para manter consistência
        arq = fopen("motocicletas.dat", "r+b");
        if (arq == NULL) {
            printf("Erro ao reabrir arquivo original.\n");
            return NULL;
        }
        return arq;
    }

    if (rename("temp.dat", "motocicletas.dat") != 0) {
        printf("Erro ao renomear arquivo temporário.\n");
        // tenta reabrir (se possível)
        arq = fopen("motocicletas.dat", "r+b");
        if (arq == NULL) {
            printf("Erro ao reabrir arquivo após falha de rename.\n");
            return NULL;
        }
        return arq;
    }

    // Reabrir o arquivo principal e retornar o ponteiro atualizado
    arq = fopen("motocicletas.dat", "r+b");
    if (arq == NULL) {
        printf("Erro ao reabrir arquivo principal após exclusão.\n");
        return NULL;
    }

    printf("\n🗑️ Registro %d excluído com sucesso!\n", pos);
    return arq;
}

// ---------------------------------------------------------------------------
// Função: gerarRelatorio()
// Gera um arquivo .txt contendo todos os registros em formato legível.
// ---------------------------------------------------------------------------
void gerarRelatorio(FILE *arq) {
    FILE *txt;
    Motocicleta m;
    int total = tamanho(arq);

    if (total == 0) {
        printf("\n⚠ Nenhum registro para gerar relatório!\n");
        return;
    }

    // abrir arquivo txt para escrita (sobrescreve se existir)
    txt = fopen("relatorio_motocicletas.txt", "w");
    if (txt == NULL) {
        printf("Erro ao criar relatório .txt\n");
        return;
    }

    rewind(arq);
    fprintf(txt, "Relatório de Motocicletas\n");
    fprintf(txt, "=========================\n\n");
    for (int i = 0; i < total; i++) {
        if (fread(&m, sizeof(Motocicleta), 1, arq) != 1) {
            printf("Erro ao ler registro %d durante geração do relatório.\n", i);
            fclose(txt);
            return;
        }
        fprintf(txt, "Índice: %d\n", i);
        fprintf(txt, "Modelo: %s\n", m.modelo);
        fprintf(txt, "Cilindrada: %dcc\n", m.cilindrada);
        fprintf(txt, "Preço: R$ %.2f\n", m.preco);
        fprintf(txt, "-----------------------------\n");
    }

    fclose(txt);
    printf("\n📄 Relatório gerado com sucesso: relatorio_motocicletas.txt\n");
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
        printf("4 - Excluir motocicleta por índice\n");
        printf("5 - Gerar relatório em arquivo .txt\n");
        printf("0 - Sair\n");
        printf("Escolha: ");
        if (scanf("%d", &opcao) != 1) {
            printf("Entrada inválida.\n");
            limpaBuffer();
            continue;
        }
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
            case 4:
                arq = excluir(arq); // reatribui, pois o arquivo pode ter sido recriado
                if (arq == NULL) {
                    printf("Erro: arquivo ficou inacessível após exclusão. Encerrando.\n");
                    return 1;
                }
                break;
            case 5:
                gerarRelatorio(arq);
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
