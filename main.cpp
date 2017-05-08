#include "aloca.h"
#include <iostream>

int main(){
  meualoc alocador(40,BESTFIT);
  char* alocacoes[10];
  char command; int address,length;

  while(std::cin >> command){
    switch(command){
      case 'A':
      std::cin >> address >> length;
      alocacoes[address] = alocador.aloca(length);
      break;
      case 'F':
      std::cin >> address;
      alocador.libera(alocacoes[address]);
      break;
      case 'M':
      alocador.imprimeDados();
      break;
      case 'V':
      std::cin >> address >> length;
      std::cout << '\n' << (alocador.verifica(alocacoes[address], length)? "Valid Offset of this pointer" : "Not alocated for this pointer") << '\n';
      break;
    }
  }
}
