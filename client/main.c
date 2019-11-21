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

const char *optString = "p:a:hv";
const char *VERSION = "0.1.1";

int getStartData(int argc, char** argv) {
  int opt = 0;

  //Separating argc to variables
  opt = getopt(argc, argv, optString);
  while (opt != -1) {
    switch (opt) {
      case 'v':
        printf("Current version: %s\n", VERSION);
        exit(EXIT_FAILURE);
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
      case 'a':
        if (inet_aton(optarg, &globalArgs.listen_ip) == 0) error("Failed to convert listen ip to net format");
        break;
      case 'd':
        pid = fork();
        break;
      default:
        break;
    }
    opt = getopt(argc, argv, optString);
  }

  //Validating input data
  if (globalArgs.portno < 0) {
    error("Incorrect port number. Can not start server.\n");
    return 0;
  }
  if (globalArgs.portno == NULL) {
    error("Can not run server without port number.\n");
    return 0;
  }
  if (globalArgs.wait_time == NULL) {
    globalArgs.wait_time = 0;
  }
  if (globalArgs.log_path == NULL) {
    globalArgs.log_path = STD_LOG_PATH;
    puts(STD_LOG_PATH);
  }

  return 1;
}

int main(int argc, char *argv[]) {
    struct sockaddr_in serv_addr;

    return 0;
}
