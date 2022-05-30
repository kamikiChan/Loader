#include "1-token.c"
#include <stdlib.h>

typedef struct Symbol
{
    char name[20];
    int address;
    struct Symbol *next;
}SYMBOL;

typedef struct
{
    char    name[20];
    int     address;
    int     len;
    struct Symbol *symbolHead;
}CS;

CS controlSection[100];

struct Symbol *newNobe(char name[], char hex[]){
    struct Symbol *NewNobe = (struct Symbol*) malloc(sizeof(struct Symbol));
    strcpy(NewNobe->name, name);
    sscanf(hex, "%x", &NewNobe->address);
    NewNobe->next = NULL;
    return NewNobe;
}

void Empty(int i){
    controlSection[i].name[0] = '\0';
    controlSection[i].address = 0;
    controlSection[i].len = 0;
    controlSection[i].symbolHead = NULL;
}

void showLoadmap(int len){
    struct Symbol *symbol = NULL;
    int i;
    printf("CS\tSYMBOL\tADDRESS\tLENGTH\n");
    printf("********************************************************\n");
    for(i = 0; i < len; i++){
        symbol = controlSection[i].symbolHead;
        printf("%s\t\t%X\t%04X\n", controlSection[i].name, controlSection[i].address, controlSection[i].len);
        while (symbol != NULL)
        {
            printf("\t%s\t%X\n", symbol->name, symbol->address);
            symbol = symbol->next;
        }
    }
}

int builLoadmap(CS *CSection, int i, int startA, char *buf){
    struct Symbol *head = NULL;
    struct Symbol *current = NULL;
    struct Symbol *rear = NULL;
    int c ,j ,a;
    char *p, hex[6],name[6];
    p = buf;
    if(*p == 'H'){
        p++;
        for(c = 0; *p != ' '; p++, c++){
            if(*p == '\0')break;
            CSection[i].name[c] = *p;
        }
        CSection[i].name[c] = '\0';
        a = ASM_token(p);
        if(startA == 0){
            for(j = 0; j < 6; j++){
                hex[j] = *p;
                p++;
            }
            sscanf(hex, "%x", &CSection[i].address);
        } else {
            CSection[i].address = startA;
            p += 6;
        }
        sscanf(p, "%x", &CSection[i].len);
    }
    else if(*p == 'D'){
        p++;
        while (*p != '\0'){
            for(c = 0; *p != ' '; p++, c++){
            if(*p == '\0')break;
                name[c] = *p;
            }
            name[c] = '\0';
            a = ASM_token(p);
            for(j = 0; j < 6; j++){
                hex[j] = *p;
                p++;
            }
            current = newNobe(name, hex);
            current->address = current->address + CSection[i].address;
            if (head == NULL){
                head = current;
            } else {
                rear->next = current;
            }
            rear = current;
        }
    }
    if(head != NULL)
        CSection[i].symbolHead = head;
    return CSection[i].address + CSection[i].len;
}

int main(int argc, char *argv[])
{
	int			i, c, startAddress = 0, fileCount = 0;
	char		buf[LEN_SYMBOL];

	if(argc < 4)
	{
		printf("Usage: %s load <address> <file 1> <file 2> …\n", argv[0]);
	}
	else
	{
        sscanf(argv[2], "%x", &startAddress);
        while (argv[fileCount+3] != NULL)
        {
            if(ASM_open(argv[fileCount+3]) == NULL){
			    printf("%s not found!!\n", argv[fileCount+3]);
                Empty(fileCount);
                fileCount++;
            }
            else
            {
                while((c = ASM_token(buf)) != EOF)
                    startAddress = builLoadmap(controlSection, fileCount, startAddress, buf);
                fileCount++;
                ASM_close();
            }
        }
        showLoadmap(fileCount);
	}
}