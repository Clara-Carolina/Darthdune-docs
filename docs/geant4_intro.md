Geant4 - introdução
O Geant4 é um toolkit ( uma coleção de códigos pré-construídos, utilidades e componentes desenvolvidos para ajudar usuários configurar novos programas sem começar do zero), desenvolvido pelo CERN para simulações de partículas passando por matéria. Ele é escrito em C++ e já contém todos os aspectos necessários para este tipo de simulação:

Compiling from source

Links - Tutorial and Geant4 website
Transport simulation - simulation of particle running through matter Needs - C++ compiler, Geant4 installed and configured with both QT library (user interface and visualisation) and GDML (Geometry Description Markup Language)


For step by step guide watch the Tutorial and follow the Instalation Guide
Main classes:
The experiment manager: G4RunManger The laws of physics: G4VUserPhysicsList The detector: G4VUserDetectorConstruction List of actions: G4VUserActionInitialization *All classes with VUser in their names are virtual classes and must be inherited by your own classes

To create a program from scratch with C++:
//Any C++ program must start with int main(){ return 0; } //tells your operational system everything is fine with the program

To create a simple G4 program:
First, VS Code's IntelliSense needs to be told where Geant4 headers are located, otherwise it will underline all #include "G4*.hh" statements in red.
1. Find your Geant4 install path
find / -name "G4RunManager.hh" 2>/dev/null
Use the path under your install directory (not source), e.g. /home/user/geant4-install/include/Geant4.

### 2. Export it in your `~/.bashrc`

```bash
export G4INCLUDE=/home/user/geant4-install/include/Geant4
```

Then run `source ~/.bashrc`.

### 3. Add it to VS Code

Open the Command Palette (`Ctrl+Shift+P`) → **C/C++: Edit Configurations (JSON)** and add `${G4INCLUDE}/**` to the `includePath`:
{
  "configurations": [
    {
      "name": "Linux",
      "includePath": [
        "${workspaceFolder}/**",
        "${G4INCLUDE}/**"
      ],
      "cppStandard": "c++17"
    }
  ],
  "version": 4
}

Back to the program:
#include "G4RunManager.hh" //the main command
#include "G4RunManagerFactory.hh" //To build a new run manager

int main(){
		auto manager = G4RunManagerFactory::CreateRunManager();
		
		delete manager;
		return 0;
}
