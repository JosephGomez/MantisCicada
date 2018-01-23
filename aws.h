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
#include <sys/time.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>

//Back-end Servers IP and Port NO.
#define SERVER_A_IP "127.0.0.1"
#define SERVER_B_IP "127.0.0.1"
#define SERVER_C_IP "127.0.0.1"
#define SERVER_A_PORT "77770"
#define SERVER_B_PORT "77771"
#define SERVER_C_PORT "77772"

//AWS Server TCP and UDP Port NO.
#define AWS_TCP_PORT "77773"
#define AWS_UDP_PORT "77774"

#define MAXCHARSIZE 20

//the information about error will be saved in the variable configure_no
int error_num = 0;

//*********************Function: my_itoa*********************//
//Effect: Transform an int to a char
//Input: int num
//Output: char num
//refer to https://stackoverflow.com/questions/2279379/how-to-convert-integer-to-char-in-c
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
    //refer to Beej.code and suggestion from TA Pranav Sakulkar and friend Lu Zao
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
    //refer to Beej.code and suggestion from TA Pranav Sakulkar and friend Lu Zao
    //Establish a socket
    socket_descpt = socket(rv->ai_family, rv->ai_socktype, rv->ai_protocol);
    if(socket_descpt == -1){
        perror("client: socket");
    }

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
int server_start(int &socket_descpt, int &socket_descpt_r){

    struct addrinfo *rv;

    //boot up the TCP recieve rv
    server_configure(rv, NULL, AWS_TCP_PORT, SOCK_STREAM);
    socket_establ(socket_descpt, rv);

    //boot up the UDP recieve rv
    server_configure(rv, NULL, AWS_UDP_PORT, SOCK_DGRAM);
    socket_establ(socket_descpt_r, rv);
    freeaddrinfo(rv);

    if(error_num) return -1;

    printf("The AWS is up and running.\n");
    return 0;
}

//*********************Function: distribute_character*********************//
//Effect: Transfer float number as character
//input: num: transfer character
//       len: the length of data inside
//       port_NO: port_NO server wants to receive from
//output: 
//       success: 0
//       error: -1
int distribute_character(char* num, int len, int port_NO){

    struct addrinfo *backend_serv;  //backend_serv is from foward to backend.
    int socket_descpt;  //socket descriptor
    int error_check;  //check message has been sent or not 
     
    //copy the data
    int data_length = len - 1;
    char buff[len]; 
    memcpy(buff, &num[1], data_length);
    buff[data_length] = '\0';  //for showing float data
    char *message_transfer = num;
    char buff_2[data_length];

    //get address info
    if(port_NO == 1)
        server_configure(backend_serv, SERVER_A_IP, SERVER_A_PORT, SOCK_DGRAM);
    if(port_NO == 2)
        server_configure(backend_serv, SERVER_B_IP, SERVER_B_PORT, SOCK_DGRAM);
    if(port_NO == 3)
        server_configure(backend_serv, SERVER_C_IP, SERVER_C_PORT, SOCK_DGRAM);
    
    //socket establishment
    struct addrinfo *p;
        for(p = backend_serv; p != NULL; p = p->ai_next) {
            if ((socket_descpt = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
                perror("talker: socket");
                continue;
            }
            break;
        }        

    //send
        error_check = sendto(socket_descpt, message_transfer, len, 0, backend_serv->ai_addr, backend_serv->ai_addrlen);
        if(error_check == -1){
            printf("send in error!");
            close(socket_descpt);
            freeaddrinfo(backend_serv);
            return -1;
        }
    
    close(socket_descpt);
    freeaddrinfo(backend_serv);
    return 0;    
}

//*********************Function: distribute_float*********************//
//Effect: Transfer float number as character
//input: num: transfer float
//       port_NO: port_NO server wants to receive from
//output: 
//       success: 0
//       error: -1
int distribute_float(double num, int port_NO){

    struct addrinfo *backend_serv;  //backend_serv is from foward to backend.
    int socket_descpt;  //socket descriptor
    int error_check;  //check message has been sent or not

    //transform char to float
    char num_r[9];
    sprintf(num_r, "%f", num);

    //get address info
    if(port_NO == 1)
        server_configure(backend_serv, SERVER_A_IP, SERVER_A_PORT, SOCK_DGRAM);
    if(port_NO == 2)
        server_configure(backend_serv, SERVER_B_IP, SERVER_B_PORT, SOCK_DGRAM);
    if(port_NO == 3)
        server_configure(backend_serv, SERVER_C_IP, SERVER_C_PORT, SOCK_DGRAM);
    

    //establish socket
    struct addrinfo *p;
    for(p = backend_serv; p != NULL; p = p->ai_next) {
        if ((socket_descpt = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
            continue;
        }
        break;
    }        

    //send
    error_check = sendto(socket_descpt, num_r, 8, 0, backend_serv->ai_addr, backend_serv->ai_addrlen);
    if(error_check == -1){
        close(socket_descpt);
        freeaddrinfo(backend_serv);
        return -1;
    }
    
    close(socket_descpt);
    freeaddrinfo(backend_serv);
    return 0;    
}

//*********************Function: print_result*********************//
//Effect: transfer char to float
double print_result(char* buf){
    char buffer[8];
    memcpy(buffer, &buf[1], 8);
    return atof(buffer);
}


//*********************Function: receive_X*********************//
//Effect: Receive data(char) from backend server
//input: socket_descpt_r, socket descriptor recieve data from backend servers
//output: 
//       receive_X: receive dat(float)
double receive_X(int socket_descpt_r){


    socklen_t size_of_addr;
    char buf[MAXCHARSIZE];
    struct sockaddr_storage sender_addr;
    double result_A, result_B, result_C;  //received result from backend A, B, C
    bool A = 1, B = 1, C = 1;

    //Get result from one of backend server
    while(A || B || C){
        size_of_addr = sizeof(sender_addr);
        //receive data
        int error_check;
        error_check = recvfrom(socket_descpt_r, buf, MAXCHARSIZE, 0, (struct sockaddr *)&sender_addr, &size_of_addr);
        if(error_check == -1){
            if(errno == EAGAIN || errno == EWOULDBLOCK) continue;
            return -1;
        }
        else{
            fflush(stdout);
            switch(buf[0]){
                case 'A':
                    result_A = print_result(buf);
                    return result_A;
                    break;
                case 'B':
                    result_B = print_result(buf);
                    return result_B;
                    break;
                case 'C':
                    result_C = print_result(buf);
                    return result_C;
                    break;
            }
        }
    }
}






    
    