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
  xway_address_t local = {0x28, EMETTEUR_RESEAU_ID, EMETTEUR_PORT_ID};
  xway_address_t automate = {DESTINATAIRE_STATION_ID, DESTINATAIRE_RESEAU_ID,
                             DESTINATAIRE_PORT_ID};

  init_write_package(&paquet, local, automate, TRAIN1, UNCHANGED, 31);
  build_write_request(paquet, requete);
  print_data_hex(requete);
  printf("Test the WRITE_OBJECTS is identical: ");
  fflush(stdout);
  // checking the modbus preamble
  assert(requete[0] == 0x00);
  assert(requete[1] == 0x00);
  assert(requete[2] == 0x00);
  assert(requete[3] == 0x01);
  assert(requete[4] == 0x00);

  assert(requete[5] == 0x16); // checking the message length
  assert(requete[6] == 0x00); // separator

  // checking the npdu
  assert(requete[7] == 0xF1);

  // checking sendner info
  assert(requete[8] == 0x28); // sender station
  assert(requete[9] == 0x10); // sender port

  // checking receiver info
  assert(requete[10] == 0x0E);// receiver station
  assert(requete[11] == 0x10);// receiver port

  // checking extended addr
  assert(requete[12] == 0x09); // module number
  assert(requete[13] == 0x10); // module number

  // checking APDU
  assert(requete[14] == 0x37); // req code
  assert(requete[15] == 0x06); // machine category
  assert(requete[16] == 0x68); // memory segment to write as per table
  assert(requete[17] == 0x07); // obj type
  assert(requete[18] == 0x27); // addr of first element[1]
  assert(requete[19] == 0x00); // addr of first element[2]
  assert(requete[20] == 0x03); // len data[1]
  assert(requete[21] == 0x00); // len data[2]
  assert(requete[22] == 0x28); // data 1[1]
  assert(requete[23] == 0x00); // data 1[2]
  assert(requete[24] == 0xFF); // data 2[1]
  assert(requete[25] == 0xFF); // data 2[2]
  assert(requete[26  == 0x1F); // data 3[2]
  assert(requete[27] == 0x00); // data 3[2]

  printf(GREEN " passed.\n" NOCOLOR);
}

void test_write_ack_validation() {
  uint8_t reponse[MAXOCTETS];
  reponse[0] = 0x00;
  reponse[1] = 0x00;
  reponse[2] = 0x00;
  reponse[3] = 0x01;
  reponse[4] = 0x00;
  reponse[5] = 0x09;
  reponse[6] = 0x00;
  reponse[7] = 0xF1;
  reponse[8] = 0x0E;
  reponse[9] = 0x10;
  reponse[10] = 0x28;
  reponse[11] = 0x10;
  reponse[12] = 0x19;
  reponse[13] = 0x00;  // we sent a 00 beforehand
  reponse[14] = 0xFE;  // success

  const bool result = is_write_ack_successful(reponse);

  printf("It validates the sample WRITE_OBJECTS_ACK: ");
  fflush(stdout);
  assert(result);
  printf(GREEN " passed.\n" NOCOLOR);
}

void test_read_validation() {
  uint8_t requete[MAXOCTETS];

  xway_package_t paquet;
  xway_address_t local = {0x28, EMETTEUR_RESEAU_ID, EMETTEUR_PORT_ID};
  xway_address_t automate = {DESTINATAIRE_STATION_ID, DESTINATAIRE_RESEAU_ID,
                             DESTINATAIRE_PORT_ID};

  init_write_package(&paquet, local, automate, TRAIN1, UNCHANGED, 31);
  build_write_request(paquet, requete);

  uint8_t reponse[MAXOCTETS];
  reponse[0] = 0x00;
  reponse[1] = 0x00;
  reponse[2] = 0x00;
  reponse[3] = 0x01;
  reponse[4] = 0x00;
  reponse[5] = 0x12;
  reponse[6] = 0x00;
  reponse[7] = 0xF1;
  reponse[8] = 0x0E;
  reponse[9] = 0x10;
  reponse[10] = 0x28;
  reponse[11] = 0x10;
  reponse[12] = 0x09;
  reponse[13] = 0x34;  // it answers a port_number
  reponse[14] = 0x37;
  reponse[15] = 0x07;
  reponse[16] = 0x68;
  reponse[17] = 0x07;
  reponse[18] = 0x09;
  reponse[19] = 0x00;
  reponse[20] = 0x01;
  reponse[21] = 0x00;
  reponse[22] = 0x1F;
  reponse[23] = 0x00;

  uint8_t port_number;
  word_t new_switch_id;
  const bool result = is_read_successful(reponse, requete, &port_number, paquet,
                                         &new_switch_id);

  printf("It validates the sample READ recieved: ");
  fflush(stdout);
  assert(result);
  assert(port_number == 0x34);
  assert(new_switch_id == 0x001F);
  printf(GREEN " passed.\n" NOCOLOR);
}

void test_ack() {
  uint8_t requete[MAXOCTETS];

  xway_package_t paquet;
  xway_address_t local = {0x28, EMETTEUR_RESEAU_ID, EMETTEUR_PORT_ID};
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
  test_write_ack_validation();
  test_read_validation();
  test_ack();
}
