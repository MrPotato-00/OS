#include<stdio.h>
#include<signal.h>
#include<unistd.h>
void demo(int sig){
	if(sig==SIGINT){
		printf("Ha Ha Not Stopping\n");
	}
}

int main(){
	if (signal(SIGINT, demo)== SIG_ERR){
		perror("\nCan't catch SIGINT\n");
	}
	while(1){
		sleep(1);		
	}
	return 0;
}
