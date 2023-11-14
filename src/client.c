/*
 * SPDX-FileCopyrightText: 2021 John Samuel
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
#include <stdbool.h>

#include "client.h"
#include "bmp.h"



void toJson(char *data){
  char substitute[1024];
  strcpy(substitute, data);
  char *saveptr;
  
  
  char *valeur = strtok_r(substitute, " ", &saveptr);
  valeur[strlen(valeur) - 1] = '\0';
  printf("{\n");
  printf("   \"code\" : \"%s\",\n", valeur);
  printf("   \"valeurs\" : [");
  valeur = strtok_r(NULL, " ", &saveptr);
  int counter = 0;
  while(valeur != NULL){
    if(counter != 0){
      printf("\",");
    }
      

    //valeur[strlen(valeur) - 1] = '"';
    printf(" \"");
    printf("%s", valeur);
    
    
    valeur = strtok_r(NULL, " ", &saveptr);
    counter++;
    
  }
  printf(" ]\n");
  printf("}\n");
  
}

/*
 * Fonction d'envoi et de réception de messages
 * Il faut un argument : l'identifiant de la socket
 */

int envoie_recois_message(int socketfd)
{

  char data[1024];
  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(data));

  // Demandez à l'utilisateur d'entrer un message
  char message[1024];
  printf("Votre message (max 1000 caracteres): ");
  fgets(message, sizeof(message), stdin);
  strcpy(data, "message: ");
  strcat(data, message);
  printf("Données envoyées : \n");
  toJson(data);
  int write_status = write(socketfd, data, strlen(data));
  if (write_status < 0)
  {
    perror("erreur ecriture");
    exit(EXIT_FAILURE);
  }

  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(data));

  // lire les données de la socket
  int read_status = read(socketfd, data, sizeof(data));
  if (read_status < 0)
  {
    perror("erreur lecture");
    return -1;
  }

  printf("Données reçues : \n");
  toJson(data);

  return 0;
}

int envoie_nom_de_client(int socketfd){
  
  char data[1024];
  
  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(data));

  // Demandez à l'utilisateur d'entrer un message
  char hostname[1024];
  gethostname(hostname, 1024);
  strcpy(data, "nom: ");
  strcat(data, hostname);
  printf("Données envoyées : \n");
  toJson(data);

  int write_status = write(socketfd, data, strlen(data));
  if (write_status < 0)
  {
    perror("erreur ecriture");
    exit(EXIT_FAILURE);
  }

  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(data));

  // lire les données de la socket
  int read_status = read(socketfd, data, sizeof(data));
  if (read_status < 0)
  {
    perror("erreur lecture");
    return -1;
  }

  printf("Données reçues : \n");

  toJson(data);

  return 0;
}

int envoie_operateur_numeros(int socketfd, char operator[2], char x[25], char y[25]){
  char data[1024];
  
  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(data));

  // Demandez à l'utilisateur d'entrer un message
  strcpy(data, "calcul: ");
  strcat(data, operator);
  strcat(data, " ");
  strcat(data, x);
  strcat(data, " ");
  strcat(data, y);
  printf("Données envoyées : \n");
  toJson(data);

  int write_status = write(socketfd, data, strlen(data));
  if (write_status < 0)
  {
    perror("erreur ecriture");
    exit(EXIT_FAILURE);
  }



  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(data));

  // lire les données de la socket
  int read_status = read(socketfd, data, sizeof(data));
  if (read_status < 0)
  {
    perror("erreur lecture");
    return -1;
  }

  printf("Données reçues : \n");
  toJson(data);

  return 0;
}

int envoie_balises(int socketfd, char **arguments){
  char data[1024];
  int i;
  //char temp[1024];
  
  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(data));


  printf("Nombre de balises : %d\n", atoi(arguments[2]));
  int limite = atoi(arguments[2]) + 3;
  strcpy(data, "balises:");
  for(i = 2; i < limite; i++){
    strcat(data, " ");
    strcat(data, arguments[i]);
    
  }

  printf("Données envoyées : \n");
  toJson(data);
  int write_status = write(socketfd, data, strlen(data));
  if (write_status < 0)
  {
    perror("erreur ecriture");
    exit(EXIT_FAILURE);
  }

  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(data));

  // lire les données de la socket
  int read_status = read(socketfd, data, sizeof(data));
  if (read_status < 0)
  {
    perror("erreur lecture");
    return -1;
  }

  printf("Données reçues : \n");
  toJson(data);

  return 0;
  
}

void analyse(char *pathname, char *data)
{
  // compte de couleurs
  couleur_compteur *cc = analyse_bmp_image(pathname);
  char temp_string[10];
  if(atoi(data) <= 30){
    strcpy(temp_string, data);
    int nbCouleurs = atoi(temp_string);
    int count;
    strcpy(data, "couleurs: ");
  
    if (cc->size < nbCouleurs)
    {
      sprintf(temp_string, "%d,", cc->size);
    }
    strcat(data, temp_string);
    strcat(data, " ");

    // choisir 10 couleurs
    for (count = 1; count < nbCouleurs + 1 && cc->size - count > 0; count++)
    {
      if (cc->compte_bit == BITS32)
      {
        sprintf(temp_string, "#%02x%02x%02x,", cc->cc.cc24[cc->size - count].c.rouge, cc->cc.cc32[cc->size - count].c.vert, cc->cc.cc32[cc->size - count].c.bleu);
      }
      if (cc->compte_bit == BITS24)
      {
        sprintf(temp_string, "#%02x%02x%02x,", cc->cc.cc32[cc->size - count].c.rouge, cc->cc.cc32[cc->size - count].c.vert, cc->cc.cc32[cc->size - count].c.bleu);
      }
      strcat(data, temp_string);
    }

    // enlever le dernier virgule
    data[strlen(data) - 1] = '\0';
    liberer_memoire(cc);
  } else {
    printf("\nVeuillez saisir un chiffre inférieur à 30.\n");
  }
  
}

int envoie_couleurs(int socketfd, char *pathname, char *nbCouleurs)
{
  
  char data[1024];
  memset(data, 0, sizeof(data));
  strcpy(data, nbCouleurs);
  analyse(pathname, data);
  
  printf("Données envoyées : \n");
  toJson(data);

  int write_status = write(socketfd, data, strlen(data));
  if (write_status < 0)
  {
    perror("erreur ecriture");
    exit(EXIT_FAILURE);
  }

  return 0;
}

int main(int argc, char **argv)
{
  int socketfd;

  struct sockaddr_in server_addr;

  if (argc < 2)
  {
    printf("usage: ./client chemin_bmp_image\n");
    return (EXIT_FAILURE);
  }

  /*
   * Creation d'une socket
   */
  socketfd = socket(AF_INET, SOCK_STREAM, 0);
  if (socketfd < 0)
  {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  // détails du serveur (adresse et port)
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  // demande de connection au serveur
  int connect_status = connect(socketfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
  if (connect_status < 0)
  {
    perror("connection serveur");
    exit(EXIT_FAILURE);
  }
  if (strcmp(argv[1], "message:") == 0)
  {
    // envoyer et recevoir un message
    envoie_recois_message(socketfd);
  }
  else if(strcmp(argv[1], "nom:") == 0){
    envoie_nom_de_client(socketfd);
  }
  else if(strcmp(argv[1], "calcul:") == 0){
    envoie_operateur_numeros(socketfd, argv[2], argv[3], argv[4]);
  }
  else if(strcmp(argv[1], "balises:") == 0){
    envoie_balises(socketfd, argv);
  }
  else
  {
    // envoyer et recevoir les couleurs prédominantes
    // d'une image au format BMP (argv[1])
    envoie_couleurs(socketfd, argv[1], argv[2]);
  }

  close(socketfd);
}
