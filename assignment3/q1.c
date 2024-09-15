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
#include<arpa/inet.h>
#include<errno.h>
#define ip "127.0.0.1"
#define port 5650
#define size 1024

int fsize(){
	struct stat st;
	stat("sample.c", &st);
	return st.st_size;
}

void compare_files(char*fp1, char*fp2){

	FILE* file1= fopen(fp1, "r");
	FILE* file2= fopen(fp2, "r");

	if(file1==NULL || file2==NULL){
		perror("[-] Error in file...\n");
		exit(1);
	}

    	char f1 = getc(file1);
    	char f2 = getc(file2);
	
	int error=0, line=1;
	while(f1!=EOF && f2!=EOF){
		if(f1=='\n' && f2=='\n'){
			line++;
		}
		if(f1!=f2){
			printf("Mismatch in line %d", line);
			fclose(file1);
			fclose(file2);
			return;
		}
		f1= getc(file1);
		f2= getc(file2);
	}

	if(f1!=EOF || f2!=EOF)
		printf("Mismatch...\n");
	
	else
		printf("No mismatch...\n");

	fclose(file1);
	fclose(file2);
}

void send_file(int sockfd, FILE*fp){
	
  	char data[size];
	int totalbytes= fsize();
	printf("File size: %d\n",totalbytes); 
	int bytes=0;
	int byte_read;
	while(bytes<totalbytes && (byte_read= fread(data, sizeof(char), size, fp))>0){
		send(sockfd, data, byte_read, 0);
		bytes+= byte_read;
		bzero(data, size);
	}
  	printf("[+]File send successfully...\n");
}


void write_file(int sockfd, FILE*fr){
	char revbuf[size];
	int totalbytes= fsize();
	int bytes=0;
	printf("File size: %d\n", totalbytes);
	int byte_recv= 0;
	while(bytes<totalbytes && (byte_recv = recv(sockfd, revbuf, size, 0))>0){
			
		fwrite(revbuf, sizeof(char), byte_recv, fr);
		bytes+= byte_recv;
		
		bzero(revbuf, size);
	}

	printf("[+]File received...\n");
	    
}




void server(char*file){

	int e;
	
	int sockfd, new_sock;
	struct sockaddr_in server_addr, new_addr;
	socklen_t addr_size;

	char buffer[size];
	
	sockfd= socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd<0){
	       	perror("[-] Error in socket..\n");
		exit(1);
	}
	printf("[+] Server socket created...\n");

	server_addr.sin_family= AF_INET;
	server_addr.sin_port= port;
	server_addr.sin_addr.s_addr= inet_addr(ip);

	e= bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
	if(e<0){
		perror("[-] Error in binding..\n");
		exit(1);
	}
	printf("[+] Binding is successfull...\n");

	e= listen(sockfd, 10);
	if(e==0){
		printf("[+] Listening...\n");
	}
	else{
		perror("[-] Error in listening..\n");
		exit(1);
	}
	
	addr_size= sizeof(new_addr);
	new_sock= accept(sockfd, (struct sockaddr*)&new_addr, &addr_size);
	
	FILE* fp= fopen(file, "wb");
	if(fp==NULL){
		perror("[-]Error in creating file...\n");
		exit(1);
	}
	write_file(new_sock, fp);
	fclose(fp);
	
	

	fp= fopen(file, "rb");
	if(fp==NULL){
		perror("[-]File error...\n");
		exit(1);
	}
	
	send_file(new_sock, fp);
	fclose(fp);
	
	close(new_sock);
	close(sockfd);
	
}	

void  client(char* file1, char*file2){
	
	int e;
	int sockfd;
	struct sockaddr_in client_addr;
	FILE* fp;

	sockfd= socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd<0){
		perror("[-] Error in socket...\n");
		exit(1);
	}
	printf("[+] Client socket created...\n");

	client_addr.sin_family= AF_INET;
	client_addr.sin_port= port;
	client_addr.sin_addr.s_addr= inet_addr(ip);

	e= connect(sockfd, (struct sockaddr*)&client_addr, sizeof(client_addr));

	if(e<0){
		perror("[-] Error in connecting...\n");
		exit(1);
	}
	printf("[+] Connected to server...\n");

	fp= fopen(file1, "rb");
	if(fp==NULL){
		perror("[-] Error in reading file...\n");
		exit(1);
	}

	send_file(sockfd, fp);
	
	fclose(fp);
	//sleep(1000);
	fp= fopen(file2 ,"wb");
	if(fp==NULL){
		perror("[-] Error in file...\n");
		exit(1);
	}
	write_file(sockfd, fp);
	
	fclose(fp);
	close(sockfd);
	printf("[+] Disconnected from server...\n");

	
}

int main(){
 	
	//system("fallocate -l 1M file.txt");
	
	/*
	char* file1= (char*)malloc(sizeof(char)*30);
	char* file2= (char*)malloc(sizeof(char)*30);

	printf("enter two file names with .txt extension other than file.txt...\n");
	fgets(file1,30, stdin);
	fgets(file2,30, stdin);
	*/
	
	clock_t time_req= clock();

	int pid= fork();

	if(pid==0){ //child process
		server("sample1.c"); // receive file1.txt and write to file2.txt

		_Exit(0);
	}
	else{
		client("sample.c","sample2.c");// send file1.txt
		

		compare_files("sample.c", "sample2.c");
		time_req= clock()- time_req;
		printf(">>>Time taken to complete the process: %f seconds\n", (float)time_req/CLOCKS_PER_SEC);

	}

}
