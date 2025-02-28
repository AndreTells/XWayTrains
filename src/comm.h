#ifndef _TPTRAINS3_COMM_H
#define _TPTRAINS3_COMM_H
#include <stdint.h>

#define MAXOCTETS 150
#define MAXEXTENSION MAXOCTETS - 14

#define CHECK_ERROR(val1, val2, msg) \
  if ((val1) == (val2)) {            \
    perror(msg);                     \
    exit(EXIT_FAILURE);              \
  }

typedef struct {
  uint8_t station_id;
  uint8_t reseau_id;
  uint8_t porte_id;
} xway_address_t;

typedef struct {
  xway_address_t emitter;
  xway_address_t reciever;
} addresses_t;

typedef uint16_t mot_t;

typedef struct {
  uint8_t code;
  uint8_t categorie;
  uint8_t segment_objet;
  uint8_t type_objet;
  mot_t adresse_premier_mot;
  mot_t nb_mots;
  mot_t *valeurs;
} xway_requete_unite_t;

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
typedef struct {
  uint8_t train;
  uint8_t troncon;  // to move
  uint8_t aig;      // to turn
  uint8_t len;
} extension_t;

typedef struct {
  uint8_t type_npdu;
  addresses_t addresses;
  extension_t extension;
  xway_requete_unite_t requete;
} xway_paquet_t;

void init_package(xway_paquet_t *paquet, const xway_address_t local,
                  const xway_address_t automate);

void build_request(xway_paquet_t paquet, uint8_t *requete);
void print_data_hex(uint8_t *requete);
void convert_mot(mot_t mot, uint8_t *res);
#endif
