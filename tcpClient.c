#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>

#define IP "98.137.11.164" //Whatever ip address you want
#define PORT 80 //standard http PORT

int main() {
    int s;
    struct sockaddr_in sock; //a structure for handling internet addresses
    char buf[512];
    char *data;

    //change this if you want different information
    data = "HEAD / HTTP/1.0\r\n\r\n";

    //create the socket
    s = socket(AF_INET, SOCK_STREAM, 0);
    //verify socket connection was successful
    if (s < 0) {
	printf("socket() error\n");
	return -1; 
    }

    //filling out the structure with the correct data
    sock.sin_family = AF_INET;
    sock.sin_port = htons(PORT);
    sock.sin_addr.s_addr = inet_addr(IP);
   
    //connect to server
    if (connect(s,(struct sockaddr *)&sock, sizeof(struct sockaddr_in)) != 0) {
	printf("connect() error\n");
	close(s);

	return -1;
    }

    //copy data
    write(s, data, strlen(data));
    memset(buf, 0, 512);
    read(s, buf, 511);
    close(s);
    //print it!
    printf("\n%s\n", buf);
}

