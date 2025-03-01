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
  // init request
  uint8_t requete[MAXOCTETS];
  xway_package_t paquet;
  xway_address_t local = {EMETTEUR_STATION_ID, EMETTEUR_RESEAU_ID,
                          EMETTEUR_PORT_ID};
  xway_address_t automate = {DESTINATAIRE_STATION_ID, DESTINATAIRE_RESEAU_ID,
                             DESTINATAIRE_PORT_ID};

  init_write_package(&paquet, local, automate, TRAIN1, UNCHANGED, 31);
  build_write_request(paquet, requete);
  printf("Request : \n");
  print_data_hex(requete);

  // init sockets

  int sd1 = socket(AF_INET, SOCK_STREAM, 0);
  CHECKERROR(sd1, -1, "Creation fail !!!\n");

  struct sockaddr_in addr_serv;
  unsigned int adr_len = sizeof(struct sockaddr_in);

  CHECKERROR(connect(sd1, (const struct sockaddr *)&addr_serv,
                     sizeof(struct sockaddr_in)),
             -1, "Connexion fail !!!\n");

  addr_serv.sin_family = AF_INET;

  if (argc >= 3) {
    addr_serv.sin_port = htons(atoi(argv[2]));
    addr_serv.sin_addr.s_addr = inet_addr(argv[1]);
  } else {
    addr_serv.sin_port = htons(REMOTEPORT);
    addr_serv.sin_addr.s_addr = inet_addr(REMOTEIP);
  }

  uint8_t reponse[MAXOCTETS];
  ssize_t nbbytes = 0;
  ssize_t nbbytes_expected = requete[5] + 6;
  uint8_t port_number;

  // Start actual communication
  nbbytes = send(sd1, requete, nbbytes_expected, 0);
  if (nbbytes < nbbytes_expected) {
    perror("send: error on initial connection");
    exit(EXIT_FAILURE);
  }

  // wait for ACK
  nbbytes = recvfrom(sd1, reponse, MAXOCTETS, 0, (struct sockaddr *)&addr_serv,
                     &adr_len);
  if (nbbytes > 0) {
    printf("Response : \n");
    print_data_hex(reponse);
  }

  if (!is_write_ack_successful(reponse, &port_number)) {
    perror("Unsuccessful request");
    exit(EXIT_FAILURE);
  }

  // wait for return that signals the train has passed
  nbbytes = recvfrom(sd1, reponse, MAXOCTETS, 0, (struct sockaddr *)&addr_serv,
                     &adr_len);
  if (nbbytes > 0) {
    printf("Response : \n");
    print_data_hex(reponse);
  }

  // make sure it is correct
  if (!is_read_successful(reponse, requete, &port_number, paquet)) {
    perror("Unsucceful response");
    exit(EXIT_FAILURE);
  }
  paquet.addresses.port_ack = port_number;
  // Send the ACK signal

  close(sd1);
  return EXIT_SUCCESS;
}
