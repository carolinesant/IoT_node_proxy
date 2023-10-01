/*
 * This is a file that implements the operation on TCP sockets that are used by
 * all of the programs used in this assignment.
 *
 * *** YOU MUST IMPLEMENT THESE FUNCTIONS ***
 *
 * The parameters and return values of the existing functions must not be changed.
 * You can add function, definition etc. as required.
 */
#include "connection.h"
#define BACKLOG 26

int tcp_connect( char* hostname, int port )
{
    int fd,wc;
    struct sockaddr_in addr;
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {perror("inet_pton"); return -1;}

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    wc = inet_pton(AF_INET, hostname, &addr.sin_addr.s_addr);
    if (wc == -1) {perror("inet_pton"); return -1;}
    if (wc == 0) {fprintf(stderr, "inet_pton: not valid adress%s\n", hostname); return -1;}

    wc = connect(fd, (struct sockaddr *) &addr, sizeof(struct sockaddr_in));
    if (wc == -1) {perror("connect"); return -1;}

    return fd;
}

int tcp_read( int sock, char* buffer, int n )
{
    int rc;
    rc = read(sock, buffer, n);
    if (rc == -1) {perror("read"); return -1;}
    return rc;
}

int tcp_write( int sock, char* buffer, int bytes )
{
    int wc;
    wc = write(sock, buffer, bytes);
    if (wc == -1) {perror("write"); return -1;}
    return wc;
}

int tcp_write_loop( int sock, char* buffer, int bytes )
{
    int wc;
    wc = 0;
    while (wc < bytes) {
        wc += write(sock, &buffer[wc], bytes-wc);
        if (wc == -1) {perror("write"); return -1;}
    }
    return 0;
}

void tcp_close( int sock )
{
    close(sock);
}

int tcp_create_and_listen( int port )
{
    int fd, rc;
    struct sockaddr_in addr;
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {perror("socket"); return -1;}

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    rc = bind(fd, (struct sockaddr *) &addr, sizeof(struct sockaddr_in));
    if (rc == -1) {perror("bind"); return -1;}

    listen(fd, BACKLOG);
    if (fd == -1) {perror("listen"); return -1;}
    return fd;
}

int tcp_accept( int server_sock )
{
    int fd;
    fd = accept(server_sock, NULL, NULL);
    if (fd == -1) {perror("accept"); return -1;}
    return fd;
}

int tcp_wait( fd_set* waiting_set, int wait_end )
{
    int rc;
    rc = select(wait_end, waiting_set, NULL, NULL, NULL);
    if (rc == -1) {perror("select"); return -1;}

    return rc;
}

int tcp_wait_timeout( fd_set* waiting_set, int wait_end, int seconds)
{
    int rc;
    struct timeval t;
    t.tv_sec = seconds;
    t.tv_usec = 0;
    rc = select(wait_end, waiting_set, NULL, NULL, &t);
    if (rc == -1) {perror("select"); return -1;}

    return rc;
}

