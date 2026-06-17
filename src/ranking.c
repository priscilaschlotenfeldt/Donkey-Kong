#include "../include/ranking.h"
#include <stdio.h>
#include <string.h>

static void garantirNomeValido(char destino[TAM_NOME_PLACAR], const char *origem)
{
    if (origem == NULL || origem[0] == '\0')
        origem = "JOGADOR";

    strncpy(destino, origem, TAM_NOME_PLACAR - 1);
    destino[TAM_NOME_PLACAR - 1] = '\0';
}

void ordenarRanking(TIPO_PLACAR ranking[], int quantidade)
{
    for (int i = 0; i < quantidade - 1; i++)
    {
        for (int j = i + 1; j < quantidade; j++)
        {
            int trocar = 0;

            if (ranking[j].pontos > ranking[i].pontos)
                trocar = 1;
            else if (ranking[j].pontos == ranking[i].pontos && ranking[j].tempo < ranking[i].tempo)
                trocar = 1;

            if (trocar)
            {
                TIPO_PLACAR temp = ranking[i];
                ranking[i] = ranking[j];
                ranking[j] = temp;
            }
        }
    }
}

int carregarRanking(TIPO_PLACAR ranking[], int maxRegistros)
{
    if (ranking == NULL || maxRegistros <= 0)
        return 0;

    FILE *arquivo = fopen(ARQUIVO_RANKING, "rb");
    if (arquivo == NULL)
        return 0;

    int quantidade = 0;
    while (quantidade < maxRegistros && fread(&ranking[quantidade], sizeof(TIPO_PLACAR), 1, arquivo) == 1)
    {
        ranking[quantidade].nome[TAM_NOME_PLACAR - 1] = '\0';
        quantidade++;
    }

    fclose(arquivo);
    ordenarRanking(ranking, quantidade);
    return quantidade;
}

int salvarPontuacaoRanking(const char *nome, int pontos, float tempo)
{
    TIPO_PLACAR ranking[MAX_REGISTROS_RANKING + 1];
    int quantidade = carregarRanking(ranking, MAX_REGISTROS_RANKING);

    if (quantidade < 0)
        quantidade = 0;
    if (quantidade > MAX_REGISTROS_RANKING)
        quantidade = MAX_REGISTROS_RANKING;

    TIPO_PLACAR novo;
    garantirNomeValido(novo.nome, nome);
    novo.pontos = pontos;
    novo.tempo = tempo;

    ranking[quantidade] = novo;
    quantidade++;

    ordenarRanking(ranking, quantidade);

    if (quantidade > TOP_RANKING)
        quantidade = TOP_RANKING;

    FILE *arquivo = fopen(ARQUIVO_RANKING, "wb");
    if (arquivo == NULL)
        return 0;

    fwrite(ranking, sizeof(TIPO_PLACAR), quantidade, arquivo);
    fclose(arquivo);
    return 1;
}
