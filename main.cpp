#include "aloca.h"
#include <iostream>
#include <map>

int main(){
  int amount, algorithm, operations;
  std::cin >> amount >> algorithm >> operations;

  meualoc alocador(amount,algorithm);
  std::map<int, char*> alocacoes;

  char command; int address,length;

  while(std::cin >> command && operations){
    --operations;
    switch(command){
      case 'A':
      std::cin >> address >> length;
      alocacoes[address] = alocador.aloca(length);
      break;
      case 'S':
      case 'F':
      std::cin >> address;
      alocador.libera(alocacoes[address]);
      break;
      case 'M':
      alocador.imprimeDados();
      break;
      case 'D':
      alocador.imprimeDados2();
      break;
      case 'V':
      std::cin >> address >> length;
      std::cout << '\n' << (alocador.verifica(alocacoes[address], length)? "Valid Offset of this pointer" : "Not alocated for this pointer") << '\n';
      break;
    }
  }

  alocador.imprimeDados3();
}
