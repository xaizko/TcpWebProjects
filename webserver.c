#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdilib.h>
#include <string.h>

#define LISTENADDR "127.0.0.1"

int main(int argc, char *argv[]) {
    int s;
    char *port;

    //check for correct usage
    if (argc < 2) {
	fprintf(stderr, "Usage: %s <port>\n", argv[0]);
	return -1;
    } else {
	port = argv[1];
    }

    //create socket
    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
	fprintf(stderr, "socket failed\n");
	return -1;
    }

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(LISTENADDR);
    server.sin_port = htons(atoi(port));

    if (bind(s, (struct sockaddr *)&server, sizeof(server))) {
	fprintf(stderr, "binding failed\n");
	close(s);
	return -1;
    }

}

