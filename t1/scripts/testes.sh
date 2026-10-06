#!/bin/sh
# Testes de correcao (rodar com: make test)
#  1. As 5 matrizes obrigatorias: compara esperado x sequencial x paralelo (1 a 4 threads).
#  2. 200 matrizes aleatorias: o paralelo (2, 3, 4 e 7 threads) tem que dar
#     exatamente o mesmo numero de objetos que o sequencial.
cd "$(dirname "$0")/.." || exit 1

falhas=0
conta() { "$@" | grep 'obj count' | awk '{print $3}'; }

echo "== Matrizes obrigatorias"
printf "%-10s %-9s %-11s %-21s %s\n" "caso" "esperado" "sequencial" "paralelo (1,2,3,4)" "status"
for linha in "caso5_5 3" "caso6_8 4" "caso8_8 5" "caso9_12 6" "caso12_12 7" "caso100_100 22" "caso200_100 29"; do
    set -- $linha
    caso=$1; esperado=$2
    seq=$(conta ./sequencial tests/$caso.txt)
    par=""; status="OK"
    [ "$seq" = "$esperado" ] || status="FALHOU"
    for t in 1 2 3 4; do
        r=$(conta ./paralelo tests/$caso.txt $t)
        par="$par$r "
        [ "$r" = "$esperado" ] || status="FALHOU"
    done
    [ "$status" = "OK" ] || falhas=$((falhas + 1))
    printf "%-10s %-9s %-11s %-21s %s\n" "$caso" "$esperado" "$seq" "$par" "$status"
done

echo
echo "== Matrizes aleatorias (sequencial x paralelo)"
tmp=$(mktemp)
ok=0
for semente in $(seq 1 200); do
    linhas=$((20 + semente % 61))          # 20 a 80 linhas
    colunas=$((15 + (semente * 7) % 66))   # 15 a 80 colunas
    densidade=$((20 + (semente * 13) % 51)) # 20% a 70% de celulas com 1
    ./gera-matriz $linhas $colunas $densidade $semente > "$tmp"
    seq=$(conta ./sequencial "$tmp")
    for t in 2 3 4 7; do
        par=$(conta ./paralelo "$tmp" $t)
        if [ "$par" != "$seq" ]; then
            echo "FALHOU: semente=$semente ${linhas}x$colunas dens=$densidade% threads=$t seq=$seq par=$par"
            falhas=$((falhas + 1))
        else
            ok=$((ok + 1))
        fi
    done
done
rm -f "$tmp"
echo "$ok comparacoes iguais ao sequencial"

echo
if [ $falhas -eq 0 ]; then echo "TODOS OS TESTES PASSARAM"; else echo "$falhas FALHA(S)"; exit 1; fi
