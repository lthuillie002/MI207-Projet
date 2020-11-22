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

int main(int argc, char *argv[])
{
  int pid_pipe0 = 0;
  char taille_c [5];
  //TODO : Alocation dynamique pour les path et pid clients
  char pipe0_path [64] = "/tmp/chat/0";
  
  int pid_client [64];

  int nb_client = 0;

  // TODO liste dynamique
  char clients[10][64] = {0};
  system("rm -rf /tmp/chat/");

  // Création du dossier pour les pipes
  // 777 - Tous les droits pour tous
  exit_if(mkdir("/tmp/chat/", 0777) == -1, "Erreur creation du dossier /tmp/chat/");

  // Création du pipe 0 pour envoyer un message
  // (666 - Lecture, écriture pour tout le monde
  exit_if(mkfifo(pipe0_path, 0666) == -1, "mkfifo");

  pid_pipe0 = open(pipe0_path, O_RDONLY);
  
  while(1) {
    exit_if(read(pid_pipe0, taille_c, 5) == -1, "read taille");
    int taille = atoi(taille_c);
    printf("Taille recu : %d\n", taille);
    if (taille == 0) {
      printf("Nouveau client detecte\n");
      char cp [1];
      int i = 0;
      printf("nb client : %d\n", nb_client);
      exit_if(read(pid_pipe0, cp, 1) == -1, "read caractere");
      // printf("caractere : %c En decimal : %d\n", cp[0], cp[0]);
      // TODO : Attention bug strcat il faut initialiser à 0 toute la liste
      strcat(clients[nb_client], cp);
      i ++;
      while (cp[0] != 0) {
        exit_if(read(pid_pipe0, cp, 1) == -1, "read caractere");
        // printf("caractere : %c En decimal : %d\n", cp[0], cp[0]);
        // printf("strlen : %lu \n", strlen(cp));
        strcat(clients[nb_client], cp);
        i++;
      }
      printf("Nouveau client connecte !\nNom du client : %s\n\n", clients[nb_client]);

      //TODO : Alloc dyna pour le path client
      char tmp_path [64] = "/tmp/chat/";
      strcat(tmp_path, clients[nb_client]);
      // printf("path client : %s, strlen : %lu\n", tmp_path, strlen(tmp_path));
      sleep(1);
      pid_client[nb_client] = open(tmp_path, O_WRONLY);
      exit_if(pid_client[nb_client] < 0, "Erreur de connexion < Ctrl + c > pour quitter");

      nb_client++;
    }
    else {
      char buf[taille];
      exit_if(read(pid_pipe0, buf, taille) == -1, "read buf");
      printf("Message recu : %s\n", buf);
      // printf("En decimale : %d\n", buf[0]);
      // printf("En decimale : %d\n", buf[1]);
      int i = 0;
      for(i = 0; i < nb_client; i++) {
        //TODO : vraiment améliorer ça
        char tmp_path [64] = "/tmp/chat/";
        strcat(tmp_path, clients[nb_client]);
        char ptaille[5];

        //TODO j ?
        int j  = snprintf(ptaille, 5, "%d", taille);
        exit_if(write(pid_client[i], ptaille, 5) == -1, "write client");
        exit_if(write(pid_client[i], buf, taille) == -1, "write client");
      }
    }
  }

  close(pid_pipe0);


  return EXIT_SUCCESS;
}
