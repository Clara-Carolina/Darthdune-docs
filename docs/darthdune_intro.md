# Darthdune
## Contexto Experimental

### O Experimento DUNE

O *Deep Underground Neutrino Experiment* (DUNE) tem como objetivo principal medir a violação de CP no setor de neutrinos e determinar a hierarquia de massa dos neutrinos, entre outros temas de física fundamental. O feixe de neutrinos produzido no Fermilab (Illinois, EUA) percorrerá 1.300 km até o *Sanford Underground Research Facility* (SURF), em Lead, Dakota do Sul, onde os detectores são instalados a cerca de 1,5 km de profundidade.

O Far Detector do DUNE será composto por quatro módulos de *Liquid Argon Time Projection Chamber* (LArTPC). Os dois primeiros módulos estão atualmente em construção, com designs de deriva horizontal e vertical. Para a Fase 2, um terceiro módulo de deriva vertical e um quarto módulo — chamado de *module of opportunity* — estão planejados.



### O sistema PoWER

O **PoWER** (*Polymer Wavelength shifter and Enhanced Reflection*) é o conceito de PDS proposto pelo grupo do Prof. André Steklain para o DUNE Fase 2. Sua ideia central é cobrir toda a face interna da field cage com um filme de PEN (0,1 mm) seguido de uma lâmina de acrílico (1 mm), e instalar painéis de *Enhanced Specular Reflector* (ESR, ~98% de refletividade) nas membranas do criostato para maximizar o número de reflexões até a detecção. No topo e no fundo, um material com ~30% de refletividade representa os painéis de leitura de carga.

Os fotodetectores são **1.872 LDUs** (*Light Detection Units*) instalados no lado externo da field cage, entre ela e a membrana do criostato. Cada LDU tem 20 cm × 20 cm e é composto por dois tipos de SiPMs:
- **Sensores VIS** (Hamamatsu S14160-6050HS): ocupam a região anular externa; detectam fótons visíveis gerados pelo PEN
- **Sensores VUV** (Hamamatsu VUV4 S13370): ocupam a região central de 5 cm × 5 cm; detectam diretamente fótons VUV

Os resultados preliminares da simulação indicam rendimento de luz médio de **90 PE/MeV** e mínimo de **35 PE/MeV**, superiores às metas do DUNE PDS.

### O veto ativo

O PoWER permite distinguir eventos que ocorreram dentro do volume ativo (dentro da field cage) de eventos ocorridos no *buffer* de LAr entre a field cage e o criostato. A lógica do veto é baseada na razão entre a detecção VUV e total:

$$r_{VUV} = \frac{LY_{VUV}}{LY_{VUV} + LY_{Visível}}$$

- **Evento dentro do volume ativo:** a luz VUV é convertida pelo PEN em luz visível antes de atingir os LDUs. Os sensores VIS dominam a detecção → **r_VUV ≲ 0,3**  
- **Evento no buffer:** a luz VUV não atravessa o acrílico (opaco ao VUV) e não é convertida. Somente os sensores VUV a detectam diretamente → **r_VUV ≳ 0,6**

Essa grandeza é o que o `readroothisto2d.C` calcula e plota no histograma `LightMapVeto`.

### O papel do darthdune

O darthdune é a simulação Geant4 standalone que o grupo utiliza para estudar o light yield do sistema PoWER e demonstrar o veto. Em cada execução:

1. Fótons ópticos são gerados uniformemente em um voxel do volume ativo via GPS
2. Cada fóton é propagado com os processos de reflexão, refração, espalhamento Rayleigh, absorção e wavelength shifting (via PEN)
3. Quando um fóton atinge um LDU, a eficiência quântica do SiPM correspondente é aplicada probabilisticamente
4. As coordenadas de origem de cada fóton detectado são registradas, construindo o light map

O programa darthdune foi desenvolvido com base no Geant4, com o objetivo de simular a Field Cage do DUNE com o PoWER. O resultado é o light yeld  representados em light maps. **A simulação não simula neutrinos, ela simula os fótons que seriam gerados da interação neutrino-LAr**. 

Para acessar o programa, precisa pedir autorização para acessar o [repositório do GitHub](https://github.com/steklain/darthdune/) para o professor André, e já ter o Geant4 e ROOT instalados no computador [(ver guia aqui)](./geant4_tutorial.md)

Após clonar o repositório do GitHub para o seu computador, siga os seguintes passos, idealmente utilizando o Visual Code:

Entra na pasta darthdune
Crie a pasta: mkdir build 
Vai para a pasta: cd build
Copie os arquivos: cp ../data/DarthTraptis.cfg .  e cp ../macros/geometry.mac .
Compile: cmake .. && make 
Rode o executável: ./darthtraptis -i geometry.mac

Essa é sua primeira simulação, deve abrir uma tela interativa para visualizar a field cage, além de gerar os light maps e histogramas.
Para compilar os outputs do ROOT, rode hadd output.root output_* && rm output_*
