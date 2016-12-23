#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include "dir.h"
#include "usage.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

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




// Here is an example of how to use the above function. It also shows
// one how to get the arguments passed on the command line.

// int main(int argc, char **argv) {

//     // This is some sample code feel free to delete it
//     // This is the main program for the thread version of nc

//     int i;
    
//     // Check the command line arguments
//     if (argc != 2) {
//       usage(argv[0]);
//       return -1;
//     }

//     // This is how to call the function in dir.c to get a listing of a directory.
//     // It requires a file descriptor, so in your code you would pass in the file descriptor 
//     // returned for the ftp server's data connection
    
//     printf("Printed %d directory entries\n", listFiles(1, "."));
//     return 0;

// }
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

void error(const char *msg)
{
    perror(msg);
    exit(1);
}
//void isListening(int socket ){
//    int val;
//    socklen_t len = sizeof(val);
//    if (getsockopt(sck, SOL_SOCKET, SO_ACCEPTCONN, &val, &len) == -1)
//        printf("fd %d is not a socket\n", sck);
//    else if (val)
//        printf("fd %d is a listening socket\n", sck);
//    else
//        printf("fd %d is a non-listening socket\n", sck);
//}


int main(int argc, char *argv[])
{
//    struct ifaddrs *ifap, *ifa;
//    struct sockaddr_in *sa;
//    char *server_address;
//    
//    getifaddrs (&ifap);
//    for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
//        if (ifa->ifa_addr->sa_family==AF_INET) {
//            sa = (struct sockaddr_in *) ifa->ifa_addr;
//            server_address = inet_ntoa(sa->sin_addr);
//            printf("Interface: %s\tAddress: %s\n", ifa->ifa_name, server_address);
//           // printf("interface %s\n", server_address);
//        }
//    }
//                   
//    
//    freeifaddrs(ifap);
    
    
     int sockfd, newsockfd, portno, data_sock, portx;
    
    socklen_t clilen;
    char buffer[256];
    char *addr;
    struct sockaddr_in serv_addr, cli_addr;
    
    
    
    
    char *server_address, *client_address;
    //client_address = inet_ntoa(cli_addr.sin_addr);
    server_address = inet_ntoa(serv_addr.sin_addr);
    //printf(client_address);
    printf(server_address);
     int n;
     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
     listen(sockfd,5);


  
     clilen = sizeof(cli_addr);
     newsockfd = accept(sockfd, 
                 (struct sockaddr *) &cli_addr, 
                 &clilen);
     if (newsockfd < 0) 
          error("ERROR on accept");
     bzero(buffer,256);
    
   // char goodLogin[] = "230 Login Successful. \n";
   // write (newsockfd, goodLogin, sizeof(goodLogin));
    int login = 5;
    int firstTime = 8;
    int PASV = 0;
    

    
    while(1){
        
    while(1)
        {

        
        if (login == 5){
            write(newsockfd, "Type USER followed by your username: ", 37);
        }
        n = read(newsockfd, buffer, sizeof(buffer));
        if (n < 0) error("ERROR reading from socket");
        //send(newsockfd, )
        //printf(buffer);
        //strtok (buffer, "\n");
        //printf(buffer);
        if (strncmp(buffer,"s",strlen("s"))==0) {
            fflush(stdout);
            fseek(stdin,0,SEEK_END);
            char goodLogin[] = "230 Login Successful. \n";
            login = 1;
            write (newsockfd, goodLogin, sizeof(goodLogin));
            //printf("asda %d", login);
            
            fseek(stdin,0,SEEK_END);
            fflush(stdout);
            fflush(stdin);
            ;
        }
        if (strncmp(buffer,"QUIT",strlen("quit"))==0 && login ==1) {
            fflush(stdout);
            fseek(stdin,0,SEEK_END);
            char quitmsg[] = "Logging out: ";
            n = write(newsockfd,quitmsg,sizeof(quitmsg));
            fseek(stdin,0,SEEK_END);
            fflush(stdout);
            fflush(stdin);
            close(newsockfd);
            close(sockfd);
            break;
            
        }
        if (strncmp(buffer,"STRU",strlen("stru"))==0 && login ==1) {
            fflush(stdout);
            fseek(stdin,0,SEEK_END);
            char strumsg[] = "STRU enabled. \n";
            n = write(newsockfd,strumsg,sizeof(strumsg));
            fseek(stdin,0,SEEK_END);
            fflush(stdout);
            fflush(stdin);
            break;
        }
        
            if (strncmp(buffer,"NLST",strlen("nlst"))==0&& login ==1 && PASV ==1) {
  
            char dirmessage[] = "226 Directory end OK.\n";
            write(newsockfd, dirmessage, sizeof(dirmessage));
            break;
            
        }
        if (strncmp(buffer,"QUIT",strlen("quit"))==0 ) {
            fflush(stdout);
            fseek(stdin,0,SEEK_END);
            printf("fucking success2");
            char quitmsg[] = "Logging out: ";
            n = write(newsockfd,quitmsg,sizeof(quitmsg));
            fseek(stdin,0,SEEK_END);
            fflush(stdout);
            fflush(stdin);
            close(newsockfd);
            close(sockfd);
            exit(0);
            
        }
            if (strncmp(buffer,"MODE",strlen("mode"))==0 && login ==1) {
            fflush(stdout);
            fseek(stdin,0,SEEK_END);
            char modemsg[] = "Stream Mode enabled\n";
            n = write(newsockfd,modemsg,sizeof(modemsg));
            fseek(stdin,0,SEEK_END);
            fflush(stdout);
            fflush(stdin);
            break;
                
            }
            if (strncmp(buffer,"RETR",strlen("retr"))==0 && login ==1) {

                if (PASV == 0){
                char no_pasv[] = "PASV not enabled\n";
                n = write(newsockfd,no_pasv,sizeof(no_pasv));
                break;
                }
                
                
//                else{
//                   
//                        
//                        int data_client = new_client(data_sock);
//                        if (data_client < 0) {
//                            printf("ERROR on accept data sock!\n");
//                            sprintf(buf, "404 Error create data sock!\n");
//                            result = send( client_sockfd, buf, (int)strlen(buf), 0);
//                        }
//                        else{ // send file
//                            sprintf(buf, "150 start send file!\n");
//                            result = send( client_sockfd, buf, (int)strlen(buf), 0);
//                            
//                            ftp_get(data_client, file_name);
//                            sprintf(buf, "226 Transfer complete!\n");
//                            send( client_sockfd, buf, (int)strlen(buf), 0);
//                        }
//                        close(data_sock); //
//                        PASV = 0;
//                    }
            }
            

                    
                    
                    
            
            
            
                
        if(strncmp(buffer,"PASV", strlen("pasv")) == 0&& login ==1){
            
            data_sock = socket(AF_INET, SOCK_STREAM, 0);
            if (data_sock < 0) error("error opening data socket\n");
            serv_addr.sin_port = htons(portx);
            //cli_addr.sin_addr = htons(
            
            
            
            PASV = 1;
            char pasv_msg[] = "Passive mode enabled ";
            send(newsockfd, pasv_msg, sizeof(pasv_msg), 0);
            send(newsockfd, server_address, sizeof(pasv_msg), 0);
            if (bind(data_sock, (struct sockaddr *) &serv_addr,
                     sizeof(serv_addr)) < 0)
                error("ERROR on binding");
            
//            listen(data_sock, 1);
//            //addr = inet_ntoa(serv_addr);
//            //printf("this is the server address%d\n", addr);
//            printf("IP address is: %s\n", inet_ntoa(serv_addr.sin_addr));
//            printf("port is: %d\n", (int) ntohs(serv_addr.sin_port));

            break;

        }

  
        
        
        if (login == 1){
            if (firstTime == 1){
            char cd500[] = "Error 500, command not supported\n";
            send(newsockfd, cd500, sizeof(cd500), 0);
                printf("ASDaadfasdfasdfasdfasdfasdf");
            }
            else {
            firstTime = 1;
            printf("firstTime %d\n", firstTime);
            printf("login %d\n", login);
            }
        }
        else {
            char retryLog[] = "Log in please \n";
            write (newsockfd, retryLog, sizeof(retryLog));
            printf("login == %d", login);
        }
        
        printf("123456");
        //clear buffer
       // fseek(stdin,0,SEEK_END);
        
        //if (n < 0) error("ERROR writing to socket");
        //break;
    }
    }


     //close(newsockfd);
     //close(sockfd);
     //return 0;
}
int new_client(int data_sock){
    listen(data_sock, 5);	
    int data_client;
    struct sockaddr_in data_client_addr;
    int llen = sizeof(data_client_addr);
    data_client = accept(data_sock, (struct sockaddr*) &data_client_addr, (socklen_t *)&llen);
    return data_client;
}










