#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define LSH_TOK_BUFFERSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"

char *lsh_read_line(void)
{
	char * line = NULL;
	ssize_t buffersize = 0;
	getline(&line, &buffersize, stdin);
	return line;
}

char **lsh_split_line(char *line)
{
	int buffersize = LSH_TOK_BUFFERSIZE, position = 0;
	char **tokens = malloc(buffersize * sizeof(char*));
	char *token;
	if(!tokens)
	{
		fprintf(stderr, "lsh: allocation error\n");
		exit(EXIT_FAILURE);
	}
	token = strtok(line, LSH_TOK_DELIM);
	while(token != NULL)
	{
		tokens[position] = token;
		position++;
		if(position >= buffersize)
		{
			buffersize +=LSH_TOK_BUFFERSIZE;
			tokens = realloc(tokens, buffersize * sizeof(char*));
			if(!tokens)
			{
				fprintf(stderr, "lsh: allocation error\n");
				exit(EXIT_FAILURE);
			}
		}
		token = strtok(NULL, LSH_TOK_DELIM);
	}
	tokens[position] = NULL;
	return tokens;
}

void lsh_loop(void)
{
	char *line;
	char **args;
	int status;
	do
	{
		printf(": ");
		line = lsh_read_line();
		args = lsh_split_line(line);
		status = lsh_execute(args);
		free(line);
		free(args);
	}
	while(status);
}

int lsh_launch(char **args)
{
	pid_t pid, wpid;
	int status;
	pid = fork();
	if(pid == 0)
	{
		if(execvp(args[0], args) == -1)
		{
			perror("lsh");
		}
		exit(EXIT_FAILURE);
	}
	else if(pid < 0)
	{
		perror("lsh");
	}
	else
	{
		do
		{
			wpid = waitpid(pid, &status, WUNTRACED);
		}
		while(!WIFEXITED(status) && !WIFSIGNALED(status));
	}
	return 1;
}

/*
  Function Declarations for builtin shell commands:
 */
int lsh_cd(char **args);
int lsh_exit(char **args);

/*
  List of builtin commands, followed by their corresponding functions.
 */
char *builtin_str[] = {
  "cd",
  "exit"
};

int (*builtin_func[]) (char **) = {
  &lsh_cd,
  &lsh_exit
};

int lsh_num_builtins()
{
	return sizeof(builtin_str) / sizeof(char *);
}

/*
  Builtin function implementations.
*/
int lsh_cd(char **args)
{
	if (args[1] == NULL)
	{
		fprintf(stderr, "lsh: expected argument to \"cd\"\n");
	}
	else
	{
		if (chdir(args[1]) != 0)
		{
			perror("lsh");
		}
	}
	return 1;
}


int lsh_exit(char **args)
{
	return 0;
}

int lsh_execute(char **args)
{
	int i;

	if (args[0] == NULL)
	{
    // An empty command was entered.
	return 1;
	}
	for (i = 0; i < lsh_num_builtins(); i++)
	{
		if (strcmp(args[0], builtin_str[i]) == 0)
		{
			return (*builtin_func[i])(args);
		}
	}
	return lsh_launch(args);
}

int main(int argc, char **argv)
{
	lsh_loop();
	return EXIT_SUCCESS;
}