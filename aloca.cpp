#include "aloca.h"
#include <cstdlib>
#include <cstdio>
#define HASH 17027
#define REAL_LENGTH_IN_BYTES (sizeof(unsigned short)<<1)
#define REAL_LENGTH_IN_BYTES_INDIVIDUAL (sizeof(unsigned short))
#define CHAR_LENGTH_IN_BYTES (sizeof(char))
#define throw_exception(x) printf("\n[ERROR] ");printf(x);printf("\n");

//* FreeMemorySpace FUNCTIONS *//

int FreeMemorySpaceFrame::getBestFreeSpace(unsigned short length){
  FreeMemorySpace* current = this->first;
  FreeMemorySpace* currentBest = current, *last = nullptr, *lastBest = nullptr;
  int realLength = length+REAL_LENGTH_IN_BYTES;
  int pointer=0;

  if(this->first != nullptr){
    while(current){

      if(currentBest->length > current->length){
        if(current->length >= realLength){
          lastBest = last;
          currentBest = current;
        }
      }

      last = current;
      current = current->next;
    }

    if(currentBest->length == realLength){

      if(lastBest){
        pointer = currentBest->address;
        lastBest->next = currentBest->next;
      }else{
        this->first = currentBest->next;
        pointer = currentBest->address;
      }

      delete(currentBest);

      return pointer;

    }else if(currentBest->length > realLength){
      pointer = currentBest->address;
      currentBest->address+=realLength;
      currentBest->length-=realLength;

      return pointer;
    }else{
      return -1;
    }
  }
  return -1;

}

int FreeMemorySpaceFrame::getFirstFreeSpace(unsigned short length){
  int realLength = length+REAL_LENGTH_IN_BYTES;

  FreeMemorySpace* current = this->first;
  FreeMemorySpace* last = nullptr;

  int pointer = 0;

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

int FreeMemorySpaceFrame::getNextFreeSpace(unsigned short length) {
  int realLength = length + REAL_LENGTH_IN_BYTES;

  FreeMemorySpace *current = this->lastFound;
  FreeMemorySpace *pivot = this->lastFound;
  FreeMemorySpace *last = this->beforeLastFound;
  int pointer = 0;

  if (length) {
    do {
      if (current->length < realLength){
        this->beforeLastFound = current;

        if (current->next)
        current = current->next;
        else current = this->first;

      } else if (current->length == realLength) {
        pointer = current->address;

        if (this->beforeLastFound){
          this->beforeLastFound->next = current->next;
        } else {
          this->first = current->next;
        }

        lastFound = current->next? current : this->first;
        printf("\n\nLast Found for Next-Fit: [address: %d, length : %d]\n\n", lastFound->address, lastFound->length);
        delete(current);

        return pointer;
      } else {
        pointer = current->address;
        current->address+=realLength;
        current->length-=realLength;

        lastFound = current->next? current : this->first;
        printf("\n\nLast Found for Next-Fit: [address: %d, length : %d]\n\n", lastFound->address, lastFound->length);
        return pointer;
      }
    } while (current != pivot);
  }

  return -1;
}

void FreeMemorySpaceFrame::freeSpace(int address, unsigned short size){
  FreeMemorySpace *currentSpace = this->first, *previousSpace = nullptr;
  int bottomAddressBound = address, topAddress = (address+size);
  FreeMemorySpace *coalsceFront = nullptr;
  FreeMemorySpace *coalsceBack = nullptr;
  FreeMemorySpace *newSpace;

  while (currentSpace) {

    if (currentSpace->address >= address) {
      if (currentSpace->address == topAddress) {
        coalsceFront = currentSpace;
      }
      break;
    }

    previousSpace = currentSpace;

    if ((currentSpace->address + currentSpace->length) == bottomAddressBound) {
      coalsceBack = currentSpace;
    }

    currentSpace = currentSpace->next;
  }

  if (coalsceBack == nullptr) { // não tem o de trás
    if (coalsceFront == nullptr) { // não tem o de trás nem o da frente
      newSpace = new FreeMemorySpace(address, size);
      if(previousSpace){
        previousSpace->next = newSpace;
      }else{
        this->first = newSpace;
      }

      newSpace->next = currentSpace? currentSpace : nullptr;
    }
    else { // não tem o de trás mas tem o da frente
      coalsceFront->length+=size;
      coalsceFront->address=address;
    }
  }else { // tem o de trás
    if (coalsceFront == nullptr) { // tem o de trás mas não o da frente
      coalsceBack->length+=size;
    }
    else { // tem o de trás e o da frente
      coalsceBack->length+=(size+coalsceFront->length);
      coalsceBack->next = coalsceFront->next;

      if(coalsceFront == lastFound){
        lastFound = coalsceBack->next? coalsceBack->next : this->first;
        previousSpace = coalsceBack;
      }

      delete(coalsceFront);
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
  int pointer = (frame.getBestFreeSpace(tamanho));

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

char *aloca_nf(int tamanho, char* memory, FreeMemorySpaceFrame& frame){
  int pointer = (frame.getNextFreeSpace(tamanho));

  if (pointer < 0) {
    throw_exception("OUT OF MEMORY");
    return ((char*)(memory - 1));
  } else {
    shortOnMemory(tamanho, memory, pointer);
    shortOnMemory(HASH, memory, pointer + REAL_LENGTH_IN_BYTES_INDIVIDUAL);

    pointer += REAL_LENGTH_IN_BYTES;
    return ((char*)(pointer + memory));
  }
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

char* meualoc::verifica(char* ponteiro,int posicao){
  int location = ((ponteiro)-this->memoria)-REAL_LENGTH_IN_BYTES;
  int length = getShortOnMemory(this->memoria,location);
  int hash = getShortOnMemory(this->memoria, ((ponteiro)-this->memoria)-REAL_LENGTH_IN_BYTES_INDIVIDUAL);
  int realSpaceThreshold = location+posicao;
  if(hash != HASH || realSpaceThreshold >= location+length || posicao < 0){
    return nullptr;
  }else{
    return ((char*) ponteiro+posicao);
  }

  return nullptr;
}

void meualoc::imprimeDados(){
  FreeMemorySpace *space = this->memoryFrame.first;
  printf("\n\n---\nCurrent Free Space : \n");

  while(space){
    printf("[address: %d, length : %d]~>",space->address,space->length);
    space = space->next;
  }
  printf("\n\nLast Found for Next-Fit: [address: %d, length : %d]\n\n", this->memoryFrame.lastFound->address, this->memoryFrame.lastFound->length);

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

    this->memoryFrame.lastFound = firstSpace;
    this->memoryFrame.beforeLastFound = nullptr;

    aloca_backend = (&aloca_nf);
    break;
    case FIRSTFIT:
    aloca_backend = (&aloca_ff);
    break;
  }
}
