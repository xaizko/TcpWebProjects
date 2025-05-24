#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

#define LISTENADDR "127.0.0.1"

struct httpRequest {
    char method[8];
    char url[128];

};

struct sFile {
    char filename[64];
    char *fileContent;
    int size;
}; typedef struct sFile File;

//function headers
int cli_accept(int s);
void cli_conn(int s, int c);
struct httpRequest *parse_http(char *str);
char *cli_read(int c); 
void http_headers(int c, int code);
void http_response(int c, char *contentType, char *data);
int sendfile(int c, char *contentType, File *file);
File *readFile(char *fileName);

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
    char *response;
    char str[96];
    File *f;

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
    if (!strcmp(req->method, "GET") && !strncmp(req->url, "/img/", 5)) {
	memset(str, 0, 96);
	snprintf(str, 95, ".%s", req->url);
	f = readFile(str);
	if (!f) {
	    response = "File not found";
	    http_headers(c, 404);
	    http_response(c, "text/plain", response);
	} else {
	    http_headers(c, 200);
	    if (!sendfile(c, "image/png", f)) {
		response = "HTTP server error";
		http_headers(c, 500);
		http_response(c, "text/plain", response);
	    }
	}
    }
    if (!strcmp(req->method, "GET") && !strcmp(req->url, "/app/webpage")) {
	response = "<html><img src='/img/kitty.png' alt='image' /></html>";
	http_headers(c, 200);
	http_response(c, "text/html", response);
    } else {
	response = "File not found";
	http_headers(c, 404);
	http_response(c, "text/plain", response);
    }

    free(req);
    close(c);
    return;
}

File *readFile(char *fileName) {
    char buf[512];
    char *p;
    int n, x, fd;
    File *f;

    fd = open(fileName, O_RDONLY);
    if (fd < 0) {
	return 0;
    }

    f = malloc(sizeof(struct sFile));
    if (!f) {
	close(fd);
	return 0;
    }

    strncpy(f->filename, fileName, 63);
    f->fileContent = malloc(512);

    x = 0; // num of bytes read
    while (1) {
	memset(buf, 0, 512);
	n = read(fd, buf, 512);
	if (!n) {
	    break;
	} else if (x == -1) {
	    close(fd);
	    free(f->fileContent);
	    free(f);
	    return 0;
	}

	strncpy(buf, (f->fileContent)+x, n);
	x += n;
	f->fileContent = realloc(f->fileContent, (512+x));
    }

    f->size = x;
    close(fd);
    return f;
}

int sendfile(int c, char *contentType, File *file) {
    char buf[512];
    int n, x;
    char *p;

    if (!file) {
	return 0;
    }

    snprintf(buf, 511,
	     "Content-Type: %s\n"
	     "Content-Length: %d\n"
	     "\r\n",
	     contentType, file->size);
    write(c, buf, strlen(buf));

    n = file->size;
    p = file->fileContent;
    while (n > 0) {
	int x = write(c, p, (n < 512) ? n : 512);
	if (x < 1) {
	    return 0;
	}
	n -= x;
	p += x;
    }
    return 1;
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

void http_headers(int c, int code) {
    char buf[512];
    int n;

    snprintf(buf, 511, 
	"HTTP/1.0 %d OK\r\n"
	"Server: webserver.c\r\n"
	"Content-Language: en\r\n"
	"Expires: -1\r\n", code);

    n = strlen(buf);
    write(c, buf, n);

    return;
}

void http_response(int c, char *contentType, char *data) {
    char buf[512];
    int n;

    n = strlen(data);
    snprintf(buf, 511,
	     "Content-Type: %s\n"
	     "Content-Length: %d\n"
	     "\n%s\n", contentType, n, data);

    n = strlen(buf);
    write(c, buf, n);

    return;
}
