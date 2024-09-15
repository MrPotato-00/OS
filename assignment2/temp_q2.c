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
#include<sys/wait.h>

void compare_file(FILE* fp1, FILE* fp2){
	char file1= getc(fp1);
	char file2= getc(fp2);

	int error=0, line=1;

	while(file1!=EOF && file2!=EOF){
		if(file1=='\n' && file2=='\n'){
			line++;
		}
		if(file1!=file2){
			printf("Mismatch in line %d", line);
			return;
		}
		file1= getc(fp1);
		file2= getc(fp2);
	}

	if(file1!= EOF || file2!= EOF){
		printf("Mismatch...\n");
		return;
	}
	else printf("No mismatch...\n");
}

int main(){
	
	
	// this generates the 1GB file named "initialfile.txt"	
	int errorcode= system("fallocate -l 1M initialfile.txt");
	if(errorcode==0){
		printf(">>>intialfile.txt created successfully...\n");
	}
	else{
		printf("\n>>>Failed to create file...\n");
	}
	
	
	system("touch sendbackfile.txt");
	

	clock_t time_req;
	int fifo1_ecode= system("mkfifo -m 666 fifo1");
	int fifo2_ecode= system("mkfifo -m 666 fifo2");

	if(fifo1_ecode==0) printf(">>>fifo1 created successfully...\n");
	else printf(">>>fifo1 not created...\n");

	if(fifo2_ecode==0) printf(">>>fifo2 created successfully...\n");
	else printf(">>>fifo2 not created...\n");

	
	
	
	
		time_req= clock();
		system("cat initialfile.txt > fifo1 &");

		printf("Before child process...\n");
		
		if(fork()==0){
			system("cat fifo1 > fifo2 &");
			_Exit(0);
		}
		
		printf("After child process...\n");
		//system("cat fifo2 > sendbackfile.txt");
		system("cp fifo2 sendbackfile.txt");
		
		FILE *file1= fopen("initialfile.txt", "r");
		FILE *file2= fopen("sendbackfile.txt", "r");
		if (file1 == NULL || file2 == NULL){
      			printf("Error : Files not open");
      			exit(0);
  		}

		//FILE* file1= fopen("initialfile.c","r");
		//FILE* file2= fopen("sendbackfile.c", "r");
		
		//if(file1==NULL || file2==NULL) printf("File error...\n");
		compare_file(file1, file2);
		//fclose(file1);
		//fclose(file2);
		
		time_req= clock()- time_req;
		printf(">>>Time taken to complete the process: %f seconds\n", (float)time_req/CLOCKS_PER_SEC);
		fclose(file1);
		fclose(file2);

		system("rm fifo1 fifo2");
		
        


}
