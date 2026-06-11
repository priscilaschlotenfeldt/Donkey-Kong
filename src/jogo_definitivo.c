#include <raylib.h>// colocar "" se ñ for
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

//cada pixel agorá serão equivalente a um quadrado de 38 por 38
#define TAMANHO 38

#define GRAVIDADE (-50 * TAMANHO)

#define FORCA_DO_PULO (10 * TAMANHO)

#define LINHA 30

#define COLUNA 30

#define QUANT_mapas 5

typedef struct{
    char tipo;
    int coluna;
    int linha;
}objeto;

typedef struct{
    double coluna;
    double linha;
}personagem;

typedef struct{
    double coluna;
    double linha;
    char mov; 
}vilao;

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

void fazchao(objeto *m){
    //Pergunta se o mario esta em cima do quadrado, então teleporta ele para cima e torna o chão verdadeiro
    if(mario.linha >= TAMANHO * (m->linha - 1) && mario.linha <= TAMANHO * (m->linha - 0.3) && mario.coluna <= TAMANHO * (m->coluna + 0.6667) && mario.coluna >= TAMANHO * (m->coluna - 0.3334)){
        mario.linha = TAMANHO * (m->linha - 1);
        chao = 1;
        velocidadeY = 0;
    }
    //Se o mario tenta entrar por baixo da caixa, ele é teletransportado de volta pra baixo
    if(mario.linha >= TAMANHO * (m->linha - 0.3) && mario.linha <= TAMANHO * (m->linha - 0.2) && mario.coluna <= TAMANHO * (m->coluna + 0.6667) && mario.coluna >= TAMANHO * (m->coluna - 0.3334)){
        mario.linha = TAMANHO * (m->linha);
    }
    //se ele tentar entrar na caixa pela esquerda ele volta pro lugar dele
    if(mario.linha >= TAMANHO * (m->linha - 0.9999999) && mario.linha <= TAMANHO * (m->linha + 0.5) && mario.coluna <= TAMANHO * (m->coluna) && mario.coluna >= TAMANHO * (m->coluna - 0.6667)){
        mario.coluna = TAMANHO * (m->coluna - 0.6667);
    }
    //ao contrario, direita
    if(mario.linha >= TAMANHO * (m->linha - 0.9999999) && mario.linha <= TAMANHO * (m->linha + 0.5) && mario.coluna <= TAMANHO * (m->coluna + 0.6667) && mario.coluna >= TAMANHO * (m->coluna)){
        mario.coluna = TAMANHO * (m->coluna + 0.6667);
    }
}

void fazparede(objeto *m){
    //Se ele tentar entrar na caixa(parede) pela esquerda ele volta pro lugar dele
    if(mario.linha >= TAMANHO * (m->linha - 0.9999999) && mario.linha <= TAMANHO * (m->linha + 0.5) && mario.coluna <= TAMANHO * (m->coluna) && mario.coluna >= TAMANHO * (m->coluna - 0.6667)){
        mario.coluna = TAMANHO * (m->coluna - 0.6667);
    }
    //ao contrario, direita
    if(mario.linha >= TAMANHO * (m->linha - 0.9999999) && mario.linha <= TAMANHO * (m->linha + 0.5) && mario.coluna <= TAMANHO * (m->coluna + 0.6667) && mario.coluna >= TAMANHO * (m->coluna)){
        mario.coluna = TAMANHO * (m->coluna + 0.6667);
    }
}

void fazescada(objeto *m){
    //espaço em que o mario pode se movimentar a vontade
    if(mario.linha >= TAMANHO * (m->linha - 1) && mario.linha <= TAMANHO * (m->linha + 1) && mario.coluna <= TAMANHO * (m->coluna + 0.6667) && mario.coluna >= TAMANHO * (m->coluna - 0.3334)){
        escada = 1;
        chao = 0; 
        velocidadeY = 0;
    }
}


void safezone(objeto *m){
    //Se entrar nele, a posição some, mas salva o spawpoint nessa posição, e assim vai atualziando conforme ele vai entrando em safezones
    if((mario.linha >= TAMANHO * (m->linha - 1)) && (mario.linha <= TAMANHO * (m->linha + 1)) && (mario.coluna <= TAMANHO * (m->coluna + 0.6667)) && mario.coluna >= TAMANHO * (m->coluna - 0.3334)){
        spawnpoint.linha = TAMANHO * m->linha;
        spawnpoint.coluna = TAMANHO * m->coluna;
        mapa[m->linha][m->coluna] = ' ';
    }
}


void fazinimigo(vilao *m){
    //pisou matou
    if(mario.linha >= (m->linha - (TAMANHO * 1.2)) && mario.linha <=  (m->linha - (TAMANHO * 0.5)) && mario.coluna <= (m->coluna + TAMANHO * (0.6667)) && mario.coluna >= (m->coluna - TAMANHO * (0.6667))){
        velocidadeY = (FORCA_DO_PULO * 0.75);
        *m = inimigo_morto; //inimigo morto == constante, quando é pisado, ele é teleportado para fora de onde o mapa mostra
        pontos += 1000; //acrescenta pontos
    }

    //encostou morreu
    //ve se o mario é invuneravel, < 3 segundos, dai ele n morre, calcula se o mario esta encostando no inimigo, ele perde uma vida e volta pro inicio
    else if((invulnerabilidade < 0) && mario.linha >= (m->linha - (TAMANHO * 0.9999999)) && mario.linha <= (m->linha + (TAMANHO * 0.5)) && mario.coluna <= (m->coluna + (TAMANHO * 0.67)) && mario.coluna >= (m->coluna - (TAMANHO * 0.67))){
        vidas--;
        mario = spawnpoint;
        invulnerabilidade = (60 * 3); //frames por segundo vezes 3, 3 segundos invuneravel
    }                
    //mov dir ou troca
    //pergunta se o movimento é para x lado, ve se tem chao ou parede, se tiver ele muda de direção, se não segue em frente tal qual um robo aspirador
    
    if(!pausado){
        if(m->mov == 'D'){
            int c = (int)(m->coluna / TAMANHO);
            int l = (int)(m->linha / TAMANHO);
            if(mapa[l][c + 1] == ' ' && mapa[l + 1][c + 1] == 'Z'){
                m->coluna += (0.05 * TAMANHO);
            }
            else
                m->mov = 'E';
        }
        //mov esq ou troca
        //mesma coisa do de cima
        else if(m->mov == 'E'){
            int c = (int)(m->coluna / TAMANHO);
            int l = (int)(m->linha / TAMANHO);
            if((m->coluna > 0) && mapa[l][c] == ' ' && mapa[l + 1][c] == 'Z'){
                m->coluna -= (0.05 * TAMANHO);
            }
            else
                m->mov = 'D';
        }
    }

    //desenha
    DrawRectangle(m->coluna, m->linha, TAMANHO, TAMANHO, RED);
}


void desenha_objeto(objeto *m){
    if(m->tipo == ' '){
        //precisa fazer nada
    }
    //se for chao
    else if(m->tipo == 'Z'){
        DrawRectangle(m->coluna * TAMANHO, m->linha * TAMANHO, TAMANHO, TAMANHO, BLACK);
        fazchao(m); //função
    }
    //se for parede
    else if(m->tipo == 'W'){
        DrawRectangle(m->coluna * TAMANHO, m->linha * TAMANHO, TAMANHO, TAMANHO, BROWN);
        fazparede(m);
    }
    //se for escada
    else if(m->tipo == 'D' || m->tipo == 'H' || m->tipo == 'S'){
        DrawRectangle(m->coluna * TAMANHO, m->linha * TAMANHO, TAMANHO, TAMANHO, BLUE);
        fazescada(m);
    }
    //se for o objetivo
    else if(m->tipo == 'F'){
        DrawRectangle(m->coluna * TAMANHO, m->linha * TAMANHO, TAMANHO, TAMANHO, GREEN);
        objetivo.coluna = m->coluna * TAMANHO;
        objetivo.linha = m->linha * TAMANHO;
    }
    //se for uma safezone
    else if(m->tipo == 'N'){
        DrawRectangle(m->coluna * TAMANHO, m->linha * TAMANHO, TAMANHO, TAMANHO, YELLOW);
        safezone(m);
    }
}

int main(void){

    srand(time(NULL));//serve para posicionar aleatoriamente os barris que caem 

    FILE *arqmapas[QUANT_mapas]; // array de mapas para cada mapa ser acessado por um arquivo diferente 

    //Aqui eu estou pegando os arquivos txt e trazendo para o programa
    
    arqmapas[0] = fopen("mapas/mapa_exemplo.txt", "r");
    if (arqmapas[0] == NULL) {
        printf("Erro ao abrir o arquivo do mapa!\n");
        return 1;
    }

    
    arqmapas[1] = fopen("mapas/mapa_exemplo.txt", "r");
    if (arqmapas[1] == NULL) {
        printf("Erro ao abrir o arquivo do mapa!\n");
        return 1;
    }


    arqmapas[2] = fopen("mapas/mapa_exemplo.txt", "r");
    if (arqmapas[2] == NULL) {
        printf("Erro ao abrir o arquivo do mapa!\n");
        return 1;
    }
    

    arqmapas[3] = fopen("mapas/mapa_exemplo.txt", "r");
    if (arqmapas[3] == NULL) {
        printf("Erro ao abrir o arquivo do mapa!\n");
        return 1;
    }
    

    arqmapas[4] = fopen("mapas/mapa_henrique.txt", "r");
    if (arqmapas[4] == NULL) {
        printf("Erro ao abrir o arquivo do mapa!\n");
        return 1;
    }
    
    /*
    arqmapas[2] = fopen("mapas/mapa_vitoria.txt", "r");
    if (arqmapas[2] == NULL) {
        printf("Erro ao abrir o arquivo do mapa!\n");
        return 1;
    }

    arqmapas[3] = fopen("mapas/mapa_priscila.txt","r");
    if (arqmapas[3] == NULL) {
        printf("Erro ao abrir o arquivo do mapa!\n");
        return 1;
    }

    arqmapas[4] = fopen("mapas/mapa_professor.txt", "r");
    if (arqmapas[4] == NULL) {
        printf("Erro ao abrir o arquivo do mapa!\n");
        return 1;
    }
    
    arqmapas[5] = fopen("mapas/mapa_professor2.txt", "r");
    if (arqmapas[5] == NULL) {
        printf("Erro ao abrir o arquivo do mapa!\n");
        return 1;
    }
    */
    morte = 0; //inicia o estado de não morto

    vidas = 5; //inicia com 5 vidas
        // pergunta se o personagem esta morto, se não, passa pro proximo mapa
    for(int i = 0; ((i < QUANT_mapas) && !morte); i++){//primeiro for seleciona o primeiro mapa e vai encrementando cada vez que fecha a janela, troca o mapa
        for(int j = 0; j < LINHA; j++){ //segundo for salva uma variavel MAPA que cada vez que troca de fase preenche as colunas e linhas com o conteudo do arquivo (mapa)
            fgets(mapa[j], COLUNA + 3, arqmapas[i]); // +3 pois no windows pega dois /n e um /0
        }

        InitWindow(TAMANHO * COLUNA, TAMANHO * LINHA, "Donkey Kong INF (DKINF)"); //função do raylib pra abrir aba, parametros dados por defines, + nome da janela
        SetTargetFPS(60);//60 quadros por segundo funçaõ do raylib que deixa o jogo passando 60 vezes por segundo
    
        int quant_inimigos = 0; // controla a quantidade de inimigos
        vilao inimigos[51] = {}; //podem ter 51 inimigos ao maximo, {} viloes spawnam em lugar especifico

        //struct vilao recebe a coluna e linha que ele esta + o movimento que ele faz(direita ou esquerda)

        // Inicializa posição do Mario e dos inimigos procurando no mapa ANTES do loop e setando spawnpoint 
        for(int l = 0; l < LINHA; l++){ //olham todo o mapa salvo, olha se algum dos elementos é do tipo P, se for, salva a posição do Mario, depois de encontrado,
            // esse P é apagado para nao ficar nascendo varios marios no mesmo lugar
            for(int c = 0; c < COLUNA; c++){ //fors estão olhando a posição da linha e coluna
                if(mapa[l][c] == 'P'){
                    spawnpoint.coluna = c * TAMANHO;
                    spawnpoint.linha = l * TAMANHO;
                    mapa[l][c] = ' ';//apaga o spawnpoint
                }
                else if(mapa[l][c] == 'E'){ // procura por E, encrementa a quANT DE INIMIGOS, e nasce um inimigo no lugar onde E foi encontrado , dps apaga para nao nascer no mesmo lugar varias vezes                   
                    inimigos[quant_inimigos].coluna = c * TAMANHO; //para ficar proporcinal ao tamanho do mapa
                    inimigos[quant_inimigos].linha = l * TAMANHO;
                        if(!(quant_inimigos % 2))//metade dos inimigos começam para a direita
                            inimigos[quant_inimigos].mov = 'D';
                        else//a outra metade para a esquerda
                            inimigos[quant_inimigos].mov = 'E';
                    mapa[l][c] = ' ';//apaga os inimigos, procura se a letra ainda está la e a apaga
                    quant_inimigos++;
                }
            }
        }

        mario = spawnpoint; // diz para o mario nascer no spawpoint

        int probabilidade = 1; //chance da bomba nascer
        personagem bomba[1000] = {};

        objeto caixas[COLUNA][LINHA]; //struct 

        do{
            if(IsKeyPressed(KEY_TAB)) pausado = !pausado;
            //reinicia toda hora as variavéis
            chao = 0;
            escada = 0;

            //Se o personagem tentar ultrapassar os limites, ele simplesmente é teleportado para onde é completamente visível
            if(mario.coluna >= TAMANHO * (COLUNA - 0.6667)){
                mario.coluna = TAMANHO * (COLUNA - 0.6667);
            }
            else if(mario. coluna <= TAMANHO * (-0.3334)){
                mario.coluna = TAMANHO * (-0.3334);
            }


        
            BeginDrawing();

                ClearBackground(RAYWHITE);//fundo branco

                //Desenhos
                for(int l = 0; l < LINHA; l++){ // for que vai percorrer o mapa, se for caixa vai desenhar os elementos "caixas" (que não se movem)
                    for(int c = 0; c < COLUNA; c++){
                        caixas[c][l].tipo = mapa[l][c];
                        caixas[c][l].coluna = c;
                        caixas[c][l].linha = l;
                
                        desenha_objeto(&caixas[c][l]); // 
                    }
                }



                //desenha o mario
                DrawRectangle(mario.coluna, mario.linha, TAMANHO, TAMANHO, PINK);

                //A movimentação do inimigo será a seguinte: ele só liga para a sua linha e a seguinte
                //daí ele pergunta se na sua frente é ' ', se não for ele muda de lado
                //se for ele pergunta se é 'Z' na sua diagonal, se não ele muda de lado
                //se for ele anda para o lado
                //funciona igual um robo aspirador
                for(int k = 0; k < quant_inimigos; k++){
                    fazinimigo(&inimigos[k]); //&inimigos[k] passa o endereço do inimigo daquele numero, para alterar os valores e ele se mexer
                }

                //Joga uma bomba de uma posiçao aleatoria a cada tempo(5.5s para a primeira fase, 4.4s para a segunda e assim por diante)
                if(i != (QUANT_mapas - 1)) // 
                    for(int x = 0; x < 999; x++){
                        if(probabilidade < ((x + 1) * (66 * (QUANT_mapas - i)))){
                            break;
                        }
                        else if(!(probabilidade % ((x + 1) * (66 * (QUANT_mapas - i))))){
                            bomba[x].coluna = (rand() % (COLUNA * TAMANHO));
                            bomba[x].linha = (2 * TAMANHO);
                        }
                    }
                else//Na ultima fase, o bagulho fica frenético
                    for(int x = 2; x < 999; x++){
                        if(!(probabilidade % ((x + 1) * (60 * (QUANT_mapas - i))))){
                            bomba[x].coluna = (rand() % (COLUNA * TAMANHO));
                            bomba[x].linha = (3 * TAMANHO);
                            velocidadeYb = TAMANHO * 12;
                        }
                    }
                
                if(!(probabilidade % (1000 * (66 * (QUANT_mapas - i))))){  
                    bomba[999].coluna = mario.coluna;
                    bomba[999].linha = (2 * TAMANHO);
                    probabilidade = 0;
                }

                for(int z = 0; z < 1000; z++){ //atualiza a posição da bomba, e olha se a colisão do mario com a bomba aconteceu
                    if(!pausado) bomba[z].linha -= velocidadeYb * tempo * 0.7;
                    DrawRectangle(bomba[z].coluna, bomba[z].linha, TAMANHO, TAMANHO, PURPLE); // desenhando a bomba
                    //quando atingido por bomba ele perde uma vida e é teleportado para o spp
                    if((invulnerabilidade < 0) && (mario.linha >= (bomba[z].linha - TAMANHO)) && (mario.linha <= (bomba[z].linha + TAMANHO)) && (mario.coluna <= (bomba[z].coluna + (TAMANHO * 0.6667))) && mario.coluna >= (bomba[z].coluna - (TAMANHO * 0.6667))){
                        mario = spawnpoint; //mario volta pro spawn se a bomba pegar ele
                        vidas--;
                        invulnerabilidade = (60 * 3);
                    }
                }

                //escreve na tela que está pausado
                if(pausado){
                    DrawText("===== Pause =====", TAMANHO * COLUNA / 3, TAMANHO * LINHA / 2, TAMANHO, ORANGE);
                }
                

            EndDrawing();

            if(vidas < 1){//se mario perder todas as vidas ele fecha os mapas
                morte++;
                CloseWindow();
            }

            //se o mario atingir o objetivo ele passa para o proximo mapa
            if((mario.linha >= (objetivo.linha - TAMANHO)) && (mario.linha <= (objetivo.linha + (TAMANHO * 0.5))) && (mario.coluna <= (objetivo.coluna + (TAMANHO * 0.6667))) && mario.coluna >= (objetivo.coluna - (TAMANHO * 0.3334))){//se mario chegar na porta outra fase se inicia
                CloseWindow();
            }

            if(IsKeyDown(KEY_ESCAPE)) morte++; // ao sair do jogo o mario morre

            if(!pausado){
                //movimentações do mário
                if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) mario.coluna += (0.16 * TAMANHO);
                if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) mario.coluna -= (0.16 * TAMANHO); 
                if ((IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) && (escada == 1)) mario.linha -= (0.06 * TAMANHO);// só sobe e desce se a escada for vdd
                if ((IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) && (escada == 1)) mario.linha += (0.06 * TAMANHO);
                if ((IsKeyDown(KEY_SPACE)) && (chao == 1)) velocidadeY = FORCA_DO_PULO; //só pula se o chao é verdadeiro
                if ((chao == 0) && (escada == 0)) velocidadeY += GRAVIDADE * tempo;
                mario.linha -= velocidadeY * tempo; // faz o mario cair caso esteja no ar

                if(velocidadeY < -vmax) //velocidade maxima pro mario
                     velocidadeY = -vmax;

                if(velocidadeYb < -vmax) //veloc max da bomba
                    velocidadeYb = -vmax;

                invulnerabilidade --;

                probabilidade++; //aumenta chances de cair a bomba

                velocidadeYb += GRAVIDADE * tempo; 

                tempo = GetFrameTime();
            }

            //----------------------------------------------------------------------------------
        }while (!WindowShouldClose()); //pergunta se a janela esta fechada, se nao esta, continua o loop do jogo
    }

// i == quant_mapas = TELA VITORIA 


    fclose(arqmapas[0]);
    fclose(arqmapas[1]);
    fclose(arqmapas[2]);
    fclose(arqmapas[3]);
    fclose(arqmapas[4]);
    
// Juntar isso tudo com o ranking
return 0;
}