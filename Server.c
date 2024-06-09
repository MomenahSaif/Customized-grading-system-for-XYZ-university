/*
** server.c -- a stream socket server demo
*/
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define PORT "3490"  // the port users will be connecting to
#define MAXDATASIZE 100 // max number of bytes we can get at once 
#define BACKLOG 10	 // how many pending connections queue will hold

void sigchld_handler(int s)
{
	(void)s; // quiet unused variable warning

	// waitpid() might overwrite errno, so we save and restore it:
	int saved_errno = errno;

	while(waitpid(-1, NULL, WNOHANG) > 0);

	errno = saved_errno;
}


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(void)
{
	int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;
	struct sigaction sa;
	int yes=1;
	char s[INET6_ADDRSTRLEN];
	int rv;
	char buf[MAXDATASIZE];
	char fbuf[MAXDATASIZE];
	char fbuf0[MAXDATASIZE];
	int numbytes;
	
	FILE* ptr;
       char ch;
       
       DIR *directory;
       struct dirent* file;
       char str[50];

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("server: socket");
			continue;
		}

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
				sizeof(int)) == -1) {
			perror("setsockopt");
			exit(1);
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("server: bind");
			continue;
		}

		break;
	}

	freeaddrinfo(servinfo); // all done with this structure

	if (p == NULL)  {
		fprintf(stderr, "server: failed to bind\n");
		exit(1);
	}

	if (listen(sockfd, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}

	sa.sa_handler = sigchld_handler; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}

	printf("server: waiting for connections...\n");

	while(1) {  // main accept() loop
		sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
		if (new_fd == -1) {
			perror("accept");
			continue;
		}

		inet_ntop(their_addr.ss_family,
			get_in_addr((struct sockaddr *)&their_addr),
			s, sizeof s);
		printf("server: got connection from %s\n", s);

		if (!fork()) { // this is the child processe32
			close(sockfd); // child doesn't need the listener
			if (send(new_fd, "Welcome from admin!", 20, 0) == -1)
				perror("send");
			/****SERVER CODE STARTS HERE"*****/
			if ((numbytes = recv(new_fd, buf, MAXDATASIZE-1, 0)) == -1) {
	 	   	perror("recv");
	 	   	exit(1);
			}
	 		buf[numbytes] = '\0';
			printf("server: received '%s'\n",buf);
			//TEACHER
			if(buf[0]=='t')
			{
				ptr = fopen(buf, "r");
 	        		if (ptr == NULL) {
 	        		        ptr = fopen(buf, "w");
 	        			printf("File not present. Creating new file\n");
               	   		if (send(new_fd, "Record doesn't exist! Creating new file", 42, 0) == -1)
						perror("send");
				}
				else {
					while (1) {
						if ((numbytes = recv(new_fd, buf, MAXDATASIZE-1, 0)) == -1) {
	 	   					perror("recv");
	 	   					exit(1);
						}	
	 					buf[numbytes] = '\0';
						printf("server: received '%s'\n",buf);
    						if (buf[0] == '1') {
    							while (fgets(buf, MAXDATASIZE-1, ptr) != NULL) {
        							printf("%s", buf);
               	   					if (send(new_fd, buf, strlen(buf), 0) == -1)
									perror("send");	        				
    							}
	               				fclose(ptr);
               	   				if (send(new_fd, "EOF", 3, 0) == -1)
								perror("send");
						}
						else if (buf[0] == '2') {
							printf("Add\n");
						}
						else if (buf[0] == '3') {
							printf("Edit\n");
						}
						
					}        				
			    	}
			}
			//STUDENT
			else //if(buf[0]=='s')
			{     
			    directory = opendir(".");
                           if (directory == NULL) {
                                printf("Error\n");
                                    exit(2);
                           }
                           while ((file=readdir(directory)) != NULL) {
                                //printf("file=%s\n", file->d_name);
                             	 if(file->d_name[0]=='t' & file->d_name[1]=='_') {
                             	     //printf("file found\n");
                                    ptr=fopen(file->d_name, "r");
                                    fgets(fbuf0, MAXDATASIZE-1, ptr);
                                    //printf("%s", fbuf);                             
                                    while(!feof(ptr))
                                    {
                                           fscanf(ptr,"%s", fbuf);
                                           //printf("buf=%s fbuf=%s\n", buf, fbuf);
                                           if(strcmp(buf, fbuf)==0)//if match found
                                           {    printf("Match found\n");  
                                                fgets(fbuf, MAXDATASIZE-1, ptr);
                                                printf("%s %s\n", fbuf0, fbuf);
						  if (send(new_fd, fbuf0, strlen(fbuf0), 0) == -1)
							perror("send");                                                
                                                if (send(new_fd, fbuf, strlen(fbuf), 0) == -1)
							perror("send");
                                                // send(new_fd, fbuf0, strlen(fbuf0));
                                                //send this to client 
                                                break;
                                           }
                                    }
                                       
                                    //printf("%s\n", file->d_name);
                                 }
                            }
                            closedir(directory);
			  
			}
			//else
			//printf("Wrong Input\n");
			close(new_fd);
			exit(0);
		}
		/****SERVER CODE ENDS HERE"*****/
		close(new_fd);  // parent doesn't need this
	}

	return 0;
}
