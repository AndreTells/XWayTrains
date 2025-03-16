/**
 * @file plc_proxy.h
 * @brief Interface for communicating with the PLC (Programmable Logic
 * Controller)
 * @details Provides functions to initialize, deinitialize, send messages to,
 * and read messages from the PLC.
 */
#ifndef PLC_PROXY_H_
#define PLC_PROXY_H_

#include "plc_info.h"

/**
 * @brief Opaque handle for the PLC Proxy instance
 * @details Encapsulates all PLC communication state and functionality.
 *          Implementation details are hidden in the corresponding .c file.
 */
typedef struct PlcProxy_t PlcProxy_t;

/**
 * @brief Initialize a new PLC Proxy instance
 * @param plcIpAddr IP Address of the plc
 * @return Pointer to the newly created PlcProxy_t instance
 * @note The caller is responsible for gracefully terminating the instance using
 *       `endPlcProxy()`.
 */
PlcProxy_t* initPlcProxy(char* plcIpAddr);

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
 * @return 0 on success, non-zero error code on failure
 */
int sendMessagePlcProxy(PlcProxy_t* plc, PlcMessage_t* msg);

/**
 * @brief Read a message from the PLC
 * @param[in] plc Proxy instance handle
 * @param[in] clientId id of who is attempting to read as message
 * @return Pointer to the received message, or NULL on failure
 * @note the caller is in charge of freeing memory of the received message
 */
PlcMessage_t* readMessagePlcProxy(PlcProxy_t* plc, int cliendtId);

#endif  // PLC_PROXY_H_
