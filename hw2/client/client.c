#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <ctype.h>
#include <stdlib.h>

#include "sock.h"

int main(int argc, char **argv)
{
    int opt;
    char *server_host_name = NULL, *server_port = NULL;

    /* Parsing args */
    while ((opt = getopt(argc, argv, "h:p:")) != -1)
    {
        switch (opt)
        {
        case 'h':
            server_host_name = malloc(strlen(optarg) + 1);
            strncpy(server_host_name, optarg, strlen(optarg));
            break;
        case 'p':
            server_port = malloc(strlen(optarg) + 1);
            strncpy(server_port, optarg, strlen(optarg));
            break;
        case '?':
            fprintf(stderr, "Unknown option \"-%c\"\n", isprint(optopt) ?
                    optopt : '#');
            return 0;
        }
    }

    if (!server_host_name)
    {
        fprintf(stderr, "Error!, No host name provided!\n");
        exit(1);
    }

    if (!server_port)
    {
        fprintf(stderr, "Error!, No port number provided!\n");
        exit(1);
    }

    /* Open a client socket fd */
    int clientfd __attribute__((unused)) = open_clientfd(server_host_name, server_port);
    /* Start your coding client code here! */
    if(clientfd < 0)
    {
        printf("ERROR connecting\n");
        return 0;
    }
    printf("[INFO] Connected to localhost:%s \n",server_port);
    printf("[INFO] Welcome please type HELP for avaiable commands\n");

    char long_scanf[2000];

    char mode[2000];
    char buffer_key[2000];
    char buffer_value[1000];
    char server_reply[2000];
    char aaa[2000];
    char bbb[2000];

    char useless[2000];

    int intmode;

    int t = 0 ;
    int check_scanf = 0;
    int count_scanf = 0;

    while (1)
    {
here:
        //sleep(1);

        memset(long_scanf, 0, sizeof(long_scanf));
        memset(useless, 0, sizeof(useless));
        memset(mode, 0, sizeof(mode));
        memset(buffer_key, 0, sizeof(buffer_key));
        memset(buffer_value, 0, sizeof(buffer_value));
        memset(server_reply, 0, sizeof(server_reply));
        memset(aaa, 0, sizeof(aaa));
        memset(bbb, 0, sizeof(bbb));

        check_scanf = 0;
        count_scanf = 0;
        t = 0;
        fflush(stdin);
        //fgets(long_scanf, strlen(long_scanf), stdin);
        scanf("%[^\n]",long_scanf);
        getchar();
        //printf("scaned %s \n",long_scanf);

        for(int i = 0; i < strlen(long_scanf); i++)
        {
            if(long_scanf[i] == ' ' || long_scanf[i] == '\t')
            {
                check_scanf++;
                t = 0;

                continue;
            }
            if(check_scanf == 0)
            {
                count_scanf = check_scanf;
                mode[t] = long_scanf[i];
                t++;
            }
            else if(check_scanf == 1)
            {
                count_scanf = check_scanf;
                buffer_key[t] = long_scanf[i];
                t++;
            }
            else if(check_scanf == 2)
            {
                count_scanf = check_scanf;
                buffer_value[t] = long_scanf[i];
                t++;
            }
            else if(check_scanf == 3)
            {
                count_scanf = check_scanf;
                useless[t] = long_scanf[i];
                t++;
            }
        }

        //printf("mode %s\n",mode);



        if(count_scanf >= 3)
        {
            printf("[COMMAND ERROR]Too many argument check HELP ,Unknown commands\n\n");
            goto here;
        }


        if(strcmp(mode,"HELP") == 0) intmode = 1;
        else if(strcmp(mode,"PUT") == 0) intmode = 2;
        else if(strcmp(mode,"GET") == 0) intmode = 3;
        else if(strcmp(mode,"DELETE") == 0) intmode = 4;
        else if(strcmp(mode,"EXIT") == 0) intmode = 5;
        else intmode = 9;

        switch(intmode)
        {
        case 1:
            printf("\nCommand \t\t Description\n");
            printf("PUT [key] [value] \t Store the pair {[key], [value]} into the database.\n");
            printf("GET [key] \t\t Get the value of [key] fro, the database\n");
            printf("DELETE [key] \t\t Delete [key] and its associated value from the database.\n");
            printf("EXIT \t\t\t Exit\n\n");
            break;
        case 2:
            if(count_scanf != 2)
            {
                printf("[COMMAND ERROR] Unknown commands, The PUT need to contain PUT [KEY] [VALUE]\n\n");
                break;
            }
            send(clientfd,mode,sizeof(mode),0);
            //scanf("%s",buffer_key); //"%s " make thing wired but can successful do the job
            send(clientfd,buffer_key,sizeof(buffer_key),0);
            strcpy(aaa, buffer_key);

            //scanf("%s",buffer_value); //"%s " make thing wired but can successful do the job
            send(clientfd,buffer_value,sizeof(buffer_value),0);
            strcpy(bbb, buffer_value);

            recv(clientfd,server_reply,sizeof(server_reply),0);
            if(strcmp("[ERROR]the_key_already_exist",server_reply) == 0)
            {
                printf("[ERROR] The key value pair is already exist!\n\n");
            }
            else
            {
                printf("[OK] Key value pair {%s, %s} is stored!\n\n",aaa,bbb);
            }
            break;
        case 3:

            if(count_scanf != 1)
            {
                printf("[COMMAND ERROR] Unknown commands, The GET need to contain GET [Key]\n\n");
                break;
            }

            send(clientfd,mode,sizeof(mode),0);

            //scanf("%s",buffer_key);
            send(clientfd,buffer_key,sizeof(buffer_key),0);

            recv(clientfd,server_reply,sizeof(server_reply),0);
            if(strcmp("NOT",server_reply) == 0)
            {
                printf("[ERROR] The key not found!\n\n");
            }
            else
            {
                printf("[OK]The value of %s is %s\n\n",buffer_key,server_reply);
            }
            break;
        case 4:
            if(count_scanf != 1)
            {
                printf("[COMMAND ERROR]Unknown commands, The DELETE need to contain DELETE [Key]\n\n");
                break;
            }
            send(clientfd,mode,sizeof(mode),0);

            //scanf("%s",buffer_key);
            send(clientfd,buffer_key,sizeof(buffer_key),0);

            recv(clientfd,server_reply,sizeof(server_reply),0);
            printf("%s\n\n",server_reply);
            break;
        case 5:
            close(clientfd);
            return 0;
            break;
        case 9:
            printf("[COMMAND ERROR]Unknown commands, Check HELP for help\n\n");
            break;
        }
    }
    close(clientfd);
    return 0;

}