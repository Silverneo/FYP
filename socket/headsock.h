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
#define MAXSIZE 2000000
#define DATALEN 2048
#define HYPLEN 5000
#define BACKLOG 10
