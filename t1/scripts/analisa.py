"""
Le results/tempos.csv (gerado por scripts/experimentos.sh) e produz:
  - results/resumo.md : tabelas com a MEDIANA das repeticoes e o speedup
  - results/speedup.png : grafico de speedup x numero de threads

Speedup S = T_sequencial / T_paralelo (medianas). Eficiencia E = S / threads.
"""
import csv
import re
from collections import defaultdict
from statistics import median

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

tempos = defaultdict(list)    # (matriz, threads) -> lista de tempos em us; threads 0 = sequencial
objetos = defaultdict(set)    # matriz -> conjunto de contagens vistas (deve ter 1 so valor)
info = {}                     # matriz -> (linhas, colunas, densidade)

with open("results/tempos.csv") as arquivo:
    for linha in csv.DictReader(arquivo):
        chave = (linha["matriz"], int(linha["threads"]))
        tempos[chave].append(float(linha["tempo_us"]))
        objetos[linha["matriz"]].add(linha["objetos"])
        info[linha["matriz"]] = (linha["linhas"], linha["colunas"], linha["densidade"])

lista_threads = sorted({t for (_, t) in tempos if t > 0})
obrigatorias = sorted([m for m in info if m.startswith("caso")], key=lambda m: int(info[m][0]) * int(info[m][1]))
grandes = sorted([m for m in info if not m.startswith("caso")], key=lambda m: (int(info[m][2]), int(info[m][0])))
nucleos = int(re.search(r"nucleos: (\d+)", open("results/maquina.txt").read()).group(1))
repeticoes = len(tempos[(obrigatorias[0], 0)])

def mediana(matriz, threads):
    return median(tempos[(matriz, threads)])

saida = []
saida.append(f"Valor representativo = mediana de {repeticoes} execuções. Máquina: ver results/maquina.txt ({nucleos} núcleos).\n")
for m in info:
    if len(objetos[m]) != 1:
        saida.append(f"**ATENÇÃO: contagens diferentes em {m}: {objetos[m]}**\n")

saida.append("### Matrizes obrigatórias (tempo em microssegundos)\n")
cab = "| Matriz | Dimensões | Objetos | Sequencial | " + " | ".join(f"Paralelo {t}T" for t in lista_threads) + " |"
saida += [cab, "|" + "---|" * (4 + len(lista_threads))]
for m in obrigatorias:
    linhas, colunas, _ = info[m]
    cols = [f"{mediana(m, t):.1f}" for t in lista_threads]
    saida.append(f"| {m} | {linhas} x {colunas} | {next(iter(objetos[m]))} | {mediana(m, 0):.1f} | " + " | ".join(cols) + " |")

saida.append("\n### Matrizes grandes (tempo em milissegundos e speedup)\n")
cab = "| Matriz | Densidade | Objetos | Sequencial (ms) | " + " | ".join(f"{t}T (ms)" for t in lista_threads) + " | " + " | ".join(f"S {t}T" for t in lista_threads) + " |"
saida += [cab, "|" + "---|" * (4 + 2 * len(lista_threads))]
for m in grandes:
    linhas, colunas, dens = info[m]
    seq = mediana(m, 0)
    cols_t = [f"{mediana(m, t) / 1000:.1f}" for t in lista_threads]
    cols_s = [f"{seq / mediana(m, t):.2f}" for t in lista_threads]
    saida.append(f"| {linhas} x {colunas} | {dens}% | {next(iter(objetos[m]))} | {seq / 1000:.1f} | " + " | ".join(cols_t) + " | " + " | ".join(cols_s) + " |")

open("results/resumo.md", "w").write("\n".join(saida) + "\n")
print("\n".join(saida))

# ---------------- grafico: speedup x threads, um painel por densidade ----------------
cores = ["#2a78d6", "#eb6834", "#1baf7a", "#eda100", "#e87ba4", "#008300"]
densidades = sorted({info[m][2] for m in grandes}, key=int)
fig, eixos = plt.subplots(1, len(densidades), figsize=(5.2 * len(densidades), 4.2), sharey=True, squeeze=False)
for eixo, dens in zip(eixos[0], densidades):
    eixo.plot(lista_threads, lista_threads, "--", color="#9a9893", linewidth=1.5, label="ideal (S = threads)")
    eixo.axhline(1, color="#c3c2b7", linewidth=1)
    eixo.axvline(nucleos, color="#52514e", linewidth=1, linestyle=":")
    eixo.text(nucleos, 0.15, f" {nucleos} núcleos", color="#52514e", fontsize=9)
    for cor, m in zip(cores, [m for m in grandes if info[m][2] == dens]):
        seq = mediana(m, 0)
        eixo.plot(lista_threads, [seq / mediana(m, t) for t in lista_threads], "-o", color=cor,
                  linewidth=2, markersize=6, label=f"{info[m][0]} x {info[m][1]}")
    eixo.set_title(f"Densidade {dens}%", fontsize=11)
    eixo.set_xlabel("threads")
    eixo.set_xticks(lista_threads)
    eixo.set_ylim(0, max(lista_threads) + 0.5)
    eixo.grid(alpha=0.25)
    for lado in ("top", "right"):
        eixo.spines[lado].set_visible(False)
eixos[0][0].set_ylabel("speedup (T_seq / T_par)")
eixos[0][-1].legend(frameon=False, fontsize=9, loc="upper left")
fig.tight_layout()
fig.savefig("results/speedup.png", dpi=150)
print("\nGráfico salvo em results/speedup.png")
