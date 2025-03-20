#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "plc/plc_facade.h"  // Contains configWritePlcMessage()
#include "plc/plc_message.h"  // Contains serializePlcMessage_t() and create/free functions
#include "plc/model_info.h"
#include "common/verbose.h"
#include "common/flags.h"

/*
 * This test verifies that configWritePlcMessage() constructs the correct
 * message payload. It then serializes the complete message and compares the
 * resulting byte array with expected values.
 *
 * Expected serialized message (byte-by-byte):
 *
 *  Bytes  0-4: Modbus ID            : {0x00, 0x00, 0x00, 0x01, 0x00}
 *  Byte     5  : Message length       : 0x16
 *  Byte     6  : Separator            : 0x00
 *  Byte     7  : NPDU type            : 0xF1
 *  Bytes  8-9: Sender info          : {0x28, 0x10}
 *  Bytes 10-11: Receiver info        : {0x0E, 0x10}
 *  Bytes 12-13: Extended address     : {0x09, 0x10}
 *  Byte    14  : APDU code            : 0x37
 *  Bytes 15-27: APDU payload (13 bytes):
 *              [ 0] = Machine category    : 0x06
 *              [ 1] = Memory segment      : 0x68
 *              [ 2] = Object type         : 0x07
 *              [3-4]= Train address       : {0x27, 0x00}
 *              [5-6]= Data length         : {0x03, 0x00}
 *              [7-8]= First data element  : {0x28, 0x00}
 *              [9-10]= Second data element: {0xFF, 0xFF}
 *              [11-12]= Third data element: {0x1F, 0x00}
 *
 * (Note: The actual positions of the NPDU and APDU parts depend on your serialization
 *  format defined in serializePlcMessage_t(). This test assumes that the above layout
 *  is produced.)
 */
void test_configWritePlcMessage_toggleRail() {
  verbose("[Plc Facade] configWritePlcMessage (TOGGLE_RAIL) ... \n");

  /* Create a new message */
  PlcMessage_t* msg = createPlcMessage();
  assert(msg != NULL);

  /*
   * Call configWritePlcMessage with the following parameters:
   * - msgType: TOGGLE_RAIL
   * - station: 0x28
   * - trainId: TRAIN_1 (assumed to be defined, e.g. TRAIN_1 == 1)
   * - target: 2
   *
   * Note: The underlying mocks should be set up such that:
   *   getTrainAddr(TRAIN_1) returns a value that, after htons, results in {0x27, 0x10} in the message.
   *   getRailAddr(TRAIN_1, 2) returns a value that, after bswap_16, yields {0x10, 0x0E}.
   */
  int res = configWritePlcMessage(msg, TOGGLE_RAIL, 0x28, TRAIN_1, 2);
  assert(res == 0);

  /* Serialize the message into a buffer */
  size_t bufSize = 512;
  uint8_t* serBuf = malloc(bufSize);
  assert(serBuf != NULL);
  size_t serSize = serializePlcMessage_t(msg, serBuf);
  assert(serSize > 0);

  /*
   * Verify the expected bytes.
   * The expected layout is:
   *
   * [0-4]   : {0x00, 0x00, 0x00, 0x01, 0x00}   -> Modbus ID
   * [5]     : 0x16                              -> Message length
   * [6]     : 0x00                              -> Separator
   * [7]     : 0xF1                              -> NPDU type (NPDU_5WAY)
   * [8-9]   : {0x28, 0x10}                      -> Sender info
   * [10-11] : {0x0E, 0x10}                      -> Receiver info
   * [12-13] : {0x09, 0x10}                      -> Extended address
   * [14]    : 0x37                              -> APDU code (APDU_WRITE_REQ)
   * [15-27] : APDU payload (13 bytes) as defined below:
   *     [15]  : 0x06                          -> Machine category (PLC_CATEGORY)
   *     [16]  : 0x68                          -> Memory segment (INTERNAL_DATA_SPACE)
   *     [17]  : 0x07                          -> Object type (UNITE_TYPE_WORD)
   *     [18-19]: {0x27, 0x00}                  -> Train address (after htons(getTrainAddr()))
   *     [20-21]: {0x03, 0x00}                  -> Data length (htons(3))
   *     [22-23]: {0x28, 0x00}                  -> First data element (station, after htons)
   *     [24-25]: {0xFF, 0xFF}                  -> Second data element (modified by TOGGLE_RAIL)
   *     [26-27]: {0x1F, 0x00}                  -> Third data element (unchanged)
   */
  assert(serBuf[0]  == 0x00);
  assert(serBuf[1]  == 0x00);
  assert(serBuf[2]  == 0x00);
  assert(serBuf[3]  == 0x01);
  assert(serBuf[4]  == 0x00);

  assert(serBuf[5]  == 0x16);
  assert(serBuf[6]  == 0x00);

  assert(serBuf[7]  == 0xF1);

  assert(serBuf[8]  == 0x28);
  assert(serBuf[9]  == 0x10);

  assert(serBuf[10] == 0x0E);
  assert(serBuf[11] == 0x10);

  assert(serBuf[12] == 0x09);
  assert(serBuf[13] == 0x10);

  assert(serBuf[14] == 0x37);

  assert(serBuf[15] == 0x06);
  assert(serBuf[16] == 0x68);
  assert(serBuf[17] == 0x07);

  assert(serBuf[18] == 0x27);
  assert(serBuf[19] == 0x00);

  assert(serBuf[20] == 0x03);
  assert(serBuf[21] == 0x00);

  assert(serBuf[22] == 0x28);
  assert(serBuf[23] == 0x00);

  assert(serBuf[24] == 0xFF);
  assert(serBuf[25] == 0xFF);

  assert(serBuf[26] == 0x1F);
  assert(serBuf[27] == 0x00);

  free(serBuf);
  freeMessage(msg);
  verbose("[Plc Facade] configWritePlcMessage (TOGGLE_RAIL) ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);
}

int main(int argc, char* argv[]) {
  bool verbose_mode = get_flag_value(argc, argv, VERBOSE_FLAG, NULL);
  setVerbose(verbose_mode);

  verbose("[Unit Testing] Plc Facade ... \n\n");

  test_configWritePlcMessage_toggleRail();

  /*
   * Additional tests can be added for TOGGLE_TRAIN_DIR and TOGGLE_SWITCH
   * by providing the appropriate expected values.
   */

  verbose("\n[Unit Testing] Plc Facade ... Done \n");
  return 0;
}
