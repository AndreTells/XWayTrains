// Server side implementation of UDP client-server model
#include <arpa/inet.h>
#include <math.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT 8080
#define MAXLINE 128
#define MAXTIME 5  // Durée en secondes
#define NUM_TRAINS 1

#define CHECK(sts, msg) \
  if (-1 == (sts)) {    \
    perror(msg);        \
    exit(EXIT_FAILURE); \
  }

int saturateSpeed(int speed, unsigned int saturationValue) {
  int tempMax = saturationValue;
  if (speed < -tempMax) return -tempMax;
  if (speed > tempMax) return tempMax;
  return speed;
}

void sendCommand(int sockfd, struct sockaddr_in *cliaddr,
                 int positions[NUM_TRAINS], int speeds[NUM_TRAINS],
                 int u[NUM_TRAINS]) {
  unsigned int n;
  printf("\n\nWaiting a message...\n");
  unsigned char buffer_rx[MAXLINE], buffer_tx[MAXLINE];

  unsigned long long int timestamp;
  unsigned int trainNumber, maxSpeed;
  int speed, position, newSpeed, speedSetPoint;
  unsigned int len = sizeof(&cliaddr);

  buffer_tx[0] = 0;

  n = recvfrom(sockfd, (char *)buffer_rx, MAXLINE, MSG_WAITALL,
               (struct sockaddr *)cliaddr, &len);

  if (n != 16) {
    printf("Received message of wrong length\n");
    return;
  }

  printf("Received %d bytes: %X %X %X %X %X %X %X %X | ", n, buffer_rx[0],
         buffer_rx[1], buffer_rx[2], buffer_rx[3], buffer_rx[4], buffer_rx[5],
         buffer_rx[6], buffer_rx[7]);
  printf("%X | %X | %X %X %X %X | %X | %X\n", buffer_rx[8], buffer_rx[9],
         buffer_rx[10], buffer_rx[11], buffer_rx[12], buffer_rx[13],
         buffer_rx[14], buffer_rx[15]);

  // timeStamp, 64bits, in milliseconds
  timestamp = buffer_rx[0];
  timestamp = timestamp << 8;
  timestamp += buffer_rx[1];
  timestamp = timestamp << 8;
  timestamp += buffer_rx[2];
  timestamp = timestamp << 8;
  timestamp += buffer_rx[3];
  timestamp = timestamp << 8;
  timestamp += buffer_rx[4];
  timestamp = timestamp << 8;
  timestamp += buffer_rx[5];
  timestamp = timestamp << 8;
  timestamp += buffer_rx[6];
  timestamp = timestamp << 8;
  timestamp += buffer_rx[7];
  // Train number, 8 bits
  trainNumber = buffer_rx[8];
  // Train max speed, 8 bits
  maxSpeed = buffer_rx[9];
  // Train current distance from flag, 32 bits unsigned
  position = buffer_rx[10];
  position = position << 8;
  position += buffer_rx[11];
  position = position << 8;
  position += buffer_rx[12];
  position = position << 8;
  position += buffer_rx[13];
  // Train current speed setpoint, 8 bits signed
  if (buffer_rx[14] >
      127)  // For compatibility issues, 2's complement is made manually here
  {
    buffer_rx[14] = ~buffer_rx[14];
    speedSetPoint = -(buffer_rx[14] + 1);
  } else
    speedSetPoint = buffer_rx[14];
  // Train current speed, 8 bits signed
  if (buffer_rx[15] > 127)  // Same here
  {
    buffer_rx[15] = ~buffer_rx[15];
    speed = -(buffer_rx[15] + 1);
  } else
    speed = buffer_rx[15];

  // Treat the error case of a wrong trains trying to connect
  if (trainNumber > NUM_TRAINS) {
    return;
  }

  speeds[trainNumber - 1] = speed;
  positions[trainNumber - 1] = position;

  // Begin sending the desired speed
  newSpeed = u[trainNumber - 1];

  buffer_tx[0] = newSpeed & 0xFF;
  sendto(sockfd, (char *)buffer_tx, 1, MSG_CONFIRM,
         (const struct sockaddr *)cliaddr, len);
  printf(
      "Client @%s :TimeStamp %llu, train#%d, Max speed %d, position %d, "
      "set point %d, speed %d, new speed %d\n",
      inet_ntoa(((struct sockaddr_in *)cliaddr)->sin_addr), timestamp,
      trainNumber, maxSpeed, position, speedSetPoint, speed, newSpeed);
}

uint8_t init = 1;

static void signalHandler(int numSig) {
  switch (numSig) {
    case SIGALRM:
      init = 0;
      break;
    default:
      printf("Signal %d not treated\n", numSig);
  }
}

void installAlarm() {
  sigset_t mask;
  sigemptyset(&mask);
  sigaddset(&mask, SIGALRM);

  struct sigaction newAction, oldAction;

  newAction.sa_handler = signalHandler;
  newAction.sa_flags = SA_RESTART;

  CHECK(sigaction(SIGALRM, &newAction, &oldAction), "sigaction()");
}

int initSockets(struct sockaddr_in servaddr, struct sockaddr_in cliaddr) {
  // Creating socket file descriptor
  int sockfd;
  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("socket creation failed");
    exit(EXIT_FAILURE);
  }

  memset(&servaddr, 0, sizeof(servaddr));
  memset(&cliaddr, 0, sizeof(cliaddr));

  // Filling server information
  servaddr.sin_family = AF_INET;  // IPv4
  servaddr.sin_addr.s_addr = INADDR_ANY;
  servaddr.sin_port = htons(PORT);

  // Bind the socket with the server address
  if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }
  return sockfd;
}

void control(int speeds[NUM_TRAINS], int positions[NUM_TRAINS],
             int u[NUM_TRAINS], int d1) {
  float K[] = {2.7435, 2.8763};
  float G = 2.8763;

  int x[] = {speeds[0], positions[0]};
  u[0] = round(-(K[0] * x[0] + K[1] * x[1]) + G * d1);
}

void treatArgs(int argc, char **argv, int *desiredSpeed, int *d1, int *d2) {
  switch (argc) {
    default:
    case 4:
      *d2 = atoi(argv[3]);
      if (*d2 <= 0 || *d2 > 1000) {
        printf("Valeur incorrecte, d2 = 100\n");
        *d2 = 100;
      }
      [[fallthrough]];
    case 3:
      *d1 = atoi(argv[2]);
      if (*d1 <= 0 || *d1 > 1000) {
        printf("Valeur incorrecte, d1 = 100\n");
        *d1 = 100;
      }
      [[fallthrough]];
    case 2:
      *desiredSpeed = atoi(argv[1]);
      if (*desiredSpeed <= 0 || *desiredSpeed > 40) {
        printf("Valeur incorrecte, v = 25\n");
        *desiredSpeed = 25;
      }
    case 1:
      break;
  }
}

int main(int argc, char **argv) {
  int desiredSpeed = 25, d1 = 100, d2 = 100;
  treatArgs(argc, argv, &desiredSpeed, &d1, &d2);
  printf("v = %d, d1 = %d, d2 = %d\n", desiredSpeed, d1, d2);

  struct sockaddr_in servaddr, cliaddr;
  int sockfd = initSockets(servaddr, cliaddr);

  int positions[NUM_TRAINS];
  int speeds[NUM_TRAINS];
  int u[NUM_TRAINS];

  for (int i = 0; i < NUM_TRAINS; i++) {
    // slower speed in the beginning
    positions[0] = 0;
    speeds[0] = 0;
    u[i] = desiredSpeed / 2;
  }

  printf(
      "Initialize the trains so that they can get a first position "
      "calibration.\n");

  // Install SIGALRM handler
  installAlarm();

  alarm(10);
  while (init) {
    continue;
    sendCommand(sockfd, &cliaddr, positions, speeds, u);  //
  }

  printf("Starting main loop\n");
  for (;;) {
    control(speeds, positions, u, d1);
    printf("v=%d, d=%d, u=%d, y_ref=%d\n", speeds[0], positions[0], u[0], d1);
    sendCommand(sockfd, &cliaddr, positions, speeds, u);
  }
  return 0;
}
