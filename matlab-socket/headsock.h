#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <math.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>

#define MYTCP_PORT 4950
#define MYUDP_PORT 5350
#define MAXSIZE 1000000
#define DATALEN 1024
#define HYPLEN 500
#define BACKLOG 10
