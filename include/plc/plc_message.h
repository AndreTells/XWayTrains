#ifndef PLC_MESSAGE_H_
#define PLC_MESSAGE_H_
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "plc/model_info.h"

/**
 * @def MAX_MSG_SIZE
 * @brief Maximum size of a PLC message.
 */
#define MAX_MSG_SIZE 28

/**
 * @struct PlcMessage_t
 * @brief Opaque structure representing a PLC message.
 *
 * The internal details of this structure are hidden from the user.
 */
typedef struct PlcMessage_t PlcMessage_t;

/**
 * @typedef XwayAddr
 * @brief Type definition for an Xway address.
 */
typedef uint16_t XwayAddr;

/**
 * @enum XwayAPDUCode_e
 * @brief Enumeration of APDU codes used in PLC messages.
 */
typedef enum { APDU_WRITE_RESP = 0xFE, APDU_WRITE_REQ = 0x37 } XwayAPDUCode_e;

/**
 * @enum XwayNPDUType_e
 * @brief Enumeration of NPDU types used in PLC messages.
 */
typedef enum {
  NPDU_3WAY = 0xF0,
  NPDU_5WAY = 0xF1,
} XwayNPDUType_e;

/**
 * @enum XwayMachineCategory_e
 * @brief Enumeration of machine categories.
 */
typedef enum { PLC_CATEGORY = 0x06, PC_CATEGORY = 0x07 } XwayMachineCategory_e;

/**
 * @enum UniteObjectSegments_e
 * @brief Enumeration of data segments in a unit object.
 */
typedef enum {
  BIT_SPACE = 0x64,
  INTERNAL_DATA_SPACE = 0x68,
  CONSTANT_DATA_SPACE = 0x69,
  SYSTEM_DATA_SPACE = 0x6A
} UniteObjectSegments_e;

/**
 * @enum UniteObjectType_e
 * @brief Enumeration of unit object types.
 */
typedef enum {
  UNITE_TYPE_DATE = 1,
  UNITE_TYPE_WORD = 7,
  UNITE_TYPE_DOUBLE = 8,
} UniteObjectType_e;

/**
 * @brief Creates a new PLC message.
 *
 * @return PlcMessage_t* Pointer to the newly created PLC message structure.
 */
PlcMessage_t* createPlcMessage();

/**
 * @brief Creates an acknowledgment message based on an existing PLC message.
 *
 * @param msg Pointer to the original PLC message.
 * @param res Boolean result to include in the acknowledgment.
 * @return PlcMessage_t* Pointer to the newly created acknowledgment PLC
 * message.
 */
PlcMessage_t* createACK(PlcMessage_t* msg, bool res);

/**
 * @brief Sets the APDU portion of a PLC message.
 *
 * @param msg Pointer to the PLC message to be configured.
 * @param code APDU code to set.
 * @param data Pointer to the data to include in the APDU.
 * @param dataLen Length of the data.
 * @return int Status code (0 for success, negative for error).
 */
int setAPDU(PlcMessage_t* msg, XwayAPDUCode_e code, uint8_t* data, int dataLen);

/**
 * @brief Sets the NPDU portion of a PLC message.
 *
 * @param msg Pointer to the PLC message to be configured.
 * @param code NPDU type to set.
 * @param sender Sender's Xway address.
 * @param receiver Receiver's Xway address.
 * @param extendedAddr Pointer to the extended address data.
 * @return int Status code (0 for success, negative for error).
 */
int setNPDU(PlcMessage_t* msg, XwayNPDUType_e code, XwayAddr sender,
            XwayAddr receiver, uint8_t* extendedAddr);

/**
 * @brief Creates an Xway address from station, network, and port values.
 *
 * @param station Station number.
 * @param network Network identifier.
 * @param port Port number.
 * @return XwayAddr The created Xway address.
 */
XwayAddr createXwayAddr(uint8_t station, uint8_t network, uint8_t port);

/**
 * @brief Serializes a PLC message into a buffer.
 *
 * @param msg Pointer to the PLC message to be serialized.
 * @param serMsg Pointer to the buffer where the serialized message will be
 * stored.
 * @return size_t Size of the serialized message.
 */
size_t serializePlcMessage(PlcMessage_t* msg, uint8_t* serMsg);

/**
 * @brief Deserializes a buffer into a PLC message.
 *
 * @param serMsg Pointer to the buffer containing the serialized message.
 * @return PlcMessage_t* Pointer to the deserialized PLC message.
 */
PlcMessage_t* deserializePlcMessage(uint8_t* serMsg);

/**
 * @brief Frees a PLC message and its associated memory.
 *
 * @param msg Pointer to the PLC message to be freed.
 * @return int Status code (0 for success, negative for error).
 */
int freeMessage(PlcMessage_t* msg);

/**
 * @brief Compares the APDU type of a PLC message with a given code.
 *
 * @param msg Pointer to the PLC message to be checked.
 * @param code APDU code to compare.
 * @return bool True if the message's APDU type matches the given code, false
 * otherwise.
 */
bool compareMsgType(PlcMessage_t* msg, XwayAPDUCode_e code);

/**
 * @brief Retrieves the data portion from a PLC message.
 *
 * @param msg Pointer to the PLC message.
 * @return uint8_t* Pointer to the message data.
 */
uint8_t* getPlcMessageData(PlcMessage_t* msg);

#endif  // PLC_MESSAGE_H_
