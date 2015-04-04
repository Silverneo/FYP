/************************************************************************
 * serverd.c: pocketsphinx server daemon program
 * using socket communication with C/Matlab
 * Port the original version into daemon
 * Author @ Zhang Chunmeng
 * Last Modified @ 04/04/15
 * TODO: - add log file function
 *       - modify the socket initialization process
 *       - multi-request process
 * *********************************************************************/

#include "headsock.h"
#include<pocketsphinx.h>

#define MODELDIR "~/Projects/FYP/model"
void ps_start_recog(int sockfd, ps_decoder_t *ps); // transmitting and receiving function

int main(void)
{
	int sockfd, con_fd, ret;
	struct sockaddr_in my_addr;
	struct sockaddr_in client_addr;
	socklen_t sin_size;
    
    ps_decoder_t *ps;
    cmd_ln_t *config;

	pid_t pid, sid;
    int status;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);          //create socket
	if (sockfd < 0)
	{
		//printf("error in socket!\n");
		exit(1);
	}
	
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(MYTCP_PORT);				//port number
	my_addr.sin_addr.s_addr = htonl(INADDR_ANY);        //any client ip address
	bzero(&(my_addr.sin_zero), 8);

	ret = bind(sockfd, (struct sockaddr *) &my_addr, sizeof(struct sockaddr)); //bind socket
	if (ret < 0)
	{
		//printf("error in binding\n");
		exit(1);
	}

    config = cmd_ln_init(NULL, ps_args(), TRUE,
            "-hmm", "/home/chunmeng/Projects/FYP/model/hmm/en-us-ptm-5.2-adapt",
            "-lm", "/home/chunmeng/Projects/FYP/model/lm/lm_giga_64k_vp_3gram.arpa.DMP",
            "-dict", "/home/chunmeng/Projects/FYP/model/lm/cmu07a.dic",
            "-logfn", "/dev/null",
            "-cmn", "current",
            "-lw", "4",
            "-frate", "85",
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
    if ((ps = ps_init(config)) == NULL) // init decoder
    {
        cmd_ln_free_r(config);
        printf("Error in ps_init!\n");
        exit(1);
    }

    pid = fork(); // fork to start the daemon process

    if (pid < 0)
    {
        printf("Error in fork!\n");
        exit(1);
    }
    else if (pid > 0)
    {
        printf("Starting daemon process: %d\n", pid);
        
        close(sockfd);
        cmd_ln_free_r(config);
        ps_free(ps);

        exit(0);
    }

    if ((sid = setsid()) < 0)
    {
        exit(1);
    }

    if((chdir("/")) < 0)
    {
        exit(1);
    }

    // close stdin, stdout, stderr
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);


	if ((ret = listen(sockfd, BACKLOG)) < 0) { // listen to the socket
		//TODO printf("error in listening");
		exit(1);
	}

	while (1)
	{
		sin_size = sizeof (struct sockaddr_in);
		con_fd = accept(sockfd, (struct sockaddr *)&client_addr, &sin_size);            //accept the packet
		if (con_fd < 0)
		{
            //TODO LOG INFO
			exit(1);
		}

		if ((pid = fork()) == 0) // creat acception process
		{
			close(sockfd);
			ps_start_recog(con_fd, ps); //receive packet and response
			close(con_fd);
            ps_free(ps);
			exit(0);
		}
		else
        {
            waitpid(pid, &status, 0);
            close(con_fd); //parent process
        }
	}
}

void ps_start_recog(int sockfd, ps_decoder_t *ps)
{
    char const *hyp;
    int rv;
    int32 score;
    char recvs[DATALEN];
    char buf[MAXSIZE];
    int16 audioBuf[DATALEN/2];
    int n = 0;
    int count = 0;
    long filesize;
    long lseek = 0;
    size_t i;

    n = recv(sockfd, &filesize, 4, 0);
    //filesize = ntohl(filesize);

    if (n == -1)
    {
        //printf("Error in receiving file size!");
        exit(1);
    }
    rv = ps_start_utt(ps);
    if (rv < 0)
        exit(1);
    while (lseek < filesize)
    {

        if ((count = recv(sockfd, &recvs, DATALEN, 0))==-1)                                   //receive the packet
        {
            //printf("receiving error!\n");
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
            if ((n = send(sockfd, "...", 4, 0)) == -1)
            {
                //printf("Error in sending hyp text!\n");
                exit(1);
            }
        }
        else
        {
            if ((n = send(sockfd, hyp, strlen(hyp), 0)) == -1)
            {
                //printf("Error in sending hyp text!\n");
                exit(1);
            }
            //printf("Recognized===> %s\n", hyp);
        }

    }

    //rv = ps_process_raw(ps, buf, lseek/2, FALSE, FALSE);
    rv = ps_end_utt(ps);
    if (rv < 0) exit(1);

}
