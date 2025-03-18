/**
 * @file comm_general.h
 * @brief Header file for general TCP communication functions.
 *
 * This file contains function declarations for creating, connecting,
 * and accepting TCP sockets in a wrapper format.
 */
#ifndef COMM_GENERAL_H_
#define COMM_GENERAL_H_

#include <stdbool.h>

/**
 * @brief Creates a TCP socket.
 *
 * This function creates a TCP socket and binds it if it is a server socket.
 *
 * @param server Boolean indicating whether the socket is for a server (true) or client (false).
 * @param ipAddress Pointer to a string containing the IP address to bind/connect to.
 * @param port The port number to bind/connect to.
 * @return The socket file descriptor on success, or a negative value on error.
 */
int tcpCreateSocketWrapper(bool server, char* ipAddress, int port);

/**
 * @brief Establishes a connection to a TCP server.
 *
 * This function connects an existing socket to a specified IP address and port.
 *
 * @param sockFd The socket file descriptor.
 * @param ipAddress Pointer to a string containing the IP address of the server.
 * @param port The port number of the server.
 * @return 0 on success, or a negative value on error.
 */
int tcpConnectWrapper(int sockFd, char* ipAddress, int port);

/**
 * @brief Accepts an incoming TCP connection.
 *
 * This function waits for an incoming connection on the specified socket and accepts it.
 *
 * @param sockFd The socket file descriptor.
 * @return The new socket file descriptor for the accepted connection, or a negative value on error.
 */
int tcpAcceptWrapper(int sockFd);

#endif  // COMM_GENERAL_H_
