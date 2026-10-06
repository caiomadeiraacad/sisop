#!/bin/sh
# Experimentos de desempenho (rodar com: make experimentos)
#  - roda as 5 matrizes obrigatorias e matrizes grandes geradas por tests/gera-matriz.c;
#  - cada configuracao e executada REPETICOES vezes (sequencial e paralelo intercalados);
#  - todos os tempos vao para results/tempos.csv; o resumo (mediana, speedup)
#    vai para results/resumo.md e o grafico para results/speedup.png.
# Os parametros podem ser trocados na chamada, ex.:
#   TAMANHOS="1000 2000" THREADS="1 2 4" REPETICOES=5 sh scripts/experimentos.sh
cd "$(dirname "$0")/.." || exit 1

TAMANHOS=${TAMANHOS:-"1000 2000 4000 8000"} # matrizes quadradas N x N
DENSIDADES=${DENSIDADES:-"30 60"}           # % de celulas com valor 1
THREADS=${THREADS:-"1 2 4 8"}
REPETICOES=${REPETICOES:-10}
SEMENTE=42

mkdir -p tests/grandes results
csv=results/tempos.csv
echo "matriz,linhas,colunas,densidade,versao,threads,repeticao,objetos,tempo_us" > $csv

# guarda a descricao da maquina usada
{
    echo "data: $(date)"
    uname -a
    echo "nucleos: $(getconf _NPROCESSORS_ONLN)"
    (lscpu 2>/dev/null | grep 'Model name') || sysctl -n machdep.cpu.brand_string 2>/dev/null
    cc --version | head -1
    echo "repeticoes por configuracao: $REPETICOES"
} > results/maquina.txt

# extrai "objetos,tempo_us" da saida dos programas
extrai() { awk '/obj count/{o=$3} /^tempo:/{t=$2} END{print o "," t}'; }

# mede NOME ARQUIVO LINHAS COLUNAS DENSIDADE
mede() {
    r=1
    while [ $r -le $REPETICOES ]; do
        echo "$1,$3,$4,$5,sequencial,0,$r,$(./sequencial "$2" | extrai)" >> $csv
        for t in $THREADS; do
            echo "$1,$3,$4,$5,paralelo,$t,$r,$(./paralelo "$2" $t | extrai)" >> $csv
        done
        r=$((r + 1))
    done
}

echo "== Matrizes obrigatorias"
for f in tests/caso*.txt; do
    nome=$(basename "$f" .txt)
    set -- $(head -1 "$f")
    echo "   $nome"
    mede "$nome" "$f" $1 $2 "-"
done

echo "== Matrizes grandes"
for n in $TAMANHOS; do
    for d in $DENSIDADES; do
        arq=tests/grandes/m${n}_d$d.txt
        [ -f "$arq" ] || ./gera-matriz $n $n $d $SEMENTE > "$arq"
        echo "   ${n}x$n densidade $d%"
        mede "m${n}_d$d" "$arq" $n $n $d
    done
done

echo "== Resumo"
if python3 -c "import matplotlib" 2>/dev/null; then
    python3 scripts/analisa.py
else
    echo "python3 + matplotlib nao encontrados: so o CSV foi gerado ($csv)"
fi
