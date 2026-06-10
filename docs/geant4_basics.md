Esse arquivo é baseado no [tutorial](https://www.youtube.com/watch?v=zV9jTIykmr0&t=1172s) e no programa [original](https://github.com/gustavogx/geant4-tutorial/blob/master/001/main.cpp) em inglês 

## Entendendo um programa básico de Geant4

No caso de um programa grande como o Geant4, ele precisa ser dividido em unidades lógicas menores chamadas de categorias de classes. Cada categoria de classe tem uma função, para maiores detalhes visitar o [link](https://geant4-userdoc.web.cern.ch/UsersGuides/AllGuides/html/ForApplicationDevelopers/Fundamentals/classCategory.html)

As classes principais são:

O gerenciador da simulação: G4RunManger
As leis da física: G4VUserPhysicsList
O detector: G4VUserDetectorConstruction
A lista de ações: G4VUserActionInitialization
Nota:** classes com `VUser` no nome são classes virtuais e não podem ser usadas diretamente. Você deve criar sua própria classe herdando delas e implementar os métodos exigidos

Abaixo temos um exemplo de um programa básico montado no Geant4, para tutorial de instalação e configuração veja [`geant4_tutorial.md`](./geant4_tutorial.md)


