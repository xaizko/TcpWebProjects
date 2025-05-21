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
    char buf[512];
    int bytes;

    //clear memory just incase
    memset(&sock, 0, sizeof(sock));
    memset(&client, 0, sizeof(client));

    //create socket
    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
	fprintf(stderr, "Failed to create socket.\n");
	close(s);
	return 1;
    }

    sock.sin_family = AF_INET;
    sock.sin_addr.s_addr = 0;
    sock.sin_port = htons(PORT);

    //bind socket to address
    if (bind(s, (struct sockaddr *)&sock, sizeof(sock))) {
	fprintf(stderr, "Failed to bind\n");
	close(s);
	return 1;
    }

    //listen for connections
    if (listen(s, 5)) {
	fprintf(stderr, "Listen failed\n");
	close(s);
	return 1;
    }

    //accept connections
    c = accept(s, (struct sockaddr *)&sock, &addrlen);
    if (c < 0) {
	fprintf(stderr, "Failed to accept connection\n");
	close(s);
	return 1;
    }

    printf("Client connected\n");
    while((bytes = read(c, buf, sizeof(buf)) -1) > 0) {
	buf[bytes] = '\0';
	printf("Received: %s\n", buf);

	write(c, "Got it\n", 7);
    }
    close(c);
    close(s);
}
