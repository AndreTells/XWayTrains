#ifndef _TPTRAINS3_COMM_H
#define _TPTRAINS3_COMM_H
#include <stdbool.h>
#include <stdint.h>

#define MAXOCTETS 150
#define MAXEXTENSION MAXOCTETS - 14

#define LOCALIP "172.31.70.17"
#define REMOTEIP "10.31.125.14"
#define REMOTEPORT 502

#define EMETTEUR_STATION_ID 36
#define EMETTEUR_RESEAU_ID 1
#define EMETTEUR_PORT_ID 0

#define DESTINATAIRE_STATION_ID 14
#define DESTINATAIRE_RESEAU_ID 1
#define DESTINATAIRE_PORT_ID 0

#define CHECK_ERROR(val1, val2, msg) \
  if ((val1) == (val2)) {            \
    perror(msg);                     \
    exit(EXIT_FAILURE);              \
  }

#define NPDU_DATA 0xF0
#define SERVICE_LEVEL_STD 0x00
#define SERVICE_LEVEL_TELEGRAM 0x04
#define REFUS_ACCEPTED 0x00
#define REFUS_REFUSED 0x02
#define EXTENSION_OFF 0x00
#define EXTENSION_ON 0x01

#define UNITE_WRITE_OBJECT 0x37
#define UNITE_READ_OBJECTS 0x36
#define UNITE_CATEGORY 0x06               // other values are possible
#define UNITE_SEGMENT_INTERNAL_DATA 0x68  // mot interne p. 185
#define UNITE_TYPE_MOT_INTERNE 0x07
#define UNITE_TYPE_DOUBLE_MOT_INTERNE 0x08

#define CODE_SEND 0x09
#define CODE_RECEIVE 0x19

#define TRAIN1 0x27
#define TRAIN2 0x2A
#define TRAIN3 0x2D

#define UNCHANGED 0xFFFF

typedef uint16_t word_t;

typedef struct {
  uint8_t station_id;
  uint8_t network_id;
  uint8_t porte_id;
} xway_address_t;

typedef struct {
  xway_address_t emitter;
  xway_address_t reciever;
  uint8_t port_ack;
} addresses_t;

typedef struct {
  word_t train_station_id;
  word_t section_id;
  word_t switch_id;
} xway_data_t;

typedef struct {
  uint8_t code;
  uint8_t category;
  uint8_t object_segment;
  uint8_t object_type;
  word_t address_start;
  word_t word_count;
  xway_data_t data;
} xway_unite_req_t;

typedef struct {
  uint8_t npdu_type;
  addresses_t addresses;
  xway_unite_req_t request;
} xway_package_t;

void init_write_package(xway_package_t *package, const xway_address_t local,
                        const xway_address_t automate, const word_t train,
                        const word_t section_id, const word_t switch_id);

void build_write_request(const xway_package_t package, uint8_t *request);
void print_data_hex(const uint8_t *request);
void convert_word(const word_t word, uint8_t *res);

bool is_write_ack_successful(const uint8_t request[MAXOCTETS]);

bool is_read_successful(const uint8_t response[MAXOCTETS],
                        const uint8_t request_bytes[MAXOCTETS],
                        uint8_t *port_number, word_t *switch_id);

void build_ack(const xway_package_t package, uint8_t request[MAXOCTETS]);
#endif
