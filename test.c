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

//char * convertIP(char *msg);

int main(int argc, char *argv[])
{
    argv[0]="2";
    argv[1]="128.111.24.43"; //my ip

    char *ip_address = argv[1];
    int port = 21;

    int sockfd = 0, sockdata = 0, n = 0;
    char recvBuff[1024];
    struct sockaddr_in serv_addr;
    char *response;
    char *dataip;

    char username[100]="USER anonymous\n";
    char password[100]="PASS user@localhost.localnet\n";
    char passive[100]="PASV\n";
    char filename[100]="RETR robots.txt\n";
    //char temp[1024] = "";


    argc=2;

    if(argc != 2)
    {
        printf("\n Usage: %s <ip of server> \n",argv[0]);
        return 1;
    }


    /*
     *
     * * * * * CREATE SOCKET
     *
     */


    memset(recvBuff, '0',sizeof(recvBuff));
    if((sockfd = createSocket()) < 0)
    {
        printf("\n Error : Could not create socket \n");
        return 1;
    }

         
    /*
     *
     * * * * * CONNECT TO FTP SERVER
     *
     */

    if ( connectServer(sockfd, ip_address, port) < 0)
    {
        printf("\n Error : Connect Failed \n");
        return 1;
    }

    /*
     *
     * * * * * LOG IN TO FTP SERVER
     *
     */
    /*if ( send(sockfd , username , strlen(username) , 0) < 0)
    {
        perror("send failed");
        return 1;
    }
    else
    {
        send(sockfd, password, strlen(password), 0);
    }

    send (sockfd, passive, strlen(passive), 0);*/

    //sockdata = socket(AF_INET, SOCK_STREAM,0);
    //send (sockdata, filename, strlen(filename), 0);

    /*
     *
     * * * * * READ MESSAGES TO FTP SERVER
     *
     */

    int i;
    char ipkey[] = "()";
    char temp[1024] = "";
    char temp2[1024] = "";

    while ( (n = read(sockfd, recvBuff, sizeof(recvBuff)-1)) > 0)
    {
        recvBuff[n] = 0;
        strcpy(temp, recvBuff);
        strcpy(temp2, recvBuff);
        response = strtok(temp, " ");
        //            printf("%s", response);
        if (strcmp(response, "227")==0)
        {
            i = strcspn(recvBuff, ipkey);
            dataip = &temp2[strlen(recvBuff) - (strlen(recvBuff)-i)];
            
            startDataSocket(dataip);
            
            //dataip = strtok(temp, " ");
            //dataip++;
            
            //printf("%s", dataip);
        }

        else if (strcmp(response, "220")==0)
        {
            sendMessage(sockfd, username);
        }

        else if (strcmp(response, "331")==0)
        {
            sendMessage(sockfd, password);

        }

        else if (strcmp(response, "230")==0)
        {
            sendMessage(sockfd, passive);
            
        }

        printf("%s", recvBuff);
        //printf("%s\n", response);
    }

    if(n < 0)
    {
        printf("\n Read error \n");
        return 0;
    } 

    return 0;
}

int connectServer(int socket, char *ip, int port)
{
    struct sockaddr_in serv_addr;

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = inet_addr(ip);

    return connect(socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
}

int createSocket()
{
    return socket(AF_INET, SOCK_STREAM, 0);
}

int sendMessage(int socket, char *msg)
{
    return send(socket , msg , strlen(msg) , 0);
}

int startDataSocket(char *msg)
{
    int port = convertPortNo(msg);
    char ip[strlen(msg)];
    char *address;
    int i = 0, count = 0;
    
        
    strcpy(ip, msg);

    while (ip[i] != '\0')
    {
        if (ip[i] == ',')
        {
            ip[i] = '.';
            count++;
        }
        if (count == 4)
        {
            ip[i] = '\0';
        }
        i++;
    }
    
    i = 0;
    count = 0;

    address = ip + 1;

    printf("%s\n", address);
    printf("%i\n", port);


}


int convertPortNo(char *msg)
{
    int size = strlen(msg), a = 0, b = 0, port = 0;
    char portString[size], first[size], second[size];
    char *temp;

    strcpy(portString, msg);

    //printf("THE PORTSTRING IS: %s\n", portString);

    int i = 0, j = 0, count = 0;

    while (portString[i] != '\0')
    {
        if (portString[i] == ',')
            count++;

        if (count >= 4)
        {
            first[j] = portString[i];
            j++;
        }

        i++;
    }
    
    first[j-4] = '\0';
    temp = first + 1;
    strcpy(first, temp);
    strcpy(portString, first);

    i = 0;
    count = 0;
    while (portString[i] != '\0')
    {
        if (portString[i] == ',')
        {
            first[i] = '\0';
            j = i;
            count++;
            i++;
            continue;
        }

        if (count == 0)
            first[i] = portString[i];
        else
        {
            second[i-j-1] = portString[i];
        }
        i++;
    }
    second[j] = '\0';

    a = toInt(first);
    b = toInt(second);

    port = (a * 256) + b;

    //printf("THE CONVERTED PORT IS: %i    \n", port);

    return port;
}

int toInt(char *num)
{
    int index = 0;
    int number = 0;
    while(num[index] != '\0'){
        if(num[index] >= '0' && num[index] <= '9'){
            number = number*10 + num[index] - '0';
        } else {
            break;
        }
        index++;
    }

    return number;

}