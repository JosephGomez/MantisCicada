#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <fcntl.h>

using namespace std;

//ip addr and port number of servers
#define SERVER_A_IP    "127.0.0.1"
#define SERVER_B_IP    "127.0.0.1"
#define SERVER_C_IP    "127.0.0.1"
#define AWS_IP         "127.0.0.1"
#define SERVER_A_PORT "77770"
#define SERVER_B_PORT "77771"
#define SERVER_C_PORT "77772"

//AWS Server TCP and UDP Port NO.
#define AWS_TCP_PORT "77773"
#define AWS_UDP_PORT "77774"
#define MAXCHARSIZE 20

//#define DEBUG

//*********************Function: my_itoa*********************//
//Effect: Transform an int to a char
//Input: int num
//Output: char num
//refet to https://stackoverflow.com/questions/2279379/how-to-convert-integer-to-char-in-c
char *my_itoa_buffer(char *buffer, size_t len, int num){
    static char local_buffer[sizeof(int) * 8]; // not thread safe 

    if (!buffer)
    {
        buffer = local_buffer;
        len = sizeof(local_buffer);
    }

    snprintf(buffer, len, "%d", num);
    
    return buffer;
}

char *my_itoa(int num){
     return my_itoa_buffer(NULL, 0, num); 
}
//the information about error will be saved in the variable error_number
int error_num = 0;

//*********************Function: server_configure*********************//
//Effect: Return INformation on a particular host name
//input: configure_no: ip address information 
//       ip: char(ip)
//       port: char(port)
//       socket_type: SOCK_STREAM or SOCK_DGRAM
//output: 
//       success: 0
//       error: -1
int server_configure(struct addrinfo* &configure_no, const char* ip, const char* port, int socket_type){
    //refer to Beej.code
    struct addrinfo hints; //parameters configuration

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;  //both ipv4 and ipv6 is fine
    hints.ai_socktype = socket_type;

    if(ip == NULL){
        hints.ai_flags = AI_PASSIVE;//self ip address
    }

    error_num = getaddrinfo(ip,port,&hints,&configure_no);
    if(error_num != 0){
        fprintf(stderr,"getaddrinfo error: %s\n",gai_strerror(error_num));
        return 0;
    }

    error_num = 0;
    return 1;
}

//*********************Function: socket_establ*********************//
//Effect: Establish a socket 
//input: socket_descpt: socket descriptor
//       rv: ip address information
//output: 
//       success: 0
//       error: -1
int socket_establ(int &socket_descpt, struct addrinfo* &rv){
    //refer to Beej.code
    //Establish a socket
    socket_descpt = socket(rv->ai_family, rv->ai_socktype, rv->ai_protocol);

    //Set to nonblock state
    int opt = ((fcntl(socket_descpt, F_GETFL)) | O_NONBLOCK);
    fcntl(socket_descpt,F_SETFL,opt);

    //give socket permission to be bonded to port
    int yes = 1;
    setsockopt(socket_descpt, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)); 
    
    //bind the socket to the port
    error_num = bind(socket_descpt, rv->ai_addr, rv->ai_addrlen);
    if(error_num == -1){
        close(socket_descpt);
        fprintf(stderr,"bind error!:%d\n",errno);
        error_num = errno;
        return -1;
    }

    error_num = 0;
    return 0;
}

//*********************Function: server_start*********************//
//Effect: Start a server 
//input: socket_descpt: socket descriptor between server and client(TCP)
//       socket_descpt_r: socket descriptor between server and backend servers(UDP)
//output: 
//       success: 0
//       error: -1
int server_start(const char* port,int &sk){

    struct addrinfo *interface;

    server_configure(interface, NULL, port, SOCK_DGRAM);
    socket_establ(sk, interface);
    freeaddrinfo(interface);

    if(error_num) return -1;

    return 0;
}

//*********************Function: result_send*********************//
//Effect: Start a server 
//input: server_name: 'A', 'B', or 'C' to represent the name of a backend server
//       buffer: original send result from float
//       result_str: return result(with backend server name on the header)
//output: 
//       success: 0
//       error: -1
int result_send(char server_name, char* buffer, char* result_str){

    int socket_judege;  //socket descriptor
    struct addrinfo *server; 
    //First 
    result_str[0] = server_name;
    memcpy(&result_str[1], buffer, 8);

    //establish a socket
    server_configure(server, AWS_IP, AWS_UDP_PORT, SOCK_DGRAM);
    struct addrinfo *p;
    for(p = server; p != NULL; p = p->ai_next) {
        if ((socket_judege = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
            perror("talker: socket");
            continue;
        }
        break;
    }        
    if (p == NULL) {
        fprintf(stderr, "talker: failed to bind socket\n");
        return -1;
    }

    //send
    //check send successful
    int error_check;
    error_check = sendto(socket_judege, result_str, 9, 0, server->ai_addr, server->ai_addrlen);
	if(error_check == -1){
#ifdef DEBUG
        fprintf(stderr, "send error!:%d", errno);
#endif
    	close(socket_judege);
    	freeaddrinfo(server);
        return -1;
    }
    close(socket_judege);
    freeaddrinfo(server);
    return 0;
    
}