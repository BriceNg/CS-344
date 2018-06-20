#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

//Error Message Function
void error(const char *msg)
{
	//Print Error
    perror(msg);
    exit(1);
}

//Char to Int Function
int charToInt (char c)
{
	if (c == ' ')
	{
		return 26;
	}
	else
	{
		return (c - 'A');
	}
	return 0;
}

//Int to Char Function
char intToChar(int i)
{
	if (i == 26)
	{
		return ' ';
	}
	else
	{
		return (i + 'A');
	}
}

//Encrypting Function
void encrypt(char message[], char key[])
{
	//Variables
	int i;
	char n;
	//Read Through Message
	//Convert to Integer, Do Encryption Operation
	//Convert Back to Character
	for (i=0; message[i] != '\n' ; i++)
	{
		char c = message[i];
		n = (charToInt(message[i]) + charToInt(key[i])) % 27;
		message[i] = intToChar(n);
	}
	//Set Last Spot to Null for Consistency
	message[i] = '\0';
	return;
}

int main(int argc, char *argv[])
{
	//Variables
    int sockfd, newsockfd, portno, optval, n, status;
    socklen_t clilen;
    char buffer[12800];
    char key[12800];
    struct sockaddr_in serv_addr, cli_addr;
    pid_t pid, sid;
	
	//Argument Validation
	if (argc != 2)
	{
        fprintf(stderr,"usage %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }  
    //Create Socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
	//If Socket Fails, Output Error	
    if (sockfd < 0)
	{
		error("Server: ERROR opening socket");
	}
    optval = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));
    //Zero Address
    bzero((char *) &serv_addr, sizeof(serv_addr));
	//Get Port Number
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    //Bind Address to Socket
	//If Bind Fails, Output Error
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) //bind address to socket
	{
		error("Encrypt Server: ERROR on binding");
	}
	//Wait for Client
    listen(sockfd,5);
	while(1)
	{
		//Loop to Handle 1-5 Connections 
		clilen = sizeof(cli_addr); 
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		//If accept Fails, Output Error
		if (newsockfd < 0)
		{ 
			error("Server: ERROR on accept");
		}
		//Fork (Same as Assignment 3 basically)
		pid = fork();
		if (pid < 0)
		{
			error("Server: ERROR forking process");
			exit(EXIT_FAILURE);
		}
		if (pid == 0)
		{
			//Child Process
		  	bzero(buffer, sizeof(buffer));
		  	int bytes_remaining = sizeof(buffer);
		  	int bytes_read = 0;
		  	char *p = buffer; //keep track of where in buffer we are
		  	char *keyStart;
		  	int numNewLines = 0;
		  	int i;
		  	 
		  	//Receive Message and Reply
			read(newsockfd, buffer, sizeof(buffer)-1);
			//If Read Fails, Output Error
			if (strcmp(buffer, "enc_bs") != 0)
			{
				char response[]  = "invalid";
				write(newsockfd, response, sizeof(response));
				_Exit(2);
			}
			//Else, Write Back to Client
			else
			{
				char response[] = "enc_d_bs";
				write(newsockfd, response, sizeof(response));
			}
			//Zero Buffer
			bzero(buffer, sizeof(buffer));
			while (1)
			{
				bytes_read = read(newsockfd, p, bytes_remaining);
				//If Done Reading, Do Nothing
				if (bytes_read == 0)
				{ 
					break;
				}
				//If Read Fails, Output Error
				if (bytes_read < 0)
				{
					error("Server: ERROR reading from socket");
				}
				//Search For Newlines in buffer
				//If Newline, Increment Count
					//Set Start of Key
				for (i=0; i < bytes_read ; i++)
				{
					if(p[i] == '\n')
					{
						numNewLines++;
						if (numNewLines == 1)
						{
					 	 	keyStart = p+i+1;
					 	}
					}
				}
				//If Second Newline, Reached End
				if (numNewLines == 2)
				{
					break;			 
				}
				p += bytes_read;
				bytes_remaining -= bytes_read;
	 		} 
	 		char message[12800];
	 		bzero(message, sizeof(message));
	 		strncpy(message, buffer, keyStart-buffer);
			//Encrypt Message
	 		encrypt(message, keyStart);
			//Write Message 
	 		write(newsockfd, message, sizeof(message));
		}
		//Close New Socket
		close(newsockfd);
		//Parent process
		while (pid > 0)
		{
			pid = waitpid(-1, &status, WNOHANG);
		}
	}
	//Close Socket
    close(sockfd);
    return 0; 
}