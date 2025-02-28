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

#define LOCALIP "172.31.70.17"
#define REMOTEIP "10.31.125.14"
#define REMOTEPORT 502

#define EMETTEUR_STATION_ID 40
#define EMETTEUR_RESEAU_ID 1
#define EMETTEUR_PORT_ID 0

#define DESTINATAIRE_STATION_ID 14
#define DESTINATAIRE_RESEAU_ID 1
#define DESTINATAIRE_PORT_ID 0

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

  uint8_t extension_data[MAXEXTENSION] = {0};

  uint8_t requete[MAXOCTETS];

  xway_paquet_t paquet;
  xway_requete_unite_t requete_unite;
  xway_address_t local = {EMETTEUR_STATION_ID, EMETTEUR_RESEAU_ID,
                          EMETTEUR_PORT_ID};
  xway_address_t automate = {DESTINATAIRE_STATION_ID, DESTINATAIRE_RESEAU_ID,
                             DESTINATAIRE_PORT_ID};

  paquet.type_npdu = 0xF1;
  paquet.addresses.emetteur = local;
  paquet.addresses.destinataire = automate;

  paquet.extension_len = 2;
  extension_data[0] = 0x09;
  extension_data[1] = 0x00;
  paquet.extension_data = extension_data;

  requete_unite.code = 0x37;
  requete_unite.categorie = 0x06;
  requete_unite.segment_objet = 0x68;
  requete_unite.type_objet = 0x07;
  requete_unite.adresse_premier_mot = 42;
  requete_unite.nb_mots = 0;

  mot_t valeurs[requete_unite.nb_mots];

  valeurs[0] = local.station_id;
  valeurs[1] = -1;
  valeurs[2] = 7;

  requete_unite.valeurs = valeurs;
  paquet.requete = requete_unite;

  build_request(paquet, requete);

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

  printf("Requete : \n");
  print_data_hex(requete);

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

  paquet.addresses.destinataire = local;
  paquet.addresses.emetteur = automate;

  close(sd1);
  return EXIT_SUCCESS;
}
