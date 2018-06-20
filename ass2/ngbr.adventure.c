#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <pthread.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

/* Global Variables */
/* Room Names */
char* roomNames[10] = {"Market", "Bakery", "Plaza", "Library", "Armory", "Crypt", "Inn", "Dungeons", "Forest", "Temple"};
/* Room Types */
char* roomType[3] = {"start_room", "end_room", "mid_room"};
/* Mutex */
pthread_mutex_t mutex;
char newestDirName[256];

/* Room Struct */
struct Room
{
    char* name;
    char* type;
    int connections[6];
    int numConnections;  
};

void* getTime()
{
	FILE* myFile;
	/* Create And Write To Time File */
	myFile = fopen("currentTime.txt", "w"); 
	char buffer[100];         
	struct tm *sTm;
  
	time_t now = time (0);
	sTm = gmtime (&now);
  
	strftime (buffer, sizeof(buffer), "%I:%M%P, %A, %B %d, %Y", sTm);
	fputs(buffer, myFile);
	fclose(myFile);     
}

void writeTime()
{
	FILE* myFile;
	/* Read From Time File */
	myFile = fopen("currentTime.txt", "r");
	char buffer[100];
	/* Check If Time File Exists */
	/* If No, Return Error */
	/* If Yes, Print Time */
	if(myFile == NULL)
	{
		perror("Not found\n");
	}
	else
	{
		fgets(buffer, 100, myFile);
		printf("\n%s\n", buffer);
		fclose(myFile);
	}
}

void threading()
{
    pthread_t threaded;                            
    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_lock(&mutex);
    
    int tid = pthread_create(&threaded, NULL, getTime, NULL); 
    pthread_mutex_unlock(&mutex);
    pthread_mutex_destroy(&mutex);
    /* Pause */
    usleep(50);
}

/* Get Most Recently Created Folder */
/* Brewster's Code */
char* getFolder(char *newestDirName)
{
	int newestDirTime = -1;
	char targetDirPrefix[32] = "ngbr.rooms.";

	memset(newestDirName, '\0', sizeof(newestDirName));

	DIR* dirToCheck;
	struct dirent *fileInDir;
	struct stat dirAttributes;

	dirToCheck = opendir(".");

	if (dirToCheck > 0)
	{
		while ((fileInDir = readdir(dirToCheck)) != NULL)
		{
			if (strstr(fileInDir->d_name, targetDirPrefix) != NULL)
			{
				stat(fileInDir->d_name, &dirAttributes);

				if ((int)dirAttributes.st_mtime > newestDirTime)
				{
					newestDirTime = (int)dirAttributes.st_mtime;
					memset(newestDirName, '\0', sizeof(newestDirName));
					strcpy(newestDirName, fileInDir->d_name);
				}
			}
		}
	}
	closedir(dirToCheck);
	return newestDirName;
}

/* Create Starting Room */
const char* firstRoom()
{
	
	
	char* directory = getFolder(newestDirName);
	char* start = malloc(sizeof(char) * 20);
	memset(start, '\0', sizeof(char)*20);
	DIR* folder;
	struct dirent* find;
  
	if((folder = opendir(directory)) != NULL)
	{
		while((find = readdir(folder)) != NULL)
		{
			/* Find Correct Room Directory */
			/* If Incorrect Directory, Continue Searching */
			/* If Correct Directory, Get Starting Room Name */ 
			if(!strcmp(find -> d_name, "..") || !strcmp(find -> d_name, "."))
			{
				continue;
			}
			strcpy(start, find -> d_name);
			/* Return Room Name */
			return start;
			printf("%s", find -> d_name);
			break;	
		}
		/* Close Room Directory */
		closedir(folder);
		free(folder);
	}
	/* Return Error If Doesn't Exist */
	else
	{

		perror("");
		return (void *)EXIT_FAILURE;
	}
}

/* Get Room Info */
int getRooms(struct Room* room, char roomName[100])
{
	char folder[100];
	/* Get Room Directory Name */
	sprintf(folder, getFolder(newestDirName));
	char fileName[100];
	sprintf(fileName, "./%s/%s", folder, roomName);

	/* Create File Pointer */
	FILE* myFile;
	/* Read File */
	myFile = fopen(fileName, "r");
	/* If No Room Directory, Exit */
	if(myFile == NULL)
	{
		return -1;
	}
	/* Make First Room In roomNames */
	room -> name = roomNames[0];
	/* Set Connections to 0 */
	room -> numConnections = 0;
  
	/*Variable to read each line */
	char eachLine[100];
  
	fgets(eachLine, 100, myFile);
	/* Split Strings Into Tokens */
	char* name = strtok(eachLine, " ");
	name = strtok(NULL, " ");
	name = strtok(NULL, "\n");
  
	int i;
	/* Loop Through List Of Names, Set Equal To roomNames[i] Array */
	for(i = 0; i < 10; i++)
	{              
		if(strcmp(name, roomNames[i]) == 0)
		{
			room -> name = roomNames[i];
			break;
		}
	}
	/* While Reading File */
	while(fgets(eachLine, 100, myFile) != NULL)
	{
		/*Find Room Connections In Files */
		char* findConn = strtok(eachLine, " ");
		/*If findConn Finds "CONNECTION", Tokenize Results */
		if(strcmp(findConn, "CONNECTION") == 0)
		{
			findConn = strtok(NULL, " ");
			findConn = strtok(NULL, "\n");
			for(i = 0; i < 10; i++)
			{
				/*If Connections Found, Store Connection */
				if(strcmp(findConn, roomNames[i]) == 0)
				{
					room -> connections[room -> numConnections] = i;
					break;
				}
			}
			/* Increment numConnections */
			room -> numConnections++;
		}
		else
		{
			/* findConn Finds Room Type And Tokenizes It */
			findConn = strtok(NULL, " ");
			findConn = strtok(NULL, "\n");
			for(i = 0; i < 10; i++)
			{
				if(strcmp(findConn, roomType[i]) == 0)
				{
					room -> type = roomType[i];
					break;
				}
			}
		}
	}
	return 0;
}

/* Check If Connection Exists Between Rooms */
int checkConnection(struct Room* room, char roomName[50])
{
	int i;
	/* Check If Valid Connection */
	for(i = 0; i < room -> numConnections; i++)
	{
		if(strcmp(roomName, roomNames[room -> connections[i]]) == 0)
		{
			return 1;
		}
	}
	return 0;
}

void playGame(char start[50])
{
	/* Variables */
	char *newestDirName[256];
	/* Step Counter */
	int steps = 0;
	/* Check For Ending */
	int ending = 0;
	int iter;
	char roomName[100];
	char* roomPath[500];
	struct Room* room = malloc(sizeof(struct Room));
	iter = getRooms(room, start);
	do
	{
		printf("\nCURRENT LOCATION: %s\n", room -> name);
		printf("POSSIBLE CONNECTIONS:");
		printf(" %s, ", roomNames[room -> connections[0]]);
		/* Print Out Room Connections */
		for(iter = 1; iter < room -> numConnections; iter++)
		{
			if(iter != 1)
			{
				printf(", ");
			}
			printf("%s", roomNames[room -> connections[iter]]);
		}
		printf(". \nWHERE TO? >");
		/* Get User Input */
		scanf("%s", roomName);
		if(strcmp(roomName, "time") == 0)
		{
			threading();
			writeTime();
		}
		/* If Invalid Room Name */
		/* Print Error */
		else if(checkConnection(room, roomName) == 0)
		{ 
			printf("\nHUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n");
		}
		/* Else, Add Step */
		else
		{  
			roomPath[steps] = room -> name;
			steps++;
			iter = getRooms(room, roomName);
		}
		/* If Player Gets To endRoom, End Game */
		/* printf("room type: %s\n", room -> type); */
		if(room -> type == "end_room")
		{
		/*	printf("Found end_room\n"); */
			ending = 1;
		}
		/* Ending */
		if(ending == 1)
		{
			int i;
			printf("\n");
			printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
			printf("YOU TOOK %i STEPS. YOUR PATH TO VICTORY WAS:\n", steps);
			/* Loop And Print Each Room In Path */
			for(i = 1; i < steps; i++)
			{
				printf("%s\n", roomPath[i]);
			}
			printf("%s\n", room -> name);
			free(room);
		}
	}
	while(ending == 0);
}

int main()
{
	/* Create Variable */
	char beginningPlace[50];
	/* Copy First Room Name into Variable */
	strcpy(beginningPlace, firstRoom());
	/* Play Game */
	playGame(beginningPlace);   
	return 0;
}