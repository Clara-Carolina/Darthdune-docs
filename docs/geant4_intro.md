# **Geant4 - introdução**

O Geant4 é um toolkit (uma coleção de códigos pré-construídos, utilidades e componentes desenvolvidos para ajudar usuários configurar novos programas sem começar do zero), desenvolvido pelo CERN para simulações de partículas e suas interações com a matéria. Ele é escrito em C++ e já contém todos os aspectos necessários para este tipo de simulação:

- A geometria do sistema,
- Os materiais envolvidos,
- As partículas fundamentais de interesse,
- A geração de eventos primários,
- O rastreamento de partículas através de materiais e campos eletromagnéticos,
- Os processos físicos que governam as interações das partículas,
- A resposta de componentes sensíveis do detector,
- A geração de dados de eventos,
- O armazenamento de eventos e trajetórias,
- A visualização do detector e das trajetórias das partículas, e
- A captura e análise de dados de simulação em diferentes níveis de detalhe e refinamento

Para maiores detalhes sobre o programa, visitar o [site oficial](https://geant4-userdoc.web.cern.ch/UsersGuides/IntroductionToGeant4/html/index.html)

O programa darthdune foi montado a partir do Geant4, então é necessária sua instalação e um pouco de familiaridade com a estrutura do programa, um guia básico pode ser encontrado aqui

## Instalação do Geant4 (Linux/Ubuntu)
Intruções baseadas na [documentação oficial do Geant4 v11.2](https://geant4-userdoc.web.cern.ch/UsersGuides/InstallationGuide/html/index.html) e no tutorial disponível no [youtube](https://www.youtube.com/watch?v=UyXFBP-dVHI)

> Nota: este guia cobre instalação em sistemas Linux (Ubuntu/Debian), para outros sistemas, consulte a documentação oficial

## Pré-requisitos

Antes de começar, verifique se os itens abaixo estão instalados:

GCC (compilador C++) 8 ou superior - como verificar: `gcc --version` 
CMake 3.16 ou superior - como verificar: `cmake --version` |
make qualquer: `make --version` |

Para instalar ou atualizar no Ubuntu/Debian:

```bash
sudo apt-get update
sudo apt-get install gcc g++ cmake make
```

Para visualização 3D e interface gráfica, instale também:

```bash
sudo apt-get install libx11-dev libxmu-dev libxi-dev \
  libxrender-dev libxrandr-dev libxxf86vm-dev \
  qt5-default libqt5opengl5-dev \
  libxerces-c-dev
```
> QT library (responsável pela interface do usuário e visualização)

## 1. Download do código-fonte

Acesse a página oficial de download:
[https://cern.ch/geant4/download](https://cern.ch/geant4/download)

Baixe o arquivo `.tar.gz` da versão mais recente (ex: `geant4-v11.2.0.tar.gz`)

## 2. Estrutura de diretórios

O Geant4 exige três diretórios separados: fonte, build e instalação -> Nunca compile dentro do diretório de fonte
Crie a seguinte estrutura (ajuste o caminho conforme preferir):

```bash
mkdir -p ~/geant4
cd ~/geant4

# Extraia o código-fonte
tar -xzf geant4-v11.2.0.tar.gz

# Crie os diretórios de build e instalação
mkdir geant4-v11.2.0-build
mkdir geant4-v11.2.0-install
```

Você deve ter agora:
~/geant4/
├── geant4-v11.2.0/          ← código-fonte (não mexa aqui)
├── geant4-v11.2.0-build/    ← onde a compilação acontece
└── geant4-v11.2.0-install/  ← onde o Geant4 será instalado

## 3. Configuração com CMake

Entre no diretório de build e rode o CMake apontando para o fonte e para o destino de instalação:

```bash
cd ~/geant4/geant4-v11.2.0-build

cmake \
  -  -DCMAKE_INSTALL_PREFIX=$HOME/geant4/geant4-v11.2.0-install \
  -DGEANT4_INSTALL_DATA=ON \
  -DGEANT4_USE_QT=ON \
  -DGEANT4_USE_GDML=ON \
  ../geant4-v11.2.0
```
Se o CMake retornar erros, encontre a mensagem de erro, ela que vai dizer qual dependência está faltando
Se o CMake finalizar com `-- Configuring done` e `-- Build files have been written to: ...`, a configuração foi bem-sucedida

## 4. Compilação

Rode o `make` com paralelismo para acelerar o processo. Substitua `N` pelo número de núcleos da sua máquina (use `nproc` para descobrir):
```bash
make -jN
```
Exemplo com 4 núcleos:
```bash
make -j4
```
A compilação pode levar de 20 minutos a mais de 1 hora dependendo da máquina

## 5. Instalação

Se a compilação terminar sem erros:
```bash
make install
```
Isso copia as bibliotecas, headers e arquivos de suporte para o diretório de instalação 

## 6. Configuração do ambiente

Para que o sistema encontre as bibliotecas do Geant4, é necessário carregar o script de ambiente a cada sessão. Adicione a linha abaixo ao seu `~/.bashrc` para que isso aconteça automaticamente:

```bash
echo "source $HOME/geant4/geant4-v11.2.0-install/bin/geant4.sh" >> ~/.bashrc
source ~/.bashrc
```
Verifique se funcionou:

```bash
echo $G4INSTALL
```
Se retornar o caminho da instalação, o ambiente está configurado corretamente

## 7. Verificação: rodando o exemplo B1

O Geant4 vem com exemplos prontos. O `B1` é o mais simples e serve para confirmar que tudo funciona

```bash
# Copie o exemplo para fora do diretório de instalação
cp -r $HOME/geant4/geant4-v11.2.0-install/share/Geant4/examples/basic/B1 \
      $HOME/geant4/example-B1

# Crie o diretório de build do exemplo
mkdir $HOME/geant4/example-B1-build
cd $HOME/geant4/example-B1-build

# Configure e compile
cmake ../example-B1
make -j4

# Execute
./exampleB1
```

Se abrir uma janela com a visualização 3D do detector, a instalação está completa

## Problemas comuns

**`cmake: command not found`**
Instale com `sudo apt-get install cmake`

**`Could not find Qt5`**
Instale com `sudo apt-get install qt5-default libqt5opengl5-dev`

**`make` muito lento ou travando**
Reduza o paralelismo: tente `make -j2` ou `make -j1`

**Erro de dataset ausente ao rodar uma simulação**
Certifique-se de que usou `-DGEANT4_INSTALL_DATA=ON` na etapa do CMake, ou baixe os datasets manualmente em [https://cern.ch/geant4-data/datasets](https://cern.ch/geant4-data/datasets).

**Variáveis de ambiente não carregam**
Confirme que a linha `source ...geant4.sh` está no `~/.bashrc` e rode `source ~/.bashrc`.


## Instalação do ROOT

O ROOT é um framework de análise de dados do CERN, utilizado pelo darthdune para histogramas, árvores de dados e análise dos outputs das simulações. Ele é instalado separadamente do Geant4

Os métodos recomendados para Ubuntu são o binário pré-compilado (descrito abaixo) ou o [conda](https://root.cern/install/#conda).

### 1. Instale as dependências

Pacotes obrigatórios:

```bash
sudo apt install binutils cmake dpkg-dev g++ gcc libssl-dev git \
  libx11-dev libxext-dev libxft-dev libxpm-dev python3 \
  libtbb-dev libvdt-dev libgif-dev
```

Pacotes opcionais mais comuns (recomendado instalar agora para evitar problemas ao compilar projetos depois):

```bash
sudo apt install gfortran libpcre3-dev \
  libglu1-mesa-dev libglew-dev libftgl-dev \
  libfftw3-dev libcfitsio-dev libgraphviz-dev \
  libavahi-compat-libdnssd-dev libldap2-dev \
  python3-dev python3-numpy libxml2-dev libkrb5-dev \
  libgsl-dev nlohmann-json3-dev libmysqlclient-dev \
  libgl2ps-dev liblzma-dev libxxhash-dev liblz4-dev \
  libzstd-dev libcurl4-openssl-dev
```

### 2. Baixe o binário pré-compilado

Acesse a página de releases e baixe o arquivo correspondente à sua distribuição e versão do GCC: [https://root.cern/install/all_releases](https://root.cern/install/all_releases)
Sempre use a versão mais recente. Verifique sua versão do Ubuntu com> `lsb_release -a` e do GCC com `gcc --version` para escolher o binário certo

### 3. Descompacte o arquivo

```bash
cd ~
tar -xzvf root_v6.40.00.Linux-ubuntu22.04-x86_64-gcc11.4.tar.gz
```

Isso cria o diretório `~/root/`

### 4. Configure o ambiente

Adicione ao `~/.bashrc`:

```bash
echo "source $HOME/root/bin/thisroot.sh" >> ~/.bashrc
source ~/.bashrc
```

Verifique:

```bash
root --version
```
Se retornar a versão instalada, o ROOT está pronto.

 **Atenção:** a linha `source thisroot.sh` deve aparecer no `.bashrc` **antes** da linha do Geant4 (`source geant4.sh`)
 
 ### Problemas comuns

**`ERROR in cling::CIFactory::createCI(): cannot extract standard library include paths`**
Você baixou o binário errado para sua versão do GCC, verifique novamente conforme o passo 1

**`root: command not found` mesmo após configurar o `.bashrc`**
Confirme que a linha `source .../thisroot.sh` está no `~/.bashrc` e que o caminho está correto. Rode `source ~/.bashrc` e tente novamente

**ROOT funcionava e parou de funcionar após instalar outras bibliotecas**
Alguns pacotes de desenvolvimento conflitam com o binário pré-compilado. Se isso acontecer, a solução mais confiável é recompilar o ROOT da fonte ou usar conda. Consulte o [ROOT Forum](https://root-forum.cern.ch) com a mensagem de erro exata

**`find_package(ROOT)` falha no CMake do darthdune**
O `thisroot.sh` não está sendo carregado antes do CMake. Confirme a ordem no `.bashrc`: ROOT deve ser sourced antes do Geant4.


