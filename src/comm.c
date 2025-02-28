#include "comm.h"

#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

void convert_mot(mot_t mot, uint8_t res[2]) {
  res[0] = mot & 0xFF;
  res[1] = (mot >> 8) & 0xFF;
}

void build_request(xway_paquet_t paquet, uint8_t *requete) {
  int i = 0, j = 0;
  int req_off = 12;
  uint8_t request_length;

  // initialisation
  memset(requete, 0, MAXOCTETS);

  // partie réseau
  requete[7] = paquet.type_npdu;
  requete[8] = paquet.addresses.emetteur.station_id;
  requete[9] = (paquet.addresses.emetteur.reseau_id << 4) |
               (paquet.addresses.emetteur.porte_id & 0x0F);
  requete[10] = paquet.addresses.destinataire.station_id;
  requete[11] = (paquet.addresses.destinataire.reseau_id << 4) |
                (paquet.addresses.destinataire.porte_id & 0x0F);

  while (i < paquet.extension_len && i < MAXEXTENSION) {
    requete[12 + i] = paquet.extension_data[i];
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
