#include <stdio.h>
//#include <conio.h> //for windows
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>  //for linux
#include <sys/types.h> //for linux

#define TRUE 1
#define FALSE 0

#define INICIO 0
#define ATUAL 1
#define FINAL 2
#define SIZE 256

typedef struct reg {
	char cod[4];
	char nome[50];
	char seg[50];
	char tipo[30];
}Registro;

Registro registros[8];
char buscaCodigos[7][4];
char buscaNomes[5][50];
int arquivosCarregados = FALSE;

void insereRegistro(Registro registro);
void pesquisaCodigo(char cod[3]);
void pesquisaNome(char nome[50]);
void hexDump(size_t, void *, int);
void carregaArquivos();
void obtemCache(int* indiceRegistro, int* indiceCodigo, int* indiceNome);
void atualizaCache(int indiceRegistro, int indiceCodigo, int indiceNome);

int main(void){
	int escolha=0;
	int indiceRegistro;
	int indiceCodigo;
	int indiceNome;
		
	DIR* temp = opendir("temp");
	
	if(temp)
		closedir(temp);
	else
		mkdir("temp", 0777); //for linux
		//mkdir("temp") //for windows
	
	obtemCache(&indiceRegistro, &indiceCodigo, &indiceNome);
	
	printf("///////////////  SISTEMA DE REGISTRO DE SEGURADORAS  ///////////////\n");
	printf("///////////////                MENU                  ///////////////\n");
	printf("///////////////  1. INSERIR                          ///////////////\n");
	printf("///////////////  2. PESQUISAR POR CODIGO             ///////////////\n");
	printf("///////////////  3. PESQUISAR POR NOME               ///////////////\n");
	printf("///////////////  4. DUMP DE ARQUIVO                  ///////////////\n");
	printf("///////////////  5. CARREGAR ARQUIVOS                ///////////////\n");
	printf("/////////////// -1. SAIR                             ///////////////\n\n$ ");
	
	while(escolha != -1){
		scanf("%d", &escolha);
		if(escolha == -1) break;
		switch(escolha){
			case 1:
				insereRegistro(registros[indiceRegistro]);
				indiceRegistro++;
				if(indiceRegistro == 8){
					printf("Todos os registros foram adicionados, voltando para o index 0...\n");
					indiceRegistro = 0;
				}
				atualizaCache(indiceRegistro, indiceCodigo, indiceNome);
				break;
			case 2:
				pesquisaCodigo(buscaCodigos[indiceCodigo]);
				indiceCodigo++;
				if(indiceCodigo == 7){
					printf("Todos os codigos foram buscados, voltando para o index 0...\n");
					indiceCodigo = 0;
				}
				atualizaCache(indiceRegistro, indiceCodigo, indiceNome);
				break;
			case 3:
				pesquisaNome(buscaNomes[indiceNome]);
				indiceNome++;
				if(indiceNome == 5){
					printf("Todos os nomes foram buscados, voltando para o index 0...\n");
					indiceNome = 0;
				}
				atualizaCache(indiceRegistro, indiceCodigo, indiceNome);
				break;
			case 4: { 
				FILE *myfile;
				int c;
				
				printf("Escolha o arquivo para fazer o dump:\n\n");
				if(fopen("./temp/data.bin", "rb") != NULL)
					printf(" 1. data.bin\n");
				if(fopen("./temp/cache.bin", "rb") != NULL)
					printf(" 2. cache.bin\n");
				if(fopen("./temp/codes.bin", "rb") != NULL)
					printf(" 3. codes.bin\n");
				if(fopen("./temp/names.bin", "rb") != NULL)
					printf(" 4. names.bin\n");
				printf("-1. RETORNAR\n\n");
				printf("$ ");
				
				scanf("%d", &c);
				
				if(c == -1) break;
				
				switch(c){
					case 1:
						myfile = fopen("./temp/data.bin", "rb");
						break;
					case 2:
						myfile = fopen("./temp/cache.bin", "rb");
						break;
					case 3:
						myfile = fopen("./temp/codes.bin", "rb");
						break;
					case 4:
						myfile = fopen("./temp/names.bin", "rb");
				}
				printf("\n");
				
				unsigned char buffer[SIZE];
			    size_t n;
			    size_t offset = 0;
			    
			    while ((n = fread(buffer, 1, SIZE, myfile)) > 0){
			        hexDump(offset, buffer, n);
			        if (n < SIZE)
			        	break;
			        offset += n;
			    }				

    			fclose(myfile);
    			break;
			}case 5:
				carregaArquivos();
				break;
		}
		printf("\n$ ");
	}
	return(0);
}

void insereRegistro(Registro registro){
	if(arquivosCarregados == FALSE){
		printf("Voce nao carregou arquivos, tente carregar!\n");
		return;
	}

	char buffer[sizeof(Registro)];
	
	sprintf(buffer, "%s#%s#%s#%s", registro.cod, registro.nome, registro.seg, registro.tipo);
	int tamanhoRegistro = strlen(buffer);
	
	FILE *data;
	
	data = fopen("./temp/data.bin", "r+b");
	
	if(data == NULL){
		printf("Arquivo data.bin criado!\n");
		data = fopen("./temp/data.bin", "w+b");
	}else
		fseek(data, 0, FINAL);	
	
	int posicaoData = ftell(data);

	fwrite(&tamanhoRegistro, sizeof(int), 1, data);
	fwrite(&buffer, sizeof(char), tamanhoRegistro, data);
	
	fclose(data);

	FILE *index;

	index = fopen("./temp/codes.bin", "r+b");

	if(index == NULL){
		printf("Arquivo codes.bin criado!\n");
		index = fopen("./temp/codes.bin", "w+b");
	}else
		fseek(index, 0, FINAL);
	
	fwrite(&registro.cod, sizeof(char), 3, index);
	int posicaoCod = ftell(index);
	fwrite(&posicaoData, sizeof(int), 1, index);

	fclose(index);

	FILE *names;

	names = fopen("./temp/names.bin", "r+b");

	if(names == NULL){
		printf("Arquivo names.bin criado!\n");
		names = fopen("./temp/names.bin", "w+b");
	}else
		fseek(names, 0, FINAL);
	
	fwrite(&registro.nome, sizeof(char), 50, names);
	fwrite(&posicaoCod, sizeof(int), 1, names);

	fclose(names);

	printf("Registro [%s, %s, %s, %s] inserido com sucesso!\n", registro.cod, registro.nome, registro.seg, registro.tipo);
}

void pesquisaCodigo(char cod[3]){
	if(arquivosCarregados == FALSE){
		printf("Voce nao carregou arquivos, tente carregar!\n");
		return;
	}

	FILE *index;

	index = fopen("./temp/codes.bin", "rb");

	if(index == NULL){
		printf("Arquivo codes.bin ainda nao existe, tente adicionar um registro!\n");
		return;
	}
	
	char bufferedCod[3];
	
	while(strcmp(bufferedCod, cod)){
		fread(&bufferedCod, sizeof(char), 3, index);
		fseek(index, sizeof(int), ATUAL);
		if(feof(index)) return;
	}

	if(feof(index)){
		printf("Codigo nao encontrado!");
		return;
	}

	int offset;
	fseek(index, -sizeof(int), ATUAL);
	fread(&offset, sizeof(int), 1, index);

	fclose(index);

	FILE *data;

	data = fopen("./temp/data.bin", "rb");

	fseek(data, offset, INICIO);
	
	int size;
	fread(&size, sizeof(int), 1, data);

	char registro[size];
	fread(&registro, sizeof(char), size, data);

	fclose(data);

	printf("O registro referente ao codigo %s eh: %s\n", cod, registro);
}

void pesquisaNome(char nome[50]){
	if(arquivosCarregados == FALSE){
		printf("Voce nao carregou arquivos, tente carregar!\n");
		return;
	}

	FILE *names;

	names = fopen("./temp/names.bin", "rb");

	if(names == NULL){
		printf("Arquivo names.bin ainda nao existe, tente adicionar um registro!\n");
		return;
	}

	char bufferedName[50];
	
	while(strcmp(bufferedName, nome)){
		fread(&bufferedName, sizeof(char), 50, names);
		fseek(names, sizeof(int), ATUAL);
		if(feof(names)) return;
	}

	if(feof(names)){
		printf("Nome nao encontrado!");
		return;
	}

	int offset;
	fseek(names, -sizeof(int), ATUAL);
	fread(&offset, sizeof(int), 1, names);

	fclose(names);

	FILE *index;

	index = fopen("./temp/codes.bin", "rb");

	fseek(index, offset, INICIO);

	fread(&offset, sizeof(int), 1, index);

	fclose(index);

	FILE *data;

	data = fopen("./temp/data.bin", "rb");

	fseek(data, offset, INICIO);
	
	int size;
	fread(&size, sizeof(int), 1, data);

	char registro[size];
	fread(&registro, sizeof(char), size, data);

	fclose(data);

	printf("O registro referente ao nome %s eh: %s\n", nome, registro);
}

void carregaArquivos(){
	FILE *insere;
	
	insere = fopen("./temp-testes/insere.bin", "r+b");
	fread(&registros, sizeof(struct reg), 8, insere);
	fclose(insere);
		
	FILE *codigos;
	
	codigos = fopen("./temp-testes/busca_p.bin", "r+b");
	fread(&buscaCodigos, sizeof(char[4]), 7, codigos);
	fclose(codigos);

	FILE *nomes;
	
	nomes = fopen("./temp-testes/busca_s.bin", "r+b");
	fread(&buscaNomes, sizeof(char[50]), 5, nomes);
	fclose(nomes);
	
	printf("Dados carregados com sucesso!\n");
	arquivosCarregados = TRUE;
}

void hexDump(size_t offset, void *addr, int len){
    int i;
    unsigned char bufferLine[17];
    unsigned char *pc = (unsigned char *)addr;

    for (i = 0; i < len; i++){
        if ((i % 16) == 0){
            if (i != 0)
                printf(" %s\n", bufferLine);
            printf("%08zx: ", offset);
            offset += (i % 16 == 0) ? 16 : i % 16;
        }

        printf("%02x ", pc[i]);

        if ((pc[i] < 0x20) || (pc[i] > 0x7e)){
            bufferLine[i % 16] = '.';
        }else{
            bufferLine[i % 16] = pc[i];
        }

        bufferLine[(i % 16) + 1] = '\0';
    }

	while((i % 16) != 0){
		printf("  ");
		if(i % 2 == 1)
			putchar(' ');
		i++;
	}
    printf("  %s\n", bufferLine);
}

void obtemCache(int* indiceRegistro, int* indiceCodigo, int* indiceNome){
	FILE* cache;
	
	cache = fopen("./temp/cache.bin", "r+b");
	
	if(cache == NULL){
		cache = fopen("./temp/cache.bin", "w+b");
		*indiceRegistro = 0;
		*indiceCodigo = 0;
		*indiceNome = 0;
		fwrite(&indiceRegistro, sizeof(int), 1, cache);
		fwrite(&indiceCodigo, sizeof(int), 1, cache);
		fwrite(&indiceNome, sizeof(int), 1, cache);
		printf("Cache criado!\n");
	}else{
		fread(indiceRegistro, sizeof(int), 1, cache);
		fread(indiceCodigo, sizeof(int), 1, cache);
		fread(indiceNome, sizeof(int), 1, cache);
		printf("Cache obtido! [%d, %d, %d]\n", *indiceRegistro, *indiceCodigo, *indiceNome);
	}
	
	fclose(cache);
}

void atualizaCache(int indiceRegistro, int indiceCodigo, int indiceNome){
	FILE *cache;
	
	cache = fopen("./temp/cache.bin", "w+b");
	
	fwrite(&indiceRegistro, sizeof(int), 1, cache);
	fwrite(&indiceCodigo, sizeof(int), 1, cache);
	fwrite(&indiceNome, sizeof(int), 1, cache);
	
	fclose(cache);
}
