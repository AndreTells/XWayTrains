/**
 * @file plc_proxy.h
 * @brief Interface for communicating with the PLC (Programmable Logic
 * Controller)
 * @details Provides functions to initialize, deinitialize, send messages to,
 * and read messages from the PLC.
 */
#ifndef PLC_PROXY_H_
#define PLC_PROXY_H_

#include <stdint.h>

#include "plc/model_info.h"
#include "plc/plc_message.h"

/**
 * @brief Opaque handle for the PLC Proxy instance
 * @details Encapsulates all PLC communication state and functionality.
 *          Implementation details are hidden in the corresponding .c file.
 */
typedef struct PlcProxy_t PlcProxy_t;

/**
 * @brief Initializes the PLC Proxy.
 *
 * This function creates and configures a new PLC Proxy instance for
 * communication with the PLC.
 *
 * @param plcIpAddr The IP address of the PLC.
 * @param port The port number to be used for communication.
 * @param remoteStation xway station number of the plc
 * @return PlcProxy_t* Pointer to the newly created PLC Proxy instance, or NULL
 * on failure.
 */
PlcProxy_t* initPlcProxy(char* plcIpAddr, const uint16_t port, uint8_t remoteStation );

/**
 * @brief Gracefully terminate a PLC Proxy instance
 * @param[in] plc Proxy instance handle to terminate
 * @return 0 on success, non-zero error code on failure
 */
int endPlcProxy(PlcProxy_t* plc);

/**
 * @brief Send a message to the PLC
 * @param[in] plc Proxy instance handle
 * @param[in] msg Pointer to the message to send
 * @return the size of the message sent or -1 on error
 */
ssize_t sendMessagePlcProxy(PlcProxy_t* plc, PlcMessage_t* msg, uint8_t hostStation);

/**
 * @brief Read a message from the PLC
 * @param[in] plc Proxy instance handle
 * @param[in] clientId id of who is attempting to read as message
 * @return Pointer to the received message, or NULL on failure
 * @note the caller is in charge of freeing memory of the received message
 */
PlcMessage_t* readMessagePlcProxy(PlcProxy_t* plc, enum TrainId_e cliendtId);

#endif  // PLC_PROXY_H_
