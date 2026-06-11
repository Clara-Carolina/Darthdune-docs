# Darthdune

O darthdune é o programa desenvolvido pelo Prof. André o grupo utiliza para estudar o light yield do sistema PoWER e demonstrar o veto. Em cada execução:

1. Fótons ópticos são gerados uniformemente em um voxel do volume ativo via GPS
2. Cada fóton é propagado com os processos de reflexão, refração, espalhamento Rayleigh, absorção e wavelength shifting (via PEN)
3. Quando um fóton atinge um LDU, a eficiência quântica do SiPM correspondente é aplicada probabilisticamente
4. As coordenadas de origem de cada fóton detectado são registradas, construindo o light map

O programa darthdune foi desenvolvido com base no Geant4, com o objetivo de simular a Field Cage do DUNE com o PoWER. O resultado é o light yeld  representados em light maps. **A simulação não simula neutrinos, ela simula os fótons que seriam gerados da interação neutrino-LAr**. 

Para acessar o programa, precisa pedir autorização para acessar o [repositório do GitHub](https://github.com/steklain/darthdune/) para o professor André, e já ter o Geant4 e ROOT instalados no computador [(ver guia aqui)](./geant4.md)

Após clonar o repositório do GitHub para o seu computador, siga os seguintes passos, idealmente utilizando o Visual Code:

### 1. Compilação

Dentro da pasta `darthdune`, execute:

```bash
mkdir build
cd build
cmake .. && make
```
O CMake localiza automaticamente o Geant4 instalado e gera o executável `darthtraptis` dentro de `build/`.

### 2. Modo interativo — visualizar a geometria

Copie a macro de visualização que você quer utilizar (no exemplo abaixo, geometry.mac) para `build/` e execute:

```bash
cp ../macros/geometry.mac .
./darthtraptis -i geometry.mac
```

A flag `-i` abre uma sessão interativa com interface Qt, exibindo a geometria 3D da field cage.

### 3. Modo batch — simulação completa

Para executar uma simulação em lote sem interface gráfica:

```bash
./darthtraptis ../macros/photonsgpsvolume.batch
```

O Geant4 distribui os eventos entre as threads configuradas (parâmetro `NUMBER_OF_THREADS` em `DarthTraptis.cfg`) e gera um arquivo de saída por thread: `output_t0.root`, `output_t1.root`, etc.

> Se tudo ocorrer bem, deve abrir uma tela interativa para visualizar a field cage, além de gerar os light maps e histogramas.


### 4. Combinando os arquivos de saída

Ao final da simulação, mescle os arquivos de todas as threads em um único arquivo e remova os individuais:

```bash
hadd output.root output_t*.root && rm output_t*.root
```

O `hadd` é um utilitário do ROOT para combinar arquivos `.root`. O resultado é o arquivo `output.root`, usado nas etapas de análise.

## Descrição dos Arquivos Principais

### `main.cpp` — ponto de entrada

Inicializa e conecta todos os componentes da simulação na ordem exigida pelo Geant4:

1. Lê `DarthTraptis.cfg` (arquico que possuí os parâmetros) via `Config::LoadFile()`
2. Cria o `G4RunManager` (gestor da simulação)
3. Registra o detector (`MyDetector`), a lista de física (`PhysicsList`) e as ações do usuário (`MyActionInitialization`)
4. Define o número de threads (lido do `.cfg`)
5. Inicializa o gerenciador de visualização (`G4VisExecutive`)
6. Interpreta o argumento da linha de comando:
   - Nenhum argumento → sessão interativa pura (Qt)
   - Flag `-i macro.mac` → executa a macro e abre a sessão interativa
   - `macro.batch` (sem `-i`) → executa em modo batch e encerra

### `MyDetector.cpp` — construção da geometria

### Detecção nos LDUs (`MySensorDetector.cpp`)

Quando um fóton óptico penetra no volume de silício de um LDU (que tem comprimento de absorção de apenas 1 nm, ou seja, absorve instantaneamente qualquer fóton que entre), o Geant4 aciona o método `ProcessHits`. Nele:

1. A energia cinética do fóton é lida
2. A eficiência quântica do SiPM é **interpolada** da curva correspondente ao tipo de detector:
   - Para LDUs VIS: curva `SIPM_EFF_VIS` (S14160-6050HS) do arquivo `data/SiPM_S14160/quantum_efficiency.dat`
   - Para LDUs VUV: curva `SIPM_EFF_VUV` (VUV4 S13370) do arquivo `data/SiPM_S13370/quantum_efficiency.dat`
3. Um número aleatório é gerado: se for menor que a eficiência interpolada, o fóton é considerado detectado e registrado no arquivo ROOT
4. Fótons que não passam no teste de eficiência são simplesmente ignorados
As superfícies ópticas entre materiais que controlam reflexão, transmissão e absorção nas interfaces também são definidas aqui, usando os dados dos arquivos `.dat` em `data/`.

### `MyPhysicsList.h` — processos físicos

Define quais processos físicos o Geant4 simulará. Para os objetivos do darthdune, o processo mais relevante é o `G4OpticalPhysics`, que habilita:

- Reflexão e refração em interfaces ópticas (Lei de Snell-Descartes)
- Absorção por comprimento de absorção do material
- Espalhamento Rayleigh
- Fluorescência / deslocamento de comprimento de onda (wavelength shifting, via PEN)
- Cintilação (parametrizada, mas as fontes são definidas nas macros)

Os processos hadrônicos (`G4HadronPhysicsFTFP_BERT`, `G4HadronElasticPhysics`, etc.) e eletromagnéticos (`G4EmStandardPhysics`) estão incluídos para completude, mas têm papel secundário numa simulação puramente óptica.

### `MyRunAction.cpp` — controle do arquivo de saída

Abre e fecha o arquivo `output.root` e define a estrutura das TTrees (tabelas de dados). Cada fóton detectado gera uma entrada em três árvores complementares (ver seção [Saídas da Simulação](#saídas-da-simulação)).

### `MyEventAction.cpp` — ações por evento

Imprime no terminal o início e o fim de cada evento. Não realiza nenhum processamento de dados — serve apenas para acompanhamento do progresso da simulação.

### `MyActionInitialization.cpp` — registro de ações

Registra todas as ações do usuário no `G4RunManager`:
- `MyEventAction`: ações por evento
- `MyPrimaryGenerator`: gerador de partículas primárias (lê os comandos `/gps/...` das macros)
- `MyStackingAction`: controla a prioridade de rastreamento de partículas secundárias
- `MyRunAction`: controle do arquivo de saída (registrado separadamente para o thread mestre)


## Macros 

As macros são arquivos de texto lidos pelo Geant4 via `G4UImanager`. Elas configuram a fonte de partículas e o modo de execução sem necessidade de recompilar o programa.

### `geometry.mac` — visualização interativa

Usada exclusivamente para inspecionar a geometria. Abre o visualizador OpenGL (`OGL`), desenha os volumes e configura acúmulo de trajetórias entre eventos. Não define fonte de partículas nem número de eventos — esses comandos podem ser digitados interativamente no terminal do Geant4.

### `photonsgpsvolume.batch` — simulação principal (light map)

A macro de referência para produzir light maps. Define dois tipos de fótons ópticos usando o GPS (General Particle Source) do Geant4:

| Fonte | Energia | Comprimento de onda | Intensidade relativa | Volume de emissão |
|-------|---------|-------------------|----------------------|-------------------|
| 1 | 9,69 eV | ≈ 128 nm (VUV) | 3 (30%) | Paralelepípedo: halfx=0,25 m, halfy=6 m, halfz=6 m |
| 2 | 7,08 eV | ≈ 175 nm | 7 (70%) | Idem |

Ambas as fontes emitem isotropicamente de posições aleatórias dentro do volume definido, centrado na origem. O total de 14.400.000 fótons corresponde a 576 voxels de 0,5 m × 0,5 m no plano Z-Y, com 25.000 fótons por voxel.

### `photonsgpsvolume10_.batch` — simulação rápida (~10%)

Idêntica em estrutura à macro principal, mas com `beamOn 1440000` (1,44 milhão de fótons). Útil para verificar rapidamente se a cadeia de simulação e análise está funcionando corretamente antes de comprometer os recursos computacionais com a simulação completa.

### `photonsgpsvolume1_.batch` — depuração (verbose)

Apenas 1.000 fótons com verbosidade nível 2. O Geant4 imprime detalhes de cada passo de cada fóton. Útil para depurar problemas de geometria ou física.

### `photonsgpsvolume_purelar.batch` — LAr puro, fonte única

Simula apenas a componente VUV (9,69 eV, ≈ 128 nm), com uma única fonte GPS. Útil para estudar separadamente o comportamento dos fótons VUV sem a segunda componente.

### `photonsteste.mac` — fonte pontual com visualização

Coloca uma fonte de fótons VUV (9,69 eV) em uma posição fixa específica no espaço e dispara 100 eventos, com visualização OpenGL ativa. Ideal para testar o comportamento da simulação em uma posição conhecida do detector.

### `photonsgun.mac` — feixe direcional (1 fóton)

Usa o `G4ParticleGun` (mais simples que o GPS) para disparar um único fóton visível (2,50 eV ≈ 496 nm) em uma direção definida. Serve para testes rápidos da cadeia de transporte óptico.

## `DarthTraptis.cfg` — Parâmetros Modificáveis

Este é o arquivo central de configuração da simulação. Nenhuma recompilação é necessária para alterar estes parâmetros; basta editar o arquivo e rodar novamente. Os parâmetros estão organizados por categoria abaixo.

### Controle da simulação

| Parâmetro | Valor padrão | Descrição |
|-----------|-------------|-----------|
| `NUMBER_OF_THREADS` | 24 | Número de threads de CPU para paralelização. Ajuste conforme os núcleos disponíveis na sua máquina. |
| `RND_SEED` | 23072020 | Semente do gerador de números aleatórios. Altere para obter resultados estatisticamente independentes entre rodadas. |
| `VERBOSITY_LEVEL` | 0 | Nível de detalhamento do output (substituído pelos comandos `/run/verbose` das macros). |

### Geometria do criostato

| Parâmetro | Valor padrão | Descrição |
|-----------|-------------|-----------|
| `CRYOSTAT_LENGTH(m)` | 60 | Comprimento total do criostato (eixo X) |
| `CRYOSTAT_HEIGHT(m)` | 13 | Altura total do criostato (eixo Y) |
| `CRYOSTAT_WIDTH(m)` | 13 | Largura total do criostato (eixo Z) |
| `CRYOSTAT_THICKNESS(mm)` | 8 | Espessura das paredes do criostato |

### Geometria da field cage

| Parâmetro | Valor padrão | Descrição |
|-----------|-------------|-----------|
| `FC_HORIZ_BAR_L(mm)` | 60000 | Comprimento das barras horizontais |
| `FC_HORIZ_BAR_H(mm)` | 46 | Altura do perfil das barras horizontais |
| `FC_HORIZ_BAR_W(mm)` | 8 | Largura das barras horizontais |
| `FC_HORIZ_BAR_DIST(mm)` | 60 | Espaçamento entre barras horizontais consecutivas |
| `FC_VERT_BAR_L(mm)` | 45 | Comprimento das barras verticais |
| `FC_VERT_BAR_H(mm)` | 12990 | Altura das barras verticais |
| `FC_VERT_BAR_W(mm)` | 8 | Largura das barras verticais |
| `FC_VERT_BAR_DIST(mm)` | 1800 | Espaçamento entre barras verticais |
| `FC_PROFILE_BASE(mm)` | 46 | Base do perfil curvado (região central) |
| `FC_PROFILE70_BASE(mm)` | 15 | Base do perfil curvado (região de 70 mm) |
| `FC_PROFILE_HEIGHT(mm)` | 7 | Altura do perfil |
| `FC_PROFILE_THICKNESS(mm)` | 3 | Espessura do perfil |
| `FC_PROFILE_WALL(m)` | 2,25 | Altura a partir do centro em que começa o perfil padrão |
| `FC_PROFILE70_WALL(m)` | 4,25 | Altura a partir do centro em que começa o perfil de 70 mm |
| `FC_PD_DISTANCE(mm)` | 500 | Distância entre a field cage e os fotodetectores |

### Fotodetectores (LDUs)

| Parâmetro | Valor padrão | Descrição |
|-----------|-------------|-----------|
| `LDU_L(mm)` | 200 | Comprimento de cada LDU |
| `LDU_H(mm)` | 200 | Altura de cada LDU |
| `LDU_THICKNESS(mm)` | 8 | Espessura de cada LDU |
| `LDU_H_DISTANCE(mm)` | 1000 | Espaçamento horizontal entre LDUs |
| `LDU_V_DISTANCE(mm)` | 1000 | Espaçamento vertical entre LDUs |
| `SIPM_EFFICIENCY` | 0,5 | Eficiência quântica genérica dos SiPMs (50%) |

### Materiais e propriedades ópticas

| Parâmetro | Valor padrão | Descrição |
|-----------|-------------|-----------|
| `ACRYLIC_THICKNESS(mm)` | 1 | Espessura das janelas de acrílico |
| `pen_COATING(um)` | 100 | Espessura do filme de PEN (wavelength shifter) |
| `pen_QE` | 0,5 | Eficiência quântica do PEN (probabilidade de re-emissão) |
| `pen_TIMECONSTANT` | 0 | Constante de tempo de emissão do PEN (ns) |
| `SIGMA_ALPHA` | 5 | Rugosidade das superfícies ópticas em graus (modelo UNIFIED) |
| `VIKUITI_EFFICIENCY` | 0,98 | Eficiência de absorção do Vikuiti |
| `VIKUITI_SIGMAALPHA(deg)` | 1,0 | Parâmetro de rugosidade do Vikuiti |
| `VIKUITI_SPECULARLOBECONSTANT` | 0,1 | Fração de reflexão especular difusa |
| `XEDLAR_FRACTIONMASS` | 0,00001 | Fração mássica de xenônio no LAr dopado |

### Catodo

| Parâmetro | Valor padrão | Descrição |
|-----------|-------------|-----------|
| `CATHODE_LENGTH(cm)` | 60 | Comprimento de cada elemento da grade do catodo |
| `CATHODE_WIDTH(cm)` | 60 | Largura de cada elemento da grade do catodo |
| `CATHODE_SPACING(mm)` | 50 | Espaçamento entre elementos da grade |
| `CATHODE_DISTANCE(mm)` | 50 | Distância do catodo ao eixo central |
| `PEN_CATHODE_RATIO_AREA` | 0,5 | Fração da área do catodo coberta por PEN |

### Propriedades de cintilação do LAr

| Parâmetro | Valor padrão | Descrição |
|-----------|-------------|-----------|
| `LAR_SCINTILLATIONYIELD(/MeV)` | 20000 | Fótons produzidos por MeV de energia depositada |
| `LAR_SCINTILLATIONTIMECONSTANT1(ns)` | 6,2 | Constante de tempo da componente singleto (rápida) |
| `LAR_SCINTILLATIONTIMECONSTANT2(ns)` | 1590 | Constante de tempo da componente tripleto (lenta) |
| `LAR_SCINTILLATIONYIELD1` | 1,0 | Fração da componente singleto (100% ativo) |
| `LAR_SCINTILLATIONYIELD2` | 0,0 | Fração da componente tripleto (desativada) |

As curvas espectrais (índice de refração, comprimento de absorção, Rayleigh, espectro de cintilação) são fornecidas como arquivos `.dat` separados, cujos caminhos também são configuráveis no `.cfg`.

## Saídas da Simulação

### Arquivo `output.root`

O arquivo ROOT contém três TTrees (tabelas de dados), cada uma com uma entrada por fóton detectado (que chegou a algum LDU):

#### `Primary` — fóton primário

Registra as propriedades do fóton no momento de sua criação.

| Branch | Tipo | Descrição |
|--------|------|-----------|
| `Wavelength` | double | Comprimento de onda na emissão (nm) |
| `posX`, `posY`, `posZ` | double | Posição de emissão (mm) |
| `momX`, `momY`, `momZ` | double | Componentes do vetor de momento inicial |

#### `Hits` — detecção

Registra onde e quando o fóton foi detectado.

| Branch | Tipo | Descrição |
|--------|------|-----------|
| `Detector` | string | Nome do detector atingido: `"VIS"` ou `"VUV"` |
| `fX`, `fY`, `fZ` | double | Posição do hit no detector (mm) |
| `Wavelength` | double | Comprimento de onda no momento da detecção (nm) |

> **Convenção importante:** O script de análise identifica o tipo de detector pelo segundo caractere da string: `Detector[1] == 'I'` seleciona os detectores VIS; `Detector[1] == 'U'` seleciona os detectores VUV.

#### `PrimaryDet` — posição primária na detecção

Registra a posição do fóton primário associada a cada evento detectado. Junto com a TTree `Primary`, permite correlacionar a posição de emissão com a posição de detecção.

| Branch | Tipo | Descrição |
|--------|------|-----------|
| `EventID` | int | Identificador do evento |
| `pX`, `pY`, `pZ` | double | Posição do fóton primário (mm) |
| `Time` | double | Tempo de detecção |
| `Distance` | double | Distância percorrida |

As três TTrees têm o mesmo número de entradas quando a simulação é bem-sucedida: cada linha da mesma posição corresponde ao mesmo fóton detectado.


## Análise dos Resultados

### `readroothisto2d.C` — script principal de análise

Execute com o ROOT a partir do diretório onde está o `output.root`:

```bash
root -l -b -q 'readroothisto2d.C(".")'
```

O argumento é o caminho do diretório contendo `output.root`. O script gera automaticamente 9 imagens PNG:

| Arquivo | Descrição |
|---------|-----------|
| `2D_histogram_XY.png` | Distribuição de hits no plano X-Y (parede lateral) |
| `2D_histogram_YZ.png` | Distribuição de hits no plano Y-Z (parede frontal e fundo) |
| `LighMapWLCut.png` | Light map com corte em comprimento de onda (λ < 200 nm excluído) |
| `LighMapInside.png` | Light map de fótons originados dentro do volume ativo (|Z_emissão| < 5,5 m) |
| `LightMapVis.png` | Light map de fótons detectados pelos sensores **VIS** |
| `LightMapVUV.png` | Light map de fótons detectados pelos sensores **VUV** |
| `LightMapVeto.png` | Mapa da fração VUV: VUV / (VIS + VUV) em cada bin Z-Y - *esse é o principal resultado para estudar o efeito do VETO*|
| `HistWL.png` | Histograma do comprimento de onda de todos os hits |
| `HistVeto.png` | Histograma do comprimento de onda separado por tipo de detector (VIS e VUV) nos fótons de buffer (|Z_primário| > 6 m) |

O script também imprime no terminal estatísticas do light map:
- Conteúdo mínimo e posição do bin com menor detecção
- Segundo mínimo
- Média do light yield `<LY>` sobre todos os bins
- Total de fótons detectados

### `make_plot.C` — utilitário de visualização de propriedades de materiais

Gera um gráfico simples a partir de arquivos `.dat` de propriedades ópticas. Edite as variáveis `path` e `file` dentro do script para selecionar o material e a propriedade desejados:

```cpp
TString path = "PEN", file = "pen_rindex";
```

Execute com:

```bash
root -l -b -q make_plot.C
```

O gráfico é salvo como PNG no mesmo diretório do arquivo `.dat`.
