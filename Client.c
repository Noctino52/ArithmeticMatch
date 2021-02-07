
#include "Game_Aritmetica.h"        

int main(int argc,char* argv[]){
    char action = MENU;
    IPANDRESS=malloc(sizeof(char)*strlen(argv[1])+1);
    IPANDRESS=strcpy(IPANDRESS,argv[1]);

    setComunication(action);
    free(IPANDRESS);
    return 0;
}
