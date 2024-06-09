/*
** client.c -- a stream socket client demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#define PORT "3490" // the port client will be connecting to 

#define MAXDATASIZE 100 // max number of bytes we can get at once 

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
	int sockfd, numbytes;  
	char buf[MAXDATASIZE];
	char type;
	char tname[10];
	int roll;
	int marks;
	int menu;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];
	
	
	

	if (argc != 2) {
	    fprintf(stderr,"usage: client hostname\n");
	    exit(1);
	}

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("client: socket");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			perror("client: connect");
			close(sockfd);
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "client: failed to connect\n");
		return 2;
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
			s, sizeof s);
	printf("client: connecting to %s\n", s);

	freeaddrinfo(servinfo); // all done with this structure
	/****CLIENT CODE STARTS HERE"*****/
	if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
	    perror("recv");
	    exit(1);
	}
	buf[numbytes] = '\0';
	printf("client: received '%s'\n",buf);
        printf("Press t for teacher & s for student\n");
        scanf("%c",&type);
        if (type == 't') {
            printf("Welcome Teacher. Enter you Name\n");
            scanf("%s", tname);
            sprintf(buf, "t_%s", tname);
            printf("%s\n", buf);
	    if (send(sockfd, buf, strlen(buf), 0) == -1)
		perror("send");      
	    while (1) {
	        printf("Menu:\n 1. List\n 2. Add\n 3. Edit\n 4. Exit\n");
	        scanf("%d", &menu);
	        if (menu == 1) {
	        	sprintf(buf, "1");
	        	printf("%s\n", buf);
	        	if (send(sockfd, buf, strlen(buf), 0) == -1)
				perror("send");
	    		if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
	    			perror("recv");
	    			exit(1);		
	    		}
	    		buf[numbytes] = '\0';
	        	printf("client: received %s\n",buf);              
	    		if (strcmp(buf, "EOF")) { 
	    			buf[numbytes] = '\0';
	    			printf("client: received %s\n",buf);
	    		}
	    		menu=0;
	        }
	        else if (menu == 2)
	        {
	        	printf("Enter Roll Number\n");
	        	scanf("%d", &roll);
	        	printf("Enter Marks\n");
	        	scanf("%d", &marks);
	        	sprintf(buf, "2 %d %d\n", roll, marks);
	        	printf("%s\n", buf);
	        	if (send(sockfd, buf, strlen(buf), 0) == -1)
				perror("send");
	        }
	        else if (menu == 3)
	        {
	        	printf("Enter Roll Number\n");
	        	scanf("%d", &roll);
	        	printf("Enter Marks\n");
	        	scanf("%d", &marks);
	        	sprintf(buf, "3 %d %d\n", roll, marks);
	        	printf("%s\n", buf);
	        	if (send(sockfd, buf, strlen(buf), 0) == -1)
				perror("send");
	        }
	        else if (menu == 4)
	        {
	        	printf("Good Bye\n");
	        	break;
	        }             
	                     
	    }      
        } 
        else if (type =='s'){
            printf("Dear Student. Enter your roll number\n");
            scanf("%d", &roll);
            sprintf(buf, "%d", roll);
            printf("%s\n", buf);
	    if (send(sockfd, buf, strlen(buf), 0) == -1)
		perror("send");
	    if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
	    	perror("recv");
	    	exit(1);
	    }
	    buf[numbytes] = '\0';
	    printf("client: received %s\n",buf);
	    if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
	    	perror("recv");
	    	exit(1);
	    }
	    buf[numbytes] = '\0';
	    printf("client: received %s\n",buf);
              
	}
	/****CLIENT CODE ENDS HERE"*****/
	close(sockfd);

	return 0;
}
