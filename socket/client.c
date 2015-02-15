/****************************************************************
 * client.c: client test for pocketsphinx
 * Author @ Zhang Chunmeng
 * Last Modified @ 05/02/15
 ***************************************************************/

#include "headsock.h"

void str_cli(FILE *fp, int sockfd);        //used for socket transmission             

int main(int argc, char **argv)
{
	int sockfd, ret;
	struct sockaddr_in ser_addr;
	char ** pptr;
	struct hostent *sh;
	struct in_addr **addrs;
    char * filename = "test.wav";

	if (argc != 2) {
		printf("parameters not match");
        exit(1);
	}

	sh = gethostbyname(argv[1]);	                            //get host's information from the input argument
	if (sh == NULL) {
		printf("error when gethostby name");
		exit(1);
	}

	printf("canonical name: %s\n", sh->h_name);
	for (pptr=sh->h_aliases; *pptr != NULL; pptr++)
		printf("the aliases name is: %s\n", *pptr);
	switch(sh->h_addrtype)
	{
		case AF_INET:
			printf("AF_INET\n");
		break;
		default:
			printf("unknown addrtype\n");
		break;
	}
        
	addrs = (struct in_addr **)sh->h_addr_list;                       //get the server(receiver)'s ip address
	sockfd = socket(AF_INET, SOCK_STREAM, 0);                           //create the socket
	if (sockfd <0)
	{
		printf("error in socket");
		exit(1);
	}
	ser_addr.sin_family = AF_INET;                                                      
	ser_addr.sin_port = htons(MYTCP_PORT);
	memcpy(&(ser_addr.sin_addr.s_addr), *addrs, sizeof(struct in_addr));	
	bzero(&(ser_addr.sin_zero), 8);
	ret = connect(sockfd, (struct sockaddr *)&ser_addr, sizeof(struct sockaddr));         //connect the socket with the server(receiver)
	if (ret != 0) {
		printf ("connection failed\n"); 
		close(sockfd); 
		exit(1);
	}

    FILE *fp = fopen(filename, "rb");
	
	str_cli(fp, sockfd);                       //perform the transmission

    fclose(fp);
	close(sockfd);
	exit(0);
}

void str_cli(FILE *fp, int sockfd)
{
    long lsize;
    long lseek = 0;
    long slen;
    int n;
    char hyp[HYPLEN];
    char sends[DATALEN];
	fseek (fp , 0 , SEEK_END);
	lsize = ftell (fp);
	rewind (fp);
    char *buf;

	printf("The file length is %d bytes\n", (int)lsize);

    // allocate memory to contain the whole file.
	buf = (char *) malloc (lsize);
	if (buf == NULL)
        exit (2);

    // copy the file into the buffer.
	fread (buf, 1, lsize, fp);

    uint32_t un = htonl(lsize);
    send(sockfd, &un, sizeof(uint32_t), 0);

    while (lseek < lsize)
    {
        if (lsize - lseek < DATALEN)
            slen = lsize - lseek;
        else
            slen = DATALEN;
        memcpy(sends, buf+lseek, slen);
        if ((n = send(sockfd, sends, slen, 0)) == -1)
        {
            printf("Error in sending packets!\n");
            exit(1);
        }

        lseek += slen;

        if ((n = recv(sockfd, hyp, HYPLEN, 0)) == -1)
        {
            printf("Error in sending packets!\n");
            exit(1);
        }

        hyp[n] = '\0';
        printf("%s\n", hyp);
    }
}
