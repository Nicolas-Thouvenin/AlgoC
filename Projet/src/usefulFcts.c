#include "usefulFcts.h"


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


int isNumber(char *strToCtrl){
    regex_t regex;
    int checkerNum;
    checkerNum = regcomp(&regex, "^(-)?([0-9]+)((,|.)([0-9]+))?$", REG_EXTENDED);
    checkerNum = regexec(&regex, strToCtrl, 0, NULL, 0);
    regfree(&regex);
    if(checkerNum == REG_NOMATCH)
    {return 0;}
    return 1;
}