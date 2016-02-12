#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])
{
    argv[0]="2";
    argv[1]="128.111.24.43"; //my ip

    int sockfd = 0, sockdata = 0, n = 0;
    char recvBuff[1024];
    struct sockaddr_in serv_addr;
    char *response;
    char temp[1024] = "";
    char *dataip;

    char username[100]="USER anonymous\n";
    char password[100]="PASS user@localhost.localnet\n";
    char passive[100]="PASV\n";
    char filename[100]="RETR robots.txt\n";


    argc=2;

    if(argc != 2)
    {
        printf("\n Usage: %s <ip of server> \n",argv[0]);
        return 1;
    }

    memset(recvBuff, '0',sizeof(recvBuff));
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Error : Could not create socket \n");
        return 1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(21);


    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);

    if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\n Error : Connect Failed \n");
        return 1;
    }

    if( send(sockfd , username , strlen(username) , 0) < 0)
    {
        perror("send failed");
        return 1;
    }
    else
    {
        send(sockfd, password, strlen(password), 0);
    }

    send (sockfd, passive, strlen(passive), 0);

    sockdata = socket(AF_INET, SOCK_STREAM,0);
    if (connect(sockdata, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("data socket connect failed \n");
        return 1;
    }
    send (sockdata, filename, strlen(filename), 0);


    int i;
    char ipkey[] = "()";

    while ( (n = read(sockfd, recvBuff, sizeof(recvBuff)-1)) > 0)
    {
        recvBuff[n] = 0;
        strcpy(temp, recvBuff);
        //printf("%s", recvBuff);


        response = strtok(temp, " ");
        //printf("%s", response);
        if (strcmp(response, "227")==0)
        {
            i = strcspn(recvBuff, ipkey);
            dataip = &recvBuff[strlen(recvBuff) - (strlen(recvBuff)-i)];

            //dataip = strtok(temp, " ");
            
            printf("%s", dataip);
        }

        printf("%s", recvBuff);
        //printf("%s\n", response);

        //printf("%s", response);
        /*if(fputs(recvBuff, stdout) == EOF)
        {
            printf("\n Error : Fputs error\n");
            return 0;
        }*/
    }

    if(n < 0)
    {
        printf("\n Read error \n");
        return 0;
    } 

    return 0;
}

//int ()