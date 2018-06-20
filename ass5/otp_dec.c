#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

//Error Message Function
void error(const char *msg)
{
    perror(msg);
    exit(0);
}

//File Length Function
//Returns Length of Input File
long get_file_length(char *filename)
{
	//Variables
	FILE *file = fopen(filename, "r");
    fpos_t position;
    long length;
	
	//Save Previous Position in File
    fgetpos( file, &position );
	//Seek to End 
    if ( fseek( file, 0, SEEK_END ) || ( length = ftell( file ) ) == -1 )
	{
        perror( "Finding file length" );
    }
	//Restore Position
    fsetpos( file, &position );
    return length;
}

//Send File Function
//Reads and Writes File
void sendFile(char *filename, int sockfd, int filelength)
{
	//Open File
	int fd = open(filename, 'r');
	
	//Variables
    char buffer[12800];
    bzero(buffer, sizeof(buffer));
    int bytes_read, bytes_written;
	char *p;
	
	//Read File
    while (filelength > 0)
	{
		bytes_read = read(fd, buffer, sizeof(buffer));
		//If Done Reading, Break
		if (bytes_read == 0)
		{
			break;
		}
		//If Read Fails, Output Error
		if (bytes_read < 0)
		{
			perror("Client: ERROR reading file");
			exit(1);
		}
		filelength -= bytes_read;
	}
	//p Keeps Track Of Buffer
	p = buffer;
	//Write File
	while (bytes_read > 0)
	{
		//If Write Fails, Output Error
		bytes_written = write(sockfd, p, bytes_read);
		if (bytes_written < 0)
		{
			perror("Client: ERROR writing to socket");
			exit(1);
		}
		bytes_read -= bytes_written;
		p += bytes_written;
	}
	return;
}

//Recieve File Function
void receiveFile(int sockfd)
{
	int n;
    char buffer[12800];
    bzero(buffer,12800);
	
	//Read File
    while (1)
	{
		int bytes_read;
		bytes_read = read(sockfd, buffer, sizeof(buffer));
		//If Done Reading, Break
		if (bytes_read == 0)
		{
			break;
		}
		//If Read Fails, Output Error
		if (bytes_read < 0)
		{
			error("ERROR reading file");
		}
		//Write to Stdout
		n = write(1, buffer, bytes_read);
		//If Write Fails, Output Error
		if (n < 0)
		{
			error("ERROR writing to socket");
		} 
	}
}

int main(int argc, char *argv[])
{
	//Variables
	int clientsockfd, portno, n, optval, plaintextfd;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	FILE *fp;
	const char hostname[] = "localhost";
	char buffer[12800];
	bzero(buffer,12800);
	
	//Argument Validation
    if (argc != 4)
	{
		fprintf(stderr,"usage %s <inputfile> <key> <port>\n", argv[0]);
		exit(1);
    }
	//Get Port Number
    portno = atoi(argv[3]);
	//Create Client Socket
    clientsockfd = socket(AF_INET, SOCK_STREAM, 0);
	//If Socket Fails, Output Error	
    if (clientsockfd < 0)
	{
		perror("ERROR opening socket");
		exit(1);
    }
	//Get Server Name
	server = gethostbyname(hostname);
	//If No Host, Obvious is Obvious, OUtput Error
	if (server == NULL)
	{
		perror("ERROR, no such host");
		exit(1);
	}
	optval = 1;
	setsockopt(clientsockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));
	//Zero Address
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(portno);
	//Connect to Server
	//If Connect Fails, Output Error and Exit
	if (connect(clientsockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
	{
		perror("ERROR connecting");
		exit(1);
    }
	char auth[]="dec_bs";
	write(clientsockfd, auth, sizeof(auth));
	read(clientsockfd, buffer, sizeof(buffer));
	//Check Buffer
	//If Fail, Print Connection Error and Exit
	if (strcmp(buffer, "dec_d_bs") != 0)
	{
		fprintf(stderr, "unable to contact otp_enc_d on given port\n");
		exit(2);
	}
	//check that key is at least as long as message
	long infilelength = get_file_length(argv[1]);
	long keylength = get_file_length(argv[2]);    
	if (infilelength > keylength)
	{
		fprintf(stderr, "key is too short\n");
		exit(1); 
	}
	//Send plaintext
	sendFile(argv[1], clientsockfd, infilelength);
	//Send Key
	sendFile(argv[2], clientsockfd, keylength);
	//Read Server Response
	n = read(clientsockfd,buffer,sizeof(buffer));
	//If Read Fails, Output Error and Exit
	if (n < 0)
	{
		perror("ERROR reading from socket");
        exit(1);
    }
    printf("%s\n",buffer);
    close(clientsockfd);
    return 0;
}