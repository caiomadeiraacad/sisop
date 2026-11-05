
## Estação 1 — Identidade do arquivo e links

_05/11/2026 19:44 · Linux 6.12.95+deb13-amd64_


### 1. Criando o arquivo original.txt e lendo seus atributos

- original.txt: ID=4086809 volume=66310 links=1 modo=-rw-r--r-- tamanho=37

### 2. Renomear


**Previsão pedida:** Ao renomear original.txt para renomeado.txt, o ID muda? E a data de modificação?
**Nossa previsão:** sim
- renomeado.txt: ID=4086809 volume=66310 links=1 modo=-rw-r--r-- tamanho=37
- O ID mudou? NÃO
- A data de modificação mudou? NÃO

### 3. Copiar


**Previsão pedida:** E se copiarmos renomeado.txt para copia.txt (cópia que preserva datas)? Mesmo ID?
**Nossa previsão:** nao
- copia.txt: ID=4086806 volume=66310 links=1 modo=-rw-r--r-- tamanho=37
- Mesmo ID do renomeado.txt? NÃO

### 4. Link rígido (hard link)


**Previsão pedida:** Criaremos o hard link rigido.txt -> renomeado.txt. Quantos links o arquivo terá? O ID do link será diferente?
**Nossa previsão:** 1 e sim
- renomeado.txt: ID=4086809 volume=66310 links=2 modo=-rw-r--r-- tamanho=37
- rigido.txt: ID=4086809 volume=66310 links=2 modo=-rw-r--r-- tamanho=37

### 5. Link simbólico (symlink)


**Previsão pedida:** Criaremos o link simbólico simbolico.txt -> renomeado.txt. Ele terá o mesmo ID do arquivo? O que ele guarda dentro?
**Nossa previsão:** nao. apenas o caminho do arquivo
- simbolico (lstat): ID=4086833 volume=66310 links=1 modo=lrwxrwxrwx tamanho=13
- simbolico (stat): ID=4086809 volume=66310 links=2 modo=-rw-r--r-- tamanho=37
- Conteúdo do link (readlink): 'renomeado.txt'

### 6. Apagar o arquivo 'original' (renomeado.txt)


**Previsão pedida:** Depois de apagar renomeado.txt: dá para ler pelo hard link? E pelo link simbólico? Quantos links sobram?
**Nossa previsão:** pelo hardlink sim. pelo link simbolico nao. sobraram 1 link
- Leitura por rigido.txt: 'Conteúdo do arquivo da estação 1.'
- rigido.txt: ID=4086809 volume=66310 links=1 modo=-rw-r--r-- tamanho=37
- simbolico.txt ainda existe (lexists)? True
- O alvo dele existe (exists)? False
- Leitura por simbolico.txt falhou: FileNotFoundError, errno 2 (ENOENT) - No such file or directory

**Para discutir:**
- Quem guarda o NOME do arquivo: o próprio arquivo ou o diretório?
- Quando os blocos de dados são liberados de fato? Que estrutura o kernel consulta?
- Por que um link simbólico pode ficar 'pendente' e um hard link não? (Lista, Questão 2)
- Por que, em geral, não se permite hard link para diretórios? (Lista, Questão 6)

## Estação 2 — O arquivo aberto: tabelas do kernel

_05/11/2026 19:51 · Linux 6.12.95+deb13-amd64_


### 1. Duas aberturas independentes do mesmo arquivo

- Descritores obtidos: fd_a=3  fd_b=4
- fd_a leu 5 bytes: '01234'

**Previsão pedida:** Agora vamos ler 5 bytes por fd_b. Quais bytes virão?
**Nossa previsão:** todos
- fd_b leu 5 bytes: '01234'

### 2. Um descritor duplicado com dup()

- fd_c = dup(fd_a) = 5

**Previsão pedida:** Lendo 5 bytes por fd_c (cópia de fd_a), quais bytes virão? E depois, lendo mais 5 por fd_a?
**Nossa previsão:** 5
- fd_c leu 5 bytes: '56789'
- fd_a leu 5 bytes: 'ABCDE'
- Posição atual de fd_a: 15
- Posição atual de fd_b: 5
- Posição atual de fd_c: 15
-   /proc/self/fd/3 (fd_a) pos=15 -> /home/24280006/Área de trabalho/Aulas_SisOp/Pratica_Arquivos_ES/_lab/e2/dados.txt
-   /proc/self/fd/4 (fd_b) pos=5 -> /home/24280006/Área de trabalho/Aulas_SisOp/Pratica_Arquivos_ES/_lab/e2/dados.txt
-   /proc/self/fd/5 (fd_c) pos=15 -> /home/24280006/Área de trabalho/Aulas_SisOp/Pratica_Arquivos_ES/_lab/e2/dados.txt

### 3. Apagar um arquivo que ainda está aberto


**Previsão pedida:** fd_a continua aberto. Conseguimos apagar dados.txt? Se sim, ainda dá para ler o conteúdo por fd_a?
**Nossa previsão:** nao. 
- os.remove(): funcionou
- O nome ainda existe na pasta? False
- fd_a (depois de apagar) leu 20 bytes: '0123456789ABCDEFGHIJ'
- Links do arquivo (fstat): 0
- /proc/self/fd/3 aponta para: /home/24280006/Área de trabalho/Aulas_SisOp/Pratica_Arquivos_ES/_lab/e2/dados.txt (deleted)
- Depois de fechar fd_a, o arquivo existe? False

**Para discutir:**
- Que informação ficou na tabela POR PROCESSO e qual na tabela GLOBAL? (Lista, Questão 4)
- Por que fd_b não 'andou' quando lemos por fd_a, mas fd_c andou?
- Quando o kernel pode, de fato, liberar os blocos de um arquivo apagado?
- Como o Linux atualiza bibliotecas em uso sem reiniciar, e por que o Windows pede reinício?

## Estação 3 — Camadas de buffer: processo, kernel e disco

_05/11/2026 19:53 · Linux 6.12.95+deb13-amd64_


### 1. A escreve uma linha, sem flush

- Processo A (escritor) iniciado, PID 5286

**Previsão pedida:** A escreveu 'Linha 1' mas não chamou flush(). Se B (este processo) ler o arquivo agora, o que encontrará?
**Nossa previsão:** o conteudo antigo
- B vê 0 bytes no arquivo: ''

### 2. A chama flush() — mas não fsync()


**Previsão pedida:** Depois do flush(), B verá a linha? Ela já está gravada no disco?
**Nossa previsão:** sim, flush força memoria em cache
- B vê 8 bytes no arquivo: 'Linha 1\n'

### 3. A chama fsync()


**Previsão pedida:** O que muda para B depois do fsync()? Quanto tempo vocês acham que ele leva?
**Nossa previsão:** sincroniza as alteracoes
- B vê 8 bytes no arquivo: 'Linha 1\n'
- Tempo do fsync() (com ida e volta do comando): 4.46 ms

### 4. Queda do PROCESSO sem flush


**Previsão pedida:** A escreveu 'Linha 2' sem flush e será morto agora. A Linha 2 sobrevive?
**Nossa previsão:** nao
- Processo A foi MORTO (kill), código de saída -9
- B vê 8 bytes no arquivo: 'Linha 1\n'

### 5. Queda do PROCESSO depois do flush

- Processo A (escritor) iniciado, PID 5538

**Previsão pedida:** Agora A escreveu 'Linha 3', chamou flush() e será morto. A Linha 3 sobrevive?
**Nossa previsão:** sim
- Processo A foi MORTO (kill), código de saída -9
- B vê 28 bytes no arquivo: 'Linha 1\nLinha 3 (com flush)\n'
- Linux: tempo máximo de um dado 'sujo' no cache: 30 s

### 6. O preço da durabilidade


**Previsão pedida:** Vamos fazer 100 escritas de 4 KiB: (a) só write(); (b) write() + fsync() a cada uma. Quantas vezes mais lento será (b)?
**Nossa previsão:** sei la 400 vezes
- (a) só write():      0.7 ms  (   134577 escritas/s)
- (b) write() + fsync():    293.7 ms  (      340 escritas/s)
- (b) ÷ (a): 395 vezes

**Para discutir:**
- Em que camada estava a Linha 2 quando A morreu? E a Linha 3?
- Se, em vez do processo, faltasse ENERGIA logo após o flush(), a Linha 3 sobreviveria?
- A Linha 1 ficou visível para B antes do fsync(). Que semântica de consistência é essa?
- Por que bancos de dados chamam fsync() mesmo sendo tão caro? (Lista, Questão 10)

## Estação 4 — Volumes, montagem e limites

_05/11/2026 19:56 · Linux 6.12.95+deb13-amd64_


### 1. Em que volume está cada pasta?


**Previsão pedida:** Vocês acham que a pasta pessoal, a pasta temporária e a raiz estão no mesmo volume? Que tipo de sistema de arquivos esperam encontrar?
**Nossa previsão:** sim, estao
- /home/24280006/Área de trabalho/Aulas_SisOp/Pratica_Arquivos_ES: volume=66310 tipo=ext4 bloco=4096 montado_em=/ origem=/dev/nvme0n1p6
- /home/24280006: volume=66310 tipo=ext4 bloco=4096 montado_em=/ origem=/dev/nvme0n1p6
- /tmp: volume=41 tipo=tmpfs bloco=4096 montado_em=/tmp origem=tmpfs
- /: volume=66310 tipo=ext4 bloco=4096 montado_em=/ origem=/dev/nvme0n1p6
- /dev/shm: volume=27 tipo=tmpfs bloco=4096 montado_em=/dev/shm origem=tmpfs
- /proc: volume=23 tipo=proc bloco=4096 montado_em=/proc origem=proc
- /sys: volume=22 tipo=sysfs bloco=4096 montado_em=/sys origem=sysfs
- /boot/efi: volume=66309 tipo=vfat bloco=4096 montado_em=/boot/efi origem=/dev/nvme0n1p5
- /media: volume=66310 tipo=ext4 bloco=4096 montado_em=/ origem=/dev/nvme0n1p6

### 2. Procurando um SEGUNDO volume onde se possa gravar

- Volume desta pasta: 66310
- Outro volume encontrado: /tmp (volume 41)

### 3. Hard link entre volumes


**Previsão pedida:** Um hard link para viajante.txt dentro de /tmp: funciona?
**Nossa previsão:** sim
- os.link() falhou: OSError, errno 18 (EXDEV) - Invalid cross-device link

### 4. Mover (renomear) para outro volume


**Previsão pedida:** os.rename() de viajante.txt para o outro volume é instantâneo como dentro do mesmo volume? E o comando 'mover' do sistema (shutil.move)?
**Nossa previsão:** nao. nao
- os.rename() falhou: OSError, errno 18 (EXDEV) - Invalid cross-device link
- shutil.move(): funcionou
- ID antes e depois de mover: 4086838 -> 985

### 5. Montar sobre uma pasta que já tem arquivos (Lista, Questão 7)

- Conteúdo de ponto/ ANTES de montar: ['a_antes.txt', 'b_antes.txt']

**Previsão pedida:** Vamos montar um sistema de arquivos vazio em ponto/ e criar nele c_novo.txt. Durante a montagem, o que acontece com a_antes.txt e b_antes.txt? E depois de desmontar, o que aparece em ponto/?
**Nossa previsão:** nada
- DURANTE a montagem: []
- Depois de criar c_novo.txt: ['c_novo.txt']
- DEPOIS de desmontar: ['a_antes.txt', 'b_antes.txt']

**Para discutir:**
- Por que mover um arquivo de 5 GB entre pastas do mesmo disco é instantâneo, mas para um pendrive demora?
- Por que o hard link não cruza volumes, e o link simbólico cruza?
- No Windows, um volume só pode ser montado em uma pasta NTFS VAZIA. No Unix, a pasta pode ter arquivos. O que acontece com eles? (Questão 7)

## Estação 5 — Proteção: bits de permissão × ACL

_05/11/2026 19:59 · Linux 6.12.95+deb13-amd64_


### 1. Atributos de proteção do arquivo

- Modo: -rw-r--r--
- Dono / grupo: 24280006 / pucrs

### 2. Tirar do DONO a permissão de leitura


**Previsão pedida:** Com chmod 200 (só escrita para o dono), o dono ainda consegue ler o arquivo? E escrever?
**Nossa previsão:** sim
- Modo agora: --w-------
- Ler: NEGADO  -> PermissionError, errno 13 (EACCES) - Permission denied
- Escrever: PERMITIDO

**Previsão pedida:** Sem poder ler, o dono consegue devolver a permissão a si mesmo (chmod 600)?
**Nossa previsão:** sim
- chmod 600: PERMITIDO
- Modo agora: -rw-------

### 3. A permissão do DIRETÓRIO

- Modo da pasta / do arquivo: dr-x------ / -rw-r--r--

**Previsão pedida:** A pasta perdeu a permissão de ESCRITA (w), mas o arquivo continua rw para o dono. Conseguimos (a) alterar o conteúdo do arquivo? (b) apagar o arquivo?
**Nossa previsão:** nao. sim
- (a) Escrever em pasta/dentro.txt: PERMITIDO
- (b) Apagar pasta/dentro.txt: NEGADO  -> PermissionError, errno 13 (EACCES) - Permission denied
- Modo da pasta agora: drw-------

**Previsão pedida:** Agora a pasta tem r e w, mas não tem x (execução). Conseguimos listar os nomes da pasta? E ler pasta/dentro.txt?
**Nossa previsão:** sim
-    nomes: ['dentro.txt']
- Listar a pasta: PERMITIDO
- Ler pasta/dentro.txt: NEGADO  -> PermissionError, errno 13 (EACCES) - Permission denied

### 4. Listas de Controle de Acesso (ACL)


**Previsão pedida:** Com bits rwx, só existem 3 categorias (dono, grupo, outros). Vamos dar leitura a um usuário ESPECÍFICO (nobody) via ACL. O que muda na saída de ls -l?
**Nossa previsão:** sla
- ls -l antes: -rw------- 1 24280006 pucrs 38 nov  5 19:59 /home/24280006/Área de trabalho/Aulas_SisOp/Pratica_Arquivos_ES/_lab/e5/segredo.txt
- setfacl -m u:nobody:r: ok
- ls -l depois: -rw-r-----+ 1 24280006 pucrs 38 nov  5 19:59 /home/24280006/Área de trabalho/Aulas_SisOp/Pratica_Arquivos_ES/_lab/e5/segredo.txt
    user::rw-
    user:nobody:r--
    group::---
    mask::r--
    other::---

**Para discutir:**
- Bits rwx × ACL: compare granularidade, espaço de metadados e custo de verificação (Questão 5).
- No Unix, apagar um arquivo depende da permissão de quem: do arquivo ou do diretório? Por quê?
- Por que uma regra DENY costuma ter precedência sobre uma regra ALLOW?
- Por que é perigoso trabalhar o tempo todo como root/Administrador?

## Estação extra — Bloqueio de arquivos: consultivo × mandatório

_05/11/2026 20:00 · Linux 6.12.95+deb13-amd64_


### 1. O processo A trava os primeiros 64 bytes do arquivo (lock exclusivo)

- Processo A diz: travado
- Mecanismo usado: fcntl.lockf() -> fcntl(F_SETLK)

### 2. B PEDE o lock


**Previsão pedida:** Se B pedir o mesmo lock (sem esperar), ele consegue?
**Nossa previsão:** sim
- B conseguiu o lock: NÃO -> BlockingIOError, errno 11 (EAGAIN) - Resource temporarily unavailable

### 3. B IGNORA o lock e escreve direto


**Previsão pedida:** E se B simplesmente IGNORAR o lock e gravar 'saldo=0' no arquivo? O sistema impede?
**Nossa previsão:** sim
- Escrita de B: FUNCIONOU (o lock foi ignorado)
- Conteúdo final: 'saldo=0'

**Para discutir:**
- O bloqueio que vocês viram é consultivo ou mandatório? Como sabem?
- Em um servidor de banco de dados, qual dos dois protege contra um programa mal escrito? (Questão 8)
- O Linux removeu o bloqueio mandatório no kernel 5.15. Pesquisem: por quê?
