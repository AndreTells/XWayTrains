#include "plc/plc_message.h"
#include <stdint.h>
#include <string.h>

#define MAX_DATA_SIZE 254
#define MODBUS_ID_SIZE 5
#define MIN_EMPTY_NPDU_SIZE 9
#define BYTE_SIZE 8

typedef struct {
  XwayAPDUCode_e code;
  int dataLen;
  uint8_t data [MAX_DATA_SIZE];
}XwayAPDU_t;

typedef struct {
  XwayNPDUType_e type;
  XwayAddr sender;
  XwayAddr receiver;
  uint8_t extendedAddr[2];
  XwayAPDU_t apdu;
}XwayNPDU_t;

typedef enum {
  MODBUS_ETHWAY
}ModuBusId_e;

static const uint8_t ModBusIdDict[1][5] = {
  [MODBUS_ETHWAY] = {0x00, 0x00, 0x00, 0x01, 0x00}
};

struct PlcMessage_t{
  uint8_t modbusId[5];
  uint8_t npduLen;
  uint8_t sep;
  XwayNPDU_t npdu;
};

PlcMessage_t* createPlcMessage(){
  PlcMessage_t* msg = malloc(sizeof(PlcMessage_t));

  memcpy(&(msg->modbusId), &(ModBusIdDict[MODBUS_ETHWAY]), 5*sizeof(uint8_t));
  msg->npduLen = MIN_EMPTY_NPDU_SIZE;
  msg->sep = 0x00;

  return msg;
}

int setAPDU(PlcMessage_t* msg, XwayAPDUCode_e code, uint8_t* data, int dataLen){
  XwayAPDU_t* apdu = &(msg->npdu.apdu);

  if(code == APDU_WRITE_RESP && dataLen > 0){
    return -1;
  }

  apdu->code = code;
  apdu->dataLen = dataLen;

  msg->npduLen += dataLen;

  for(int i=0; i<dataLen; i++){
    apdu->data[i] = data[i];
  }

  return 0;
}

int setNPDU(PlcMessage_t* msg, XwayNPDUType_e code, XwayAddr sender,
            XwayAddr receiver, uint8_t extendedAddr[2]){
  XwayNPDU_t* npdu = &(msg->npdu);

  npdu->type = code;
  npdu->sender = sender;
  npdu->receiver = receiver;

  if(code == NPDU_5WAY){
    msg->npduLen += 2;
    npdu->extendedAddr[0] = extendedAddr[0];
    npdu->extendedAddr[1] = extendedAddr[1];
  }

  return 0;
}

XwayAddr createXwayAddr(int8_t station, int8_t network, int8_t port){
  int16_t expandedStation = (int16_t)station;
  return (expandedStation << BYTE_SIZE) + (network << (BYTE_SIZE/2)) + (port & 0x0F);
}

// returns the msg size
size_t serializePlcMessage_t(PlcMessage_t* msg, uint8_t* serMsg){
  uint8_t* initSerMsg = serMsg;
  // setting preamble
  memcpy(serMsg,msg->modbusId, 5*sizeof(uint8_t));
  serMsg +=5;

  // setting Msg size & separator
  serMsg[0] = msg->npduLen;
  serMsg[1] = msg->sep;

  serMsg += 2;

  //setting npdu
  XwayNPDU_t* npdu = &(msg->npdu);
  serMsg[0] = (uint8_t)npdu->type;
  memcpy(serMsg+1,&(npdu->sender), 2*sizeof(uint8_t));
  memcpy(serMsg+2,&(npdu->receiver), 2*sizeof(uint8_t));

  serMsg += 3;

  // setting addr extension if present
  if(npdu->type == NPDU_5WAY){
    memcpy(serMsg,&(npdu->extendedAddr), 2*sizeof(uint8_t));

    serMsg += 2;
  }

  // setting apdu
  XwayAPDU_t* apdu = &(msg->npdu.apdu);

  serMsg[0] = apdu->code;

  serMsg +=1;

  if(apdu->code == APDU_WRITE_REQ){
    memcpy(serMsg,&(apdu->data), apdu->dataLen*sizeof(uint8_t));

    serMsg+=apdu->dataLen;
  }


  return sizeof(uint8_t)*(serMsg - initSerMsg);
}

// returns the msg
PlcMessage_t* deserializePlcMessage_t( uint8_t* serMsg, size_t msgSize){
  // TODO: implement
  return NULL;
}

int freeMessage(PlcMessage_t* msg){
  free(msg);
  return 0;
}
