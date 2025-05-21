#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT 8181

int main() {
    int s, c;
    struct sockaddr_in sock, client;
    socklen_t addrlen;

    //clear memory just incase
    memset(&srv, 0, sizeof(srv));
    memset(&cli, 0, sizeof(cli));

    //create socket
    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
	fprintf(stderr, "Failed to create socket.\n");
	close(s);
	return 1;
    }

    srv.sin_family = AF_INET;
    srv.sin_addr.s_addr = 0;
    srv.sin_port = htons(PORT);

    //bind socket to address
    if (bind(s, (struct sockadrr*)&srv, sizeof(srv))) {
	fprintf(stderr, "Failed to bind\n");
	close(s);
	return 1;
    }

    if (listen(s, 5)) {
	fprintf(stderr, "Listen failed\n");
	close(s);
	return 1;
    }

    c = accept(s, (struct sockaddr *)&srv, &addrlen);
    if (c < 0) {
	fprintf(stderr, "Failed to accept connection\n");
	close(s);
	return 1;
    }

    printf("Client connected\n");
}
