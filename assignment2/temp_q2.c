#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<stdbool.h>
#include<math.h>
#include<time.h>

void compare_file(FILE *firstFile, FILE *secondFile)
{
    char firstFileChar = getc(firstFile);
    char secondFileChar = getc(secondFile);


    int lineNumber = 1;
    int totalErrors = 0;

    while (firstFileChar != EOF && secondFileChar != EOF)
    {
        if (firstFileChar == '\n' && secondFileChar == '\n')
        {
            lineNumber++;
        }
        if (firstFileChar != secondFileChar)
        {
            totalErrors++;
            printf(">>> Mismatch occurred at line number : %d\n", lineNumber);
	    exit(0);
        }

        firstFileChar = getc(firstFile);
        secondFileChar = getc(secondFile);
    }

    if (totalErrors == 0)
    {
        printf(">>> Both the files are same, there is no mismatch!\n");
    }
}

int main(){
	
	// this generates the 1GB file named "initialfile.txt"	
	int errorcode= system("fallocate -l 1G initialfile.txt");
	if(errorcode==0){
		printf(">>>intialfile.txt created successfully...\n");
	}
	else{
		printf("\n>>>Failed to create file...\n");
	}
	clock_t time_req;
	int fifo1_ecode= system("mkfifo -m 666 fifo1");
	int fifo2_ecode= system("mkfifo -m 666 fifo2");

	if(fifo1_ecode==0) printf(">>>fifo1 created successfully...\n");
	else printf(">>>fifo1 not created...\n");

	if(fifo2_ecode==0) printf(">>>fifo2 created successfully...\n");
	else printf(">>>fifo2 not created...\n");

	int p_id= fork();
	
	if(p_id==0){
		// child process
		
		time_req= clock();
		system("cat initialfile.txt > fifo1 &");

		sleep(1);
		system("cat fifo2 >> sendbackfile.txt &");
		
		
		FILE *file1= fopen("initialfile.txt", "r");
		FILE *file2= fopen("sendbackfile.txt", "r");
		if (file1 == NULL || file2 == NULL){
      			printf("Error : Files not open");
      			exit(0);
  		}
		compare_file(file1, file2);
		time_req= clock()- time_req;
		printf(">>>Time taken to complete the process: %f seconds\n", (float)time_req/CLOCKS_PER_SEC);
		exit(0);
	}
		// parent process	
		system("cat fifo1 > fifo2 &");

		sleep(1);


}
