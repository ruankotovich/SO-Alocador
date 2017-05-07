#include "aloca.h"
#include <iostream>

int main(){
    meualoc alocador(30,FIRSTFIT);
    char* alocacoes[30];
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
        }
    }
}
