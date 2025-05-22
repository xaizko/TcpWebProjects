#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define LISTENADDR "127.0.0.1"

//function headers
int cli_accept(int s);
void cli_conn(int s, int c);

int main(int argc, char *argv[]) {
    int s, c;
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

    // bind socket to address
    if (bind(s, (struct sockaddr *)&server, sizeof(server))) {
	fprintf(stderr, "binding failed\n");
	close(s);
	return -1;
    }

    //listen on port
    if (listen(s, 5) < 0) {
	close(s);
	fprintf(stderr, "failed to listen on port\n");
	return -1;
    }

    printf("Listening on %s:%s\n", LISTENADDR, port);
    while (1) {
	c = cli_accept(s);
	if (!c) {
	    fprintf(stderr, "failed to accept");
	    continue;
	}
	
	printf("Incoming connection\n");
	if (!fork()) {
	    cli_conn(s, c); 
	}
    }

    return 0;
}

int cli_accept(int s) {
    int c;
    socklen_t addrlen = 0;
    struct sockaddr_in cli;

    memset(&cli, 0, sizeof(cli));
    c = accept(s, (struct sockaddr*)&cli, &addrlen);
    if (c < 0) {
	fprintf(stderr, "failed to accept connection\n");
	return -1;
    }

    return 0;
}

void cli_conn(int s, int c) {

}
