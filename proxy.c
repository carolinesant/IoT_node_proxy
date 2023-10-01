/*
 * This is the main program for the proxy, which receives connections for sending and receiving clients
 * both in binary and XML format. Many clients can be connected at the same time. The proxy implements
 * an event loop.
 *
 * *** YOU MUST IMPLEMENT THESE FUNCTIONS ***
 *
 * The parameters and return values of the existing functions must not be changed.
 * You can add function, definition etc. as required.
 */
#include "xmlfile.h"
#include "connection.h"
#include "record.h"
#include "recordToFormat.h"
#include "recordFromFormat.h"

#include <arpa/inet.h>
#include <sys/errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>

//I choose this BUFSIZE because it is big enough to fit most records, while also not making it so big that the program takes up too much memory for no reason
#define BUFSIZE 500


/* Struct that contains the information for one connected client.*/
struct Client
{
    int fd;
    char type;
    char id;
    char * rec_buf;
    int bytes_count;
    struct Client *next;
    struct Client *prev;
};

typedef struct Client Client;
Client *start, *end;
int num_clients;

/*
 * This function is called to add a client c to the linked list of clients
 */
void add_client(Client *c) {
    c->next = NULL;
    c->prev = end;

    if (end != NULL) {
        end->next = c;
    } else {
        start = c;
    }
    end = c;
    num_clients++;
}

/*
 * This function is called to free the fds and the allocated memory for the clients
 */
void free_clients() {
    Client *tmp = start;
    while (tmp != NULL) {
        Client *tmp2 = tmp;
        tmp = tmp2->next;
        tcp_close(tmp2->fd);
        if (tmp2->rec_buf != NULL) free(tmp2->rec_buf);
        free(tmp2);
    }
    start = NULL;
    end = NULL;
    num_clients = 0;
}

/*
 * This function finds a Client based on the id
 * if it finds a Client that matches this is returned, otherwise it returns NULL
 */
Client* find_client(char dest_id) {
    Client *c = start;
    while (c != NULL) {
        if (c->id == dest_id) return c;
        c = c->next;
    }
    return NULL;
}

/*
 * This function is called from handleClient when a new Record is found, to remove it from the buffer.
 * It removes n bytes from a buffer of size s
 */
void remove_bytes_from_buf(char** buffer, int s, int n, int server_sock) {
    int newSize = s - n;

    char* newBuffer = malloc((newSize + 1) * sizeof(char));
    if (newBuffer == NULL) {
        printf("remove_bytes_from_buf: Memory allocation failed.\n");
        tcp_close( server_sock );
        free_clients();
        exit(EXIT_FAILURE);
    }
    memcpy(newBuffer, (*buffer) + n, newSize);
    newBuffer[newSize] = '\0';

    free(*buffer);
    *buffer = newBuffer;
}

void usage( char* cmd )
{
    fprintf( stderr, "Usage: %s <port>\n"
                     "       This is the proxy server. It takes as imput the port where it accepts connections\n"
                     "       from \"xmlSender\", \"binSender\" and \"anyReceiver\" applications.\n"
                     "       <port> - a 16-bit integer in host byte order identifying the proxy server's port\n"
                     "\n",
                     cmd );
    exit( -1 );
}

/*
 * This function is called when a new connection is noticed on the server
 * socket.
 * The proxy accepts a new connection and creates the relevant data structures.
 *
 * *** The parameters and return values of this functions can be changed. ***
 */
void handleNewClient(int server_sock)
{
    int fd, rc;
    char id, type;
    Client *newC;
    fd = tcp_accept(server_sock);
    if (fd == -1) {return;}

    rc = tcp_read(fd, &type, 1);
    if (rc <= 0) {return;}
    if (type != 'B' && type != 'X') {return;}

    rc = tcp_read(fd, &id, 1);
    if (rc <= 0) {return;}

    newC = malloc(sizeof(Client));
    if (newC == NULL) {
        fprintf(stderr, "handleNewClient: Memory allocation failed\n"); 
        tcp_close( server_sock );
        free_clients();
        exit(EXIT_FAILURE);
    }

    newC->fd = fd;
    newC->type = type;
    newC->id = id;
    newC->bytes_count = 0;
    newC->rec_buf = NULL;
    add_client(newC);
}

/*
 * This function is called when a connection is broken by one of the connecting
 * clients. Data structures are clean up and resources that are no longer needed
 * are released.
 *
 * *** The parameters and return values of this functions can be changed. ***
 */
void removeClient( Client* client )
{
    if (client == start && client == end) {
        start = NULL; 
        end = NULL;
    } else if (client == start) {
        start = client->next;
        start->prev = NULL;
    } else if (client == end) {
        end = client->prev;
        end->next = NULL;
    } else {
        client->prev->next = client->next;
        client->next->prev = client->prev;
    }
    tcp_close(client->fd);
    if (client->rec_buf != NULL) free(client->rec_buf);
    free(client);
    num_clients--;
}

/*
 * This function is called when the proxy received enough data from a sending
 * client to create a Record. The 'dest' field of the Record determines the
 * client to which the proxy should send this Record.
 *
 * If no such client is connected to the proxy, the Record is discarded without
 * error. Resources are released as appropriate.
 *
 * If such a client is connected, this functions find the correct socket for
 * sending to that client, and determines if the Record must be converted to
 * XML format or to binary format for sendig to that client.
 *
 * It does then send the converted messages.
 * Finally, this function deletes the Record before returning.
 *
 * *** The parameters and return values of this functions can be changed. ***
 */
void forwardMessage( Record* msg )
{
    int rc;
    char dest_id;

    if (!msg->has_dest) {deleteRecord(msg); return;}
    dest_id = msg->dest; 
    
    Client * client = find_client(dest_id);
    if (client == NULL) {deleteRecord(msg); return;}
    
    char type = client->type;
    int msg_len = 0;
    char* conv_msg;
    
    if( type == 'B') {conv_msg = recordToBinary(msg, &msg_len);}
    else {conv_msg = recordToXML(msg, &msg_len);}
    
    if (conv_msg == NULL) {deleteRecord(msg); return;}
    
    rc = tcp_write(client->fd, conv_msg, msg_len);
    if (rc == -1) {
        deleteRecord(msg);
        free(conv_msg);
        return;
    }
    free(conv_msg);
    deleteRecord(msg);
}

/*
 * This function is called whenever activity is noticed on a connected socket,
 * and that socket is associated with a client. This can be sending client
 * or a receiving client.
 *
 * The calling function finds the Client structure for the socket where acticity
 * has occurred and calls this function.
 *
 * If this function receives data that completes a record, it creates an internal
 * Record data structure on the heap and calls forwardMessage() with this Record.
 *
 * If this function notices that a client has disconnected, it calls removeClient()
 * to release the resources associated with it.
 *
 * *** The parameters and return values of this functions can be changed. ***
 */
void handleClient( Client* client, int server_sock)
{
    int rc;
    
    if (client->rec_buf == NULL) {
        client->rec_buf = malloc(BUFSIZE + 1);
        if (client->rec_buf == NULL) {
            fprintf(stderr, "handleClient: Memory allocation failed\n");
            tcp_close( server_sock );
            free_clients();
            exit(EXIT_FAILURE);
        }
    } else {
        char* buf = realloc(client->rec_buf, client->bytes_count + BUFSIZE + 1);
        if (buf == NULL) {
            fprintf(stderr, "handleClient: Memory re-allocation failed\n");
            tcp_close( server_sock );
            free_clients();
            exit(EXIT_FAILURE);
        }
        client->rec_buf = buf;
    }

    rc = tcp_read(client->fd, &(client->rec_buf[client->bytes_count]), BUFSIZE);
    
    if (rc == -1) {
        return;
    } else if (rc == 0) {
        removeClient(client);
        return;
    } else {
        int b_to_read = client->bytes_count + rc;
        client->rec_buf[b_to_read] = 0; //Adding null-terminating byte, for when I am doing string operations in XMLtoRecord
        int b_read = 0;

        while (b_read < b_to_read) {
            Record *rec;
 
            //Checks the client type and calls the corresponding function
            if ( client->type == 'B') {
                rec = BinaryToRecord(client->rec_buf, b_to_read, &b_read); }
            else {
                rec = XMLtoRecord(client->rec_buf, b_to_read, &b_read); 
            }

            if (rec == NULL) {
                client->bytes_count += rc; 
                return;
            } else {
                if (b_read < b_to_read) {remove_bytes_from_buf(&client->rec_buf, b_to_read, b_read, server_sock);}
                client->bytes_count -= b_read;
                b_to_read -= b_read; 
                b_read = 0;
                forwardMessage(rec);
            }
        }
        //If there are no partial records, I clean up the buffer and bytes_count before the next potensial read
        free(client->rec_buf);
        client->rec_buf = NULL; 
        client->bytes_count = 0;
    }

}

int main( int argc, char* argv[] )
{
    int port;
    int server_sock;

    if( argc != 2 )
    {
        usage( argv[0] );
    }

    port = atoi( argv[1] );

    server_sock = tcp_create_and_listen( port );
    if( server_sock < 0 ) exit( -1 );

    start = NULL; end = NULL;

    fd_set fds;
    int rc;
    
    /*
     * The following part is the event loop of the proxy. It waits for new connections,
     * new data arriving on existing connection, and events that indicate that a client
     * has disconnected.
     *
     * This function uses handleNewClient() when activity is seen on the server socket
     * and handleClient() when activity is seen on the socket of an existing connection.
     *
     * The loops ends when no clients are connected any more.
     */
    do
    {
        int max_fd = server_sock;

        FD_ZERO(&fds);
        FD_SET(server_sock, &fds);
        
        Client *c = start;
        while (c != NULL) {
            FD_SET(c->fd, &fds);
            if (c->fd > max_fd) {max_fd = c->fd;}
            c = c->next;
        }

        rc = tcp_wait(&fds, max_fd + 1);
        if (rc == -1) {
            tcp_close( server_sock );
            free_clients();
            return EXIT_FAILURE;
        }
        
        if (FD_ISSET(server_sock, &fds)) {
            handleNewClient(server_sock);
            rc--;
        }
        c = start;
        while (rc > 0 && c != NULL) {
            Client * c2 = c->next;
            if (FD_ISSET(c->fd, &fds)){
                handleClient(c, server_sock);
                rc --;
            }
            c = c2;
        }
    }
    while(num_clients > 0);

    tcp_close( server_sock );

    return 0;
}

