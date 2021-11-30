#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <netdb.h>
#include <ctype.h>

#include "types.h"
#include "sock.h"


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct listnode *listPointer;
typedef struct listnode
{
    char key[255];
    char value[255];
    listPointer next;
};

listPointer head = NULL;






void *server_handler(void *fd_pointer);


int main(int argc, char **argv)
{
    char *server_port = 0;
    int opt = 0;
    /* Parsing args */
    while ((opt = getopt(argc, argv, "p:")) != -1)
    {
        switch (opt)
        {
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

    if (!server_port)
    {
        fprintf(stderr, "Error! No port number provided!\n");
        exit(1);
    }

    /* Open a listen socket fd */
    int listenfd __attribute__((unused)) = open_listenfd(server_port);

    /* Start coding your server code here! */
    int client_sock,addr_size,*new_sock;
    // server_socket = listenfd

    struct sockaddr_in client_addr;

    //pthread_t thread_id;
    printf("[INFO] Start with a clean database\n");
    printf("[INFO] Initizlizing the server\n");
    printf("[INFO] Server Initialized\n");
    printf("[INFO] Listening on the port %s...\n",server_port);


    while(client_sock = accept(listenfd, (struct sockaddr *)&client_addr, (socklen_t*)&addr_size))
    {
        printf("connect accept\n");

        pthread_t server_thread;
        new_sock = malloc(sizeof(int));
        *new_sock = client_sock;
        pthread_create(&server_thread,NULL,server_handler,(void*) new_sock);

        printf("This is the Thread %ld service~\n",server_thread);

        //pthread_join(server_thread, NULL);
    }
    if (client_sock < 0)
    {
        printf("accept failed\n");
        return 1;
    }

    return 0;
}

void *server_handler (void *fd_pointer)
{
    //printf("Hello Server Handler \n");
    int sock = *(int *)fd_pointer;
    int read_size;
    //char *message;
    int mode_PUT = 1,mode_GET = 1,mode_DELETE = 1;

    static char client_message[2000];
    static char client_message_value[2000];
    static char client_message_key[2000];
    static char server_give_client[2000];


    listPointer goahead = NULL;
    listPointer infront_of_goahead = NULL;

    listPointer ptr = NULL;
    listPointer for_delete = NULL;

    //int change= 0;

    //int k = 0;

    //int incheck = 0;

    int count = 1;


loop:
    count = 0;
    memset(client_message, 0, sizeof(client_message));
    memset(client_message_value, 0, sizeof(client_message_value));
    memset(client_message_key, 0, sizeof(client_message_key));
    memset(server_give_client, 0, sizeof(server_give_client));

    read_size = recv(sock,client_message,sizeof(client_message),0); //first receive is mode
    //printf("%s \n",client_message);
    mode_PUT = strcmp(client_message, "PUT");
    mode_GET = strcmp(client_message, "GET");
    mode_DELETE = strcmp(client_message, "DELETE");
    //printf("mode_PUT %d\t",mode_PUT);
    //printf("mode_GET %d\t",mode_GET);
    //printf("mode_DELETE %d\n",mode_DELETE);

    // the put function
    pthread_mutex_lock(&mutex);
    if(mode_PUT == 0)
    {
        read_size = recv(sock,client_message_key,sizeof(client_message_key),0); //second receive is key
        //printf("KEY %s \n",client_message_key);
        read_size = recv(sock,client_message_value,sizeof(client_message_value),0); //third receive is value
        //printf("VALUE %s \n",client_message_value);

        if(head == NULL)
        {
            head = (listPointer)malloc(sizeof(struct listnode));
            goahead = head;
            infront_of_goahead = head;
            ptr = head;
            count = 1;

            strcpy(head -> key,client_message_key);
            strcpy(head -> value,client_message_value);
            head -> next = NULL;

            strcpy(server_give_client, head -> value);
            send(sock,server_give_client,sizeof(server_give_client),0);
            //printf("head -> key in the head operate %s\n",head -> key);
            //printf("manipulate the head\n");

        }
        else
        {
            count = 1;
            goahead = head;
            infront_of_goahead = head;
            ptr = head;
            // the bug is that the head is changed to the new key without reason fatal cannot go to next step
            while(goahead  != NULL)
            {
                if(strcmp(goahead->key,client_message_key) == 0 )
                {
                    //printf("the key already exist \n");
                    strcpy(server_give_client,"[ERROR]the_key_already_exist");
                    send(sock,server_give_client,sizeof(server_give_client),0);

                    pthread_mutex_unlock(&mutex);
                    goto loop;
                }
                infront_of_goahead = goahead;
                goahead = goahead -> next;

            }
            if(goahead == NULL)
            {
                //printf("creat new node\n");
                listPointer temp;
                temp = (listPointer)malloc(sizeof(struct listnode));
                temp -> next = NULL;
                strcpy(temp -> key,client_message_key);
                strcpy(temp -> value,client_message_value);
                strcpy(server_give_client, temp -> value);
                send(sock,server_give_client,sizeof(server_give_client),0);
                goahead = temp;
                infront_of_goahead -> next = temp;
            }
        }
        /*
        while(ptr){
            printf("node %d is %s\n",count,ptr -> key);
            ptr = ptr->next;
            count++;
            sleep(1);
        }
        */
        pthread_mutex_unlock(&mutex);
        goto loop;
    }
    // the get function
    if(mode_GET == 0) //same
    {
        //printf("in get function\n");
        ptr = head;
        read_size = recv(sock,client_message_key,sizeof(client_message_key),0); //second receive is key
        //printf("KEY %s \n",client_message_key);  //now client_message_key is key we need to usse to find

        while(ptr)
        {
            if(strcmp(ptr->key,client_message_key) == 0)
            {
                //printf("1\n");
                strcpy(server_give_client,ptr->value);
                send(sock,server_give_client,sizeof(server_give_client),0);
                pthread_mutex_unlock(&mutex);
                goto loop;
            }
            ptr = ptr -> next;
        }
        //printf("2\n");
        strcpy(server_give_client,"NOT");
        send(sock,server_give_client,sizeof(server_give_client),0);
        pthread_mutex_unlock(&mutex);
        goto loop;
    }
    if(mode_DELETE == 0) //same
    {
        ptr = head;
        for_delete = head;

        read_size = recv(sock,client_message_key,sizeof(client_message_key),0); //second receive is key
        //printf("KEY %s \n",client_message_key);  //now client_message_key is key we need to usse to find

        while(ptr)
        {
            if(strcmp(ptr->key,client_message_key) == 0)
            {
                //printf("1\n");
                if(ptr == head)
                {
                    if(ptr -> next != NULL)
                    {
                        strcat(server_give_client,"[OK] Key ");
                        strcat(server_give_client, head->key );
                        head = head -> next;
                        for_delete = for_delete -> next;
                        free(ptr);
                    }
                    else
                    {
                        strcat(server_give_client,"[OK] Key ");
                        strcat(server_give_client, head->key );
                        head = NULL;
                        free(ptr);
                    }
                }
                else if(ptr->next == NULL)
                {
                    strcat(server_give_client,"[OK] Key ");
                    strcat(server_give_client, ptr->key );
                    for_delete -> next =NULL;
                    free(ptr);
                }
                else
                {
                    //printf("delete %s\n",ptr -> key);
                    strcat(server_give_client,"[OK] Key ");
                    strcat(server_give_client, ptr->key );
                    for_delete -> next = ptr->next;
                    free(ptr);
                }
                strcat(server_give_client," is removed!");
                send(sock,server_give_client,sizeof(server_give_client),0);
                pthread_mutex_unlock(&mutex);
                goto loop;
            }
            for_delete = ptr;
            ptr = ptr -> next;
        }
        //printf("2\n");
        strcpy(server_give_client,"[ERROR] Nothing to remove !\n");
        send(sock,server_give_client,sizeof(server_give_client),0);
        pthread_mutex_unlock(&mutex);
        goto loop;
    }


    if(read_size == 0)
    {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }
    pthread_mutex_unlock(&mutex);
    free(fd_pointer);
    pthread_detach(pthread_self());

    return 0;
}


