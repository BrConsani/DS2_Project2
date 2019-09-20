#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

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
char buscaCodigos[5];
char buscaNomes[5];

void insereRegistro(Registro registro);
void pesquisaCodigo(char cod[3]);
void pesquisaNome(char nome[50]);
void hexDump(size_t, void *, int);
void carregaArquivos();
void obtemCache(int* indiceRegistro, int* indiceCodigo, int* indiceNome);
void atualizaCache(int indiceRegistro, int indiceCodigo, int indiceNome);

int main(void)
{
	int escolha=0;
	int indiceRegistro;
	int indiceCodigo;
	int indiceNome;
	
	DIR* temp = opendir("temp");
	
	if(temp)
		closedir(temp);
	else
		mkdir("temp");
	
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
				atualizaCache(indiceRegistro, indiceCodigo, indiceNome);
				break;
			case 2:
				//TODO pesquisa por codigo
				break;
			case 3:
				//TODO pesquisa por nome
				break;
			case 4: { 
				FILE *myfile;
				int c;
				
				printf("Escolha o arquivo para fazer o dump:\n\n");
				printf(" 1. data.bin\n");
				printf(" 2. cache.bin\n");
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
	char buffer[sizeof(Registro)];
	
	sprintf(buffer, "%s#%s#%s#%s", registro.cod, registro.nome, registro.seg, registro.tipo);
	int tamanhoRegistro = strlen(buffer);
	
	FILE *data;
	
	data = fopen("./temp/data.bin", "r+b");
	
	if(data == NULL){
		printf("Arquivo criado!\n");
		data = fopen("./temp/data.bin", "w+b");
	}else
		fseek(data, 0, FINAL);	
	
	fwrite(&tamanhoRegistro, sizeof(int), 1, data);
	fwrite(&buffer, sizeof(char), tamanhoRegistro, data);
	
	fclose(data);
	
	printf("Registro [%s, %s, %s, %s] inserido com sucesso!\n", registro.cod, registro.nome, registro.seg, registro.tipo);
}

void pesquisaCodigo(char cod[3]){
}

void pesquisaNome(char nome[50]){
}

void carregaArquivos(){
	FILE *insere;
	
	insere = fopen("./temp-testes/insere.bin", "r+b");
	fread(&registros, sizeof(struct reg), 8, insere);
	fclose(insere);
		
	FILE *codigos;
	
	codigos = fopen("./temp-testes/busca_p.bin", "r+b");
	fread(&buscaCodigos, sizeof(char[5]), 1, codigos);
	fclose(codigos);
	
	FILE *nomes;
	
	nomes = fopen("./temp-testes/busca_s.bin", "r+b");
	fread(&buscaNomes, sizeof(char[7]), 1, nomes);
	fclose(nomes);
	
	printf("Dados carregados com sucesso!\n");
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
    
    printf("  %s\n", bufferLine);
}

void obtemCache(int* indiceRegistro, int* indiceCodigo, int* indiceNome){
	FILE* cache;
	
	cache = fopen("./temp/cache.bin", "r+b");
	
	if(cache == NULL){
		cache = fopen("./temp/cache.bin", "w+b");
		indiceRegistro = 0;
		indiceCodigo = 0;
		indiceNome = 0;
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
