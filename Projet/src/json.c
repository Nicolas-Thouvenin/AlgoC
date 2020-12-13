
#include "json.h"
#include "usefulFcts.h"



void json_code_getter(char *data, char *code, char *tabVals){
  int wordCounter = 0;
  char * delim = "\"";
  char * token = strtok(data, delim);
  strcpy(code, "");
  strcpy(tabVals, "");
  do {
    //printf("%d : %s\n", wordCounter, token);
    if(wordCounter == 3){
      strcat(code, token);
      code = trim(code);
    }
    else if(wordCounter >= 7 && strcmp(code, "calcul") == 0)
    {
      if(wordCounter == 7){
        strcat(tabVals, token);
        strcat(tabVals, " ");
      }
      delim = " ";
      if(wordCounter >= 9 && token[0] != ']'){
        if(token[strlen(token)-1] == ',')
        {token[strlen(token)-1] = '\0';}
        strcat(tabVals, token);
        strcat(tabVals, " ");
      }
    }
    else if(wordCounter >= 7 && (wordCounter%2 != 0)){
      strcat(tabVals, token);
      strcat(tabVals, " ");
    }
    
    token = strtok(NULL, delim);
    wordCounter++;
  }while(token != NULL);
  tabVals = trim(tabVals);
}



void json_creator(char *code, char *values, char *data){  
    values = trim(values);
      regex_t regex;
  /*
  {  "code" : "calcul",  "valeurs" : [ "+", "23", "45" ]}
  */
  char toReturn[1024];
  strcpy(toReturn, "{ \"code\" : \"");
  strcat(toReturn, code);
  strcat(toReturn,"\", \"valeurs\" : [ ");

  if (strcmp(code, "message ") != 0 && strcmp(code, "nom ") != 0){
    char * delim = "";
    if (strcmp(code, "calcul ") == 0)
      delim = " ";
    else 
      delim = ", ";
    char * token = strtok(values, delim);
    while(token != NULL) {
      if(isNumber(token) == 0)
      {strcat(toReturn, "\"");}
      strcat(toReturn, token);
      if(isNumber(token) == 0)
      {strcat(toReturn, "\"");}
      strcat(toReturn, ", ");
      token = strtok(NULL, delim);
    }
    toReturn[strlen(toReturn)-2] = '\0';
  }
  else{
    strcat(toReturn, "\"");
    strcat(toReturn, values);
    strcat(toReturn, "\"");
  }
  strcat(toReturn, " ]}");
  printf("Data sent (JSON) : %s\n\n", toReturn);
  strcpy(data, toReturn);
}

void json_response(char *data){
    data = trim(data);
  /*
  { "return" : "message" }
  */
  char toReturn[1024];
  strcpy(toReturn, "{ \"return\" : \"");
  strcat(toReturn, data);
  strcat(toReturn,"\" }");
  strcpy(data, toReturn);
}