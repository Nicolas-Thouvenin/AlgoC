#include "usefulFcts.h"
#include "validator.h"


const char * datavalidator(const char *data){
    ////////if(data.isempty) return 0

    FILE * fp;
    char fichier[20] = "./logValidator.txt";
    fp = fopen (fichier ,"w");

    char *possibleCode[6] = 
        {"\"message \"", "\"calcul \"", "\"couleurst1 \"", "\"balises \"", "\"nom \"", "\"perso\""};

    char *ptr = (char *)malloc(strlen(data)+1);
    strcpy(ptr,data);

    //char * ptr = data;

    fprintf(fp, "ptr: %s\n", ptr);
    ptr = strstr(ptr, "{");
    if (ptr == NULL) {
        fclose(fp);
        return "Pas de { ouvrante en premier char\n";
    }

    //rajouter le trim

    fprintf(fp, "ptr: %s\n", ptr);
    ptr = strstr(ptr, "\"code\"");
    if (ptr == NULL) {
        fclose(fp);
        return "Pas de \"code\"\n";
    }

    fprintf(fp, "ptr: %s\n", ptr);
    ptr = strchr(ptr, ':');
    if (ptr == NULL) {
        fclose(fp);
        return "Pas de : \n";
    }

    fprintf(fp, "ptr: %s\n", ptr);
    int returnCode = 0;
    for(int i = 0; i < 5; i++){
        if (strstr(ptr, possibleCode[i]) != NULL) {
            returnCode = 1;
            ptr = strstr(ptr, possibleCode[i]);
        }
    }
    if(returnCode == 0){
        fclose(fp);
        return "Aucun code ne correspond à la valeur saisit\n";
    }

    fprintf(fp, "ptr: %s\n", ptr);
    ptr = strstr(ptr, "\"valeurs\"");
    if (ptr == NULL) {
        fclose(fp);
        return "Pas de \"valeurs\"\n";
    }

    fprintf(fp, "ptr: %s\n", ptr);
    ptr = strchr(ptr, ':');
    if (ptr == NULL) {
        fclose(fp);
        return "Pas de : \n";
    }
    
    fprintf(fp, "ptr: %s\n", ptr);
    ptr = strchr(ptr, '[');
    if (ptr == NULL) {
        fclose(fp);
        return "Pas de [ \n";
    }
    else{
        ptr++;
        //permet de skip les deux premier char de la string qui sont [[:space:]
        if(ptr[0] == ' ')
        {ptr++;}
    }

    fprintf(fp, "ptr: %s\n", ptr);
    if (strstr(ptr, "]}") == NULL) {
        fclose(fp);
        return "Pas de ]}\n";
    }
    
    ptr[strlen(ptr)-3] = '\0';
    fprintf(fp, "ptr: %s\n", ptr);
    //check if there is value inside the json array [ ]
    if(ptr[0] == '\0'){
        fclose(fp);
        return "Pas de valeurs dans l'array [ ]\n";
    }

    char * values = ptr;
    fprintf(fp, "values: %s\n", values);
    
    //Si y'a qu'une virgule dans le tableau
    char * token = strtok(values, ",");
    fprintf(fp, "token: %s\n", token);
    token = trim(token);
    if(token == NULL){
        fclose(fp);
        return "Il n'y a qu'une virgule dans l'array, merci de mettre des valeurs\n";
    }
    do {
        token = trim(token);
        fprintf(fp, "token: %s\n", token);
        //Si c'est une string sans guillemet
        if(token[0] != '\"'){
            if(isNumber(token) == 0)
            {
                fclose(fp);
                return "Des guillemets doivent entourer une string\n";
            }
        }
        else{
            if(strrchr(token, '\"') == NULL)
            {
                fclose(fp);
                return "Une guillement doit fermer la string\n";
            }
        }
        token = strtok(NULL, ",");
    }while(token != NULL);

    fclose(fp);
    return NULL;
}