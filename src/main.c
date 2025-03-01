/****************************************************/
/* Application : clienTCP                            */
/* Date : 13/01/2023                                */
/* Version : 2                                      */
/* Compilation : gcc clientUdp.c -o clientUdpc          */
/* Usage : ./clientUdpc [adrIPserv] [portServ]   [adrIPcli] */
/****************************************************/
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#include "comm.h"

#define CHECKERROR(var, val, msg) \
  if (var == val) {               \
    perror(msg);                  \
    exit(1);                      \
  }

int main(int argc, char *argv[]) {
  int sd1 = 0;
  struct sockaddr_in addr_serv;

  uint8_t reponse[MAXOCTETS];
  ssize_t nbbytes = 0;
  unsigned int adr_len = sizeof(struct sockaddr_in);

  uint8_t requete[MAXOCTETS];

  xway_paquet_t paquet;
  xway_address_t local = {EMETTEUR_STATION_ID, EMETTEUR_RESEAU_ID,
                          EMETTEUR_PORT_ID};
  xway_address_t automate = {DESTINATAIRE_STATION_ID, DESTINATAIRE_RESEAU_ID,
                             DESTINATAIRE_PORT_ID};

  init_package(&paquet, local, automate);
  build_request(paquet, requete);
  printf("Requete : \n");
  print_data_hex(requete);
  return 0;
  sd1 = socket(AF_INET, SOCK_STREAM, 0);

  CHECKERROR(sd1, -1, "Creation fail !!!\n");

  addr_serv.sin_family = AF_INET;

  if (argc >= 3) {
    addr_serv.sin_port = htons(atoi(argv[2]));
    addr_serv.sin_addr.s_addr = inet_addr(argv[1]);
  } else {
    addr_serv.sin_port = htons(REMOTEPORT);
    addr_serv.sin_addr.s_addr = inet_addr(REMOTEIP);
  }

  CHECKERROR(connect(sd1, (const struct sockaddr *)&addr_serv,
                     sizeof(struct sockaddr_in)),
             -1, "Connexion fail !!!\n");

  nbbytes = send(sd1, requete, requete[5] + 6, 0);
  if (nbbytes < 0) {
    perror("Erreur lors de l'envoi de la requete");
    exit(1);
  }

  nbbytes = recvfrom(sd1, reponse, MAXOCTETS, 0, (struct sockaddr *)&addr_serv,
                     &adr_len);
  if (nbbytes > 0) {
    printf("Reponse : \n");
    print_data_hex(reponse);
  }

  paquet.addresses.reciever = local;
  paquet.addresses.emitter = automate;

  close(sd1);
  return EXIT_SUCCESS;
}
