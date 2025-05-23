#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define LISTENADDR "127.0.0.1"

struct httpRequest {
    char method[8];
    char url[128];

}; //typedef struct httpRequest;

//function headers
int cli_accept(int s);
void cli_conn(int s, int c);
struct httpRequest *parse_http(char *str);
char *cli_read(int c); 

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

    //make sure address is reusable
    int opt = 1;
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
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

    //scan for connects
    while (1) {
	c = cli_accept(s);
	if (c < 0) {
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

    return c;
}

void cli_conn(int s, int c) {
    struct httpRequest *req;
    char *p;

    p = cli_read(c);
    if (!p) {
	fprintf(stderr, "failed to read\n");
	close(c);
	return;
    }

    req = parse_http(p);
    if (!req) {
	fprintf(stderr, "failed to parse\n");
    }

    printf("%s\n%s", req->method, req->url);
    free(req);
    close(c);
    return;
}


struct httpRequest *parse_http(char *str) {
    struct httpRequest *req;
    char *p;

    req = malloc(sizeof(struct httpRequest));

    for (p=str; p && *p != ' '; p++);
    if (*p == ' '){
	*p = 0;
    } else {
	fprintf(stderr, "no space found while parsing\n");
	free(req);
	return 0;
    }

    strncpy(req->method, str, 7);

    for (str=++p; p && *p != ' '; p++);
    if (*p == ' '){
	*p = 0;
    } else {
	fprintf(stderr, "2nd space not found while parsing\n");
	free(req);
	return 0;
    }

    strncpy(req->url, str, 127);
    return req;
}

char *cli_read(int c) {
    static char buf[512];
    memset(buf, 0, 512);
    if (read(c, buf, 511) < 0) {
	printf("read() error\n");
	return 0;
    }
    return buf;
}
