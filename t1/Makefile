PAR_SRC = src/conta-obj-paralelo.c
SEQ_SRC = src/conta-obj-sequencial.c

CC     = gcc
CFLAGS = -O2 -Wall

all: paralelo sequencial

paralelo: $(PAR_SRC)
	$(CC) $(CFLAGS) -pthread $(PAR_SRC) -o paralelo

sequencial: $(SEQ_SRC)
	$(CC) $(CFLAGS) $(SEQ_SRC) -o sequencial

clean:
	rm -f paralelo sequencial

.PHONY: all clean