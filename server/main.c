#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

const char *optString = "p:w:l:h";
const int BUFFER_LENGTH = 256;

struct globalArgs_t {
  int portno;
  int wait_time;
  char* log_path;
} globalArgs;

void error(char* msg) {
  perror(msg);
  exit(1);
}

void displayUsage(char* name) {
  printf("\nUSAGE:\n%s [-h] [-p <port number>]\n\nARGS: \n-p Port number to listen\n-w Time to wait\n-h: Help\n\n", name);
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
        displayUsage(argv[0]);
        break;
      case 'w':
        globalArgs.wait_time = atoi(optarg);
        break;
      case 'l':
        globalArgs.log_path = optarg;
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
  int listenfd; //Socket descriptors
  int portno; //Port number
  socklen_t struct_len = sizeof(struct sockaddr_in);
  struct sockaddr_in serv_addr; //IP struct
  struct sockaddr from;

  globalArgs.portno = NULL;
  globalArgs.wait_time = 0;
  globalArgs.log_path = NULL;

  char buffer[BUFFER_LENGTH];

  if (!getStartData(argc, argv)) exit(EXIT_FAILURE); //Stop server with wrong input

  //Create socket
  listenfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (listenfd < 0) {
    perror("Can not open socket\n");
    exit(EXIT_FAILURE);
  }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port = htons(globalArgs.portno);

   //Bind socket
  int test = bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

  int counter;
  short is_SNILS;

  while(1) {
    memset(buffer, 0, BUFFER_LENGTH);
    int count = recvfrom(listenfd, buffer, BUFFER_LENGTH - 1, 0, &from, &struct_len);
    counter = 0;
    is_SNILS = 1;

    for (int i = 0; i < count-1; i++) {
      if (buffer[i] == ' ' || buffer[i] == '-') {
        if (buffer[i-1] != ' ' && buffer[i-1] != '-') continue;
        else {
          is_SNILS = 0;
        }
      }
      if (buffer[i] >= 48 && buffer[i] <= 57) {
        counter++;
      } else {
        is_SNILS = 0;
      }
    }

    if (buffer[count-1] != '\n') is_SNILS = 0;
    if (counter != 11) is_SNILS = 0;

    if (is_SNILS == 0) {
      int test = sendto(listenfd, "FAILED\n", 7, 0, &from, struct_len);
    } else {
      int test = sendto(listenfd, "OK\n", 3, 0, &from, struct_len);
    }
    printf("Accepted: \"%s\"!\nSend status: %d \n", buffer, test);

    sleep(globalArgs.wait_time);
 }
}
