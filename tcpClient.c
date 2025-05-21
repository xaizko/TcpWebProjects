#include <stdio.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>

#define IP "76.76.21.21" //Whatever ip address you want
#define PORT 80 //standard http PORT

int main() {
    int s;
    struct sockaddr_in sock; //a structure for handling internet addresses

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
}
