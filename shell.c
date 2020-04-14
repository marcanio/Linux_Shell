#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX 100

void printPID(int pick);
void printPWD();
void printCD(char *userInput);
int countArgs(char *userInput);
int ampDetect(char *userInput);


int main(int argc, char **argv){


  char userInput[MAX];
  int pickPID; //Choose between PID and PPID
  int i,j,k; //Loops
  char *prompt = "308sh> ";
  int argCount =1;
  int backProcess = 0;
  int child, child2;




  if(argc == 3 ){
    if((strlen(argv[1]) != 2) || (strncmp("-p", argv[1], 2) != 0)){ //Make sure -p and then something after is on the inputline
      printf("Invalid command line argument.\n");
      return 0; //Exit
    }
    prompt =argv[2];
  }else if(argc != 3 && argc != 1){
    printf("Invalid command-line argument. \n");
    return 0; //Exit
  }


  //Infinite loop for user input
  while(1){

    printf(prompt); //Print prompt

    fgets(userInput, MAX, stdin);//Get the users input and store in userInput

    if((userInput[strlen (userInput)-1] == '\n') && (strlen(userInput)>0)){
      userInput[strlen(userInput)-1] = '\0'; //Null terminate users input
    }

    /*Built In commands*/

    if(strncmp("exit", userInput, 4) == 0 && (strlen(userInput) == 4)) //Exit command- Check for 4 letters and the word "Exit"
		{
			return 0;

		}else if(strncmp("pid",userInput,3) == 0 && (strlen(userInput) == 3)){
      pickPID = 1;
      printPID(pickPID);

    }else if(strncmp("ppid",userInput,4) == 0 && (strlen(userInput) == 4)){
      pickPID = 2;
      printPID(pickPID);

    }else if(strncmp("pwd",userInput,3) == 0 && (strlen(userInput) == 3)){
      printPWD();

    }else if((strncmp("cd",userInput,2) == 0) && ((strlen(userInput)==2) || userInput[2] == ' ')){ //Either cd is by itself or cd has a space after for a certain
      printCD(userInput);

    }else{ //Not built in commands


      			argCount = countArgs(userInput);
            backProcess = ampDetect(userInput);

            if(backProcess == 1){ //Take out an argument
              	argCount--;
            }

      			/*fill args[] using temp string x for each individual argument*/

      			char* args[argCount + 1];
            char x[argCount][MAX];
            argCount = 0;
            int spot =0;

            for(i = 0; i < strlen(userInput) + 1; i++)
            {
              if (userInput[i] == ' ' || userInput[i] == '\0') //Format args
              {
                x[argCount][spot] = '\0'; //Add nulls inbetween arguments and at the end of the line
                args[argCount] = x[argCount];
                argCount++; //since there was a space there is another argument
                spot = 0;
              }
              else if (userInput[i] == '&' && i == strlen(userInput) - 1) //If background process
              {
                i = strlen(userInput) + 1; //
              }
              else
              {
                x[argCount][spot] = userInput[i];
                spot++;
              }
            }


            args[argCount] = (char*) NULL;


      		 /*background process spawns 2 childs- One for current command and another to run what the user types in next*/
      			if(backProcess == 1)
      			{
      				backProcess = 0;
      				child2 = fork();
      				if(child2 == 0)
      				{
      					child = fork();
      					if(child == 0)
      					{
      						printf("[%d] %s\n", getpid(), args[0]); //Print PID of current process and command
      						/*execute command*/
                  printf("TEST: %s", args[0]);
      						execvp(args[0], args);
      						perror("\0"); //Not a real command
      						return 0;
      					}
      					else
      					{
      						waitpid(-1, -1, 0);
                  //{print child process and then new prompt}
      						printf("\n[%d] %s Exit %d\n", child, args[0], WEXITSTATUS(-1));
      					  printf(prompt);
      						return 0;
      					}
      				}
      				else
      				{
      					usleep(1000);
      				}
      			}

      			/*No Back process*/
      			else
      			{

      				child = fork(); //Run the process

      				if(child == 0)
      				{

      					printf("[%d] %s\n", getpid(), args[0]); //Pid and process command
      					/*execute command*/
      					execvp(args[0], args);

      					perror("\0"); //No such command
      					return 0;
      				}
      				/*parent process*/
      				else
      				{
      					usleep(1000);
      					waitpid(child, -1, 0);

      					printf("[%d] %s Exit %d\n", child, args[0], WEXITSTATUS(-1));
      				}
      			}

    }


  }
}

/*Look in users input for background process*/
int ampDetect(char *userInput){
  if(userInput[strlen(userInput) - 1] == '&')//Check if last char == &.. If so there is a background process
  {
    return 1;
  }else{
    return 0;
  }

}
/*Prints PID: pick =1, PPID pick =2 */
void printPID(int pick){

  if(pick == 1){
      printf("Proccess ID of this shell: %d \n", getpid()); //Print out PID usng getpid()
  }else if(pick == 2){
      printf("Proccess ID of this shell's parent: %d \n", getppid());//Print out PPID using getppid()
  }
}

/*Prints the PWD*/
void printPWD(){
  char wd[2048]; //Current working dirrectory
  if(getcwd(wd,2048) == NULL){
    printf("Failed to print working directory");
  }
    printf("%s\n",wd); //If it was successful than print out WD
}

/*Prints the CD*/
void printCD(char *userInput){

  char directory[MAX];

  if(strlen(userInput) == 2){ //If input is "cd" bring to home directory
    chdir(getenv("HOME"));
  }else{
    for(int i =0; i < strlen(userInput)-3; i++){
      directory[i] = userInput[i+3]; //Start copying after "cd "
      //printf("%c", directory[i]);
      directory[i+1] = '\0'; //Make sure null terminated
    }
    if(chdir(directory)== -1){ //If not found
    printf("Direcotry not found..\n");
    }

  }
}

/*Count the amount of arguments*/
int countArgs(char *userInput){
  int i =0;
  int argCount =1;
  for(i = 0; i < strlen(userInput); i++)
  {
    if (userInput[i] == ' ')
    {
      argCount++;
    }
  }
  return argCount;
}
