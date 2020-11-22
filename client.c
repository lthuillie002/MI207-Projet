#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "utils.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <math.h>

#define MIN_BUFFER 8
#define MAX_PID_CARAC 4

/*
Fonction récupérée sur 
https://nh2blog.wordpress.com/2012/12/11/saisie-dune-chaine-de-caracteres-en-c/
Paragraphe Solution " fait-main"
Elle permet de récupérer une entrée du terminal de taille inconnue
en doublant s'il le faut la taille du buffer. */
char * lire_chaine(char * message){

  printf("%s", message);
  size_t taillbuff = MIN_BUFFER;
  char *buffer = malloc(taillbuff);
  exit_if(buffer == NULL, "malloc"); // échec de malloc()
  char *p;

  for(p=buffer ; (*p=getchar()) != '\n' && *p!=EOF ; ++p) {
    if (p - buffer == taillbuff - 1) {   // buffer plein,
      p = realloc(buffer, taillbuff *= 2); //on le double
      if (p == NULL) {     // échec de realloc()
        free(buffer);
        return NULL;
      } else buffer = p; //bloc réalloué != buffer
      p += taillbuff/2 - 1; // p reprend sa place dans
    }                       // la nouvelle zone
  }
  *p = 0;
  p = realloc(buffer, p - buffer + 1); //réajustement
  if (p == NULL) {          // échec de realloc
    free(buffer);
    return NULL;
  } else return p;
}

int main(int argc, char *argv[])
{
  // --------------------------------------
  // Initialisation des variables générales
  // --------------------------------------
  char *pseudo = NULL; //chaîne de caractère stockant le pseudo de l'utilisateur
  int pid_pipe0;
  
  // ------------------------
  // Creation du pipe interne
  // ------------------------
  // Buffer de lecture
  // Allocation mémoire (1 seul caractère est utiliser pour la communicaiton pere/fil)
  char *intern_buf = malloc(1);
  // Descripteur du tube
  // desc[0] pour la lecture
  // desc[1] pour l'écriture
  int intern_desc[2];

  // Creation du pipe
  exit_if(pipe(intern_desc) == -1, "Erreur lors de la creation du pipe interne");

  // Lecture non-bloquante
  exit_if(fcntl(intern_desc[0], F_SETFL, fcntl(intern_desc[0], F_GETFL) | O_NONBLOCK) == -1, "Error fcntl");


  // --------------------
  // Connexion au serveur
  // --------------------
  printf("Connexion au chat...\n");

  // Ouverture du pipe serveur
  /////////////// TODO : Tester si le tube est bien présent //////////////
  char carac;
  if((pid_pipe0 = open("/tmp/chat/0", O_WRONLY | O_APPEND | O_NONBLOCK)) == -1) {
    printf("Le serveur n'est pas disponible, Voulez vous le lancer ? (y/n)\n");
    printf("Cette fonction n'est pas encore programée.\n");
    exit_if(1 == 1, "Fin de l'exécution ");
  }


  //exit_if(pid_pipe0 < 0, "Erreur de connexion, veuilliez vérifier le serveur < Ctrl + c > pour quitter");
  
  printf("Connexion établie !\n");

  // Demande du pseudo stocké dans la variable pseudo
  // TODO desc identifié à rien
  pseudo = lire_chaine("Veulliez choisir un identifiant : ");
  printf("Bonjour %s, Bienvenue!\n", pseudo);


  // Envoie du message spécial HELLO
  char *c;
  c = "0";
  char *identifiant_client;
  // On attribue une mémoire de MAX_PID_CARAC + 1 = 5 (+1 pour le \0) caractères car un PID n'excède pas 9999.
  identifiant_client = malloc(sizeof(char)*(MAX_PID_CARAC + 1));
  exit_if(identifiant_client == NULL, "maloc identifiant client");
  // Transformation de l'entier identifiant_client en chaîne de caractères pour l'envoie
  snprintf(identifiant_client, MAX_PID_CARAC + 1, "%d", getpid());
  
  // Envoie du protocole
  // TODO : Gestion de la taille du message
  // exit_if(write(pid_pipe0, identifiant_client, MAX_PID_CARAC) == -1, "write");
  exit_if(write(pid_pipe0, c, 5) == -1, "write");
  exit_if(write(pid_pipe0, pseudo, strlen(pseudo)+1) == -1, "write");


  // Création du pipe client pour envoyer un message
  // (666 - Lecture, écriture pour tout le monde
  int pid_client = 0;
  int taille_path = 10 + (int)strlen(pseudo) + 1;
  char *pipeclient_path;
  pipeclient_path = malloc(taille_path);
  exit_if(pipeclient_path == NULL, "malloc pipelcient_path");
  strcat(pipeclient_path, "/tmp/chat/");
  strcat(pipeclient_path, pseudo);
  exit_if(mkfifo(pipeclient_path, 0777) == -1, "Erreur de creation du pipe client ");
  pid_client = open(pipeclient_path, O_RDONLY);
  exit_if(pid_client < 0, "Erreur d'ouverture du pipe client.");

  printf("Connexion au pipe client établie\n");

  // Séparation
  // 1 processus pour la lecture des messages reçus
  // 1 processus pour la récupération du message à envoyer
  int pid_ = fork();

  if (pid_ == -1) {
		// Il y a une erreur
		perror("fork");
		return EXIT_FAILURE;
  }
  // Processus fil
  // Gestion de la saisie pour envoyer un message
  else if (pid_ == 0) {
    // printf("DEBUG0:");
    while (1) {

      // TODO affichage du message
      printf("\n[ALL] Message : ");
      size_t taillbuff = 0;
      char *buffer = malloc(1);
      exit_if(buffer == NULL, "malloc buffer"); // échec de malloc()

      char p;
      char flag = 0;

      char flag_inter;
      while(flag == 0) {

        if (intern_buf[0] == 'e') printf("\n%s", buffer);
        p = getchar();
        if (p == '\n') flag = 1;
        taillbuff ++;
        exit_if(realloc(buffer, taillbuff) == NULL, "realloc");
        buffer[taillbuff - 1] = p;
      }
      int i = 0;
      for(i = 0; i < taillbuff + 2; i++) {
        printf("carac [%d] : %c en dec : %d\n", i, buffer[i], buffer[i]);
      }
      printf("buffer : %s", buffer);

      // END WORKIN
      //////////////////////
      
      exit_if((int)strlen(buffer) > (pow(2, (8*sizeof(int))-1) -1),
        "Taille maximum de buffer atteind pour <taille> ");
      int taille = (int)strlen(buffer);
      char ptaille[5];

      //TODO j ?
      int j  = snprintf(ptaille, 5, "%d", taille);

      exit_if(write(pid_pipe0, ptaille, 5) == -1, "write");
      exit_if(write(pid_pipe0, buffer, taille) == -1, "write");
    }
  }
  // Processus pere
  // Gestion de la lecture du pipe client
  else {
    while (1) {
      char taille_c [5];
      exit_if(read(pid_client, taille_c, 5) == -1, "read taille");


      
      int taille = atoi(taille_c);
      printf("\nTaille recu : %d\n", taille);
      char *buf = malloc(taille);
      exit_if(buf == NULL, "malloc buf");
      exit_if(read(pid_client, buf, taille) == -1, "read buf");
      printf("Message recu : %s\n", buf);
    }
  }

  return EXIT_SUCCESS;
}
