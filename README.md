# sisop

Trabalhos de Sistemas Operacionais (PUCRS, 2026/II), Prof. Filipo Mór. Autoria: Caio Madeira, João Sperb e Victor.

| Trabalho | Pasta | Relatório |
|---|---|---|
| T1: Contagem paralela de objetos em uma matriz binária (sequencial + Pthreads) | [`t1/`](t1/) | [`t1/README.md`](t1/README.md) |

Uso rápido:

```sh
cd t1
make                              # compila sequencial, paralelo e gerador de matrizes
make test                         # testes de correção
./paralelo tests/caso8_8.txt 2    # versão paralela com 2 threads
make experimentos                 # medições de desempenho (gera t1/results/)
```

Slides: [`t1/slides/apresentacao.pdf`](t1/slides/apresentacao.pdf)
