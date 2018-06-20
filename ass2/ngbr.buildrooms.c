 #include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>

/* Global Variables */
/*Character Array For Room Directory */
char fileName[100];
/* Room Names */
char* roomNames[10] = {"Market", "Bakery", "Plaza", "Library", "Armory", "Crypt", "Inn", "Dungeons", "Forest", "Temple"};
/* Room Types */
char* roomType[3] = {"start_room", "end_room", "mid_room"};

/* Room Struct */
struct Room
{
    char* name;
    char* type;
    int connections[6];
    int numConnections;    
};

/* Creates Directory To Put Rooms In, With Correct Format. Returns Name of Directory. */
char* makeDir()
{
	/* Creates String With Correct Format */
	sprintf(fileName, "ngbr.rooms.%d", getpid());
	/* Makes Directory With String And Correct Permissions */
	mkdir(fileName, 0700);
	/* Return fileName */
	return fileName; 
}

/* Create Rooms */
void makeRooms(struct Room rooms[7])
{
	int i;
	for(i = 0; i < 7; i++)
	{
		/* Set All Room Connections To 0 */
		rooms[i].numConnections = 0;
	}

	int x;
	for(x = 0; x < 7; x++)
	{
		int loop, rand_num, y;
		loop = 1;
		while(loop)
		{
			/* Pick A Number Bewteen 1-10 */
			rand_num = rand() % 10;
			loop = 0;
			/* If Name Is Already Used, Loop Again */
			for (y = 0; y < 7; y++)
			{
				if(rooms[y].name == roomNames[rand_num])
				{
					loop = 1;
				}
			}
		}
		rooms[x].name = roomNames[rand_num];  
		/* Make First Room Always Starting Room */
		if(x == 0)
		{
			rooms[x].type = roomType[0];
		}
		/* Make Second Room Always Ending Room */
		else if(x == 1)
		{
			rooms[x].type = roomType[1];
		}
		/* Make Every Other Room A Mid Room */
		else
		{
			rooms[x].type = roomType[2];
		}
		/* Random Number of Connections */
		int totalConnections = rand() % 4 + 3;
		/* If The Room Can Still Have Connections, Add a Connection If Needed */
		while(rooms[x].numConnections < totalConnections)
		{
			makeConnection(x, rooms);
		}
	}
}

/* Make Connections Between Rooms */
void makeConnection(int room, struct Room rooms[7])
{
	int randConnect, loop;
	loop = 1;

	while(loop)
	{
		loop = 0;
		randConnect = rand() % 7;
		/* If Room Picks Itself, Loop Again */
		if(randConnect == room)
		{
			loop = 1;
		}
		int i;
		/* If Room Picks Existing Connection, Loop Again */
		for(i = 0; i < rooms[room].numConnections; i++)
		{
			if(rooms[room].connections[i] == randConnect)
			{
			loop = 1;
			}
		}
	}
	/*Set Number Of Connections */
	rooms[room].connections[rooms[room].numConnections] = randConnect;          
	rooms[room].numConnections++;
	/*Increase Number Of Connections In Room */
	rooms[randConnect].connections[rooms[randConnect].numConnections] = room;  
	rooms[randConnect].numConnections++;
}

/* Swaps Room Names Around */
void swap(int temp[])
{
	int i;
	for(i = 0; i < 10; i++)
	{ 
		int j;
		j = i + (rand() % 9);
		int place = temp[j];
		temp[j] = temp[i];
		temp[i] = place;
	}
}

void writeFiles(struct Room rooms[7], char* filename)
{
	/* Get Rooms Directory */
	chdir(filename);
	int i;
	for(i = 0; i < 7; i++)
	{
		/* Make Room File in Room Directory */
		FILE* myFile = fopen(rooms[i].name, "a"); 
		/* Print Each Room Name */
		fprintf(myFile, "ROOM NAME: %s\n", rooms[i].name);
		int j;
		for(j = 0; j < rooms[i].numConnections; j++)
		{
			/* Print Connections For Each Room */
			fprintf(myFile, "CONNECTION %d: %s\n", j + 1, rooms[rooms[i].connections[j]].name);
		}
		/*Print Type For Each Room */
		fprintf(myFile, "ROOM TYPE: %s", rooms[i].type);
		/* Close File */
		fclose(myFile);
	}
}

int main()
{
	/* Random Time */
	srand(time(NULL));
	/* Call makeDir */
	char* roomsDir = makeDir();
	/* Initialize 7 Rooms */
	struct Room rooms[7];
	/* Make Rooms */
	makeRooms(rooms);
	/* Write Room Data to Room Files */
	writeFiles(rooms, roomsDir);
	return 0;  
}