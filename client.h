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
#include <vector>

using namespace std;

#define SERVER_IP "127.0.0.1"//Server IP address
#define SERVER_PORT_NO "77775"//Server Port No.
#define MAXCHARSIZE 20  //Maximum char which can transmit

//*********************Function: my_itoa*********************//
//Effect: Transform an int to a char
//Input: int num
//Output: char num
//refer to https://stackoverflow.com/questions/2279379/how-to-convert-integer-to-char-in-c
const char *my_itoa_buffer(char *buffer, size_t len, int num){
    static char local_buffer[sizeof(int) * 8]; // not thread safe 

    if (!buffer)
    {
        buffer = local_buffer;
        len = sizeof(local_buffer);
    }

    if (snprintf(buffer, len, "%d", num) == -1)
        return ""; // Ensure character will not modified by others
    
    return buffer;
}

const char *my_itoa(int num){
     return my_itoa_buffer(NULL, 0, num); 
}

//*********************Function: my_ctof*********************//
//Effect: Identify plus or minus of float
//Input: char(float)
//Output: float(final_result)
float my_ctof(char end_buffer[9]){

    if(end_buffer[0] != '-'){
        char result_format[8];
        memcpy(result_format, end_buffer, 8);
        return atof(result_format);
    }
    else{
        return atof(end_buffer);
    }
}


int configure_no = 0;  //Get information about particular host name

//*********************Function: server_configure*********************//
//Effect: Return INformation on a particular host name
//input: rv: ip address information 
//       ip: char(ip)
//       port: char(port)
//       socket_type: SOCK_STREAM or SOCK_DGRAM
//output: 
//       success: 0
//       error: -1
int server_configure(struct addrinfo* &rv, const char* ip, const char* port, int socket_type){
    //refer to Beej.code
    struct addrinfo hints; //parameters configuration

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;  //both ipv4 and ipv6 is fine
    hints.ai_socktype = socket_type;

    if(ip == NULL){
        hints.ai_flags = AI_PASSIVE;//self ip address
    }

    configure_no = getaddrinfo(ip,port,&hints,&rv);
    if(configure_no != 0){
        fprintf(stderr,"getaddrinfo error: %s\n",gai_strerror(configure_no));
        return 0;
    }

    configure_no = 0;
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
    struct addrinfo* p;

    for(p = rv; p != NULL; p = p->ai_next) {
        socket_descpt = socket(p->ai_family, p->ai_socktype, p->ai_protocol);

        int yes = 1;  //for setsockopt
        setsockopt(socket_descpt, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        connect(socket_descpt, p->ai_addr, p->ai_addrlen);
        break;
    }

    if(p == NULL){
        printf("fail to connect\n");
        return -1;
    }
    configure_no = 0;
    return 0;
}


//*********************Function: server_connection*********************//
//Effect: Server Connection
//input: socket_descpt: socket descriptor no.
//       port_no: char(port)
//output: 
//       success: 0
//       error: -1
int server_connection(int &socket_descpt, const char* port_no){

    struct addrinfo *rv;

    server_configure(rv, SERVER_IP, port_no, SOCK_STREAM);
    socket_establ(socket_descpt, rv);
    freeaddrinfo(rv);

    if(configure_no){
        return -1;
    }

    else{
       return 0; 
    }       
}


