#include <string.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <ctype.h>


#include "json.h"


char *ltrim(char *s)
{
    while(isspace(*s)) s++;
    return s;
}

char *rtrim(char *s)
{
    char* back = s + strlen(s);
    while(isspace(*--back));
    *(back+1) = '\0';
    return s;
}

char *trim(char *s)
{
    return rtrim(ltrim(s)); 
}

void json_code_getter(char *data, char *code, char *tabVals){
  int wordCounter = 0;
  char * delim = "\"";
  char * token = strtok(data, delim);
  strcpy(code, "");
  strcpy(tabVals, "");
  do {
    if(wordCounter == 3){
      strcat(code, token);
    }
    else if (wordCounter >= 7 && (wordCounter%2 != 0)){
      strcat(tabVals, token);
      strcat(tabVals, " ");
    }
    token = strtok(NULL, delim);
    wordCounter++;
  }while(token != NULL);
  code = trim(code);
  tabVals = trim(tabVals);
}



void json_creator(char *code, char *values, char *data){
    values = trim(values);
  /*
  {  "code" : "calcul",  "valeurs" : [ "+", "23", "45" ]}
  */
  char toReturn[1024];
  strcpy(toReturn, "{ \"code\" : \"");
  strcat(toReturn, code);
  strcat(toReturn,"\", \"valeurs\" : [ ");

  char * delim = "";
  if (strcmp(code, "message") == 0)
    delim = "";
  else if (strcmp(code, "calcul") == 0)
    delim = " ";
  else 
    delim = ", ";
  char * token = strtok(values, delim);
  while(token != NULL) {
    strcat(toReturn, "\"");
    strcat(toReturn, token);
    strcat(toReturn, "\", ");
    token = strtok(NULL, delim);
  }
  toReturn[strlen(toReturn)-2] = '\0';
  strcat(toReturn, " ]}");
  printf("data sent (JSON) : %s\n\n", toReturn);
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