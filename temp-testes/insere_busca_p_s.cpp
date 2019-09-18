#include<stdio.h>
#include<conio.h>

int main() {
    FILE *fd;
    
    //////////////////////////////
    struct livro {
        char codigo[4];
        char nome[50];
        char seguradora[50];
        char tipo_seg[30];
    } vet[8] = {{"001", "Joao", "Maritima", "Automovel"},
                {"002", "Paulo", "Porto Seguro", "Automovel"},
                {"003", "Maria", "Porto Seguro", "Automovel"},
                {"004", "Ana", "Zurich", "Vida"},
                {"005", "Paulo", "Bradesco", "Previdencia"},
                {"006", "Renan", "Zurich", "Residencial"},
                {"007", "Joao", "Sompo", "Automovel"},
                {"008", "Paulo", "Sompo", "Residencial"}};
       
    fd = fopen("insere.bin", "w+b");
    fwrite(vet, sizeof(vet), 1, fd);
    fclose(fd);
    
    //////////////////////////////
	struct busca_p {
        char codigo[4];
    } vet_bp[7] = {{"003"},
                   {"002"},
                   {"007"},
                   {"002"},
                   {"006"},
                   {"002"},
                   {"006"}};
       
    fd = fopen("busca_p.bin", "w+b");
    fwrite(vet_bp, sizeof(vet_bp), 1, fd);
    fclose(fd);
    
    //////////////////////////////
	const int quantidade = 5;
    char nomes[quantidade][50] = {"Ana", "Paulo", "Maria", "Joao", "Paulo"};
       
    fd = fopen("busca_s.bin", "w+b");
    for (int i=0; i<quantidade; i++)
       fwrite(nomes[i], sizeof(char), 50, fd);
    fclose(fd);
    
    /*char buffer[50];
    fd = fopen("busca_s.bin", "r+b");
    for (int i=0; i<quantidade; i++)
       {
         fread(buffer, sizeof(buffer), 1, fd);
         printf("\n%s",buffer);
       }
    fclose(fd);
    getch();*/
}
