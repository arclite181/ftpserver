#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include "dir.h"
#include "usage.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stddef.h>
#include <time.h>


//Connected to ftp.cs.ubc.ca
//220

//Username:

//230 Login successful

//for NLIST:
//150 Here comes the directory listing.
//a list of all the dir
//226 Directory end OK.

//type
//200 Using binary mode to transfer files.

//mode
//200 Stream mode activated

//nlist
//if error then: 501 invalid paramater, no parameter required.





#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <ifaddrs.h>

#define PASV_MODE 0
#define FTP_BUF_SIZE 1024

void error(const char *msg)
{
    perror(msg);
    exit(1);
}




//parses user input and divides the commands by blank spaces and inputs them in command array
//returns num of arguments given by user
int lineParser(char *line, char *command_array[]) {
    char *p;
    int count=0;
    p = strtok(line, " \r\n"); // divide string into different pieces
    while (p != 0 ) {
        command_array[count++] = p;
        p = strtok(NULL," \r\n");// continue to divide the string if not empty
    }
    return count;
}

int main(int argc, char *argv[])
{
    //struct for getting interface ip addresses
    struct ifaddrs *ifap, *ifa;
    struct sockaddr_in *sa;
    char *server_address;
    
    getifaddrs (&ifap);
    //get ip addresses of all interfaces (we only need 1 for pasv)
    for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr->sa_family==AF_INET) {
            sa = (struct sockaddr_in *) ifa->ifa_addr;
            server_address = inet_ntoa(sa->sin_addr);
        }
    }
    freeifaddrs(ifap);
    
    int sockfd, newsockfd, portno, data_sock;
//    void *thread1 = Thread_create(entrypoint, NULL)
//    Thread_join(thread1, NULL)
    
    //seed random variable
    srand(time(0));
    int portx = (rand()%65535+ 1025);
    socklen_t clilen;
    char buffer[256];
    char *addr;
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    
    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
    
    //make a socket for connection
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;//will listen on any of its interfaces
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr,//bind
             sizeof(serv_addr)) < 0)
        error("ERROR on binding");
    listen(sockfd,5);//listen on the created socket
    
    
    //Connection socket, for communication
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd,
                       (struct sockaddr *) &cli_addr,
                       &clilen);
    if (newsockfd < 0)
        error("ERROR on accept");
    bzero(buffer,256);
    
    int login = 5;//Not logged, and trying for first time
    int firstTime = 8;
    int PASV = 0;
    
    while(1){
        while(1){
            int *copy;
            int line;
            if (login == 5){//intial display
                char loginmsg1[] = "220 connection succesful. Type USER followed by your username: \n";

                write(newsockfd, loginmsg1, sizeof(loginmsg1));
                
            }
            //array for commands parsed by lineparser method
            char *command_array[4] = {0};
            n = read(newsockfd, buffer, sizeof(buffer));
            if (n < 0) error("ERROR reading from socket");
            line = lineParser(buffer, command_array);
            
            //comparses  command and user  cs317 and doesn't care about case
            if (strcasecmp(command_array[0], "User") == 0 && strcasecmp(command_array[0], "cs317") &&
                line == 2){
                fflush(stdout);//clear buffer
                char goodLogin[] = "230 Login Successful. \n";
                login = 1;
                write (newsockfd, goodLogin, sizeof(goodLogin));
                fflush(stdout);
                break;
                
            }
            
            //Type A, Type I, Type A N
            if (strcasecmp(command_array[0],"Type") == 0 && login ==1) {
                
                
                if (strcasecmp(command_array[1], "i") ==0 ){
                    //set type to I
                    char typamsg[] = "200 Type set to type Image. \n";
                    send(newsockfd, typamsg, sizeof(typamsg), 0);
                    
                }
                else if (strcasecmp(command_array[1], "A") ==0 && strcasecmp(command_array[2],"N")==0){
                    //set type to A N
                    char typamsg[] = "200 Type set to type Ascii Nonprint \n";
                    send(newsockfd, typamsg, sizeof(typamsg), 0);
                    
                }
                else if (strcasecmp(command_array[1], "a") ==0 ){
                    //set type to A
                    char typamsg[] = "200 Type set to type Ascii \n";
                    send(newsockfd, typamsg, sizeof(typamsg), 0);
                }
                
                else {
                    char typecheck[] = "Type, check incorrect arguments\n";//If not any of the above commands
                    send(newsockfd, typecheck, sizeof(typecheck), 0);
                }
                
                break;
            }
            //User command = 'QUIT'
            //close socket
            if (strcasecmp(command_array[0], "quit")==0) {
                fflush(stdout);
                fseek(stdin,0,SEEK_END);
                char quitmsg[] = "Logging out: \n";
                n = write(newsockfd,quitmsg,sizeof(quitmsg));
                fseek(stdin,0,SEEK_END);
                fflush(stdout);
                close(newsockfd);
                close(sockfd);
                break;
                
            }
            //User command = STRU
            //only supported type is FILE
            if (strncmp(buffer,"STRU",strlen("stru"))==0 && login ==1) {
                fflush(stdout);
                fseek(stdin,0,SEEK_END);
                char strumsg[] = "200 Structure set to FILE. \n";
                n = write(newsockfd,strumsg,sizeof(strumsg));
                fseek(stdin,0,SEEK_END);
                fflush(stdout);
                fflush(stdin);
                break;
            }
            //User command is NLST
            //If in PASV mode, call list files to display dir files through data socket
            if (strcasecmp(command_array[0], "nlst")==0 && login == 1 && PASV ==1) {
                
                int nlst_sock;
                struct sockaddr_in data_client_addr;
                socklen_t llen = sizeof(data_client_addr);
                nlst_sock = accept(data_sock, (struct sockaddr*) &data_client_addr, &llen);
                
                
                
                
                //Listen to data socket, if no connection is made within
                //20 seconds it times out
                if (listen (data_sock, 4) <0 )
                {
                    perror("listen");
                    break;
                }
                fd_set set;
                struct timeval timeout;
                int rv;
                FD_ZERO(&set); /* clear the set */
                FD_SET(data_sock, &set); /* add our file descriptor to the set */
                
                timeout.tv_sec = 150;
                timeout.tv_usec = 0;
                
                rv = select(data_sock + 1, &set, NULL, NULL, &timeout);
                if(rv == -1)
                {
                    perror("select"); /* an error accured */
                    //return 1;
                    break;
                }
                else if(rv == 0)
                {
                    printf("timeout occurred (120 second) \n"); /* a timeout occured */
                    //return 1;
                    break;
                }
                
                char dirlistmsg[] = "150 Here comes the directory list.\n";
                write(newsockfd, dirlistmsg, sizeof(dirlistmsg));
                
                listFiles(nlst_sock, ".");
                char dirmessage[] = "226 Directory end OK.\n";
                write(newsockfd, dirmessage, sizeof(dirmessage));
                close(nlst_sock);
                break;
                
            }
            //User command is 'MODE'
            //Only supported mode is Stream.
            if (strcasecmp(command_array[0], "mode") == 0 && login ==1){
                fflush(stdout);
                fseek(stdin,0,SEEK_END);
                char modemsg[] = "200 Stream mode enabled\n";
                n = write(newsockfd,modemsg,sizeof(modemsg));
                fseek(stdin,0,SEEK_END);
                fflush(stdout);
                fflush(stdin);
                break;
                
            }
            //USER command is RETR followed by file name
            //If pasv is enabled, the file is sent through a datasocket
            //the socket accepts here
            
            if (strcasecmp(command_array[0], "retr") == 0 && login ==1
                && PASV ==1){
                //counts to see how many arguments there are, if there are more than 2 then error
                
                if (line != 2){
                    
                    fprintf(stderr, "%i", line);
                    fflush(stderr);
                    char cd500[] = "Error 500 incorrect arguments\n";
                    send(newsockfd, cd500, sizeof(cd500), 0);
                }
                int retr_sock;
                struct sockaddr_in data_client_addr;
                
                //Listen to data socket, if no connection is made within
                //20 seconds it times out
                if (listen (data_sock, 4) <0 )
                {
                    perror("listen");
                    break;
                }
                fd_set set;
                struct timeval timeout;
                int rv;
                FD_ZERO(&set); /* clear the set */
                FD_SET(data_sock, &set); /* add our file descriptor to the set */
                
                timeout.tv_sec = 150;
                timeout.tv_usec = 0;
                
                rv = select(data_sock + 1, &set, NULL, NULL, &timeout);
                if(rv == -1)
                {
                    perror("select"); /* an error accured */
                    //return 1;
                    break;
                }
                else if(rv == 0)
                {
                    printf("timeout occurred (120 second) \n"); /* a timeout occured */
                    //return 1;
                    break;
                }
                
                else{
                    //sending a copy of file
                    socklen_t rlen = sizeof(data_client_addr);
                    retr_sock = accept(data_sock, (struct sockaddr*) &data_client_addr, &rlen);
                    
                    FILE *file;
                    file = fopen(command_array[1], "rb");//user specified file
                    if(file == NULL){
                        printf("Error, the given filename does not exist\n");
                        close(retr_sock);
                        return 0;
                    }
                    
                    int file_len;
                    fseek(file, 0, SEEK_END);
                    file_len = ftell(file);
                    fseek(file, 0, SEEK_SET);

                    int read_len;
                    char read_buf[1024];//of size 1024
                    do{
                        read_len = fread(read_buf, sizeof(char), 1024, file);
                        
                        if (read_len > 0){
                            send(retr_sock, read_buf, read_len, 0);
                            file_len -= read_len;
                        }
                        bzero(read_buf, FTP_BUF_SIZE);
                    } while ((read_len > 0) && (file_len > 0));
                    
                    fclose(file);
                    close(retr_sock);
                    //return 1;
                }
            }
            
            //User command is PASV
            //Creates a Data socket and enters passive mode by listening
            //displays server address and port # of socket (pasv1)
            if(strcasecmp(command_array[0], "pasv")==0 && login == 1){
                struct sockaddr_in data_client_addr;
                data_sock = socket(AF_INET, SOCK_STREAM, 0);
                if (data_sock < 0) error("error opening data socket\n");
                bzero((char *) &data_client_addr, sizeof(data_client_addr));
                serv_addr.sin_family = AF_INET;
                data_client_addr.sin_port = 0;//Randomized port in allowable port range
                data_client_addr.sin_addr.s_addr = INADDR_ANY;
                
                if (bind(data_sock, (struct sockaddr *) &data_client_addr,
                         sizeof(data_client_addr)) < 0)
                    error("ERROR on binding");
                
                socklen_t len = sizeof(data_client_addr);
                if(getsockname(data_sock, (struct sockaddr *) &data_client_addr
                               , &len) < 0)
                    error("ERROR on getting socket port number");
                
                portx = ntohs(data_client_addr.sin_port);
                
                int div;
                div = (portx/256);//converting  p1 and p2 concatenated to ip address of server
                int mod;
                mod = (portx%256);
                
                char* ip0 = strtok(server_address, ".");
                char* ip1 = strtok(NULL, ".");
                char* ip2 = strtok(NULL, ".");
                char* ip3 = strtok(NULL, ".");
                
                char newbuf[100] = {'\0'};
                
                sprintf(newbuf, "227 Passive mode enabled (%s,%s,%s,%s,%i,%i)\r\n", ip0,ip1,ip2,ip3, div,mod);
                send(newsockfd, newbuf, strlen(newbuf),0);
                char addressmsg[] = "port";
                fflush(stdout);
                
                //198.162.33.46
                PASV = 1;
                //char pasv_msg[] = "227 Passive mode enabled. \n";
                //send(newsockfd, pasv_msg, sizeof(pasv_msg), 0);
                
                listen(data_sock, 1);
                break;
                
            }
            if (login == 1){
                if (firstTime == 1){
                    char cd500[] = "Error 500, command not supported\n";//If not any of the above commands
                    send(newsockfd, cd500, sizeof(cd500), 0);
                }
                else {
                    firstTime = 1;
                }
            }
            else {
                char retryLog[] = "Log in please \n";//Error 500 not displayed on incorrect log in
                write (newsockfd, retryLog, sizeof(retryLog));
            }
            
        }
    }
    
}











