#include "client.h"

int main(int argc, char* argv[]){
//argc must be equal to 3. 
//argv[1] should be one of DIV, LOG, which stand for 2 operation(all capital letter)
//return 0 on success

    //check input is legal or not
    if( 
    	(strncasecmp(argv[1],"LOG",3)&& strncasecmp(argv[1],"DIV",3))
        || (strlen(argv[1]) > 3)
        || (argc != 3)
       ){
        printf("INPUT WRONG!!\n");
        exit(1);
    }

    //print the message
    printf("The client is up and running\n");

    //server connect
    int socket_descriptor = 0;
    if(server_connection(socket_descriptor, SERVER_PORT_NO)) {
    	return -1;
    }

    //send the data
    //In this attempt, we put the real length of char(float) in the header
	send(socket_descriptor,argv[1],3,0);
	send(socket_descriptor, my_itoa(strlen(argv[2])), strlen(my_itoa(strlen(argv[2]))), 0);
    send(socket_descriptor,argv[2],strlen(argv[2]),0);

    printf("The client sent <%s> and <%s> to AWS\n",argv[2], argv[1]);

    char end_buffer[9];  //receive results
    recv(socket_descriptor, end_buffer, 8, 0);

    float final_result = my_ctof(end_buffer);

    printf("According to AWS <%s> on <%s>: <%f>\n", argv[1], argv[2], final_result);
}  