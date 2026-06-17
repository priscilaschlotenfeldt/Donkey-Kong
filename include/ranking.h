#ifndef RANKING_H
#define RANKING_H

#define ARQUIVO_RANKING "placar.bin"
#define TAM_NOME_PLACAR 32
#define TOP_RANKING 10
#define MAX_REGISTROS_RANKING 100

typedef struct tipo_placar
{
    char nome[TAM_NOME_PLACAR];
    float tempo;
    int pontos;
} TIPO_PLACAR;

int carregarRanking(TIPO_PLACAR ranking[], int maxRegistros);
void ordenarRanking(TIPO_PLACAR ranking[], int quantidade);
int salvarPontuacaoRanking(const char *nome, int pontos, float tempo);

#endif
