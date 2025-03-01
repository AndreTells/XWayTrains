#include "comm.h"

#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

void convert_mot(mot_t mot, uint8_t res[2]) {
  // least significant bits
  res[0] = mot & 0x00FF;
  // more significant bits
  res[1] = (mot >> 8) & 0xFF;
}

void init_package(xway_paquet_t *paquet, const xway_address_t local,
                  const xway_address_t automate) {
  paquet->type_npdu =
      NPDU_DATA | SERVICE_LEVEL_STD | REFUS_ACCEPTED | EXTENSION_ON;
  paquet->addresses.emitter = local;
  paquet->addresses.reciever = automate;

  paquet->requete.code = UNITE_WRITE_OBJECT;
  paquet->requete.categorie = UNITE_CATEGORY;
  paquet->requete.segment_objet = UNITE_SEGMENT_INTERNAL_DATA;
  paquet->requete.type_objet = UNITE_TYPE_MOT_INTERNE;
  paquet->requete.adresse_premier_mot = TRAIN1;
  paquet->requete.nb_mots = 0x03;

  paquet->requete.valeurs.trainStationId = local.station_id;
  paquet->requete.valeurs.troncon = UNCHANGED;
  paquet->requete.valeurs.aig = 31;
}

void build_request(xway_paquet_t paquet, uint8_t *requete) {
  // initialisation
  memset(requete, 0, MAXOCTETS);

  // partie réseau
  requete[7] = paquet.type_npdu;
  requete[8] = paquet.addresses.emitter.station_id;
  requete[9] = (paquet.addresses.emitter.reseau_id << 4) |
               (paquet.addresses.emitter.porte_id & 0x0F);
  requete[10] = paquet.addresses.reciever.station_id;
  requete[11] = (paquet.addresses.reciever.reseau_id << 4) |
                (paquet.addresses.reciever.porte_id & 0x0F);

  requete[12] = CODE_SEND;
  requete[13] = 0x00;

  // requête UNITE
  requete[14] = paquet.requete.code;
  requete[15] = paquet.requete.categorie;

  requete[16] = paquet.requete.segment_objet;
  requete[17] = paquet.requete.type_objet;

  uint8_t adresse_premier_mot[2], nb_mots[2];
  uint8_t values[2];

  convert_mot(paquet.requete.adresse_premier_mot, adresse_premier_mot);
  convert_mot(paquet.requete.nb_mots, nb_mots);

  requete[18] = adresse_premier_mot[0];
  requete[19] = adresse_premier_mot[1];
  requete[20] = nb_mots[0];
  requete[21] = nb_mots[1];

  convert_mot(paquet.requete.valeurs.trainStationId, values);
  requete[22] = values[0];
  requete[23] = values[1];

  convert_mot(paquet.requete.valeurs.troncon, values);
  requete[24] = values[0];
  requete[25] = values[1];

  convert_mot(paquet.requete.valeurs.aig, values);
  requete[26] = values[0];
  requete[27] = values[1];

  // header
  requete[3] = 0x01;      // always like this
  requete[5] = 0x15 + 1;  // hard-coded, length starting at this point
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
