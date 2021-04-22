/* Ahmad Rasul
  COP 5570 Project 1
  02/26/2021 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <readline/readline.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <time.h>
#include <sys/time.h>

// functions declarations
int parsePipe(char *, char **);
void parseSpace(char *, char **);
void execCmd(char **);
void pipeline(char **);
int mycd(char *);
char * mypwd();
void mytree(char [], int);

int main()
{
  char * input; // takes input from user
  char * command[80]; // support up to 80 characters
  char * pipeCommands[10]; // support up to 10 pipes

  // loop that runs toolkit
  while (1)
  {
    // read and parse command from user
    input = readline("$ ");

    // check for pipes
    int numPipes = parsePipe(input, pipeCommands);

    // if there are pipes in input
    if (numPipes > 1)
    {
      pipeline(pipeCommands);
      continue;
    }

    // if single command
    else
    {
      // parse command
      parseSpace(pipeCommands[0], command);

      // if mytime command is called
      if (strcmp(command[0], "mytime") == 0)
      {
        // error checking: no cmd passed
        if (command[1] < 0)
        {
          perror(command[1]);
          continue;
        }

        // parse input for mytime command
        int index1 = 0;
        int index2 = 1;
        char * cmd[80]; // new array to store command after mytime
        char * parsed;

        // parse command with space
        parsed = strtok(command[index2], " ");

        // store parsed command in cmd
        while (parsed != NULL)
        {
          cmd[index1] = parsed;
          index1++;
          index2++;
          parsed = strtok(command[index2], " ");
        }

        // finish cmd with NULL
        cmd[index1] = NULL;

        // measure time for function to execute
        struct timeval begin, end;
        clock_t cbegin, cend;

        // execute command and time it
        gettimeofday(&begin, 0);
        cbegin = clock();
        execCmd(cmd);
        gettimeofday(&end, 0);
        cend = clock();

        // calculate elapsed time
        long seconds = end.tv_sec - begin.tv_sec;
        long microseconds = end.tv_usec - begin.tv_usec;
        double elapsed = seconds + microseconds*1e-6;

        // calculate cpu time
        double cpuTime = ((double)(cend - cbegin))/CLOCKS_PER_SEC;

        // print time
        printf("CPU: %.3f Elapsed: %.3f\n", cpuTime, elapsed);
      }

      else
        execCmd(command); // call execute function
    }

    // free memory
    free(input);
  }

  return 0;
}

// function parses input by | and stores it in second parameter
int parsePipe(char * input, char ** pipeCommands)
{
  char * parsed;
  parsed = strtok(input, "|");

  int index = 0;
  while (parsed != NULL)
  {
    pipeCommands[index] = parsed;
    index++;
    parsed = strtok(NULL, "|");
  }

  pipeCommands[index] = NULL;

  return index;
}

// function parses input by space and stores it in second parameter
void parseSpace(char * input, char ** command)
{
  // parse the input
  char * separator = " ";
  char * parsed;
  parsed = strtok(input, separator);

  // store parsed input in command
  int index = 0;
  while (parsed != NULL)
  {
    command[index] = parsed;
    index++;
    parsed = strtok(NULL, separator);
  }

  // finish command with NULL
  command[index] = NULL;
}

void execCmd(char ** command)
{
  // if command is empty
  if (!command[0])
    return;

  /* CUSTOM COMMANDS */

  // myexit command
  else if (strcmp(command[0], "myexit") == 0)
    exit(0); // exits program without any error msg

  // mycd command
  else if (strcmp(command[0], "mycd") == 0)
  {
    // error checking: no dir passed to mycd
    if (mycd(command[1]) < 0)
      perror(command[1]);

    return;
  }

  // mypwd command
  else if (strcmp(command[0], "mypwd") == 0)
  {
    printf("%s\n", mypwd());
    return;
  }

  // mytree command
  else if (strcmp(command[0], "mytree") == 0)
  {
    // max directory name
    char direntName[256];
    // set array to null characters
    memset(direntName, 0, sizeof(direntName));

    // if dir is passed with function
    if (command[1] != NULL)
    {
      // copy dir parameter into direntName
      strcat(direntName, command[1]);
      printf("%s\n", direntName);
      mytree(direntName, 0);
    }
    // else pass current dir as parameter
    else
    {
      direntName[0] = '.';
      printf(".\n");
      mytree(direntName, 0);
    }

    return;
  }

  /* EXISTING EXTERNAL COMMANDS */

  // fork variables
  pid_t child_pid;
  int stat_loc;

  // forking a new process
  child_pid = fork();
  // error checking: fork failed
  if (child_pid < 0)
  {
    perror("Fork failed");
    exit(1);
  }

  // child process
  if (child_pid == 0)
  {
    // error checking: execvp failed
    if (execvp(command[0], command) < 0)
    {
      perror(command[0]);
      exit(1);
    }
  }

  // parent process
  else
    waitpid(child_pid, &stat_loc, WUNTRACED); // wait for child
}

void pipeline(char ** pipeCommands)
{
  char * command[80]; // support up to 80 characters

  // file descriptor variables
  int fd[2];
  int fdd = 0;

  // fork variables
  pid_t child_pid;
  int stat_loc;

  // loop to go through all piped commands
  int index = 0;
  while(pipeCommands[index] != NULL)
  {
    // parse each command with spaces
    parseSpace(pipeCommands[index], command);

    // creating pipe with file descriptors
    if (pipe(fd) < 0)
    {
      // error checking: pipe failed
      perror("Pipe failed");
      exit(1);
    }

    // fork
    child_pid = fork();

    // error checking: fork failed
    if (child_pid < 0)
    {
      perror("Fork failed");
      exit(1);
    }

    // child process
    else if (child_pid == 0)
    {
      // duplicate read descriptor
      dup2(fdd, 0);
      // if next command exists
      if (pipeCommands[index+1] != NULL)
        dup2(fd[1], 1); // duplicate write descriptor
      // close read descriptor
      close(fd[0]);

      if (execvp(command[0], command) < 0)
      {
        // error checking: execvp failed
        perror(command[0]);
        exit(1);
      }
    }

    // parent process
    else
    {
      waitpid(child_pid, &stat_loc, WUNTRACED); // wait for child process
      close(fd[1]); // close write descriptor
      fdd = fd[0];
      index++; // move to next command
    }
  }
}

// function mimics cd
int mycd(char * path)
{
  return chdir(path); // returns directory path
}

// function mimics pwd
char * mypwd()
{
  char buff[FILENAME_MAX]; // stores name of cwd
  return getcwd(buff, FILENAME_MAX); // returns cwd
}

// function mimics tree
void mytree(char direntName[], int level)
{
  // points to directory
  DIR * parentDir = NULL;

  // directory structure
  struct dirent * parentDirent = NULL;

  // open dir passed by user
  parentDir = opendir(direntName);

  // error checking: directory couldn't be opened
  if (parentDir == NULL)
  {
    perror("Opendir failed");
    return;
  }

  // read every directory in path
  while((parentDirent = readdir(parentDir)) != NULL)
  {
    // directory name before backup
    char * backupDir = NULL;

    // if current directory, continue
    if (parentDirent->d_name[0] == '.')
      continue;

    // print lines that mimic tree command structure
    int i = 0;
    for (i = 0; i < level; i++)
    {
      printf("|");
      printf("     ");
    }
    printf("|--- ");
    printf("%s\n", parentDirent->d_name);

    // if directory exists
    if (parentDirent->d_type == DT_DIR)
    {
      // length of directory
      int curDirentLen = strlen(direntName)+1;

      // backup directory name
      backupDir = (char *) malloc(curDirentLen);
      memset(backupDir, 0, curDirentLen);
      memcpy(backupDir, direntName, curDirentLen);

      // recursively call mytree on directory
      strcat(direntName, "/");
      strcat(direntName, parentDirent->d_name);
      mytree(direntName, level+1);

      // restore directory name
      memcpy(direntName, backupDir, curDirentLen);

      // free backupDir
      free(backupDir);
      backupDir = NULL;
    }
  }

  // close opened directory
  closedir(parentDir);
}
