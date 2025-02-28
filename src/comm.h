#ifndef _TPTRAINS3_COMM_H
#define _TPTRAINS3_COMM_H
#include <stdint.h>

typedef struct {
  uint8_t station_id;
  uint8_t reseau_id;
  uint8_t porte_id;
} xway_address_t;

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

typedef struct {
  uint8_t type_npdu;
  xway_address_t emetteur;
  xway_address_t destinataire;
  xway_requete_unite_t requete;
  uint8_t *extension_data;
  uint8_t extension_len;
} xway_paquet_t;

#define MAXOCTETS 150
#define MAXEXTENSION MAXOCTETS - 14

#define CHECK_ERROR(val1, val2, msg) \
  if (val1 == val2) {                \
    perror(msg);                     \
    exit(EXIT_FAILURE);              \
  }

void build_request(xway_paquet_t paquet, uint8_t *requete);
void print_data_hex(uint8_t *requete);
void convert_mot(mot_t mot, uint8_t *res);
#endif
