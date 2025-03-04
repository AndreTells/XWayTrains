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

int send_command(const int port, const in_addr_t addr, const word_t train,
                 const word_t section_id, const word_t switch_id) {
  // init request
  uint8_t requete[MAXOCTETS];

  xway_package_t paquet;
  xway_address_t local = {EMETTEUR_STATION_ID, EMETTEUR_RESEAU_ID,
                          EMETTEUR_PORT_ID};
  xway_address_t automate = {DESTINATAIRE_STATION_ID, DESTINATAIRE_RESEAU_ID,
                             DESTINATAIRE_PORT_ID};

  init_write_package(&paquet, local, automate, train, section_id, switch_id);
  build_write_request(paquet, requete);

  printf("Request : \n");
  print_data_hex(requete);

  // init sockets
  int sd1 = socket(AF_INET, SOCK_STREAM, 0);
  if (sd1 == -1) {
    perror("Creation fail !!!\n");
    return 1;
  }

  struct sockaddr_in addr_serv;
  unsigned int adr_len = sizeof(struct sockaddr_in);

  addr_serv.sin_family = AF_INET;
  addr_serv.sin_port = port;
  addr_serv.sin_addr.s_addr = addr;

  if (connect(sd1, (const struct sockaddr *)&addr_serv,
              sizeof(struct sockaddr_in)) == -1) {
    perror("Connexion fail !!!\n");
    return 2;
  }

  uint8_t reponse[MAXOCTETS];
  ssize_t nbbytes = 0;
  ssize_t nbbytes_expected = requete[5] + 6;
  uint8_t port_number;

  // STEP 1 - Start actual communication
  printf("Send\n");
  nbbytes = send(sd1, requete, nbbytes_expected, 0);
  if (nbbytes < nbbytes_expected) {
    perror("send: error on initial connection");
    return 3;
  }

  // STEP 2 - Wait for ACK
  printf("Wait\n");
  nbbytes = recvfrom(sd1, reponse, MAXOCTETS, 0, (struct sockaddr *)&addr_serv,
                     &adr_len);
  if (nbbytes > 0) {
    perror("Response : \n");
    print_data_hex(reponse);
  }

  if (!is_write_ack_successful(reponse)) {
    printf("Unsuccessful request\n");
    return 4;
  }

  // STEP 3 - Wait for return that signals the train has passed
  printf("Wait for completion\n");
  nbbytes = recvfrom(sd1, reponse, MAXOCTETS, 0, (struct sockaddr *)&addr_serv,
                     &adr_len);
  if (nbbytes > 0) {
    printf("Response : \n");
    print_data_hex(reponse);
  }

  // make sure it is correct
  word_t new_switch_id;
  if (!is_read_successful(reponse, requete, &port_number, paquet,
                          &new_switch_id)) {
    perror("Unsucceful response");
    return 5;
  }
  paquet.addresses.port_ack = port_number;

  // STEP 4 - Send the ACK signal
  // TODO: send ACK even if we detect an error

  printf("Send the ACK\n");
  build_ack(paquet, requete);
  nbbytes_expected = requete[5] + 6;
  nbbytes = send(sd1, requete, nbbytes_expected, 0);
  if (nbbytes < nbbytes_expected) {
    perror("send: error on initial connection");
    return 6;
  }

  // cleanup - success
  close(sd1);
  return 0;
}

int main(int argc, char *argv[]) {
  int port;
  in_addr_t addr;

  if (argc >= 3) {
    port = htons(atoi(argv[2]));
    addr = inet_addr(argv[1]);
  } else {
    port = htons(REMOTEPORT);
    addr = inet_addr(REMOTEIP);
  }

  send_command(port, addr, TRAIN1, 29, UNCHANGED);
  return EXIT_SUCCESS;
}
