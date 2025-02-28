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
  xway_requete_unite_t requete_unite;

  paquet->type_npdu =
      NPDU_DATA | SERVICE_LEVEL_STD | REFUS_ACCEPTED | EXTENSION_ON;
  paquet->addresses.emitter = local;
  paquet->addresses.reciever = automate;

  paquet->extension.len = 3;
  paquet->extension.train = 39;    // TRAIN 1
  paquet->extension.troncon = 26;  // enable T26
  paquet->extension.aig = 0;

  requete_unite.code = UNITE_WRITE_OBJECT;
  requete_unite.categorie = UNITE_CATEGORY;
  requete_unite.segment_objet = UNITE_SEGMENT_INTERNAL_DATA;
  requete_unite.type_objet = UNITE_TYPE_DOUBLE_MOT_INTERNE;
  requete_unite.adresse_premier_mot = 42;
  requete_unite.nb_mots = 0;

  mot_t valeurs[requete_unite.nb_mots];

  valeurs[0] = local.station_id;
  valeurs[1] = -1;
  valeurs[2] = 7;

  requete_unite.valeurs = valeurs;
  paquet->requete = requete_unite;
}

void build_request(xway_paquet_t paquet, uint8_t *requete) {
  int i = 0, j = 0;
  uint8_t request_length;

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
  uint8_t values[paquet.requete.nb_mots][2];

  convert_mot(paquet.requete.adresse_premier_mot, adresse_premier_mot);
  convert_mot(paquet.requete.nb_mots, nb_mots);

  requete[18] = adresse_premier_mot[0];
  requete[19] = adresse_premier_mot[1];
  requete[20] = nb_mots[0];
  requete[21] = nb_mots[1];

  while (j < paquet.requete.nb_mots && (j + i < MAXEXTENSION)) {
    convert_mot(paquet.requete.valeurs[j], values[j]);
    requete[22 + j * 2] = values[j][0];
    requete[23 + j * 2] = values[j][1];
    j++;
  }

  // en tête modbus
  request_length = 0x15;
  requete[3] = 0x01;
  requete[5] = request_length + 1;
}

void print_data_hex(uint8_t *data) {
  printf("\tData (HEX): ");
  int len = data[5] + 6;
  for (int i = 0; i < len; i++) {
    printf("%02X ", data[i]);
  }
  printf("\n");
}
