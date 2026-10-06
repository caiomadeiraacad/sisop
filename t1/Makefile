PAR_SRC = src/conta-obj-paralelo.c
SEQ_SRC = src/conta-obj-sequencial.c
GER_SRC = tests/gera-matriz.c

CC     = cc
CFLAGS = -std=c89 -Wall -Wextra -pedantic -O2

all: paralelo sequencial gera-matriz

paralelo: $(PAR_SRC)
	$(CC) $(CFLAGS) -pthread $(PAR_SRC) -o paralelo

sequencial: $(SEQ_SRC)
	$(CC) $(CFLAGS) $(SEQ_SRC) -o sequencial

gera-matriz: $(GER_SRC)
	$(CC) $(CFLAGS) $(GER_SRC) -o gera-matriz

test: all
	sh scripts/testes.sh

experimentos: all
	sh scripts/experimentos.sh

clean:
	rm -f paralelo sequencial gera-matriz

.PHONY: all test experimentos clean
