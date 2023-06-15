#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>

void *client_thread(void *);
int num_client =0;
int clients[64];
int main() 
{
    int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listener == -1)
    {
        perror("socket() failed");
        return 1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(9000);

    if (bind(listener, (struct sockaddr *)&addr, sizeof(addr))) 
    {
        perror("bind() failed");
        return 1;
    }

    if (listen(listener, 5)) 
    {
        perror("listen() failed");
        return 1;
    }
    
    while (1)
    {
        int client = accept(listener, NULL, NULL);
        
        if (client == -1)
        {
            perror("accept() failed");
            continue;
        }
        printf("New client connected: %d\n", client);

        clients[num_client] = client;
        num_client++;
        if (num_client %2 ==1){
            char *msg = "Please wait for your partner\n";
            send(client, msg,strlen(msg),0);
        }
        else {
            char *msg = "You can chat with your partner\n";
            send(client, msg,strlen(msg),0);
        }
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, client_thread, &client);
        pthread_detach(thread_id);
        
    }
    
    close(listener);    

    return 0;
}

void *client_thread(void *param)
{   
    int client = *(int *)param;
    char buf[256];

    while (1)
    {
        char *msg = "Your Partner exit, Please wait for another one\n";
        int ret = recv(client, buf, sizeof(buf), 0);
        if (ret <= 0)
        {
         
             if (client%2 ==0){
                    send(client+1,msg,strlen(msg),0);
                }
                else 
                {   
                    send(client-1,msg,strlen(msg),0);
                }
            for (int i = 0; i < num_client; i++){
                if (clients[i] == client){
                    clients[i]= clients[num_client-1];
                    num_client--;
                }
            }
            
                break;
        }
        buf[ret] = 0;

    

        if (strcmp(buf,msg)==0){
            for (int i = 0; i < num_client; i++){
                if (clients[i] == client){
                    clients[i]= clients[num_client-1];
                    num_client--;
                }
            }
            char *msg = "Please wait for your partner\n";
            send(client, msg,strlen(msg),0);
            printf("Client");
            close(client);
            break;
        }
        printf("Received from %d: %s\n", client, buf);

        if (num_client %2 ==1 && clients[num_client-1]==client)
        {
            char *msg = "Please wait for your partner\n";
            send(client, msg,strlen(msg),0);
        }
        else
        {
            int i;
            for (int i=0; i<num_client; i++){
                if (clients[i]==client) break;
            }
            if (i!=num_client) {
                if (client%2 ==0){
                    send(client+1,buf,strlen(buf),0);
                }
                else 
                {   
                    send(client-1,buf,strlen(buf),0);
                }
            }
            
        }    
}   
    
    close(client);
}