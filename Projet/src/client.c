/*
 * SPDX-FileCopyrightText: 2020 John Samuel
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>  
#include <sys/socket.h>
#include <netinet/in.h>

#include "client.h"
#include "bmp.h"

/* 
 * Fonction d'envoi et de réception de messages
 * Il faut un argument : l'identifiant de la socket
 */

int envoyeur_recepteur_avec_msg(int socketfd, char msg[100], char code[10]){
  char data[1024];
  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(data));

  char message[100];
  printf("%s", msg);
  fgets(message, 1024, stdin);
  strcpy(data, code);
  strcat(data, message);
  printf("Data to send : %s", data);
  
  int write_status = write(socketfd, data, strlen(data));
  if ( write_status < 0 ) {
    perror("erreur ecriture");
    exit(EXIT_FAILURE);
  }
  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(data));


  // lire les données de la socket
  int read_status = read(socketfd, data, sizeof(data));
  if ( read_status < 0 ) {
    perror("erreur lecture");
    return -1;
  }

  printf("Message recu: %s\n", data);
  return 0;
}

int envoie_recois_message(int socketfd) {
  return envoyeur_recepteur_avec_msg(socketfd, "Votre message (max 1000 caracteres): ", "message: ");
}

int envoie_operateur_numeros(int socketfd) {
  return envoyeur_recepteur_avec_msg(socketfd, "Votre calcul (operateur nombre1 nombre 2) : ", "calcul: ");
}

int envoie_couleurs_tache1(int socketfd) {
  return envoyeur_recepteur_avec_msg(socketfd, "Vos couleurs (nbCouleur, #couleur1, #couleur2 ...) : ", "couleurst1: ");
}
int envoie_balises(int socketfd) {
  return envoyeur_recepteur_avec_msg(socketfd, "Vos balises (nbBalise, #Balise1, #Balise2 ...) : ", "balises: ");
}

int envoie_nom_de_client(int socketfd){
  return envoyeur_recepteur_avec_msg(socketfd, "Veuillez entrer le nom de votre machine : ", "nom: ");
}

void analyse(char *pathname, char *data, char *nbCouleurs) {
  //compte de couleurs
  couleur_compteur *cc = analyse_bmp_image(pathname);

  int count;
  strcpy(data, "couleurs: ");
  int x = atoi(nbCouleurs);
  char temp_string[10]; 
  strcat(temp_string,nbCouleurs);
  strcat(temp_string,",");
  if (cc->size < x) {
    sprintf(temp_string, "%d,", cc->size);
  }
  strcat(data, temp_string);
  
  //choisir 10 couleurs
  for (count = 1; count < (x+1) && cc->size - count >0; count++) {
    if(cc->compte_bit ==  BITS32) {
      sprintf(temp_string, "#%02x%02x%02x,", cc->cc.cc24[cc->size-count].c.rouge,cc->cc.cc32[cc->size-count].c.vert,cc->cc.cc32[cc->size-count].c.bleu);
    }
    if(cc->compte_bit ==  BITS24) {
      sprintf(temp_string, "#%02x%02x%02x,", cc->cc.cc32[cc->size-count].c.rouge,cc->cc.cc32[cc->size-count].c.vert,cc->cc.cc32[cc->size-count].c.bleu);
    }
    strcat(data, temp_string);
  }

  //enlever le dernier virgule
  data[strlen(data)-1] = '\0';
}

int envoie_couleurs(int socketfd, char *pathname, char *nbCouleurs) {
  char data[1024];
  memset(data, 0, sizeof(data));
  analyse(pathname, data, nbCouleurs);
  
  int write_status = write(socketfd, data, strlen(data));
  if ( write_status < 0 ) {
    perror("erreur ecriture");
    exit(EXIT_FAILURE);
  }

  return 0;
}


int main(int argc, char **argv) {
  int socketfd;
  int bind_status;

  struct sockaddr_in server_addr, client_addr;

  /*
   * Creation d'une socket
   */
  socketfd = socket(AF_INET, SOCK_STREAM, 0);
  if ( socketfd < 0 ) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  //détails du serveur (adresse et port)
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  //demande de connection au serveur
  int connect_status = connect(socketfd, (struct sockaddr *) &server_addr, sizeof(server_addr));
  if ( connect_status < 0 ) {
    perror("connection serveur");
    exit(EXIT_FAILURE);
  }
  if(argc > 1)
    envoie_couleurs(socketfd, argv[1], argv[2]);
  else
  {
    char toExe[10];
    printf("Choisissez votre action : \n");
    printf("a : message\n");
    printf("b : nom client\n");
    printf("c : calcul\n");
    printf("d : couleurs\n");
    printf("e : balises\n");
    fgets(toExe, 10, stdin);
    printf("%s", toExe);
    if(strncmp(toExe, "a", 1) == 0)
      envoie_recois_message(socketfd);
    else if(strncmp(toExe, "b", 1) == 0)
      envoie_nom_de_client(socketfd);
    else if(strncmp(toExe, "c", 1) == 0)
      envoie_operateur_numeros(socketfd);
    else if(strncmp(toExe, "d", 1) == 0)
      envoie_couleurs_tache1(socketfd);
    else if(strncmp(toExe, "e", 1) == 0)
      envoie_balises(socketfd);
  }

close(socketfd);
}
