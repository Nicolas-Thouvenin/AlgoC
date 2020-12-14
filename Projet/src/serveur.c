/*
 * SPDX-FileCopyrightText: 2020 John Samuel
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#include "serveur.h"
#include "json.h"
#include "validator.h"

void plot(char *data) {
  char code[20];
  int nbCouleurs;
  char numbers[1000];

  sscanf(data, "%s %d %1000[^\n]", code, &nbCouleurs, numbers);
  char nbCchar[10];
  sprintf(nbCchar, "%d", nbCouleurs);
  char title[50];
  strcpy(title, "set title 'Top ");
  strcat(title, nbCchar);
  strcat(title, " colors'\n");


  //Extraire le compteur et les couleurs RGB 
  FILE *p = popen("gnuplot -persist", "w");
  printf("Plot");
  int count = 0;
  int n;
  char *saveptr = NULL;
  char *str = data;
  fprintf(p, "set xrange [-15:15]\n");
  fprintf(p, "set yrange [-15:15]\n");
  fprintf(p, "set style fill transparent solid 0.9 noborder\n");
  fprintf(p, "%s", title);
  fprintf(p, "plot '-' with circles lc rgbcolor variable\n");
  while(1) {
    char *token = strtok_r(str, ",", &saveptr);
    if (token == NULL) {
      break;
    }
    str=NULL;
    if (count == 0) {
      n = atoi(token);
    }
    else {
      // Le numéro 36, parceque 360° (cercle) / 10 couleurs = 36
      fprintf(p, "0 0 10 %d %d 0x%s\n", (count-1)*(360/nbCouleurs), count*(360/nbCouleurs), token+1);
    }
    count++;
  }
  fprintf(p, "e\n");
  printf("Plot: FIN\n");
  pclose(p);
}

/* renvoyer un message (*data) au client (client_socket_fd)
 */
int renvoie_message(int client_socket_fd, char *data) {
  json_response(data);
  int data_size = write (client_socket_fd, (void *) data, strlen(data));
      
  if (data_size < 0) {
    perror("erreur ecriture");
    return(EXIT_FAILURE);
  }
  return 1;
}

int renvoie_couleurs(int client_socket_fd, char *vals) {  
  FILE * fp;
  char * token = strtok(vals, " #");
  char fichier[20] = "./couleurs.txt";
  fp = fopen (fichier ,"w");
  while(token != NULL){
    fprintf(fp, "#%s\n",token); 
    token = strtok(NULL, " #");
  }
  fclose(fp);

  char data[1024];
  strcpy(data, "Enregistrement dans : ");
  strcat(data, fichier);
  return renvoie_message(client_socket_fd, data);
}
int renvoie_balises(int client_socket_fd, char *vals) {
  FILE * fp;
  char * token = strtok(vals, " #");
  char fichier[20] = "./balises.txt";
  fp = fopen (fichier ,"w");
  while(token != NULL){
    fprintf(fp, "#%s\n",token);
    token = strtok(NULL, " #");
  }
  fclose(fp);
  
  char data[1024];
  strcpy(data, "Enregistrement dans : ");
  strcat(data, fichier);
  return renvoie_message(client_socket_fd, data);
}
int renvoie_calcul(int client_socket_fd, char *vals) {
  char * delim = " ";
  char operateur[20];
  //printf("%s", vals);
  char * token = strtok(vals, delim);
  float numbers[100];
  strcpy(operateur, token);
  //printf("%s", operateur);
  int numberCounter = 0;
  token = strtok(NULL, delim);
  while(token != NULL){
    numbers[numberCounter] = strtof(token, NULL);
    numberCounter++;
    token = strtok(NULL, delim);
  }
  double res = numbers[0];
  int i = 1;
  if (strcmp(operateur, "+") == 0){
    for(i = 1; i < numberCounter; i++){
      res += numbers[i];
    }
  }
  else if (strcmp(operateur, "*") == 0){
    for(i = 1; i < numberCounter; i++){
      res *= numbers[i];
    }
  }
  else if (strcmp(operateur, "-") == 0){
    for(i = 1; i < numberCounter; i++){
      res -= numbers[i];
    }
  }
  else if (strcmp(operateur, "/") == 0){
    for(i = 1; i < numberCounter; i++){
      res /= numbers[i];
    }
  }
  else if (strcmp(operateur, "moyenne") == 0){
    for(i = 1; i < numberCounter; i++){
      res += numbers[i];
    }
    res /= i;
  }
  else if (strcmp(operateur, "minimum") == 0){
    float tmp;
    for (int i=0 ; i < numberCounter-1; i++)
    {
        for (int j=0 ; j < numberCounter-i-1; j++)
        {
            if (numbers[j] > numbers[j+1]) 
            {
                tmp = numbers[j];
                numbers[j] = numbers[j+1];
                numbers[j+1] = tmp;
            }
        }
    }
    res = numbers[0];
  }
  else if (strcmp(operateur, "maximum") == 0){
    float tmp;
    for (int i=0 ; i < numberCounter-1; i++)
    {
        for (int j=0 ; j < numberCounter-i-1; j++)
        {
            if (numbers[j] < numbers[j+1]) 
            {
                tmp = numbers[j];
                numbers[j] = numbers[j+1];
                numbers[j+1] = tmp;
            }
        }
    }
    res = numbers[0];
  }
  else {
    //ecart-type
    double ecart = 0.0;
    int i;
    for(i = 1; i < numberCounter; i++){
        res += numbers[i];
    }
    res /= i;

    for (i = 0; i < numberCounter; ++i){
        ecart += pow(numbers[i] - res, 2);
    }
    res = sqrt(ecart / numberCounter);
  }
  char sRes[10];
  sprintf(sRes, "%lf", res);
  char data[1024];
  strcpy(data, sRes);
  return renvoie_message(client_socket_fd, data);
}

/* accepter la nouvelle connection d'un client et lire les données
 * envoyées par le client. En suite, le serveur envoie un message
 * en retour
 */
int recois_envoie_message(int socketfd) {
  struct sockaddr_in client_addr;
  char data[1024];

  int client_addr_len = sizeof(client_addr);
 
  // nouvelle connection de client
  int client_socket_fd = accept(socketfd, (struct sockaddr *) &client_addr, &client_addr_len);
  if (client_socket_fd < 0 ) {
    perror("accept");
    return(EXIT_FAILURE);
  }

  //fork pour créer plusieurs process

  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(data));

  //lecture de données envoyées par un client
  int data_size = read (client_socket_fd, (void *) data, sizeof(data));
      
  if (data_size < 0) {
    perror("erreur lecture");
    return(EXIT_FAILURE);
  }
  
  /*
   * extraire le code des données envoyées par le client. 
   * Les données envoyées par le client peuvent commencer par le mot "message :" ou un autre mot.
   */
  printf ("Message recu: %s\n", data);
  const char * resValidation = datavalidator(data);
  if(resValidation == NULL){
    printf("The JSON data is OKKKKK\n");
    char code[10];
    char tabValues[1000];
    json_code_getter(data, code, tabValues);

    //printf("%s", code);

    //Si le message commence par le mot: 'message:' 
    if (strcmp(code, "message") == 0) {
      // Demandez à l'utilisateur d'entrer un message
      char message[900];
      printf("Votre message (max 900 caracteres): ");
      fgets(message, 1024, stdin);
      strcpy(data, message);
      renvoie_message(client_socket_fd, data);
    }
    else if (strcmp(code, "nom") == 0) {
      renvoie_message(client_socket_fd, tabValues);
    }
    else if (strcmp(code, "calcul") == 0){ 
      renvoie_calcul(client_socket_fd, tabValues);
    }
    else if(strcmp(code, "couleurst1") == 0){
      renvoie_couleurs(client_socket_fd, tabValues);
    }
    else if(strcmp(code, "balises") == 0){
      renvoie_balises(client_socket_fd, tabValues);
    }
    else{
      plot(data);
    }
  }
  else{
    printf("Désolé mais votre format JSON n'est pas accepté\nERREUR : %s", resValidation);
    char msg[1024];
    strcpy(msg, "Désolé mais votre format JSON n'est pas accepté\nERREUR : ");
    strcat(msg, resValidation);
    renvoie_message(client_socket_fd, msg);
  }
  //fermer le socket 
  close(socketfd);
  return 1;
}
int main() {

  int socketfd;
  int bind_status;
  int client_addr_len;

  struct sockaddr_in server_addr, client_addr;

  /*
   * Creation d'une socket
   */
  socketfd = socket(AF_INET, SOCK_STREAM, 0);
  if ( socketfd < 0 ) {
    perror("Unable to open a socket");
    return -1;
  }

  int option = 1;
  setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

  //détails du serveur (adresse et port)
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  // Relier l'adresse à la socket
  bind_status = bind(socketfd, (struct sockaddr *) &server_addr, sizeof(server_addr));
  if (bind_status < 0 ) {
    perror("bind");
    return(EXIT_FAILURE);
  }
 
  // Écouter les messages envoyés par le client
  listen(socketfd, 10);

  //Lire et répondre au client
  recois_envoie_message(socketfd);
  return 0;
}
