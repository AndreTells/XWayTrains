#include "plc/plc_message.h"
#include <byteswap.h>
#include <stdint.h>
#include <string.h>

#define MAX_DATA_SIZE 254
#define MODBUS_ID_SIZE 5
#define MIN_NPDU_SIZE 7
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
  msg->npduLen = MIN_NPDU_SIZE;
  msg->sep = 0x00;

  return msg;
}

PlcMessage_t* createACK(PlcMessage_t* msg, bool res){
  // supposes f1 address
  PlcMessage_t* ack = createPlcMessage();

  setAPDU(ack, APDU_WRITE_RESP, NULL, 0);
  setNPDU(ack, msg->npdu.type, msg->npdu.receiver, msg->npdu.sender, msg->npdu.extendedAddr);

  return ack;
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

XwayAddr createXwayAddr(uint8_t station, uint8_t network, uint8_t port){
  uint16_t expandedStation = (int16_t)station;
  return (expandedStation << BYTE_SIZE) + (network << (BYTE_SIZE/2)) + (port & 0x0F);
}

// returns the msg size
size_t serializePlcMessage(PlcMessage_t* msg, uint8_t* serMsg){
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
  uint16_t senderNet = bswap_16(npdu->sender);
  uint16_t receiverNet = bswap_16(npdu->receiver);
  memcpy(serMsg+1,&(senderNet), sizeof(uint16_t));
  memcpy(serMsg+3,&(receiverNet), sizeof(uint16_t));

  serMsg += 5;

  // setting addr extension if present
  if(npdu->type == NPDU_5WAY){
    serMsg[0] = npdu->extendedAddr[0];
    serMsg[1] = npdu->extendedAddr[1];

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


  return sizeof(uint8_t)*(serMsg - initSerMsg - 1);
}

// returns the msg
PlcMessage_t* deserializePlcMessage( uint8_t* serMsg){
  PlcMessage_t* msg = createPlcMessage();
  serMsg+=7;

  // reading npdu
  XwayNPDUType_e npduType = serMsg[0];

  XwayAddr sender;
  XwayAddr receiver;

  memcpy(&sender, serMsg+1, sizeof(uint16_t));
  memcpy(&receiver, serMsg+3, sizeof(uint16_t));
  serMsg+=5;

  // reading npdu extensions
  uint8_t extension[2] = {0,0};
  if(npduType == NPDU_5WAY){
    extension[0] = serMsg[0];
    extension[1] = serMsg[1];

    serMsg+=2;
  }

  setNPDU(msg, npduType, bswap_16(sender), bswap_16(receiver), extension);

  // reading apdu
  XwayAPDUCode_e apduCode = serMsg[0];
  serMsg+=1;

  uint16_t len = 0;
  uint8_t data[MAX_DATA_SIZE];
  memset(&data,0,sizeof(data));

  if(apduCode == APDU_WRITE_REQ){
    len += 7;
    uint16_t msgDataLen = 0;
    memcpy(&msgDataLen, serMsg+5, sizeof(uint16_t)); // implicitly inverts the bit order
    len += msgDataLen*2;

    for(int i=0;i<len;i++){
      data[i] = serMsg[i];
    }
  }

  setAPDU(msg, apduCode, data, len);

  return msg;
}

int freeMessage(PlcMessage_t* msg){
  free(msg);
  return 0;
}
