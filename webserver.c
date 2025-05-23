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

int main(int argc, char *argv[]) {
    int s, c;
    char *port;
    struct httpRequest *req;
    char buf[512];
    

    char *template = "GET /sdfsdfd HTTP/1.1\n";
    memset(buf, 0, 512);
    strncpy(buf, template, 511);
    req = parse_http(buf);
    printf("%s", req->method);
    free(req);

    return 0;

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

    return 0;
}

void cli_conn(int s, int c) {
    return; 
}


struct httpRequest *parse_http(char *str) {
    struct httpRequest *req;
    char *p;

    req = malloc(sizeof(struct httpRequest));
    memset(req, 0, sizeof(struct httpRequest));

    for (p=str; p && *p != ' '; p++);
    if (*p == ' '){
	*p = 0;
    } else {
	fprintf(stderr, "no space found while parsing\n");
	free(req);
	return 0;
    }

    strncpy(req->method, str, 7);
    return req;
}
