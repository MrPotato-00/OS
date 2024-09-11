#include<stdio.h>
#include<stdlib.h>
#include<string.h>


int main(){
	char* file1= (char*)malloc(sizeof(char)*30);
	//char* file2= (char*)malloc(sizeof(char)*30);
	//char* file3= (char*)malloc(sizeof(char)*30);
	
	printf("enter the names of the three files..\n");
	fgets(file1, 30, stdin);
	//fgets(file2, 30, stdin);
	//fgets(file3, 30, stdin);

	printf("%s", file1);
	//printf("%s", file2);
	//printf("%s", file3);

	char* file= file1;
	printf("%s", file);


}
