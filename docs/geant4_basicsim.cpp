// Esse arquivo é baseado no tutoria (https://www.youtube.com/watch?v=zV9jTIykmr0&t=1172s) e no programa original (https://github.com/gustavogx/geant4-tutorial/blob/master/001/main.cpp) em inglês.

/* ========================================================================

	TUTORIAL 001 - Simulação mais simples 

	Essa é uma simulação feita da forma mais básica usando Geant4, v11.2.2.
	Esse programa simula 1 GeV de elétrons por 10m de argônio líquido. 
	O programa separado original é nomeado main.cpp.
	
	Para compilar:
	
	1) Usando o próprio compilador:

	$ g++ main.cpp -o tutorial `geant4-config --libs -cflags` 

	2) Usando cmake e make:

	Crie um diretório para o build, e rode o cmake dele:

	$ mkdir build && cd build
	$ cmake ..
	$ make

	Gustavo Valdiviesso 2024, traduzido e adaptado por Clara Carolina, 2026
   ======================================================================== */


// Primeiro precisamos chamar as classes básicas do Geant4
#include "G4VUserActionInitialization.hh"
#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "G4Electron.hh"
#include "G4Event.hh"

// Essa é a sua fonte de partículas. As primárias são as primeiras partículas
// entrando no detector. Precisamos especificar qual partículas queremos,
// sua origem e momentum (incluindo a energia).
class MyPrimaryGenerator : public G4VUserPrimaryGeneratorAction {
public:
	MyPrimaryGenerator();
	~MyPrimaryGenerator();
	virtual void GeneratePrimaries(G4Event* anEvent) override;

private:
	G4ParticleGun* fParticleGun;  // Atributo da classe para evitar memory leak
};

MyPrimaryGenerator::MyPrimaryGenerator() {
	// O gun é criado uma única vez, no construtor, não a cada evento
	fParticleGun = new G4ParticleGun( G4Electron::Definition() );

	// Definimos explicitamente a energia, direção e posição inicial
	// para deixar claro o que estamos simulando
	fParticleGun->SetParticleEnergy(1*GeV);
	fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0, 0, 1)); // direção +z
	fParticleGun->SetParticlePosition(G4ThreeVector(0, 0, 0));          // origem
}

MyPrimaryGenerator::~MyPrimaryGenerator() {
	delete fParticleGun;
}

void MyPrimaryGenerator::GeneratePrimaries(G4Event* anEvent){

	fParticleGun->GeneratePrimaryVertex(anEvent);

	G4cout << "Event ID      " << anEvent->GetEventID() << G4endl;
	G4cout << "Particle Name " << fParticleGun->GetParticleDefinition()->GetParticleName() << G4endl;
	G4cout << "Energy (MeV)  " << fParticleGun->GetParticleEnergy() << G4endl;
};

// Para nossa primeira ação, precisamos gerar nossas partículas
class MyActionInitialization: public G4VUserActionInitialization{
public:
	virtual void Build() const override;
};

void MyActionInitialization::Build() const {

	// E então, iniciar a fonte de partículas
	SetUserAction( new MyPrimaryGenerator );
};

// O detector:
#include "G4VUserDetectorConstruction.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"
```
class MyDetector: public G4VUserDetectorConstruction {
public:
	virtual G4VPhysicalVolume* Construct() override;
};

G4VPhysicalVolume* MyDetector::Construct() {

	// A geometria do detector = O Modelo Geométrico + O Modelo de Materiais
	// O Modelo Geométrico: Refere-se à definição dos volumes, formatos, tamanhos e posições dos componentes na sua simulação
	// O Modelo de Materiais: define os materiais específicos (ex: chumbo, argônio, silício, água) que compõem cada parte da geometria
	// Para os detalhes do material, podemos pedir ao Gerenciador NIST um material pronto para ser usado
    // Você pode encontrar mais informações em:
    // https://geant4-userdoc.web.cern.ch/UsersGuides/ForApplicationDeveloper/html/Appendix/materialNames.html
	auto lAr = G4NistManager::Instance()->FindOrBuildMaterial("G4_lAr");
	
	// No Geant4, nenhuma simulação acontece no vazio. Tudo precisa existir dentro de um espaço delimitado: o Mundo (World). 
	// Pense nele como as paredes do laboratório, se uma partícula cruzar essa fronteira, ela deixa de ser rastreada
	// A construção de qualquer volume exige três etapas. A linha abaixo define a primeira delas: o Sólido (a forma geométrica).
	// Para outros sólidos, consultar
	// https://geant4-userdoc.web.cern.ch/UsersGuides/ForApplicationDeveloper/html/Detector/Geometry/geomSolids.html
	auto worldBox = new G4Box("worldBox", 10000, 10000, 10000);

	// A segunda etapa é a definição de Um volume lógico, que é a união de uma geometria (sólido) e um material
    auto logicalWorld = new G4LogicalVolume(worldBox, lAr, "logicalWorld");

    // A terceira etapa é o volume físico, que é realização de um volume lógico. É algo que possui posição e
    // orientação no espaço.
    auto physicalWorld = new G4PVPlacement(0, {0,0,0}, logicalWorld, "physicalWorld", 0, false, 0);

return physicalWorld;
};


// O gerenciador
#include "G4RunManager.hh"
#include "G4RunManagerFactory.hh"

// A física
#include "G4VUserPhysicsList.hh"
#include "G4PhysListFactory.hh"
// Nós vamos importar uma Physics List para definir as as partículas e processos físicos (como ionização, efeito fotoelétrico, etc.) que estarão ativos na simulação da físca 
// A utilizada nesse caso é chamada de "Shielding", para outras opções visite: 
// https://geant4-userdoc.web.cern.ch/UsersGuides/ForApplicationDeveloper/html/TrackingAndPhysics/physicsProcess.html

// Esse é o código principal
int main(){

	auto manager = G4RunManagerFactory::CreateRunManager();

	// Podemos obter um ponteiro para uma Physics List existente a partir da Factory
	auto physicsListFactory = new G4PhysListFactory();
	auto physicsList = physicsListFactory->GetReferencePhysList("Shielding");
	delete physicsListFactory; // Não esquecer de deletar a Physics List 

	manager->SetUserInitialization( new MyDetector );
	manager->SetUserInitialization( physicsList );
	manager->SetUserInitialization( new MyActionInitialization );
	manager->Initialize();
	manager->BeamOn(1000);

	delete manager; // O gerenciador vai deletar todos os processos gerenciados por ele.
	
	return 0;
}

/* ========================================================================

	O QUE ESPERAR AO RODAR

	O terminal deve mostrar algo como:

	**************************************************************
	 Geant4 version Name: geant4-11-02-patch-02 [MT]
	 ...
	Event ID      0
	Particle Name e-
	Energy (MeV)  1000

	... (repetido para cada evento)

	Se a simulação encerrar com "Run Summary: Total number of events = 10",
	funcionou corretamente.

	PRÓXIMOS PASSOS

	Este tutorial é a base. No projeto darthdune, o detector não é uma
	caixa de argônio simples — é a geometria do campo elétrico de uma LArTPC
	com o sistema de detecção de fótons PoWeR. Mas os mesmos três passos
	(Sólido → Volume Lógico → Volume Físico) aparecem em toda a construção
	dessa geometria.

   ======================================================================== */

