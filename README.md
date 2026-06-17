#  Donkey Kong — Jogo em C com Raylib

Jogo de plataforma estilo Donkey Kong desenvolvido em C usando a biblioteca [Raylib](https://www.raylib.com/).

##  Controles

| Tecla | Ação |
|---|---|
| `A` / `←` | Mover esquerda |
| `D` / `→` | Mover direita |
| `W` / `↑` | Subir escada |
| `S` / `↓` | Descer escada |
| `Espaço` | Pular |
| `ESC` | Sair |
| `TAB` | Pause |

## Comandos do Makefile para rodar o jogo 

```bash
(LINUX)		(WINDOWS)	          # (Resultado)
make		mingw32-make          # Compila tudo
make run	mingw32-make run      # Compila e executa o menu
make clean	mingw32-make clean    # Remove arquivos compilados
```

```comando do VS
'Ctrl' + 'Shift' + 'B'    # Executa o comando de limpeza, de compilar e de executar do Makefile independente se é win ou linux
```
