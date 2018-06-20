#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char * argv[])
{
	//Argument Checking
	if(argc != 2)
	{
		fprintf(stderr, "Incorrect number of arguments");
		exit(1);
	}
	//Get Key Length
	int length = atoi(argv[1]);
	// Seed Random
	srand(time(0));
	//Variables
	int i = 0;
	int key;
	//Generate Keys
	//Get Random Number
	//If Number is 0-25, Exchange it with A-Z
	//If Number is 26, Exchange it For a Space
	for(i; i < length; i++)
	{
		key = rand() % 27;
		if(key != 26)
		{
			printf("%c", 'A'+(char)key);
		}
		else
		{
			printf(" ");
		}
	}
	//Print Newline
	printf("\n");
	return 0;
}