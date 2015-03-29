/************************************************************************
 * server.c: pocketsphinx server program
 * using socket communication with C/Matlab
 * Author @ Zhang Chunmeng
 * Last Modified @ 11/03/15
 * *********************************************************************/

#include "headsock.h"
#include<pocketsphinx.h>

void str_ser(int sockfd, ps_decoder_t *ps); // transmitting and receiving function

int main(void)
{
    ps_decoder_t *ps;
    cmd_ln_t *config;

    config = cmd_ln_init(NULL, ps_args(), TRUE,
            "-hmm", MODELDIR "/hmm/voxforge-en-0.1.4",
            //"-hmm", MODELDIR "/hmm/cmusphinx-en-us-ptm-5.2",
            "-lm", MODELDIR "/lm/lm_giga_64k_vp_3gram.arpa.DMP",
            //"-lm", MODELDIR "/lm/generic.lm",
            "-dict", MODELDIR "/lm/cmu07a.dic",
            "-logfn", "/dev/null",
//            "-lw", "7.0",
//            "-topn", "16",
//            "-fillprob", "1e-6",
//            "-silprob", "0.1",
//            "-wip", "0.5",
//            "-compallsen", "yes",
//            "-beam", "1e-50",
            NULL);
    if (config == NULL)
    {
        printf("PocketSphinx Config Error!\n");
        exit(1);
    }
    ps = ps_init(config);
    if (ps == NULL)
    {
        printf("Error in ps_init!\n");
        exit(1);
    }

	int sockfd, con_fd, ret;
	struct sockaddr_in my_addr;
	struct sockaddr_in their_addr;
	int sin_size;

	pid_t pid;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);          //create socket
	if (sockfd <0)
	{
		printf("error in socket!");
		exit(1);
	}
	
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(MYTCP_PORT);				//port number
	my_addr.sin_addr.s_addr = htonl(INADDR_ANY);        //any client ip address
	bzero(&(my_addr.sin_zero), 8);

	ret = bind(sockfd, (struct sockaddr *) &my_addr, sizeof(struct sockaddr)); //bind socket
	if (ret < 0)
	{
		printf("error in binding");
		exit(1);
	}
	
	ret = listen(sockfd, BACKLOG);                              //listen
	if (ret <0) {
		printf("error in listening");
		exit(1);
	}

    printf("Start receiving...\n");

	while (1)
	{
		sin_size = sizeof (struct sockaddr_in);
		con_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);            //accept the packet
		if (con_fd <0)
		{
			printf("Error in accept socket connection!\n");
			exit(1);
		}

		if ((pid = fork())==0) // creat acception process
		{
			close(sockfd);
			str_ser(con_fd, ps); //receive packet and response
			close(con_fd);
            ps_free(ps);
            printf("Accept and Decode finished!\n");
			exit(0);
		}
		else close(con_fd); //parent process
	}
    ps_free(ps);
	close(sockfd);
	exit(0);
}

void str_ser(int sockfd, ps_decoder_t *ps)
{
    char const *hyp;
    int rv;
    int32 score;
//    FILE *fp = fopen("received.wav", "wb");
    char recvs[DATALEN];
    char buf[MAXSIZE];
    int16 audioBuf[DATALEN/2];
    int n = 0;
    int count = 0;
    long filesize;
    long lseek = 0;
    size_t i;
    n = recv(sockfd, &filesize, 4, 0);
    filesize = ntohl(filesize);

    if (n == -1)
    {
        printf("Error in receiving file size!");
        exit(1);
    }
    rv = ps_start_utt(ps);
    if (rv < 0)
        exit(1);
    while (lseek < filesize)
    {

        if ((count = recv(sockfd, &recvs, DATALEN, 0))==-1)                                   //receive the packet
        {
            printf("receiving error!\n");
            exit(1);
        }
        // record recevived wav file
		memcpy((buf+lseek), recvs, count);
        lseek += count;

        for (i = 0; i < count; i += 2)
        {
            audioBuf[i/2] = (unsigned char)recvs[i] + ((int16)recvs[i+1] << 8);
        }

        rv = ps_process_raw(ps, audioBuf, count/2, FALSE, FALSE);
        hyp = ps_get_hyp(ps, &score);

        if (hyp == NULL)
        {
            if ((n = send(sockfd, "Wait!", 6, 0)) == -1)
            {
                printf("Error in sending hyp text!\n");
                exit(1);
            }
        }
        else
        {
            if ((n = send(sockfd, hyp, strlen(hyp), 0)) == -1)
            {
                printf("Error in sending hyp text!\n");
                exit(1);
            }
            //printf("Recognized===> %s\n", hyp);
        }

    }

    //rv = ps_process_raw(ps, buf, lseek/2, FALSE, FALSE);
    rv = ps_end_utt(ps);

    if (rv < 0)
        exit(1);

    //fwrite (buf, 1, lseek, fp);					//write data into file
    //fclose(fp);
    printf("File received! %li %li\n", lseek, filesize);
}
