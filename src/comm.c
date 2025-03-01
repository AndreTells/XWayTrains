#include "comm.h"

#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

void invert_byte_order(word_t word, uint8_t res[2]) {
  // least significant bits
  res[0] = word & 0x00FF;
  // more significant bits
  res[1] = (word >> 8) & 0xFF;
}

void init_write_package(xway_package_t *package, const xway_address_t local,
                        const xway_address_t automate, word_t train,
                        word_t section_id, word_t switch_id) {
  package->npdu_type =
      NPDU_DATA | SERVICE_LEVEL_STD | REFUS_ACCEPTED | EXTENSION_ON;
  package->addresses.emitter = local;
  package->addresses.reciever = automate;

  package->request.code = UNITE_WRITE_OBJECT;
  package->request.category = UNITE_CATEGORY;
  package->request.object_segment = UNITE_SEGMENT_INTERNAL_DATA;
  package->request.object_type = UNITE_TYPE_MOT_INTERNE;
  package->request.word_count = 0x03;

  package->request.address_start = train;

  package->request.data.train_station_id = local.station_id;
  package->request.data.section_id = section_id;
  package->request.data.switch_id = switch_id;
}

void build_write_request(xway_package_t package, uint8_t *request) {
  // initialisation
  memset(request, 0, MAXOCTETS);

  // partie réseau
  request[7] = package.npdu_type;
  request[8] = package.addresses.emitter.station_id;
  request[9] = (package.addresses.emitter.network_id << 4) |
               (package.addresses.emitter.porte_id & 0x0F);
  request[10] = package.addresses.reciever.station_id;
  request[11] = (package.addresses.reciever.network_id << 4) |
                (package.addresses.reciever.porte_id & 0x0F);

  request[12] = CODE_SEND;
  request[13] = 0x00;

  // requête UNITE
  request[14] = package.request.code;
  request[15] = package.request.category;

  request[16] = package.request.object_segment;
  request[17] = package.request.object_type;

  uint8_t address_start[2], nb_mots[2];
  uint8_t values[2];

  invert_byte_order(package.request.address_start, address_start);
  invert_byte_order(package.request.word_count, nb_mots);

  request[18] = address_start[0];
  request[19] = address_start[1];
  request[20] = nb_mots[0];
  request[21] = nb_mots[1];

  invert_byte_order(package.request.data.train_station_id, values);
  request[22] = values[0];
  request[23] = values[1];

  invert_byte_order(package.request.data.section_id, values);
  request[24] = values[0];
  request[25] = values[1];

  invert_byte_order(package.request.data.switch_id, values);
  request[26] = values[0];
  request[27] = values[1];

  // header
  request[3] = 0x01;      // always like this
  request[5] = 0x15 + 1;  // hard-coded, length starting at this point
}

void print_data_hex(uint8_t *data) {
  printf("\tData (HEX): ");
  int len = data[5] + 6;
  for (int i = 0; i < len; i++) {
    printf("%02X ", data[i]);
  }
  printf("\n");
  printf(
      "\tPosition:    0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 "
      "19 20 21 22 23 24 25 26 27\n\n");
}
