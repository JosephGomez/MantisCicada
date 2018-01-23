================================================================================
                         EE450 Socket Project README
================================================================================


================================================================================
I have created a socket system to module talor series algorithm.

client.cc client.h(client side): user enters a function and a float which is smaller than 1 to get the result.

aws.cc aws.c(center server side): It receives the data from client. Send and received the related results from three back-end server and perform some necessary algorithm on these results. Finally, send back to server.

serverA.cc serverA.h/serverB.cc serverB.h/serverC.cc serverC.h(back-end server): Calculate received data from aws and send the result back.

================================================================================
format of all messages exchnged:
client to aws: char 
function + length of data + data(e.g: ./client DIV 0.3 -> DIV30.3)

aws to backend server(1st time):char 
length of data + data(e.g: 0.09->40.09)

aws to backend server(2nd time):char 
data(e.g: 0.09->0.090000)

backend server to aws:char
data(e.g: 0.09->0.090000)

aws to client: char 
data(e.g: 0.09->0.090000)

================================================================================
no idiosyncrasy detected so far

================================================================================
Reused code:
Some code I refer to Beej's code (mainly) and refer some of suggestions on how to build these function from TA Pranav Sakulkar and friend Lu Zao. 
These codes include in each .h file:
int server_configure(struct addrinfo* &configure_no, const char* ip, const char* port, int socket_type)
int socket_establ(int &socket_descpt, struct addrinfo* &rv)

Another code block refer from stackoverflow(The basic idea is the same):
char *my_itoa_buffer(char *buffer, size_t len, int num)
refer to https://stackoverflow.com/questions/2279379/how-to-convert-integer-to-char-in-c

================================================================================
Especially thanks for TA Pranav Sakulkar and other friends' help!