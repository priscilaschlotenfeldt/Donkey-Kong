#include <raylib.h> // colocar "" se ñ for
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

// cada pixel agorá serão equivalente a um quadrado de 38 por 38
#define TAMANHO 38

#define GRAVIDADE (-50 * TAMANHO)

#define FORCA_DO_PULO (10 * TAMANHO)

#define LINHA 30

#define COLUNA 30

#define QUANT_mapas 5

typedef struct
{
    char tipo;
    int coluna;
    int linha;
} objeto;

typedef struct
{
    double coluna;
    double linha;
} personagem;

typedef struct
{
    double coluna;
    double linha;
    char mov;
} vilao;

typedef struct{
    Texture2D Plataforma;
    Texture2D Parede;
    Texture2D Escada;
    Texture2D Objetivo;
    Texture2D Safezone;
    Texture2D Spawnpoint;
}TEX;

int chao = 0;
int escada = 0;
int invulnerabilidade = (60 * 3);
double velocidadeY = 0;
double velocidadeYb = 0;
double tempo = 0;
int morte;
int vidas;
int vmax = 30 * TAMANHO;
bool pausado = false;

personagem mario;

personagem spawnpoint;

personagem objetivo;

vilao inimigo_morto = {(-100), (-100), (' ')};

int pontos = 0;

char mapa[LINHA][COLUNA + 3]; // +3 para \n(*2 por causa do win) e para o \0


Texture2D CarregaTex(const char *Path) {
Image image = LoadImage(Path);     // Carrega do disco para a RAM
ImageResize(&image, TAMANHO, TAMANHO);    // Redimensiona a imagem na RAM
Texture2D tex = LoadTextureFromImage(image); // Envia para a GPU
UnloadImage(image);                    // Limpa a RAM
return tex;
}


void fazchao(objeto *m){
    // Pergunta se o mario esta em cima do quadrado, então teleporta ele para cima e torna o chão verdadeiro
    if (mario.linha >= TAMANHO * (m->linha - 1) && mario.linha <= TAMANHO * (m->linha - 0.3) && mario.coluna <= TAMANHO * (m->coluna + 0.6667) && mario.coluna >= TAMANHO * (m->coluna - 0.3334))
    {
        mario.linha = TAMANHO * (m->linha - 1);
        chao = 1;
        velocidadeY = 0;
    }
    // Se o mario tenta entrar por baixo da caixa, ele é teletransportado de volta pra baixo
    if (mario.linha >= TAMANHO * (m->linha - 0.3) && mario.linha <= TAMANHO * (m->linha - 0.2) && mario.coluna <= TAMANHO * (m->coluna + 0.6667) && mario.coluna >= TAMANHO * (m->coluna - 0.3334))
    {
        mario.linha = TAMANHO * (m->linha);
    }
    // se ele tentar entrar na caixa pela esquerda ele volta pro lugar dele
    if (mario.linha >= TAMANHO * (m->linha - 0.9999999) && mario.linha <= TAMANHO * (m->linha + 0.5) && mario.coluna <= TAMANHO * (m->coluna) && mario.coluna >= TAMANHO * (m->coluna - 0.6667))
    {
        mario.coluna = TAMANHO * (m->coluna - 0.6667);
    }
    // ao contrario, direita
    if (mario.linha >= TAMANHO * (m->linha - 0.9999999) && mario.linha <= TAMANHO * (m->linha + 0.5) && mario.coluna <= TAMANHO * (m->coluna + 0.6667) && mario.coluna >= TAMANHO * (m->coluna))
    {
        mario.coluna = TAMANHO * (m->coluna + 0.6667);
    }
}

void fazparede(objeto *m)
{
    // Se ele tentar entrar na caixa(parede) pela esquerda ele volta pro lugar dele
    if (mario.linha >= TAMANHO * (m->linha - 0.9999999) && mario.linha <= TAMANHO * (m->linha + 0.5) && mario.coluna <= TAMANHO * (m->coluna) && mario.coluna >= TAMANHO * (m->coluna - 0.6667))
    {
        mario.coluna = TAMANHO * (m->coluna - 0.6667);
    }
    // ao contrario, direita
    if (mario.linha >= TAMANHO * (m->linha - 0.9999999) && mario.linha <= TAMANHO * (m->linha + 0.5) && mario.coluna <= TAMANHO * (m->coluna + 0.6667) && mario.coluna >= TAMANHO * (m->coluna))
    {
        mario.coluna = TAMANHO * (m->coluna + 0.6667);
    }
}

void fazescada(objeto *m)
{
    // espaço em que o mario pode se movimentar a vontade
    if (mario.linha >= TAMANHO * (m->linha - 1) && mario.linha <= TAMANHO * (m->linha + 1) && mario.coluna <= TAMANHO * (m->coluna + 0.6667) && mario.coluna >= TAMANHO * (m->coluna - 0.3334))
    {
        escada = 1;
        chao = 0;
        velocidadeY = 0;
    }
}

void safezone(objeto *m)
{
    // Se entrar nele, a posição some, mas salva o spawpoint nessa posição, e assim vai atualziando conforme ele vai entrando em safezones
    if ((mario.linha >= TAMANHO * (m->linha - 1)) && (mario.linha <= TAMANHO * (m->linha + 1)) && (mario.coluna <= TAMANHO * (m->coluna + 0.6667)) && mario.coluna >= TAMANHO * (m->coluna - 0.3334))
    {
        spawnpoint.linha = TAMANHO * m->linha;
        spawnpoint.coluna = TAMANHO * m->coluna;
        mapa[m->linha][m->coluna] = ' ';
    }
}

void fazinimigo(vilao *m, Texture2D *tex)
{
    // pisou matou
    if (mario.linha >= (m->linha - (TAMANHO * 1.2)) && mario.linha <= (m->linha - (TAMANHO * 0.5)) && mario.coluna <= (m->coluna + TAMANHO * (0.6667)) && mario.coluna >= (m->coluna - TAMANHO * (0.6667)))
    {
        velocidadeY = (FORCA_DO_PULO * 0.65);
        *m = inimigo_morto; // inimigo morto == constante, quando é pisado, ele é teleportado para fora de onde o mapa mostra
        pontos += 1000;     // acrescenta pontos
    }

    // encostou morreu
    // ve se o mario é invuneravel, < 3 segundos, dai ele n morre, calcula se o mario esta encostando no inimigo, ele perde uma vida e volta pro inicio
    else if ((invulnerabilidade < 0) && mario.linha >= (m->linha - (TAMANHO * 0.9999999)) && mario.linha <= (m->linha + (TAMANHO * 0.5)) && mario.coluna <= (m->coluna + (TAMANHO * 0.67)) && mario.coluna >= (m->coluna - (TAMANHO * 0.67)))
    {
        vidas--;
        mario = spawnpoint;
        invulnerabilidade = (60 * 3); // frames por segundo vezes 3, 3 segundos invuneravel
    }
    // mov dir ou troca
    // pergunta se o movimento é para x lado, ve se tem chao ou parede, se tiver ele muda de direção, se não segue em frente tal qual um robo aspirador

    if (!pausado)
    {
        if (m->mov == 'D')
        {
            int c = (int)(m->coluna / TAMANHO);
            int l = (int)(m->linha / TAMANHO);
            if (mapa[l][c + 1] == ' ' && mapa[l + 1][c + 1] == 'Z')
            {
                m->coluna += (0.05 * TAMANHO);
            }
            else
                m->mov = 'E';
        }
        // mov esq ou troca
        // mesma coisa do de cima
        else if (m->mov == 'E')
        {
            int c = (int)(m->coluna / TAMANHO);
            int l = (int)(m->linha / TAMANHO);
            if ((m->coluna > 0) && mapa[l][c] == ' ' && mapa[l + 1][c] == 'Z')
            {
                m->coluna -= (0.05 * TAMANHO);
            }
            else
                m->mov = 'D';
        }
    }

    // desenha
    DrawTexture(*tex, m->coluna, m->linha, WHITE);
}

void desenha_objeto(objeto *m, TEX *tex)
{
    if (m->tipo == ' ')
    {
        // precisa fazer nada
    }
    // se for chao
    else if (m->tipo == 'Z')
    {   
        DrawTexture(tex->Plataforma ,m->coluna * TAMANHO, m->linha * TAMANHO, WHITE);
        fazchao(m); // função
    }
    // se for parede
    else if (m->tipo == 'W')
    {
        DrawTexture(tex->Parede ,m->coluna * TAMANHO, m->linha * TAMANHO, WHITE);
        fazparede(m);
    }
    // se for escada
    else if (m->tipo == 'D' || m->tipo == 'H' || m->tipo == 'S')
    {
        
        DrawTexture(tex->Escada ,m->coluna * TAMANHO, m->linha * TAMANHO, WHITE);
        fazescada(m);
    }
    // se for o objetivo
    else if (m->tipo == 'F')
    {
        
        DrawTexture(tex->Objetivo ,m->coluna * TAMANHO, m->linha * TAMANHO, WHITE);
        objetivo.coluna = m->coluna * TAMANHO;
        objetivo.linha = m->linha * TAMANHO;
    }
    // se for uma safezone
    else if (m->tipo == 'N')
    {
        DrawTexture(tex->Safezone ,m->coluna * TAMANHO, m->linha * TAMANHO, WHITE);
        safezone(m);
    }
}

int main(void)
{
    // 1. Inicializa a janela e o áudio APENAS UMA VEZ no começo de tudo
    InitWindow(TAMANHO * COLUNA, TAMANHO * LINHA, "Donkey Kong INF (DKINF)");
    SetTargetFPS(60);

    // 2. Carrega as texturas APENAS UMA VEZ
    // Elas serão redimensionadas automaticamente para TAMANHO x TAMANHO pela sua função CarregaTex
    Texture2D texMarioCorrendo = CarregaTex("assets/images/sprites/spriteMarioCorrendo.png");
    Texture2D texMarioSubindo = CarregaTex("assets/images/sprites/spriteMarioSubindo.png");
    Texture2D texMarioPulando = CarregaTex("assets/images/sprites/spriteMarioPulando.png");
    Texture2D texInimigo = CarregaTex("assets/images/sprites/spriteInimigoFogo.png");
    Texture2D texBarril = CarregaTex("assets/images/sprites/spriteBarrilFrente.png");
    
    TEX tex = {//todos os que não se movem
        CarregaTex("assets/images/cenario/plataformaRosaUnico.png"), //plataforma
        CarregaTex("assets/images/cenario/paredeBlocoUnico.png"), //parede
        CarregaTex("assets/images/cenario/escadaMenor.png"), //escada
        CarregaTex("assets/images/cenario/porta.png"), //objetivo
        CarregaTex("assets/images/sprites/spriteBandeiraSavePoint.png"),//safezone
        CarregaTex("assets/images/sprites/spriteBandeiraSavePointConquistado.png")//SpawnpointSpawnpoint
    };

    srand(time(NULL));

    FILE *arqmapas[QUANT_mapas];
    arqmapas[0] = fopen("mapas/mapa_exemplo.txt", "r");
    arqmapas[1] = fopen("mapas/mapa_exemplo.txt", "r");
    arqmapas[2] = fopen("mapas/mapa_exemplo.txt", "r");
    arqmapas[3] = fopen("mapas/mapa_exemplo.txt", "r");
    arqmapas[4] = fopen("mapas/mapa_henrique.txt", "r");

    for(int k=0; k<QUANT_mapas; k++) {
        if (arqmapas[k] == NULL) {
            printf("Erro ao abrir o arquivo do mapa %d!\n", k);
            return 1;
        }
    }

    morte = 0;
    vidas = 5;

    // 3. Loop dos mapas
    for (int i = 0; ((i < QUANT_mapas) && !morte); i++)
    {
        for (int j = 0; j < LINHA; j++)
        {
            fgets(mapa[j], COLUNA + 3, arqmapas[i]);
        }

        int quant_inimigos = 0;
        vilao inimigos[51] = {};

        for (int l = 0; l < LINHA; l++)
        {
            for (int c = 0; c < COLUNA; c++)
            {
                if (mapa[l][c] == 'P')
                {
                    spawnpoint.coluna = c * TAMANHO;
                    spawnpoint.linha = l * TAMANHO;
                    mapa[l][c] = ' ';
                }
                else if (mapa[l][c] == 'E')
                {
                    inimigos[quant_inimigos].coluna = c * TAMANHO;
                    inimigos[quant_inimigos].linha = l * TAMANHO;
                    if (!(quant_inimigos % 2))
                        inimigos[quant_inimigos].mov = 'D';
                    else
                        inimigos[quant_inimigos].mov = 'E';
                    mapa[l][c] = ' ';
                    quant_inimigos++;
                }
            }
        }

        mario = spawnpoint;
        int probabilidade = 2;
        personagem bomba[1000] = {};
        objeto caixas[COLUNA][LINHA];

        // 4. Loop da fase (do-while)
        do
        {
            if (IsKeyPressed(KEY_TAB))
                pausado = !pausado;

            chao = 0;
            escada = 0;

            if (mario.coluna >= TAMANHO * (COLUNA - 0.6667))
                mario.coluna = TAMANHO * (COLUNA - 0.6667);
            else if (mario.coluna <= TAMANHO * (-0.3334))
                mario.coluna = TAMANHO * (-0.3334);

            BeginDrawing();
            ClearBackground(RAYWHITE);

            for (int l = 0; l < LINHA; l++)
            {
                for (int c = 0; c < COLUNA; c++)
                {
                    caixas[c][l].tipo = mapa[l][c];
                    caixas[c][l].coluna = c;
                    caixas[c][l].linha = l;
                    desenha_objeto(&caixas[c][l], &tex);
                }
            }

            for (int k = 0; k < quant_inimigos; k++)
            {
                fazinimigo(&inimigos[k], &texInimigo);
            }

            if (i != (QUANT_mapas - 1))
                for (int x = 0; x < 999; x++)
                {
                    if (probabilidade < ((x + 1) * (66 * (QUANT_mapas - 1 - i))))
                        break;
                    else if (!(probabilidade % ((x + 1) * (66 * (QUANT_mapas - 1 - i)))))
                    {
                        bomba[x].coluna = (rand() % (COLUNA * TAMANHO));
                        bomba[x].linha = (2 * TAMANHO);
                    }
                }
            else
                for (int x = 2; x < 999; x++)
                {
                    if (!(probabilidade % ((x + 1) * (60 * (QUANT_mapas - i)))))
                    {
                        bomba[x].coluna = (rand() % (COLUNA * TAMANHO));
                        bomba[x].linha = (3 * TAMANHO);
                    }
                }

            if (!(probabilidade % (1000 * (66 * (QUANT_mapas - i)))))
            {
                bomba[999].coluna = mario.coluna;
                bomba[999].linha = (2 * TAMANHO);
                probabilidade = 0;
            }

            for (int z = 0; z < 1000; z++)
            {
                if (!pausado)
                    bomba[z].linha -= velocidadeYb * tempo * 0.7;
                
                // Desenha bomba
                DrawTexture(texBarril, bomba[z].coluna, bomba[z].linha, WHITE);

                if ((invulnerabilidade < 0) && (mario.linha >= (bomba[z].linha - TAMANHO)) && (mario.linha <= (bomba[z].linha + TAMANHO)) && (mario.coluna <= (bomba[z].coluna + (TAMANHO * 0.6667))) && mario.coluna >= (bomba[z].coluna - (TAMANHO * 0.6667)))
                {
                    mario = spawnpoint;
                    vidas--;
                    invulnerabilidade = (60 * 3);
                }
            }

            if (pausado)
                DrawText("===== Pause =====", TAMANHO * COLUNA / 3, TAMANHO * LINHA / 2, TAMANHO, ORANGE);

            DrawTexture(tex.Spawnpoint, spawnpoint.coluna, spawnpoint.linha, WHITE);
            DrawTexture(texMarioCorrendo, mario.coluna, mario.linha, WHITE);

            EndDrawing();

            // 5. Lógica de saída/morte/passagem de fase USANDO BREAK
            if (vidas < 1)
            {
                morte = 1; 
                break;
            }

            if ((mario.linha >= (objetivo.linha - TAMANHO)) && (mario.linha <= (objetivo.linha + (TAMANHO * 0.5))) && (mario.coluna <= (objetivo.coluna + (TAMANHO * 0.6667))) && mario.coluna >= (objetivo.coluna - (TAMANHO * 0.3334)))
            {
                break;
            }

            if (WindowShouldClose() || IsKeyDown(KEY_ESCAPE)) {
                morte = 1;
                break;
            }

            if (!pausado)
            {
                if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
                    mario.coluna += (0.16 * TAMANHO);
                if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
                    mario.coluna -= (0.16 * TAMANHO);
                if ((IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) && (escada == 1))
                    mario.linha -= (0.06 * TAMANHO);
                if ((IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) && (escada == 1))
                    mario.linha += (0.06 * TAMANHO);
                if ((IsKeyDown(KEY_SPACE)) && (chao == 1))
                    velocidadeY = FORCA_DO_PULO;
                if ((chao == 0) && (escada == 0))
                    velocidadeY += GRAVIDADE * tempo;
                
                mario.linha -= velocidadeY * tempo;

                if (velocidadeY < -vmax) velocidadeY = -vmax;
                if (velocidadeYb < -vmax) velocidadeYb = -vmax;

                invulnerabilidade--;
                probabilidade++;
                velocidadeYb += GRAVIDADE * tempo;
                tempo = GetFrameTime();
            }

        } while (!WindowShouldClose());

        if (WindowShouldClose()) break;

    }

    // 6. Limpeza final
    for(int k=0; k<QUANT_mapas; k++) if(arqmapas[k]) fclose(arqmapas[k]);
    
    UnloadTexture(texMarioCorrendo);
    UnloadTexture(texMarioSubindo);
    UnloadTexture(texMarioPulando);
    UnloadTexture(texInimigo);
    UnloadTexture(texBarril);
    UnloadTexture(tex.Plataforma);
    UnloadTexture(tex.Parede);
    UnloadTexture(tex.Escada);
    UnloadTexture(tex.Objetivo);
    UnloadTexture(tex.Safezone);
    UnloadTexture(tex.Spawnpoint);


    CloseWindow();
    return 0;
}