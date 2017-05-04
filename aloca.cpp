#include "aloca.h"
#include <cstdlib>
#include <cstdio>
#define HASH 17027
#define REAL_LENGTH_IN_BYTES (sizeof(unsigned short)<<1)
#define REAL_LENGTH_IN_BYTES_INDIVIDUAL (sizeof(unsigned short))
#define CHAR_LENGTH_IN_BYTES (sizeof(char))
#define throw_exception(x) printf("\n[ERROR] ");printf(x);printf("\n");


//* FreeMemorySpace FUNCTIONS *//

int FreeMemorySpaceFrame::getFirstFreeSpace(unsigned short length){
  int realLength = length+REAL_LENGTH_IN_BYTES;

  FreeMemorySpace* current = this->first;
  FreeMemorySpace* last = nullptr;
  int pointer =0;
  if(length){
    while(current){

      if(current->length < realLength){
        current = current->next;
      }else if(current->length == realLength){
        pointer = current->address;

        if(last){
          last->next = current->next;
        }else{
          this->first = current->next;
        }

        delete(current);

        return pointer;
      }else{
        pointer = current->address;
        current->address+=realLength;
        current->length-=realLength;
        return pointer;
      }

      last = current;
    }
  }
  return -1;
}

// I lost this function
void FreeMemorySpaceFrame::freeSpace(int address, unsigned short end){
  FreeMemorySpace *currentSpace = this->first, *lastSpace = nullptr;
  int bottomBound = address, topBound = (address+end+REAL_LENGTH_IN_BYTES);
  bool frontMoved=false;

  if(this->first != nullptr){

    while(currentSpace){

      if(currentSpace->address >= address){

        if(currentSpace->address == topBound){
          currentSpace->address=address;
          currentSpace->length+=end+REAL_LENGTH_IN_BYTES;
          frontMoved = true;
        }

        break;
      }else{
        lastSpace = currentSpace;
        currentSpace = currentSpace->next;
      }
    }

  }
}

FreeMemorySpace::FreeMemorySpace(int address_fms, unsigned short length_fms){
  this->address = address_fms;
  this->length = length_fms;
  this->next = nullptr;
}


FreeMemorySpaceFrame::FreeMemorySpaceFrame(){
  this->first = nullptr;
  this->fragments = 0;
}






//* MEUALOC FUNCTIONS *//

void shortOnMemory(unsigned short number, char* memory, int offset){
  char compass;
  int endOffset = ((REAL_LENGTH_IN_BYTES_INDIVIDUAL)+offset);
  int CHAR_LENGTH_IN_BYTES_IN_BITS = CHAR_LENGTH_IN_BYTES*8;
  int currentIndex = (endOffset-1);
  for(unsigned int i=offset;i<endOffset;i++, currentIndex--){
    compass = 0;

    for(unsigned int c = 0; c < CHAR_LENGTH_IN_BYTES_IN_BITS; c++){
      compass |= (number & (1<<c));
      //printf("[%d]",(number & (1<<c)) > 0);
    }
    //printf("\n");

    number = number >> CHAR_LENGTH_IN_BYTES_IN_BITS;
    memory[currentIndex] = compass;
  }
  //printf("\n");
}

unsigned short getShortOnMemory(char* memory, int position){
  unsigned short number=0, sumNumber;
  char compass;
  int endOffset = ((REAL_LENGTH_IN_BYTES_INDIVIDUAL)+position);
  int CHAR_LENGTH_IN_BYTES_IN_BITS = CHAR_LENGTH_IN_BYTES*8;
  int offsetIndex = (endOffset-1);
  int bitwiseIndex = 0;
  for(unsigned int i=position;i<endOffset;i++, offsetIndex--, bitwiseIndex++){

    compass = memory[offsetIndex];
    sumNumber = 0;

    for(unsigned int c = 0; c < CHAR_LENGTH_IN_BYTES_IN_BITS; c++){
      sumNumber |= compass & (1 << c);
    }
    number |= sumNumber << (CHAR_LENGTH_IN_BYTES_IN_BITS*bitwiseIndex);
  }
  return number;
}

char *aloca_ff(int tamanho, char* memory, FreeMemorySpaceFrame& frame){
  int pointer = (frame.getFirstFreeSpace(tamanho));

  if(pointer<0){
    throw_exception("OUT OF MEMORY");
    return ((char*)(memory-1));
  }else{
    shortOnMemory(tamanho, memory, pointer);
    shortOnMemory(HASH, memory, pointer+REAL_LENGTH_IN_BYTES_INDIVIDUAL);

    pointer+=REAL_LENGTH_IN_BYTES;
    return ((char*)(pointer+memory));
  }

}

char *aloca_bf(int tamanho, char* memory, FreeMemorySpaceFrame& frame){
  return nullptr;
}

char *aloca_nf(int tamanho, char* memory, FreeMemorySpaceFrame& frame){
  return nullptr;
}


int meualoc::libera(char* ponteiro){
  int location = ((ponteiro)-this->memoria)-REAL_LENGTH_IN_BYTES;
  int length = getShortOnMemory(this->memoria,location);
  int hash = getShortOnMemory(this->memoria, ((ponteiro)-this->memoria)-REAL_LENGTH_IN_BYTES_INDIVIDUAL);
  if(hash == HASH || ponteiro < this->memoria || ponteiro > this->memoria+(this->length*CHAR_LENGTH_IN_BYTES)){
    this->memoryFrame.freeSpace(location,length+REAL_LENGTH_IN_BYTES);
    return 1;
  }else{
    throw_exception("Free Pointer Corruption");
    return 0;
  }
}

char *meualoc::verifica(int ponteiro){
  return nullptr;
}

void meualoc::imprimeDados(){
  FreeMemorySpace *space = this->memoryFrame.first;
  printf("\n\n---\nCurrent Free Space : \n");

  while(space){
    printf("[address: %d, length : %d]~>",space->address,space->length);
    space = space->next;
  }
  printf("\nCurrent Memory State : \n");
  for(int i=0;i<length;i++){
    printf("[%d]", this->memoria[i]);
  }

  printf("\n---\n\n");
}

char* meualoc::aloca(unsigned short int tamanho){
  return (this->aloca_backend)(tamanho,this->memoria,this->memoryFrame);
}

meualoc::~meualoc(){
  delete(this->memoria);
}

meualoc::meualoc(int tamanhoMemoria,int politicaMem){
  FreeMemorySpace *firstSpace = new FreeMemorySpace(0, tamanhoMemoria);
  this->memoria = new char[tamanhoMemoria];
  this->memoryFrame.first = firstSpace;
  this->length = tamanhoMemoria;
  
  switch(politicaMem){
    case BESTFIT:
    aloca_backend = (&aloca_bf);
    break;
    case NEXTFIT:
    aloca_backend = (&aloca_nf);
    break;
    case FIRSTFIT:
    aloca_backend = (&aloca_ff);
    break;
  }
}
