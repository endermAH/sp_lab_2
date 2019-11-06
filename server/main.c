#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

const char *optString = "p:h";

struct globalArgs_t {
  int portno;
} globalArgs;

void displayUsage() {
  printf("\nUSAGE:\n%s [-h] [-p <port number>]\n\nARGS: \n-p Port number to listen\n-h: Help\n\n", name);
  return 1;
}

int getStartData(int argc, char** argv) {
  int opt = 0;

  //Separating argc to variables
  opt = getopt(argc, argv, optString);
  while (opt != -1) {
    switch (opt) {
      case 'p':
        globalArgs.portno = atoi(optarg);
        break;
      case 'h':
        display_usage(argv[0]);
        break;
      default:
        break;
    }
    opt = getopt(argc, argv, optString);
  }

  //Validating input data
  if (globalArgs.portno < 0) {
    fprintf(stderr, "Incorrect port number\n");
    return 0;
  }

  return 1;
}

int main(int argc, char *argv[]) {
  int sockfd; //Socket descriptor
  int portno; //Port number
  struct sockaddr_in serv_addr; //IP struct
  struct hostnet *server;

  char buffer[256];

  if (!getStartData(argc, argv)) return 1; //Stop server with wrong input


}
