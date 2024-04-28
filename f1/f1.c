#include <stdio.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <dirent.h>
#define BUFFER_SIZE 1024

void requestfile(int clientSock){
    // Receive file from the server
    char buffer[BUFFER_SIZE];
    memset(buffer,0,BUFFER_SIZE);
    if (recv(clientSock, buffer, BUFFER_SIZE, 0) == -1) {
        perror("Receiving file failed");
        exit(EXIT_FAILURE);
    }
    // Create a copy of the received file in the server directory
    FILE* outputFile = fopen("clientfileCopier.txt", "wb");
    if (outputFile == NULL) {
        perror("Opening output file failed");
        exit(EXIT_FAILURE);
    }
    fwrite(buffer, sizeof(char), strlen(buffer), outputFile);
    fclose(outputFile);
    printf("Data written in the text file\n");
}
int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <Receiver IP> <Receiver Port>\n", argv[0]);
        exit(1);
    }
    int clientSocket;
    struct sockaddr_in serverAddress;
    char buffer[BUFFER_SIZE];
    
    // Create socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        perror("Socket creation failed\n");
        exit(EXIT_FAILURE);
    }
    else printf("Socket creation succesful\n");
    
    // Set up server address
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(atoi(argv[2]));
    serverAddress.sin_addr.s_addr = atoi(argv[1]);
    
    // Connect to server
    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        perror("Connection failed\n");
        exit(EXIT_FAILURE);
    }
    else{
    printf("Connected to server on port %d...\n",atoi(argv[2]));
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    int num = recv(clientSocket, buffer, BUFFER_SIZE - 1, 0);
    if (num < 0) {
        perror("Receiving error\n");
    }  
    int flag=0;
    while(flag==0){
        printf("%s\n", buffer);
    	char credentials[BUFFER_SIZE];
    	scanf("%s",credentials);
    	credentials[strcspn(credentials,"\n")]='\0';
    	int num1=send(clientSocket, credentials, strlen(credentials), 0); 
    	if (num1 == -1) {
        	perror("Sending request failed");
        	exit(EXIT_FAILURE);
    		}
    	else printf("Credentials Sent\n");
    	char buffer1[BUFFER_SIZE];
   	memset(buffer1, 0, BUFFER_SIZE);
    	int num = recv(clientSocket, buffer1, BUFFER_SIZE - 1, 0);
    	if (num < 0) {
        perror("Receiving error\n");
        }
        flag=atoi(buffer1);
        if(flag==0)
        printf("Incorrect login credential try again\n");
        else printf("Login Succesful\n");
    	 
    }
    flag=0;
    while(flag!=1){
    	//Waiting for request from server
    	printf("Waiting for request from server\n");
    	char request[BUFFER_SIZE];
    	memset(request, 0, BUFFER_SIZE);
    	int n=recv(clientSocket,request, BUFFER_SIZE, 0);
    	printf("%s\n",request);
    	//lock 
    	while(n<0);
    	 if(strcmp(request,"end")==0){
    	 	printf("Connection Terimation request made\n");
    	 	close(clientSocket);
    	 	return 0;
    	}
	else if(strcmp(request,"recievefile")==0){ 
    	requestfile(clientSocket);
    	}
    	// Get a list of file names from the local directory from client
    	else if(strcmp(request,"sendfiles")==0){
    	printf("Request for Directory is made by Server");
    	DIR *directory;
    	struct dirent *entry;
    	char fileList[BUFFER_SIZE] = "";
    	directory = opendir(".");
    	if (directory) {
        	while ((entry = readdir(directory)) != NULL) {
        	strcat(fileList, entry->d_name);
        	strcat(fileList, "\n");
    		}
    	closedir(directory);
    	}
 
    	// Send the list of file names to the server
    	if (send(clientSocket, fileList, strlen(fileList), 0) == -1) {
        	perror("Sending file list failed");
        	exit(EXIT_FAILURE);
    	}	
    
    	// Receive a request for a file from the server
 
    	int recvlen=recv(clientSocket, buffer, BUFFER_SIZE, 0);
    	if (recvlen== -1) {
        	perror("Receiving request failed");
        	exit(EXIT_FAILURE);
    	}
    	buffer[recvlen]='\0';
    	// Send the chosen file to the server
    	FILE *file = fopen(buffer, "rb");
    	if (file == NULL) {
        	perror("Opening file failed");
        	exit(EXIT_FAILURE);
    	}
    	//Sets the file pointer to end to file
    	fseek(file, 0, SEEK_END);
        //returns the current postion with respect to beginning of file
    	long fileSize = ftell(file);
    	//Sets the file position to the beginning of the file
        rewind(file);
    	char *fileBuffer = malloc(fileSize);
        //the whole txt files gets stored in the filebuffer
    	fread(fileBuffer, fileSize, 1, file);
    	if (send(clientSocket, fileBuffer, fileSize, 0) == -1) {
        	perror("Sending file failed\n");
        	exit(EXIT_FAILURE);
    	}
    	fclose(file);
    	printf("File sent successfully\n");
    	}
    	else printf("Something is wrong Try Again\n");
    	}
    // Close the connection
    close(clientSocket);
    }
    return 0;
}
