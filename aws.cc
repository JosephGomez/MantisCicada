#include "aws.h"

int main(){

    int socket_descpt,transmit_socket_descpt,socket_descpt_r;//socket descriptor
    int len_recv;//the length of the recieved data
    struct sockaddr_storage sender_addr;
    socklen_t addr_size;
    char buf[MAXCHARSIZE], opr[3];//buffer and reduction type
    double result;
    double result_A_1, result_A_2, result_B_1, result_B_2, result_C_1;
    double final_result;

    //start server
    if(server_start(socket_descpt,socket_descpt_r)){
        return -1;
    } 

    //port listenning
    if(listen(socket_descpt, 20) == -1){
        close(socket_descpt);
#ifdef DEBUG
        fprintf(stderr, "listen error!:%d\n",errno);
#endif
        return errno;
    }
    while(1){
    //accept one link
        while(1){      
    	   addr_size = sizeof sender_addr;
    	   transmit_socket_descpt = accept(socket_descpt, (struct sockaddr *)&sender_addr, &addr_size);
    	   if(transmit_socket_descpt == -1){
        	   continue;
    	   }
    	   else{
    		    fflush(stdout);
          		
                  //received data type: operator(3 char) + length of data(1 char) + data(MAXCHARSIZE char)
    		    len_recv = recv(transmit_socket_descpt, &buf[0], MAXCHARSIZE, 0);
            if(len_recv == -1){
                   if(errno == EAGAIN || errno == EWOULDBLOCK) continue;
                      fprintf(stderr,"ERROR RECEIVING!!:%d\n",errno);
                      continue;
                  }
                  if(len_recv == 0){
                      fprintf(stderr, "socket closed by the client\n");
                      continue;
                  }
      
                  //copy operator
        	    memcpy(opr,buf,3);
      
                  //copy the length of data(int) and transfer to char
        	     int length_l = buf[3] - '0';
        	     char num_transfer[length_l + 1];
      
                  //copy the data(char)
        	     memcpy(num_transfer, &buf[4], length_l);
                  //To show the string add end symbol
        	     num_transfer[length_l] = '\0';
      
                  //transfer the data(char) to result(float)
        	     char num_transfer_2[length_l];
        	     memcpy(num_transfer_2, num_transfer, length_l);
        	     result = atof(num_transfer_2);
              		

                  	
                  if(opr[0] == 'D' || opr[0] == 'L'){	
                      printf("The AWS received input <%s> and function = %c%c%c from the client using TCP over port %s\n",num_transfer, opr[0], opr[1], opr[2], AWS_TCP_PORT);	 
    		    } 
      
                  //send and receive data
                printf("The AWS sent <%s> numbers to Backend­-Server A\n", num_transfer);
                printf("The AWS sent <%s> numbers to Backend­-Server B\n", num_transfer);
                printf("The AWS sent <%s> numbers to Backend­-Server C\n", num_transfer);

              //send and receive data
              int i;
    		    for(i = 1; i < 4; i++){
    			   if(distribute_character(&buf[3], length_l + 1, i)){
    				      close(transmit_socket_descpt);
    				      continue;
    			   }
            
                  switch(i){
                          case 1:{
                          result_A_1 = receive_X(socket_descpt_r);
                          distribute_float(result_A_1, 1);                     
                          result_A_2 = receive_X(socket_descpt_r);
                          break;                       
                          }                   
                          
                          case 2:{
                          result_B_1 = receive_X(socket_descpt_r);
                          distribute_float(result_B_1, 2);
                          result_B_2 = receive_X(socket_descpt_r);
                          break;
                          }
      
                          case 3:{
                          result_C_1 = receive_X(socket_descpt_r);
                          break;                       
                          }                   
                    }    			   			
    		}
    		//print the result
              printf("The AWS received <%g> Backend-Server<A> using UDP over port <%s>\n", result_A_1, AWS_UDP_PORT);
              printf("The AWS received <%g> Backend-Server<B> using UDP over port <%s>\n", result_B_1, AWS_UDP_PORT);
              printf("The AWS received <%g> Backend-Server<A> using UDP over port <%s>\n", result_C_1, AWS_UDP_PORT);
              printf("The AWS sent <%g> numbers to Backend­-Server A\n", result_A_1);
              printf("The AWS sent <%g> numbers to Backend­-Server B\n", result_B_1);
              printf("The AWS received <%g> Backend-Server<A> using UDP over port <%s>\n", result_A_2, AWS_UDP_PORT);
              printf("The AWS received <%g> Backend-Server<B> using UDP over port <%s>\n", result_B_2, AWS_UDP_PORT);
    		  printf("Values of powers received by AWS: <%g, %g, %g, %g, %g, %g>\n", result, result_A_1, result_B_1, result_A_2, result_C_1, result_B_2);
    
                //get the final result
    		  char result_final_char[8];
    		  if(opr[0] == 'D'){
    			 final_result = 1 + result + result_A_1 + result_B_1 + result_A_2 + result_C_1 + result_B_2;
    			 printf("AWS calculated DIV on <%g>: <%g>\n", result, final_result);
    		  }
    		  else{
    			 final_result = -result - result_A_1/2.0 - result_B_1/3.0 - result_A_2/4.0 - result_C_1/5.0 - result_B_2/6.0;
    			 printf("AWS calculated LOG on <%g>: <%g>\n", result, final_result);
    		  }
    
    		  //transfer float to char
    		  sprintf(result_final_char, "%f", final_result);
    		  len_recv = send(transmit_socket_descpt,result_final_char,8,0);
    		  if(len_recv == -1){
#ifdef DEBUG
            fprintf(stderr,"fail to send to client\n");
#endif
            close(transmit_socket_descpt);
            continue;
        }
    		  printf("The AWS sent <%g> to client.\n", final_result);
                close(transmit_socket_descpt);
                continue;
            }

           		
    }   	
}
}
 

    
    	
    	
   
    		

    
    
 	       
        