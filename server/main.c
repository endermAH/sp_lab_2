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

char *STD_LOG_PATH = "/tmp/lab2.log";
const char *optString = "p:w:l:hdv";
const int BUFFER_LENGTH = 256;
const char *VERSION = "0.9.1";

const char *LOG_SUCCESS = "\x1b[32mSUCCESS\x1b[0m";
const char *LOG_ERROR = "\x1b[31mERROR\x1b[0m";
const char *LOG_REQUEST = "\x1b[33mREQUEST\x1b[0m";
const char *LOG_WARNING = "\x1b[35mWARNING\x1b[0m";

pid_t pid;

struct globalArgs_t {
  int portno;
  int wait_time;
  char* log_path;
  FILE* log_file;
} globalArgs;

void l2log(char* msg, const char* status) {
  time_t cur_time;
  cur_time = time(NULL);
  struct tm *u;
  u = localtime(&cur_time);
  char str_time[256];
  strftime(str_time, 256, "%d.%m.%y %T", u);
  globalArgs.log_file = fopen(globalArgs.log_path, "a");
  fprintf(globalArgs.log_file, "%s: %s - %s\n", status, str_time, msg);
  fclose(globalArgs.log_file);
}

void term_handler() {
  l2log("Server terminated by SIGTERM", LOG_WARNING);
  exit(EXIT_SUCCESS);
}

void quit_handler() {
  l2log("Server terminated by SIGQUIT", LOG_WARNING);
  exit(EXIT_SUCCESS);
}

void interrupt_handler() {
  l2log("Server terminated by SIGINT", LOG_WARNING);
  exit(EXIT_SUCCESS);
}

void error(char* msg) {
  perror(msg);
  l2log(msg, LOG_ERROR);
  exit(EXIT_FAILURE);
}

void displayUsage(char* name) {
  printf("\nUSAGE:\n%s [-h] [-p <port number>] [-w <wait time>] [-l <path to log file>]\n\nARGS: \n-p Port number to listen\n-w Time to wait\n-h: Help\n\n", name);
  exit(EXIT_SUCCESS);
}

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
      case 'd':
        pid = fork();
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
  struct sigaction sa;
  sa.sa_handler = term_handler;
  sigaction(SIGTERM, &sa, 0);
  sa.sa_handler = quit_handler;
  sigaction(SIGQUIT, &sa, 0);
  sa.sa_handler = interrupt_handler;
  sigaction(SIGINT, &sa, 0);

  int listenfd; //Socket descriptors
  int portno; //Port number
  socklen_t struct_len = sizeof(struct sockaddr_in);
  struct sockaddr_in serv_addr; //IP struct
  struct sockaddr from;

  if (getenv("L2PORT") != NULL) globalArgs.portno = atoi(getenv("L2PORT"));
  if (getenv("L2WAIT") != NULL) globalArgs.wait_time = atoi(getenv("L2WAIT"));
  globalArgs.log_path = getenv("L2LOGFILE");

  char buffer[BUFFER_LENGTH];

  if (!getStartData(argc, argv)) exit(EXIT_FAILURE); //Stop server with wrong input

  char *daemon_log;
  if (pid == 0) {
    asprintf(&daemon_log, "Daemon started with pid: %d", getpid());
    l2log(daemon_log, LOG_SUCCESS);
  } else if (pid == -1) {
    error("Failed fork daemon");
  }else {
    l2log("Daemod forked", LOG_SUCCESS);
    exit(EXIT_SUCCESS);
  }

  if ((globalArgs.log_file = fopen(globalArgs.log_path, "a")) != NULL) {
    fclose(globalArgs.log_file);
    char req_str[256]; // TODO: dynamic allocation
    sprintf(req_str, "Server started with log file: %s", globalArgs.log_path);
    l2log(req_str, LOG_SUCCESS);
  } else {
    fprintf(stderr, "FAILED TO OPEN LOG FILE: %s\n", globalArgs.log_path);
    exit(EXIT_FAILURE);
  };

  //Create socket
  listenfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (listenfd < 0) {
    error("Can not open socket. Server sopped.\n");
    exit(EXIT_FAILURE);
  }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port = htons(globalArgs.portno);

   //Bind socket
  int test = bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

  int counter;
  short is_SNILS;

  pid_t val_pid;

  while(1) {
    memset(buffer, 0, BUFFER_LENGTH);
    int count = recvfrom(listenfd, buffer, BUFFER_LENGTH - 1, 0, &from, &struct_len);
    counter = 0;
    is_SNILS = 1;

    val_pid = fork();
    if (val_pid == -1) {
      error("Failed to fork validating process");
    } else if (val_pid != 0) { continue; }

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

    char *msg_str;

    sleep(globalArgs.wait_time);

    if (is_SNILS == 0) {
      int test = sendto(listenfd, "FAILED\n", 7, 0, &from, struct_len);
      asprintf(&msg_str, "Accepted request with message: %sResponded: FAILED", buffer);
      l2log(msg_str, LOG_REQUEST);
    } else {
      int test = sendto(listenfd, "OK\n", 3, 0, &from, struct_len);
      asprintf(&msg_str, "Accepted request with message: %sResponded: OK", buffer);
      l2log(msg_str, LOG_REQUEST);
    }

    if (val_pid == 0) exit(EXIT_SUCCESS);
 }
}
