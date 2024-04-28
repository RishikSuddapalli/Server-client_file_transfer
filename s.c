#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#define BUFFER_SIZE 1024
void sendfile(int clientsock){
	//Send the request for files from clients
    	char message[BUFFER_SIZE]="recievefile";
    	message[strcspn(message, "\n")] = '\0'; 
    	send(clientsock, message, strlen(message), 0);
    	printf("Recivefile request made to client\n");
    	// Send the chosen file to the client
    	FILE *file = fopen("server_copy.txt", "rb");
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
    	if (send(clientsock, fileBuffer, fileSize, 0) == -1) {
        	perror("Sending file failed");
        	exit(EXIT_FAILURE);
    	}
    	fclose(file);
    	printf("File sent successfully\n");
    	
}
void requestfile(int clientSock){
    //Send the request for files from clients
    char message[BUFFER_SIZE]="sendfiles";
    message[strcspn(message, "\n")] = '\0'; 
    send(clientSock, message, strlen(message), 0);
    // Receive the list of file names from the client
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    int n=recv(clientSock, buffer, BUFFER_SIZE, 0);
    if (n == -1) {
        perror("Receiving file list failed");
        exit(EXIT_FAILURE);
    }
    // Choose a file from the list of file names
    puts(buffer);
    // You can implement your logic here to choose a file
    printf("write filname to request:");
    // Send a request to the client for the chosen file
    char requestMessage[BUFFER_SIZE];
    scanf("%s",requestMessage);
    if (send(clientSock, requestMessage, strlen(requestMessage), 0) == -1) {
        perror("Sending request failed");
        exit(EXIT_FAILURE);
    }
    // Receive the chosen file from the client
    memset(buffer, 0, BUFFER_SIZE);
    if (recv(clientSock, buffer, BUFFER_SIZE, 0) == -1) {
        perror("Receiving file failed");
        exit(EXIT_FAILURE);
    }
    // Create a copy of the received file in the server directory
    FILE* outputFile = fopen("server_copy.txt", "wb");
    if (outputFile == NULL) {
        perror("Opening output file failed");
        exit(EXIT_FAILURE);
    }
    fwrite(buffer, sizeof(char), strlen(buffer), outputFile);
    fclose(outputFile);
    printf("Data written in the text file\n");
}
void testcredential(int clientSock){
    int flag=0;
    while(flag==0){
    char message[BUFFER_SIZE]="Enter User Id & Password in 'userid_password',Where userid is UserID and password is Password";
    message[strcspn(message, "\n")] = '\0'; 
    send(clientSock, message, strlen(message), 0);
    char buf[BUFFER_SIZE];
    memset(buf, 0, BUFFER_SIZE);
    if(recv(clientSock,buf
    ,BUFFER_SIZE - 1, 0)<0){
        perror("Receiving error");
    }
    printf("%s\n",buf);
    char mess[BUFFER_SIZE]="1";
    if(strcmp(buf,"cli1_pass")==0 || strcmp(buf,"cli2_pass")==0 || strcmp(buf,"cli3_pass")==0){
        printf("Client login Succesful\n");
        flag=1;
    	mess[strcspn(mess, "\n")] = '\0';
    	send(clientSock, mess, strlen(mess), 0);
        }
    else printf("Client login failed try again");
    }    
}
int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <Receiver Port>\n", argv[1]);
        exit(1);
    }

    int serverSocket,clientSock1,clientSock2,clientSock3;
    struct sockaddr_in serverAddress,clientAddress1,clientAddress2,clientAddress3;
    char buffer[BUFFER_SIZE];
    
    // Create socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        printf("Socket creation unsuccesful");
    }
    else printf("Socket creation Succesful\n");
    
    // Set up server address
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(atoi(argv[1]));
    
    // Bind socket to address
    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        perror("Socket binding failed");
        exit(EXIT_FAILURE);
    }
    else printf("Bind Succesful\n");
    
    // Listen for incoming connections
    if (listen(serverSocket, 10) !=0) {
        perror("Listening failed due to limit\n");
        exit(EXIT_FAILURE);
    }
    else
    printf("Server listening on port %d...\n", atoi(argv[1]));
    
    // Accept incoming connection and login
    socklen_t len1 = sizeof(clientAddress1);
    socklen_t len2 = sizeof(clientAddress2);
    socklen_t len3 = sizeof(clientAddress3);

    clientSock1 = accept(serverSocket, (struct sockaddr*)&clientAddress1, &len1);
    if (clientSock1<0){
        perror("Accepting client1 connection failed\n");
        exit(EXIT_FAILURE);
    }
    else
    printf("Connection with client 1 Succesful\n");
    clientSock2 = accept(serverSocket, (struct sockaddr*)&clientAddress2, &len2);
    if (clientSock2<0) {
        perror("Accepting client2 connection failed\n");
        exit(EXIT_FAILURE);
    }
    else
    printf("Connection with client 2 Succesful\n");	
    clientSock3 = accept(serverSocket, (struct sockaddr*)&clientAddress3, &len3);
    if (clientSock3<0) {
        perror("Accepting client3 connection failed\n");
        exit(EXIT_FAILURE);
    }
    else
    printf("Connection with client 3 Succesful\n");
    	
    testcredential(clientSock1);
    testcredential(clientSock2);
    testcredential(clientSock3);
    int flag=0;
    while(flag==0){
    	printf("1-Request file from client1\n");
    	printf("2-Request file from client2\n");
    	printf("3-Request file from client3\n");
    	printf("4-Request file from client1 & send to client2\n");
    	printf("5-Request file from client3 & send to client2\n");
    	printf("6-Request file from client2 & send to client1\n");
    	printf("7-Request file from client3 & send to client1\n");
    	printf("8-Request file from client1 & send to client3\n");
    	printf("9-Request file from client2 & send to client3\n");
    	printf("Enter random key to end connection\n");
    	int choice;
    	scanf("%d",&choice);
    	switch (choice) {
        	case 1:
            	requestfile(clientSock1);
            	break;
 
        	case 2:
            	requestfile(clientSock2);
            	break;
 
        	case 3:
            	requestfile(clientSock3);
            	break;
            	
            	case 4:
            	requestfile(clientSock1);
            	sendfile(clientSock2);
            	break;
            	case 5:
            	requestfile(clientSock3);
            	sendfile(clientSock2);
            	break;
            	case 6:
            	requestfile(clientSock2);
            	sendfile(clientSock1);
            	break;
            	case 7:
            	requestfile(clientSock3);
            	sendfile(clientSock1);
            	break;
            	case 8:
            	requestfile(clientSock1);
            	sendfile(clientSock3);
            	break;
            	case 9:
            	requestfile(clientSock2);
            	sendfile(clientSock3);
            	break;
 
        	default:
        	flag=1;
        	char mess[BUFFER_SIZE]="end";
        	mess[strcspn(mess, "\n")] = '\0'; 
    		send(clientSock1, mess, strlen(mess), 0);
    		send(clientSock2, mess, strlen(mess), 0);
    		send(clientSock3, mess, strlen(mess), 0);
            	break;
    	}
    }
    
    // Close the connection
    close(clientSock1);
    close(clientSock2);
    close(clientSock3);
    close(serverSocket);
    
    
    return 0;
}
