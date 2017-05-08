#define BESTFIT 0
#define FIRSTFIT 1
#define NEXTFIT 2

struct FreeMemorySpace{
	unsigned int address;
	unsigned int length;
	FreeMemorySpace *next;
	FreeMemorySpace(int address_fms, unsigned short length_fms);
};

struct FreeMemorySpaceFrame{
	FreeMemorySpace *first;
	int fragments;
	int getFirstFreeSpace(unsigned short length);
	int getNextFreeSpace(unsigned short length);
	int getBestFreeSpace(unsigned short length);
	void freeSpace(int address, unsigned short length);
	FreeMemorySpaceFrame();
};

class meualoc{
	int length;
	char* memoria; //char* pois eh byte a byte
	char*(*aloca_backend)(int tamanho, char* memory, FreeMemorySpaceFrame &frame);
	FreeMemorySpaceFrame memoryFrame;
public:
	//tamanhoMemoria vai definir o tamanho da memória que o alocador vai utilizar
	//politicaMem define como escolher o bloco de onde saira a memória
	meualoc(int tamanhoMemoria,int politicaMem);

	//Pega um pedaco da variavel memoria e aloca, retornando o ponteiro para o comeco dessa regiao e atualizando a lista de livres.
	char *aloca(unsigned short int tamanho);

	//Verifica se a posicao dada pelo ponteiro esta alocada
	char* verifica(char* ponteiro,int posicao=0);

	//Libera o espaco ocupado na posicao, de forma analoga ao free. Ou seja, liberar toda a memoria alocada para este ponteiro na funcao aloca.
	int libera(char* ponteiro);

	//Imprime o numero de elementos na lista de vazios, o maior e a media de tamanhos dos blocos vazios
	void imprimeDados();

	~meualoc();


};
