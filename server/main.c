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

void error(char* msg) {
  fprintf(stderr, msg);
  exit(1);
}

void displayUsage() {
  printf("\nUSAGE:\n%s [-h] [-p <port number>]\n\nARGS: \n-p Port number to listen\n-h: Help\n\n", name);
  exit(0);
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
  if (globalArgs.portno == NULL) {
    fprintf(stderr, "Can not run without port number\n");
    return 0;
  }

  return 1;
}

int main(int argc, char *argv[]) {
  int listenfd; //Socket descriptor
  int portno; //Port number
  struct sockaddr_in serv_addr; //IP struct
  struct hostnet *server;

  char buffer[256];

  if (!getStartData(argc, argv)) exit(1); //Stop server with wrong input

  //Creating socket
  listenfd = socket(AF_INET, SOCK_STREAM, IPPROTO_UDP);
  if (sockfd < 0) error("Can not open socket");

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port = htons(5000);

  bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); //Binding socket

  listen(listenfd, 10);

  while(1) {
    connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);

    snprintf(sendBuff, sizeof(sendBuff), "DAROVA VI POPALI V UGANDU\r\n")
    write(connfd, sendBuff, strlen(sendBuff));

    close(connfd);
    sleep(1);
 }
}
