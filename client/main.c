#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

const char *optString = "p:a:m:hv";
const char *VERSION = "0.9.3";
const int BUFFER_LENGTH = 256;

struct globalArgs_t {
  int portno;
  struct in_addr listen_ip;
  char *msg;
} globalArgs;

void error(char *msg) {
  fprintf(stderr, "\x1b[31mERROR:\x1b[0m %s\n", msg);
  exit(EXIT_FAILURE);
}

void displayUsage(char* name) {
  printf("\nUSAGE:\n%s [-h] [-p <port number>] [-v] [-i <ip address>]", name);
  exit(EXIT_SUCCESS);
}

void getStartData(int argc, char** argv) {
  int opt = 0;

  //Separating argc to variables
  opt = getopt(argc, argv, optString);
  while (opt != -1) {
    switch (opt) {
      case 'v':
        printf("Current version: %s\n", VERSION);
        exit(EXIT_SUCCESS);
      case 'p':
        globalArgs.portno = atoi(optarg);
        break;
      case 'h':
        displayUsage(argv[0]);
        break;
      case 'a':
        if (inet_aton(optarg, &globalArgs.listen_ip) == 0) error("Failed to convert listen ip to net format");
        break;
      case 'm':
        globalArgs.msg = optarg;
        break;
      default:
        break;
    }
    opt = getopt(argc, argv, optString);
  }

  //Validating input data
  if (globalArgs.portno < 0) {
    error("Incorrect port number. Can not attach server.\n");
  }
  if (globalArgs.portno == NULL) {
    error("Can not run without port number.\n");
  }
}

int main(int argc, char *argv[]) {
    struct sockaddr_in serv_addr;
    struct sockaddr_in client_addr;

    if (getenv("L2PORT") != NULL) globalArgs.portno = atoi(getenv("L2PORT"));
    if (getenv("L2ADDR") != NULL) {
      if (inet_aton(getenv("L2ADDR"), &globalArgs.listen_ip) == 0)
        printf("\x1b[35mWARNING:\x1b[0m Failed to convert listen ip from $L2ADDR to net format");
    }

    getStartData(argc, argv);

    if (globalArgs.listen_ip.s_addr == NULL) {
        error("No ip to connect");
    }

    int listenfd; //Socket descriptors
    socklen_t struct_len = sizeof(struct sockaddr_in);
    struct sockaddr from;

    listenfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (listenfd < 0) {
      error("Can not open socket\n");
      exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr = globalArgs.listen_ip;
    serv_addr.sin_port = htons(globalArgs.portno);

    client_addr.sin_family = AF_INET;
    client_addr.sin_addr.s_addr = INADDR_ANY;
    client_addr.sin_port = 0;

    int test = bind(listenfd, (struct sockaddr*)&client_addr, sizeof(serv_addr));

    char *resp;

    asprintf(&resp, "%s\n", globalArgs.msg);
    test = sendto(listenfd, resp, strlen(resp), 0, (struct sockaddr*)&serv_addr, struct_len);
    if (test == -1) {
      error("Failed to send to socket");
    }

    char buffer[BUFFER_LENGTH];
    memset(buffer, 0, BUFFER_LENGTH);
    int count = recvfrom(listenfd, buffer, BUFFER_LENGTH - 1, 0, &from, &struct_len);

    printf("Answer: %s", buffer);
    return 0;
}
