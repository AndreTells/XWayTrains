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
  uint8_t extension_data[MAXEXTENSION] = {0};

  paquet->type_npdu =
      NPDU_DATA | SERVICE_LEVEL_STD | REFUS_ACCEPTED | EXTENSION_ON;
  paquet->addresses.emitter = local;
  paquet->addresses.reciever = automate;

  paquet->extension.len = 2;
  extension_data[0] = 0x09;
  extension_data[1] = 0x00;
  paquet->extension.data = extension_data;

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
  paquet->requete = requete_unite;
}

void build_request(xway_paquet_t paquet, uint8_t *requete) {
  int i = 0, j = 0;
  int req_off = 12;
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

  while (i < paquet.extension.len && i < MAXEXTENSION) {
    requete[12 + i] = paquet.extension.data[i];
    i++;
  }
  req_off += i;

  // requête UNITE
  requete[req_off] = paquet.requete.code;
  requete[req_off + 1] = paquet.requete.categorie;

  if (paquet.requete.segment_objet != 0)
    requete[req_off + 2] = paquet.requete.segment_objet;
  if (paquet.requete.type_objet != 0)
    requete[req_off + 3] = paquet.requete.type_objet;
  uint8_t adresse_premier_mot[2], nb_mots[2];
  uint8_t values[paquet.requete.nb_mots][2];

  convert_mot(paquet.requete.adresse_premier_mot, adresse_premier_mot);
  convert_mot(paquet.requete.nb_mots, nb_mots);

  requete[req_off + 4] = adresse_premier_mot[0];
  requete[req_off + 5] = adresse_premier_mot[1];
  requete[req_off + 6] = nb_mots[0];
  requete[req_off + 7] = nb_mots[1];

  while (j < paquet.requete.nb_mots && (j + i < MAXEXTENSION)) {
    convert_mot(paquet.requete.valeurs[j], values[j]);
    requete[req_off + 8 + j * 2] = values[j][0];
    requete[req_off + 9 + j * 2] = values[j][1];
    j++;
  }

  // en tête modbus
  request_length = 7 + i + 4 + (j + 1) * 2;
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
