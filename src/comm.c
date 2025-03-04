#include "comm.h"

#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

void invert_byte_order(const word_t word, uint8_t res[2]) {
  // least significant bits
  res[0] = word & 0x00FF;
  // more significant bits
  res[1] = (word >> 8) & 0xFF;
}

void init_write_package(xway_package_t *package, const xway_address_t local,
                        const xway_address_t automate, const word_t train,
                        const word_t section_id, const word_t switch_id) {
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

void build_write_request(const xway_package_t package, uint8_t *request) {
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

  // addr extension
  request[12] = CODE_SEND;
  request[13] = 0x10;

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

void print_data_hex(const uint8_t *data) {
  printf("\tData (HEX): ");
  int len = data[5] + 6;
  for (int i = 0; i < len; i++) {
    if (i == 5 || i == 6 || i == 8 || i == 18) printf("| ");
    printf("%02X ", data[i]);
  }
  printf("\n");
  printf(
      "\tPosition:    0  1  2  3  4 |  5 |  6  7 |  8  9 10 11 12 13 14 15 16 "
      "17 | 18 "
      "19 20 21 22 23 24 25 26 27\n\n");
}

bool is_write_ack_successful(const uint8_t request[MAXOCTETS]) {
  const bool length_success = request[5] == 0x09;
  const bool status_success = request[14] == 0xFE;
  return length_success && status_success;
}

bool is_read_successful(const uint8_t response[MAXOCTETS],
                        const uint8_t request_bytes[MAXOCTETS],
                        uint8_t *port_number, const xway_package_t request,
                        word_t *switch_id) {
  // TODO: emit an error message for each verification ?
  uint8_t values[2];
  *port_number = response[13];

  const bool length_success = response[5] == 0x12;

  // reponse[18, 19] have an obscure meaning
  // reponse[20, 21] = 0x0100
  *switch_id = ((uint16_t)response[23] << 8) + response[22];

  const bool reciever_success =
      response[8] == request_bytes[10] && response[9] == request_bytes[11];
  const bool emitter_success =
      response[10] == request_bytes[8] && response[11] == request_bytes[9];

  const bool success = length_success && reciever_success && emitter_success;
  if (!success)
    printf("Conditions: %d, %d, %d, %d\n", length_success, reciever_success,
           emitter_success);
  return success;
}

void build_ack(const xway_package_t package, uint8_t request[MAXOCTETS]) {
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

  request[12] = CODE_RECEIVE;
  request[13] = package.addresses.port_ack;

  // STATUS
  request[14] = 0xFE;
  // header
  request[3] = 0x01;  // always like this
  request[5] = 0x09;  // hard-coded, length starting at this point
}
