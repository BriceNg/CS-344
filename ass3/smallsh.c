#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

/*
void catchSIGINT(int signal);
void showStatus(int status);
void catchSigStop(int signal);
*/

//Shows Status
//Both Exit Value or Terminating Signal
void showStatus(int status)
{
	if(WIFEXITED(status))
	{
		printf("exit value %i\n", WEXITSTATUS(status));
	} 
	else
	{
		printf("terminated by signal %i\n", status);
	}
}

//Catch and Display Messages for Signals
void catchSIGINT(int signo)
{
	//Display Message for SIGINT
	char* message = "\nCaught SIGINT.\n";
	write(STDOUT_FILENO, message, 38);
}

//Catch and Display Messages for Signals
void catchSigStop(int signal)
{
	//Display Message for SIGSTP
	char* message = "\nCaught SIGTSTP.\n";
	write(STDOUT_FILENO, message, 25);
	exit(0);
}

int main()
{
	//Variables
    int fileIn = -1;
    int fileOut = -1;
    int fgProcess;
    int status = 0;
    int cpid;
    char userInput[2048];
    char* arg[512];
    char* token;
    char* inFile = NULL;
    char* outFile = NULL;
    struct sigaction action = {0};
    struct sigaction action2 = {0};
	
	//Ctrl+z and Ctrl+c Handlers
    action.sa_handler = SIG_IGN;
    action.sa_flags = 0;
    sigfillset(&(action.sa_mask));
    sigaction(SIGINT, &action, NULL);
    action2.sa_handler = catchSigStop;
    action2.sa_flags = 0;
    sigfillset(&(action2.sa_mask));
    sigaction(SIGTSTP, &action2, NULL);
    
	//Run Shell
    while(1)
	{
        fgProcess = 1;
		//Shell Prompt
        printf(": ");
		//Flush Output Buffers
        fflush(stdout);
        //If User Entered Nothing
        if(fgets(userInput, 512, stdin) == NULL)
		{
            return 0;
        }
		//Parse the user input for instructions
        int count = 0;
        token = strtok(userInput, " \n");
        while(token != NULL)
		{
			if(strstr(token, "$$") != NULL)
			{
            char* dollar = strstr(token, "$$");
            int indexOfLoc = dollar - token;
            token[indexOfLoc] = '%';
            token[indexOfLoc + 1] = 'd';
            sprintf(token, token, getpid());
            //printf("token: %s \n", token);
			}
			//If Input File
			//Parse and store
            if(strcmp(token, "<") == 0)
			{
                token = strtok(NULL, " \n");
                inFile = strdup(token);
                token = strtok(NULL, " \n");
            }
			//If Output File
			//Parse and store
            else if(strcmp(token, ">") == 0)
			{
                token = strtok(NULL, " \n");
                outFile = strdup(token);
                token = strtok(NULL, " \n");
            }
			//If Ampersand
			//Background process
			//End Foreground and Enter Background
            else if(strcmp(token, "&") == 0)
			{
                fgProcess = 0;
                break;
            }			
			//Dup Token
            else
			{
                arg[count] = strdup(token);
                token = strtok(NULL, " \n");
				//Increment Counter
                ++count;
            }
        }
        arg[count] = NULL;
		//If No Input or # Command, Do Nothing
        if(arg[0] == NULL || *(arg[0]) == '#')
		{
            continue;
        }
		//If cd Command
        else if(strcmp(arg[0], "cd") == 0)
		{
			//If only cd, get Home Directory
            if(arg[1] == NULL)
			{
                chdir(getenv("HOME"));
            }
			//Else, Use cd As Usual
            else
			{
                chdir(arg[1]);
            }
        }
		//If Status Command
		//Print Status
        else if(strcmp(arg[0], "status") == 0)
		{
            showStatus(status);
        }
		//If Quit Command
		//Quit
        else if (strcmp(arg[0], "exit") == 0)
		{
            exit(0);
        }
		//Fork New Process
        else
		{
			
            cpid = fork();
            switch(cpid)
			{
            case 0:
				//Interrupt Foreground Process
                if(fgProcess)
				{
                    action.sa_handler = SIG_DFL;
                    sigaction(SIGINT, &action, NULL);
                }
				//Unable To Open inFile
                if(inFile != NULL)
				{
                    fileIn = open(inFile, O_RDONLY);
                    if(fileIn == -1)
					{
						//Print Error
                        printf("smallsh: cannot open %s for input\n", inFile);
						//Flush stdout
                        fflush(stdout);
                        _Exit(1);
                    }
                    if(dup2(fileIn, 0) == -1)
					{
						//Error In Dup2
                        perror("dup2");
                        _Exit(1);
                    }
                    close(fileIn);
                }
				//Background Process
                else if(!fgProcess)
				{
					//Redirect To /dev/null
                    fileIn = open("/dev/null", O_RDONLY);       
                    if(fileIn == -1)
					{
                        perror("open");
                        _Exit(1);
                    }
                    if(dup2(fileIn, 0) == -1)
					{
                        perror("dup2");
                        _Exit(1);
                    }
                }
				//Unable To Open outFile
                if(outFile != NULL)
				{
                    fileOut = open(outFile, O_WRONLY | O_CREAT | O_TRUNC, 0744);
                    if(fileOut == -1)
					{
						//Print Error
                        printf("smallsh: cannot open %s\n", outFile);
                        fflush(stdout);
                        _Exit(1);
                    }
                    if(dup2(fileOut, 1) == -1)
					{
						//Error In Dup2
                        perror("dup2");
                        _Exit(1);
                    }
                    close(fileOut);
                }
				//If Command Not Recognized
				//Output Error and Exit Status
                if(execvp(arg[0], arg))
				{
                    printf("smallsh: Command \"%s\" is not a valid command\n", arg[0]);
                    fflush(stdout);
                    _Exit(1);
                }
                break;
            //Fork Failed
            case -1:    
                perror("fork");
                status = 1;
                break;
            //Wait For Foreground By Default
            default:
                if (fgProcess)
				{
                    waitpid(cpid, &status, 0);
                }
                else
				{
                    printf("Background PID: %i\n", cpid);
                    break;
                }
            }
		//Free Argument Array
        }
        int i;
        for(i = 0; arg[i] != NULL; i++)
		{
            free(arg[i]);
        }
        //Free Input/Output Files
        free(inFile);
        inFile = NULL;
        free(outFile);
        outFile = NULL;
        //Check Completed Processes
        cpid = waitpid(-1, &status, WNOHANG);
        while(cpid > 0)
		{
            printf("background process, %i, is done: ", cpid);
            showStatus(status);
            cpid = waitpid(-1, &status, WNOHANG);
        }
    }

    return 0;
}