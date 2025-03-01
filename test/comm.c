#include "comm.h"

#include <arpa/inet.h>
#include <assert.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

#define GREEN "\033[1;32m"
#define NOCOLOR "\033[1;0m"

void test_write_req() {
  uint8_t requete[MAXOCTETS];

  xway_package_t paquet;
  xway_address_t local = {EMETTEUR_STATION_ID, EMETTEUR_RESEAU_ID,
                          EMETTEUR_PORT_ID};
  xway_address_t automate = {DESTINATAIRE_STATION_ID, DESTINATAIRE_RESEAU_ID,
                             DESTINATAIRE_PORT_ID};

  init_write_package(&paquet, local, automate, TRAIN1, UNCHANGED, 31);
  build_write_request(paquet, requete);

  printf("Test the WRITE_OBJECTS is identical: ");
  fflush(stdout);
  assert(requete[0] == 0x00);
  assert(requete[1] == 0x00);
  assert(requete[2] == 0x00);
  assert(requete[3] == 0x01);
  assert(requete[4] == 0x00);
  assert(requete[5] == 0x16);

  assert(requete[6] == 0x00);
  assert(requete[7] == 0xF1);
  assert(requete[8] == 0x28);
  assert(requete[9] == 0x10);
  assert(requete[10] == 0x0E);
  assert(requete[11] == 0x10);
  assert(requete[12] == 0x09);
  assert(requete[13] == 0x00);
  assert(requete[14] == 0x37);
  assert(requete[15] == 0x06);
  assert(requete[16] == 0x68);
  assert(requete[17] == 0x07);
  assert(requete[18] == 0x27);
  assert(requete[19] == 0x00);
  assert(requete[20] == 0x03);
  assert(requete[21] == 0x00);
  assert(requete[22] == 0x28);
  assert(requete[23] == 0x00);
  assert(requete[24] == 0xFF);
  assert(requete[25] == 0xFF);
  assert(requete[26] == 0x1F);
  assert(requete[27] == 0x00);
  printf(GREEN " passed.\n" NOCOLOR);
}

void test_ack() {
  uint8_t requete[MAXOCTETS];

  xway_package_t paquet;
  xway_address_t local = {EMETTEUR_STATION_ID, EMETTEUR_RESEAU_ID,
                          EMETTEUR_PORT_ID};
  xway_address_t automate = {DESTINATAIRE_STATION_ID, DESTINATAIRE_RESEAU_ID,
                             DESTINATAIRE_PORT_ID};

  init_write_package(&paquet, local, automate, TRAIN1, UNCHANGED, 31);
  paquet.addresses.port_ack = 0x67;
  build_ack(paquet, requete);

  printf("Test the ACK is identical: ");
  fflush(stdout);
  assert(requete[0] == 0x00);
  assert(requete[1] == 0x00);
  assert(requete[2] == 0x00);
  assert(requete[3] == 0x01);
  assert(requete[4] == 0x00);
  assert(requete[5] == 0x09);

  assert(requete[6] == 0x00);
  assert(requete[7] == 0xF1);
  assert(requete[8] == 0x28);
  assert(requete[9] == 0x10);
  assert(requete[10] == 0x0E);
  assert(requete[11] == 0x10);
  assert(requete[12] == 0x19);
  assert(requete[13] == 0x67);
  assert(requete[14] == 0xFE);
  printf(GREEN " passed.\n" NOCOLOR);
}

int main() {
  test_write_req();
  test_ack();
}
