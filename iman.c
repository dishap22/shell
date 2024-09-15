#include "headers.h"

void iMan(const char* command) {
    char host[] = "man.he.net";
    char request[BUFFER_SIZE * 2];
    char response[BUFFER_SIZE * 2];
    struct hostent *server;
    struct sockaddr_in server_addr;
    int sockfd, bytes_received;
    int header_done = 0;
    char *header_end;

    // ensure there are no leading/trailing spaces
    char cleaned_command[BUFFER_SIZE];
    snprintf(cleaned_command, sizeof(cleaned_command), "%s", command);
    
    // get the server's ip 
    server = gethostbyname(host);
    if (server == NULL) {
        fprintf(stderr, "Error: No such host\n");
        return;
    }

    // create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return;
    }

    // address structure of server
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(80); // HTTP port
    memcpy(&server_addr.sin_addr.s_addr, server->h_addr_list[0], server->h_length);

    // connect to server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        close(sockfd);
        return;
    }

    // GET request
    snprintf(request, sizeof(request),
             "GET /?topic=%s&section=alls HTTP/1.1\r\n"
             "Host: %s\r\n"
             "Connection: close\r\n\r\n",
             cleaned_command, host);

    // send request
    if (write(sockfd, request, strlen(request)) < 0) {
        perror("write");
        close(sockfd);
        return;
    }

    // reading response
    while ((bytes_received = read(sockfd, response, sizeof(response) - 1)) > 0) {
        response[bytes_received] = '\0';

        if (!header_done) {
            // searching for the end of the header
            // header and page separated by \n\n
            header_end = strstr(response, "\n\n");
            if (header_end) {
                header_end += 2; // skip "\n\n" after headers
                printf("%s", header_end);
                header_done = 1;
            } else {
                printf("%s", response);
            }
        } else {
            printf("%s", response);
        }
    }
    close(sockfd);
}
