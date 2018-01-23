using namespace std;

#include "serverC.h"

int main(){

    //variable
    struct sockaddr_storage aws_address;
    int socket_descript;//socket descriptor
    int received_len;  //received data length
    socklen_t addr_size;
    char buf[MAXCHARSIZE];  //character buffer
    char buf2[8];  //float to character buffer
    double result;  //first calculated result
    double result_2;  //second calculated result

    //boot up
    if(server_start(SERVER_C_PORT,socket_descript)){
        return -1;
    } 

    //print message
    fprintf(stderr,"The server C is up and running using UDP on port %s.\n",SERVER_C_PORT);
    while(1){
            while(1){    
            //recieve data from server
            addr_size = sizeof(aws_address);
            received_len = recvfrom(socket_descript, buf, MAXCHARSIZE, 0, (struct sockaddr *)&aws_address, &addr_size);
            if(received_len == -1){
#ifdef DEBUG
            fprintf(stderr,"receive error!:%d\n",errno);
#endif
            continue;
        }
            
            //print the message
            int length_l = buf[0] - '0';
            char num_transfer[length_l + 1];
            memcpy(num_transfer, &buf[1], length_l);
            num_transfer[length_l] = '\0';
    
            fprintf(stderr,"The Server C received input < %s >\n",num_transfer);
    
            //char to float
            double dispose_data = atof(num_transfer);

            //get the first result
            result = dispose_data * dispose_data * dispose_data * dispose_data * dispose_data;
            fprintf(stderr,"The Server C calculated 5th power <%g> \n",result);

            //float to char to transfer
            char buffer[8];
            sprintf(buffer, "%f", result);
            
            //add backend header
            char result_str[9];      
    
            if(result_send('C', buffer, result_str)){
                continue;
            }
            
            printf("The server C finished sending the output to AWS \n");
            break;
        }
    }
}