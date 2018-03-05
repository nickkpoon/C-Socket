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

    char logfile[100]="";
    int iflog = 0;
    int ifstd = 0;


int main(int argc, char *argv[])
{
    //argv[0]="2";
    //argv[1]="128.111.24.43"; //my ip

    char *ip_address = "";
    int port = 21;

    int sockfd = 0, sockdata = 0, n = 0;
    char recvBuff[1024];
    struct sockaddr_in serv_addr;
    char *response;
    char *dataip;

    char username[100]="USER ";
    char password[100]="PASS ";
    char passive[100]="PASV\n";
    char active[100]="PORT ";
    char type[100]="TYPE ";
    char fileName[100]="RETR ";
    char sizeMsg[100]="SIZE ";
    char quit[100]="QUIT\n";
    char localfile[100]="";

    char hostname[100] = "";
    int arg;

 
    int ifname = 0;
    int ifpass = 0;
    int iftype = 0;
    int ifactive = 0;
    for(arg = 0; arg < argc; ++arg)
    {   
        if(strcmp(argv[arg], "-h") == 0 || strcmp(argv[arg], "--help") == 0) //helpfile flag
        {
            printf("USAGE:[-f file] or [--file file]\nSpecifies the file to download.\n[-s hostname] or [--server hostname]\nSpecifies the server to download the file from.\n[-p port] or [--port port]\nSpecifies the port to be used when contacting the server. (default value: 21).\n[-n user] or [--username user]\nUses the username user when logging into the FTP server (default value: anonymous).\n[-P password] or [--password password]\nUses the password password when logging into the FTP server (default value: user@localhost.localnet).\n-a or --active\nForces active behavior (the server opens the data connection to the client) (default behavior: passive behavior).\n[-m mode] or [--mode mode]\nSpecifies the mode to be used for the transfer (ASCII or binary) (default value: binary).\n[-l logfile] or [--log logfile]\n");
            exit(0);
        }

        if(strcmp(argv[arg], "-v") == 0 || strcmp(argv[arg], "--version") == 0) //version flag
        {
            printf("MFTP\nVersion: 0.1\nBy:Nicholas Poon");
            exit(0);
        }

        if(strcmp(argv[arg], "-f") == 0 || strcmp(argv[arg], "--file") == 0) //set filename
        {
            strcat(localfile, argv[arg+1]);

            strcat(fileName, argv[arg+1]);
            strcat(fileName, "\n");

            strcat(sizeMsg, argv[arg+1]);
            strcat(sizeMsg, "\n");
            //printf("RETRIEVE::%s\n", fileName);
        }

        if(strcmp(argv[arg], "-s") == 0 || strcmp(argv[arg], "--server") ==0) //set hostname
        {
            //printf("%s\n", argv[arg+1]);
            ip_address=argv[arg+1];

        }

        if(strcmp(argv[arg], "-p") == 0 || strcmp(argv[arg], "--port")==0) //set port number 
        {
            //printf("%s\n", argv[arg+1]);
            port = toInt(argv[arg+1]);

        }

        if(strcmp(argv[arg], "-n") == 0 || strcmp(argv[arg], "--username")==0)
        {
            ifname++;
            //printf("%s\n", argv[arg+1]);
            strcat(username, argv[arg+1]);
            strcat(username, "\n");

        }

        if(strcmp(argv[arg], "-P") == 0 || strcmp(argv[arg], "--password")==0)
        {
            ifpass++;
            //printf("%s\n", argv[arg+1]);
            strcat(password, argv[arg+1]);
            strcat(password, "\n");

        }

        if(strcmp(argv[arg], "-a") == 0 || strcmp(argv[arg], "--active")==0)
        {
            ifactive++;

        }

        if(strcmp(argv[arg], "-m") == 0 || strcmp(argv[arg], "--mode")==0)
        {
            iftype++;
            strcat(type, argv[arg+1]);
            strcat(type, "\n");
        }

        if(strcmp(argv[arg], "-l") == 0 || strcmp(argv[arg], "--log")==0)
        {
            if (strcmp(argv[arg+1], "-")==0)
                ifstd++;
            else
                iflog++;

            strcat(logfile, argv[arg+1]);
        }


        //printf("Argument %d : %s\n", arg, argv[arg]);
    }


    if (ifname == 0)
    {
        strcat(username, "anonymous\n");
    }

    if (ifpass == 0)
    {
        strcat(password, "user@localhost.localnet\n");
    }

    if (iftype == 0)
    {
        strcat(type, "I\n");
    }
    //return 0;

/*
 *
 * * * * * CREATE CLIENT SOCKET, CONNECT CLIENT TO FTP SERVER
 *
 */

    memset(recvBuff, '0',sizeof(recvBuff));
    sockfd = createSocket();
 
    connectServer(sockfd, ip_address, port);
    
/*
 *
 * * * * * ACTIVE MODE 
 *
 */   
  
    struct sockaddr_in sin;
        socklen_t len = sizeof(sin);
    if (getsockname(sockfd, (struct sockaddr *)&sin, &len) == -1)
    perror("getsockname");

    int activeport = ntohs(sin.sin_port);
     /*printf("port number %d\n", activeport);
     printf("first half = %d\n", activeport/256);
     printf("second half = %d\n", activeport%256);*/
    
    int z;
    char buf[32];
    char firstport[32];
    char secondport[32];
    struct hostent *he;


    char *activeip;

    z = gethostname(buf,sizeof buf);

    if ( z == -1 ) 
    {
        fprintf(stderr, "%s: gethostname(2)\n",
        strerror(errno));
        exit(1);
    }

    if ( (he = gethostbyname(buf) ) == NULL ) 
    {
      exit(1); 
    }

    activeip = inet_ntoa(*(struct in_addr *)&he->h_addr);
    sprintf(firstport, "%d", activeport/256);
    sprintf(secondport, "%d", activeport%256);
    strcat (activeip, ".");
    strcat (activeip, firstport);
    strcat (activeip, ".");
    strcat (activeip, secondport);

    strcat (active, activeip);
    strcat (active, "\n");

    /*printf("%s", active);
/*
 *
 * * * * * SEND/READ MESSAGES TO FTP SERVER
 *
 */
    int i, j, k, l, size = 1024;
    char ipkey[] = "()";
    char temp[1024] = "";
    char temp2[1024] = "";
    char fileSize[1024] = "";
    char localfilename[100] = "";

    int lm = 0, ao = 0;

    while(localfile[lm] != '\0')
    {
        lm++;
    }

    while(localfile[lm] != '/')
    {
        lm--;
    }
    lm++;

    while(localfile[lm] != '\0')
    {
        localfilename[ao] = localfile[lm];
        ao++;
        lm++;
    }
    
    //printf("%s\n\n\n", localfilename);
    FILE *f = fopen(localfilename, "w+");
    FILE *logging = fopen(logfile, "w");
    logging = fopen(logfile, "a+");

    

    while ( (n = read(sockfd, recvBuff, sizeof(recvBuff)-1)) > 0)
    {
        recvBuff[n] = 0;
        strcpy(temp, recvBuff);
        strcpy(temp2, recvBuff);
        response = strtok(temp, " ");

        if(ifstd != 0)
        {
            printf("s->c: %s", recvBuff);
        }

        if(iflog != 0)
        {
            fprintf(logging, "s->c: %s", recvBuff);


        }

        //            printf("%s", response);
        if (strcmp(response, "227")==0)
        {
            i = strcspn(recvBuff, ipkey);
            dataip = &temp2[strlen(recvBuff) - (strlen(recvBuff)-i)];

            if((sockdata = createSocket()) < 0)
            {
                printf("%s\n", strerror(errno));;
                exit(7);
            }

            if(startDataSocket(sockdata, dataip)<0)
                printf("FAILED TO CONNECT DATA SOCKET");
            
        }

        else if (strcmp(response, "220")==0)
        {
            sendMessage(sockfd, username, logging);
        }

        else if (strcmp(response, "331")==0)
        {
            sendMessage(sockfd, password, logging);

        }

        else if (strcmp(response, "230")==0)
        {
            sendMessage(sockfd, type, logging);
            if (ifactive == 0)
            sendMessage(sockfd, passive, logging);
            else
            sendMessage(sockfd, active, logging);
            
        }

        else if (strcmp(response, "200")==0)
        {
            sendMessage(sockfd, sizeMsg, logging);
        }

        else if (strcmp(response, "213")==0)
        {
            sendMessage(sockfd, fileName, logging);
        }

        else if (strcmp(response, "150")==0)
        {
            j = 0;
            k = 0;
            l = 0;

            while (temp[j] != ')')
            {
                if (k > 0)
                {
                    fileSize[l] = temp[j];
                    l++;
                }

                if (temp[j] == '(')
                {
                    k++;
                }               
                j++;
            }

            size = toInt(fileSize);
            char recvFile[size + 4];
            //memset(recvFile, 0, 10244);

           //printf("\nBeginning File Transfer... of %i bytes\n", sizeof(recvFile));
           if (read(sockdata, recvFile, sizeof(recvFile))<0)
            {
                printf("DATASOCKET READ FAILED!");
                return -1;
            }
            fprintf(f, "%s", recvFile);
            //printf("%s", recvFile);
            fclose(f);
        }

        else if (strcmp(response, "226")==0)
        {
            sendMessage(sockfd, quit, logging);
            exit(0);
        }        

        else if (strcmp(response, "550")==0)
        {
            exit(3);
        }    

        else if (strcmp(response, "430")==0)
        {
            exit(2);
        }    

        else if (strcmp(response, "501")==0)
        {
            exit(4);
        }              

        else if (strcmp(response, "500")==0 || strcmp(response, "502")==0 || strcmp(response, "504")==0)
        {
            exit(5);
        }     
    }
    fclose(logging);
    if(n < 0)
    {
        printf("%s\n", strerror(errno));        
        exit(7);
    } 

    return 0;
}



int connectServer(int socket, char *ip, int port)
{
    struct hostent *he;
    struct sockaddr_in serv_addr;
    int n;

    memset(&serv_addr, '0', sizeof(serv_addr));
    
    if ( (he = gethostbyname(ip) ) == NULL ) 
    {
      exit(1); /* error */
     } 

    memcpy(&serv_addr.sin_addr, he->h_addr_list[0], he->h_length);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    //TO USE FOR IP:: serv_addr.sin_addr.s_addr = inet_addr(ip);

    n = connect(socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (n < 0)
    {
        printf("%s\n", strerror(errno));
        exit(1);
    }
    return n;
}

int createSocket()
{
    int n = socket(AF_INET, SOCK_STREAM, 0);
    if (n < 0)
    {
        printf("%s\n", strerror(errno));
        exit(7);

    }
    return n;
}

int sendMessage(int socket, char *msg, FILE *logging)
{
    int n = send(socket , msg , strlen(msg) , 0);
    //FILE *logging = fopen(logfile, "a+");
    if (n < 0)
    {
        printf("%s\n", strerror(errno));
        exit(7);

    }

    if(ifstd != 0)
    {
        printf("c->s: %s", msg);
    }

    if(iflog != 0)
    {
        fprintf(logging, "c->s: %s", msg);

    }

    return n;
}

int startDataSocket(int socket, char *msg)
{
    int sockdata = 0;
    int port = convertPortNo(msg);
    char ip[strlen(msg)];
    char *address;
    int i = 0, count = 0, n = 0;

    char recvBuff[1024];
    char filename[100]="fileName\n";

/*
 *
 * * * * * CONVERT PASV MESSAGE TO IP ADDRESS
 *
 */
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

    //printf("\n Creating Data Socket... \n");
    //printf("\n portno:%i", port);
    

    //printf("\n Connecting Data Socket To Server... \n");

    n = connectServer(socket, address, port);
    if (n < 0)
    {
        printf("%s\n", strerror(errno));
        exit(1);
    }    

    return n;
}

/*
 *
 * * * * * CONVERT PASV MESSAGE TO PORT NUMBER
 *
 */
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
