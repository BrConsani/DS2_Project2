#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>

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

typedef struct cab {
	int offset;
}Cabecalho;

Cabecalho cabecalho;
Registro registros[8];
char buscaCodigos[5];
char buscaNomes[5];

void insereRegistro(struct reg registro);
void pesquisaCodigo(char cod[3]);
void pesquisaNome(char nome[50]);
void carregaArquivos();
void hexDump(size_t, void *, int);
void atualizaCache(int indiceInsere, int indiceRemove);

int main(void)
{
	int escolha=0;
	int indiceRegistro = 0;
	int indiceCodigo = 0;
	char arqv[100];
	
	/*FILE *cache;
	
	cache = fopen("cache.bin", "r+b");
	
	if(cache== NULL){
		cache = fopen("cache.bin", "w+b");
		fprintf(cache, "%d", 0);
		fprintf(cache, "%d", 0);
		fclose(cache);
		printf("Cache criado! Se voce interromper o programa a qualquer momento, suas insercoes estarao salvas.\n");
	}else{
		cache = fopen("cache.bin", "r+b");
		fscanf (cache, "%1d", &indiceRegistro);
		fseek(cache, 1, SEEK_SET );
		fscanf (cache, "%1d", &indiceCodigo);
		fclose(cache);
		printf("Cache carregado! Se voce interromper o programa a qualquer momento, suas insercoes estarao salvas.\n");
	}*/
	
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
				atualizaCache(indiceRegistro,indiceCodigo);
				break;
			case 2:
				//TODO pesquisa por codigo
				break;
			case 3:
				//TODO pesquisa por nome
				break;
			case 4:
				{
				printf("Digite o nome do arquivo a 'receber' o dump (default data.bin): ");
				scanf("%s", &arqv);
				printf("\n");
				FILE *myfile = fopen(arqv, "rb");
				unsigned char buffer[SIZE];
			    size_t n;
			    size_t offset = 0;
			    while ((n = fread(buffer, 1, SIZE, myfile)) > 0)
			    {
			        hexDump(offset, buffer, n);
			        if (n < SIZE)
			            break;
			        offset += n;
			    }				

    			fclose(myfile);
   				break;
   				}
			case 5:
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
	
	data = fopen("data.bin", "r+b");
	
	if(data == NULL){
		printf("Arquivo criado!\n");
		data = fopen("data.bin", "w+b");
		cabecalho.offset = -1;
		fwrite(&cabecalho.offset, sizeof(int), 1, data);
		fwrite(&tamanhoRegistro, sizeof(int), 1, data);
		fwrite(&buffer, sizeof(char), tamanhoRegistro, data);
		fclose(data);
	}else{
		fread(&cabecalho.offset, sizeof(Cabecalho), 1, data);
		if(cabecalho.offset == -1){
			fseek(data, 0, FINAL);
			fwrite(&tamanhoRegistro, sizeof(int), 1, data);
			fwrite(&buffer, sizeof(char), tamanhoRegistro, data);
		}else{
			fseek(data, cabecalho.offset, INICIO);
			bool flag;
			int ultimoMarcador = 0;
			do{
				flag = false;
				int capacidadeRegistro;
				fread(&capacidadeRegistro, sizeof(int), 1, data);
				if(capacidadeRegistro > strlen(buffer)){
					fseek(data, 1, ATUAL);
					int proximoDisponivel;
					fread(&proximoDisponivel, sizeof(int), 1, data);
					fseek(data, -(sizeof(int))-1, ATUAL);
					fwrite(&buffer, sizeof(char), tamanhoRegistro, data);
                    if(capacidadeRegistro - strlen(buffer) != 0)
					    fwrite("*", sizeof(char), 1, data);
					if(ultimoMarcador == 0){
						rewind(data);
					}else{
						fseek(data, ultimoMarcador, INICIO);
					}
					fwrite(&proximoDisponivel, sizeof(int), 1, data);
					flag = true;
				}else{
					fseek(data, 1, ATUAL);
					ultimoMarcador = ftell(data);
					int offset;
					fread(&offset, sizeof(int), 1, data);
					if(offset == -1){
						fseek(data, 0, FINAL);
						fwrite(&tamanhoRegistro, sizeof(int), 1, data);
						fwrite(&buffer, sizeof(char), tamanhoRegistro, data);
						flag = true;
					}else{
						fseek(data, offset, INICIO);
					}					
				}
			}while(!flag);
	
		}
		fclose(data);
	}

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

void atualizaCache(int indiceInsere, int indiceRemove){			
	FILE *cache;
	
	cache = fopen("cache.bin", "w+b");
	fseek(cache, 0, INICIO);
	
	fprintf(cache, "%d", indiceInsere);
	fprintf(cache, "%d", indiceRemove);
	
	fclose(cache);
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

        printf("%02x", pc[i]);
        if ((i % 2) == 1)
            printf(" ");

        if ((pc[i] < 0x20) || (pc[i] > 0x7e)){
            bufferLine[i % 16] = '.';
        }else{
            bufferLine[i % 16] = pc[i];
        }

        bufferLine[(i % 16) + 1] = '\0';
    }

    while ((i % 16) != 0){
        printf("  ");
        if (i % 2 == 1)
            putchar(' ');
        i++;
    }
    
    printf(" %s\n", bufferLine);
}
