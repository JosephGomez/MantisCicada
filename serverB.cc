
using namespace std;

#include "serverB.h"

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
    if(server_start(SERVER_B_PORT,socket_descript)){
        return -1;
    } 

    //print message
    fprintf(stderr,"The server B is up and running using UDP on port %s.\n",SERVER_B_PORT);
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
    
            fprintf(stderr,"The Server B received input <%s>\n",num_transfer);
    
            //char to float
            double dispose_data = atof(num_transfer);

            //get the first result
            result = dispose_data * dispose_data * dispose_data;
            fprintf(stderr,"The Server B calculated cube <%g> \n",result);

            //float to char to transfer
            char buffer[8];
            sprintf(buffer, "%f", result);
            
            //add backend header
            char result_str[9];      
    
            if(result_send('B', buffer, result_str)){
                continue;
            }
            
            printf("The server B finished sending the output to AWS \n");
            break;
        }
    
        while(1){
            //recieve the second data from server
            addr_size = sizeof(aws_address);
            received_len = recvfrom(socket_descript, buf2, 8, 0, (struct sockaddr *)&aws_address, &addr_size);
            if(received_len == -1){
#ifdef DEBUG
            fprintf(stderr,"receive error!:%d\n",errno);
#endif
            continue;
        }
            
            //get float of second received data
            char num_transfer_2[8];
            memcpy(num_transfer_2, buf2, 8);
            double dispose_data_2;
            dispose_data_2 = atof(num_transfer_2);

            //print the result
            fprintf(stderr,"The Server B received input <%g>\n",dispose_data_2);

            //calculate the second result
            result_2 = dispose_data_2 * dispose_data_2;
            fprintf(stderr,"The Server B calculated cube <%g> \n",result_2);
            char buffer_2[8];
            sprintf(buffer_2, "%f", result_2);
            
            char result_str_2[9]; 

            //send the second output to server
            if(result_send('B', buffer_2, result_str_2)){
                continue;
            }
            
            printf("The server B finished sending the output to AWS \n");
            break;
    
        }
    }
}



