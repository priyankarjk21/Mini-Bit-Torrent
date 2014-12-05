/**/
/*MNC Socket Programming by Priyanka R Kulkarni*/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <sys/time.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <sys/time.h>
#include <sys/fcntl.h>
#include <time.h>
#include <sys/unistd.h>
#include<stdlib.h>
#include <fcntl.h>
# define STDIN 0
#define SIZE 1
#define NUMELEM 5
#define BILLION  1000000000L;



int newClient_fd=0;  //to add to master on new connection
char ip_buffer[100];  		//to store ip to check for self connection
int deleteFlag=0;
char filename[10];		//filename to download

int file_chunks_send_from=0;
char chunks_to[2];
int file_chunks_send_to=0;
int file_chunks_to_send=0;
int count=0;				//track of the clients and server connected

void removeClient(int);				//method to remove client
void retrieveConnectionList();			//retrieve the connection list
void help(char);
int myip();
char temp_ipaddr[16];				//temp add to store for adding new client address
char temp_hostname[50];
char portbuff[5];
int registerFlag=0;				//to check if client is registered or not before its connecting
struct connection_list					// connection_list structs
{
	int connection_id;
	char port[5];
	char ipAddress[16];
	char hostname[50];
	int fd;

};

struct connection_list cl[5];


/*
Retrieve the list of connections
*/

void retrieveConnectionList()
{
	int i=0;
	memset(filename, 0, 10);
	if(count>0)
	{
		char port_num[5];
		printf("Connection_id\t hostname\t\t IPAddress\t Port No\n");
		for(i=0;i<count;i++)
		{
			strncpy(port_num,cl[i].port,sizeof(cl[i].port));
			//printf("port number is-->%s\n",cl[i].port);
			printf("%d\t\t %s\t %s\t\t %s\n",cl[i].connection_id,cl[i].hostname,cl[i].ipAddress,port_num);   //print the list
			port_num[0]='\0';
		
		}
	}
	else
		printf("List is empty\n");		//if list is empty


}

void addTolist(char port[],char ipaddress[],char hostname[],int fd)		// to add elements to the list 
{
  struct connection_list list;

  list.connection_id=count+1;
  strcpy(list.port,port);
  strcpy(list.ipAddress,ipaddress);
  strcpy(list.hostname,hostname);
  list.fd=fd;
  cl[count]=list;
  count=count+1;
	
}

/*TO remove a connection on termination*/

void removeClient(int connectionId)	//source help:http://cboard.cprogramming.com/c-programming/24685-deleting-records-structure.html
{
	int i;
	if(connectionId==count+1)
	{
		cl[connectionId]=cl[count];			//find the client to delete and assign it to the last connection
		
	}
	else
	{
		for(i=0;i<count;i++)
		{
			if(connectionId==(i+1))
			{
				cl[connectionId]=cl[count];	//decrement the count
				count--;		
			}
		}
	}

}

/*Validate IP address in input*/
 int validateIPAddress(char ip_addr[])		//reference:http://www.ncooltips.com/2012/05/program-to-validate-hostname-and-ipv4.html
{
    int rc=0;
 
    struct addrinfo hints;
 
    struct addrinfo *result=NULL;
 
    int address_validate_flag;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;   
    hints.ai_socktype = SOCK_DGRAM; 
    hints.ai_flags = 0;
    hints.ai_protocol = 0;    
    address_validate_flag= getaddrinfo(ip_addr, NULL, &hints, &result);
    if (address_validate_flag != 0) 
    {
 
       if( result != NULL )
 
               freeaddrinfo(result);
 
        return 0;
 
    } 
    else 
    {
 
     return 1;
 
    }
 
}
 
 



/*To get the IP address*/


int getMyIPAddr(int flag)						//source "http://www.binarytides.com/get-local-ip-c-linux/"
{
    const char* google_server = "8.8.8.8";
    struct sockaddr_in server_addr;
    struct sockaddr_in name;
    socklen_t name_len = sizeof(name);
    int sock = socket ( AF_INET, SOCK_DGRAM, 0);
     char local_ip[100];
    memset(local_ip,0,sizeof(local_ip));
    //Socket could not be created
    if(sock < 0)
    {
        perror("Socket error");
    }
     
    memset( &server_addr, 0, sizeof(server_addr) );				//clear the address 
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(google_server);		//defining the server to connect to
    server_addr.sin_port = htons(53);
    int err = connect(sock,(const struct sockaddr*) &server_addr , sizeof(server_addr) );
     
    err = getsockname(sock, (struct sockaddr*) &name, &name_len);  //using getsockname to get IP address
         
 
    const char* p = inet_ntop(AF_INET, &name.sin_addr, local_ip, 100);			//putting the ip address in ip_Buffer
      
    if(p != NULL)
    {
	if(flag==1)
	{
        	printf("My IP Address is--> %s \n" , local_ip);			//printing the IP address
	}	
	else
	{
		strcpy(ip_buffer,local_ip);
		ip_buffer[strlen(ip_buffer)]='\0';
	}									
    }
    else
    {
       
        printf ("Error number : %d . Error message : %s \n" , errno , strerror(errno));		//error if unable to get IP address
    }
 
    close(sock);
     
    return 0;
}

/*
The control enters main program on execution
*/

int main(int argc, char *argv[])
{
	int port;
	int returnFlag;
	
	if(argc!=3)						//if arguments less than 3,then insufficient
	{
		printf("Insufficicent arguments\n");
	}
	else
	{
		port=atoi(argv[2]);
		if(port==0)			//check if valid port number
		{
			printf("Please enter proper port number\n");
			return 0;
		}
		else if(port<1025 && port>65536)			//port number should be greater than 1024 and less than 65536
		{
			printf("Please enter port number of higher range,above 1024\n");
			return 0;
		}
		else
		{
			
			if(strcmp(argv[1],"s")==0)			//check to enter server side
			{
				printf("-------------Welcome to server--------\n");	//call server side 
				returnFlag=server_call(argv);			
				return returnFlag;					
			}
			else if(strcmp(argv[1], "c")==0)				//check to enter client sideS
			{
				printf("-------------Welcome to client----------\n");	//call client side
				returnFlag=client_call(argv); 			
				return returnFlag;				
			}
			else
			{
				printf("Choose s for server and c for client\n");		//if neither c or s chosen,throw error
				return 0;
			}
		}
	}
	return 0;

}



/*
Help on commands to be used
*/
void help(char client_or_server)
{
	if(client_or_server=='s')
	{
		printf("1.MYIP is used to retrieve the IP Address\n");
		printf("2.MYPORT is used to get listening port number\n");
		printf("3.CREATOR to know the creator of the project\n");
		printf("4.LIST is used to display the list of server and client details client is connected to\n");
	}
	else if(client_or_server=='c')
	{
		printf("1.MYIP is used to retrieve the IP Address\n");
		printf("2.MYPORT is used to get listening port number\n");
		printf("3.CREATOR to know the creator of the project\n");		
		printf("4.REGISTER is used to register with the server, REGISTER <IP address><port no>is format\n");
		printf("4.CONNECT is used to connect with the client, CONNECT<IP address><port no>is format\n");
		printf("5.LIST is used to display the list of server and client details client is connected to\n");
		printf("6.TERMINATE is used to terminate the conection. TERMINATE<connection_id> is the format\n");
		printf("7.EXIT is used to the exit the client from all connections\n");
	}

}

/*On registering or connecting client with server and other clients*/
void connectClient(char input_1[], char input2[],char *argv[])
{
	int sockfd = 0, n = 0;
	struct sockaddr_in serv_addr; 
	struct addrinfo hints, *res, *p;
	char buff[100];
	int bytes=0;

	struct connection_list list;
	memset(&hints, 0, sizeof (hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	int cmdlen=0;
	int flag = getaddrinfo(input_1, input2,&hints,&res);
	int len = sizeof(serv_addr);
	
	struct hostent* hostnames;
	struct in_addr ip;
	
	for(p = res; p!= NULL; p = p->ai_next)
	{	
		if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0)
		{
			printf("\n Error : Could not create socket \n");
			exit(1);
		} 
		else
		{
			printf("socket created=%d\n", sockfd);
		}

		if(connect(sockfd, p->ai_addr, p->ai_addrlen)== -1)
		{
			close(sockfd);
			perror("client:connect");
			continue;
		}
		else
		{
			printf("connected\n");
			
			registerFlag=1;
		 	ip.s_addr = inet_addr(input_1); 					//retrieving the client details to add in list 
   			 hostnames = gethostbyaddr(&ip, sizeof(ip), AF_INET);
   			if (hostnames != NULL && hostnames[0].h_name != NULL) 
			{
       				 printf("hostname :%s\n", hostnames[0].h_name);
        				
    			} 
		 	else 
			{
        			herror("gethostbyaddr");
          	 	}
		
 						
			
			cmdlen = strlen(input_1);
			input_1[cmdlen]='\0';
			
			strcpy(temp_ipaddr,input_1);
			cmdlen=strlen(temp_ipaddr);
			temp_ipaddr[cmdlen]='\0';

			strcpy(portbuff,input2);
			cmdlen=strlen(portbuff);
			portbuff[cmdlen]='\0';
			newClient_fd=sockfd;
			addTolist(portbuff,temp_ipaddr,hostnames[0].h_name,sockfd);
			if ((bytes = recv(sockfd, buff, 100-1, 0)) == -1) 
			{
					perror("recv");
				
			}
			if(bytes==0)
			{

				printf("No data received\n");
			}
			else
			{
				if (send(sockfd, argv[2], sizeof(argv[2]), 0) == -1)
				{
						perror("send server");
						
				}
			} 
			buff[bytes] = '\0';
			printf("client: received %s\n",buff);
			
		}
		break;
	}
	return;

}


/*To get client address*/

void *get_in_addr(struct sockaddr *sa)			//program sourc: beej to get address of client
{
	if (sa->sa_family == AF_INET) 
	{
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

/*Control enters server side*/

int server_call(char *argv[])
{
	int n=0;
	char s[INET6_ADDRSTRLEN];
	char ipaddr[INET6_ADDRSTRLEN];
	char command[50];
	struct timeval tv;
	fd_set readfds;
	fd_set master;				
	int fdmax=0;
	int addrlen=0;
	int newfd=0;
	int i=0;
	int j=0;
	int length=0;
	struct sockaddr_in clientaddr;
	struct sockaddr_in serveraddr;
	char *serv;
	 size_t servlen;
	char service[20];		
	int listener;
	struct in_addr ip;
	char buffer[5];
	char portbuffer[5];
	int rec_bytes=0;
	int portno;
	char newClient_buffer[20];
	portno=atoi(argv[2]);
	char *new_client;
	char list_to_client[300];
	int connection_id;
	char buff[300];
	int temp_fd=4;
	char *temp_fdVal;
	char tempfd[0];
	int connect_id=0;	
	struct hostent* hostnames;
	if((listener = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("Error ssocket creation ");
		
	}
	
	
	
	fcntl(listener, F_SETFL, O_NONBLOCK);  //non-blocking
 	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = INADDR_ANY;
	serveraddr.sin_port = htons(portno);
	memset(&(serveraddr.sin_zero), '\0', 8);
 	if(bind(listener, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) == -1)
	{
		perror("Server-bind()");
	       
	}


	if(listen(listener, 10) == -1)	
	{
	     perror("Server-listen()");
	     
	}

	
	addrlen=sizeof(clientaddr);		//adding and defining fd sets
	FD_ZERO(&master);
	FD_ZERO(&readfds);
 	FD_SET(listener, &master);
	FD_SET(STDIN, &master);
	fdmax=listener;
	

	for(;;)
	{
		printf("serv~");
		fflush(stdout);
		readfds=master;
		if((select(fdmax+1,&readfds,NULL,NULL,NULL))==-1)
		{
			perror("select");
			//exit(4);
		}
		
		if(FD_ISSET(STDIN,&readfds))
		{
			memset(&command,0,sizeof(command));
			read(0,command, sizeof(command));
			length = strlen(command);
			command[length-1]='\0';
			
			if (strcmp(command, "myip")==0)   		//fetch ip address
			{
				
				getMyIPAddr(1);
				
			}
			else if(strcmp(command,"help")==0)         	//help of commands
			{
				help('s');
			}
			else if(strcmp(command,"exit")==0)		// on exit
			{
				exit(1);
				
			}
			else if(strcmp(command,"myport")==0)		// to get the listening port
			{
				printf("My port is-->%s\n",argv[2]);				
				
			}
			else if(strcmp(command, "list")==0)			//to get list
			{
				
				retrieveConnectionList();
			}
			else if(strcmp(command,"creator")==0)		//to get the details of the creator
			{
				printf("Project by Priyanka R Kulkarni\n");
				printf("UBIT:50098043\n");
				printf("Email Id: pkulkarn@buffalo.edu\n");
				fflush(stdout);
			}
			else
			{
				printf("invalid command,use help command to get command and its format\n");  //invalid command
			}
					
		}
		   for(i = listener; i <= fdmax; i++) 		//to listen for the file descriptors(source:beej multi-chat server program)
		    {
		    if (FD_ISSET(i, &readfds)) 
			{ 
				if (i == listener)			//new connection received
				 {
				    
				   
				    if((newfd = accept(listener, (struct sockaddr*)&clientaddr, &addrlen)) == -1) //accepting new connection
				    {
						//perror("Server-accept() error:");					
				    }
				   
				   else 
				   {
						memset(s,0,sizeof(s));
						memset(buffer,0,sizeof(buffer));
						inet_ntop(clientaddr.sin_family,get_in_addr((struct sockaddr *)&clientaddr),s, sizeof s);

						if (send(newfd, "Welcome to my NetWORK :)", 24, 0) == -1)
						{
								perror("send");
					
						}
				
						 if ((rec_bytes = recv(newfd, buffer, sizeof(buffer), 0)) == -1) 
						{
								perror("recv");
			
						}
						else
						{

							memset(temp_ipaddr,0,sizeof(temp_ipaddr));
							printf("New client connected\n");
							ip.s_addr = inet_addr(s); 	//retrieving the client details to add in list 
		   					hostnames = gethostbyaddr(&ip, sizeof(ip), AF_INET);
		   				 	if (hostnames != NULL && hostnames[0].h_name != NULL) 
							{
		       						printf("Connected To:%s\n",hostnames[0].h_name); 
						
		    				 	} 
						 	else 
						 	{
								herror("gethostbyaddr");
		
		    				 	}
							 strcpy(portbuffer,buffer);       //copy port number to buffer
							 
							 int portlen = strlen(portbuffer);
							 portbuffer[portlen]='\0';
					
							length = strlen(s);
							s[length]='\0';
							strcpy(temp_ipaddr,s);
							length=strlen(temp_ipaddr);
							temp_ipaddr[length]='\0';
						
							addTolist(portbuffer,temp_ipaddr,hostnames[0].h_name,newfd); // add the client to list
							FD_SET(newfd, &master); // add to master set
							 		
							if (newfd > fdmax)
							{   
										 
							    fdmax = newfd;		//track of the newfd
							}
							new_client=hostnames[0].h_name;
					  }
						

					if(count>0)
					{
					
					 memset(list_to_client, 0, 300);
					 strcat(list_to_client,"The list is");
					 strcat(list_to_client,"\nconnection_id\t");
					 strcat(list_to_client,"hostname\t");
					 strcat(list_to_client,"ipAddress\t");
					 strcat(list_to_client,"Port Number\n");
					
					 for(i=0;i<count;i++)
					 {
						
						
						connect_id=cl[i].connection_id;
						sprintf(tempfd,"%d",connect_id);
						
						strcat(list_to_client,tempfd);
						strcat(list_to_client,"\t");
						strcat(list_to_client,cl[i].hostname);
						strcat(list_to_client,"\t");
						strcat(list_to_client,cl[i].ipAddress);
					        strcat(list_to_client,"\t");
						strncat(list_to_client,cl[i].port,5);
						strcat(list_to_client,"\n");

					 }		
					
					for(i=0;i<count;i++)
					{
						temp_fd=cl[i].fd;
						if (send(temp_fd,list_to_client, strlen(list_to_client), 0) == -1)
						{
							perror("send\n");
					
						}
						
						
					}
					 
					}
				        fflush(stdout);

						
						
					    }
					} 
					else 
					{
					
					    memset(buff, 0, 300);
					    if ((rec_bytes = recv(i, buff, sizeof buff, 0)) <= 0)	//recv data from client
					     {
						
						if (rec_bytes == 0) 					//when connection gets closed
						{
						    // connection closed
						    printf("selectserver: socket %d left the network\n", i);	
						    for(n=0;n<count;n++)
						    {
							if(i==cl[n].fd)
							{		
									
								connection_id=cl[n].connection_id;	//on terminate remove from list
								removeClient(connection_id);
								
							}
						    }
				
						} 
						else
						{
						    perror("recv");
						}
						close(i); //close the socket on exit or terminate
						FD_CLR(i, &master); // once closed remove from master
					    } 
					
					  else
					   {
						
						for(j = listener; j <= fdmax; j++)
						 {
						   
						    if (FD_ISSET(j, &master)) 
						    {
						        
						        if (j != listener) 
							{
								if(sizeof (buff)!=0)
								{
									printf("buffer content are %s\n",buff);
								}
		
										
						        }
						    }
						}
					    }
					} 
				    } 
				} 
		
		
	}
	return 0;  
}





/*Control enters client side program*/

int client_call(char *argv[])
{
	
	char s[INET6_ADDRSTRLEN];
	char ipaddr[INET6_ADDRSTRLEN];
	char input_1[50];
	struct timeval tv;
	fd_set readfds;
	fd_set master;				// don't care about writefds and exceptfds:
	int fdmax=0;
	int flag=1;
	int addrlen=0;
	int newfd=0;
	int i=0;
	int length=0;
	struct sockaddr_in clientaddr;
	struct sockaddr_in serveraddr;
	int n=0;
	int m=0;
	
	struct hostent* hostnames;
	struct in_addr ip;
	char buffer[5];
	char buff[300]="\0";
	char portbuffer[5];
	int rec_bytes=0;
	char input_2[50];
	char input_3[50];
	int portno;
	int connectionId;
	int j;
	int terminate_fd=0;
	
	int listener;
	portno=atoi(argv[2]);
	
	int connection_id;
	FILE* fd = NULL;
   	char received_buffer[20000]; 
	int l=0;
	int file_size_download;
	int k=0;
	int each_client_download=0;
	struct timespec start, stop;
   	 double accum;	
	char file_download_request_buff[20];
	char each_client_downloadStr[20];
	if((listener = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("Error in socket creation()\n");			//error in creation of socket
		
	}

	
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = INADDR_ANY;
	serveraddr.sin_port = htons(portno);
	memset(&(serveraddr.sin_zero), '\0', 8);
 	if(bind(listener, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) == -1)
	{
		perror("Error in binding server\n");
	        
	}


	if(listen(listener, 10) == -1)	
	{
	     perror("Error in listen\n");
	     //exit(1);
	}
	
	addrlen=sizeof(clientaddr);		
	FD_ZERO(&master);			//clear the master set
	FD_ZERO(&readfds);			//clear the fd set
 	FD_SET(listener, &master);		//add fd set to master
	FD_SET(STDIN, &master);		//add standard input and output to master

	fdmax=listener;
	
	if(newClient_fd>0)
	{
		
		FD_SET(newClient_fd,&master);
		if(newClient_fd>listener)
		{
			
			fflush(stdout);
			fdmax=newClient_fd;			//making new fd as max of file descriptors
			newClient_fd=0;		
		}
	}
	
	for(;;)					//for loop begins
	{
		if(!(FD_ISSET(newClient_fd,&master))&& (newClient_fd!=0))		//check if a client connected to the client
		{
			
			fflush(stdout);
			FD_SET(newClient_fd,&master);		// add new connection to the master 
			if(newClient_fd>listener)
			{
				
				fflush(stdout);
				fdmax=newClient_fd;	
				newClient_fd=0;	
			}
		}
		printf("cln~");
		fflush(stdout);
		readfds=master;
		if((select(fdmax+1,&readfds,NULL,NULL,NULL))==-1)			//setting the select criteria
		{
			perror("select() \n");
			printf("cln~");
			
		}
		
		
		if (FD_ISSET(STDIN, &readfds))			//reading standard input output file descriptor
		{
			scanf("%s",input_1);


			if (strcmp(input_1, "myip")==0)		//to get the IP Address
			{
				
				getMyIPAddr(1);	
				fflush(stdout);	
			}
			else if(strcmp(input_1,"help")==0)		//to fetch the list of commands
			{
				help('c');
				fflush(stdout);	
			}
			else if(strcmp(input_1,"exit")==0)		//on exit(0)
			{
				exit(0);

			}
			else if(strcmp(input_1,"myport")==0)		//to get myport
			{
				printf("My port is-->%s\n",argv[2]);				
				
				fflush(stdout);	
			}
			else if(strcmp(input_1,"creator")==0)		//to get the details of the creator
			{
				printf("Project by Priyanka R Kulkarni\n");
				printf("UBIT:50098043\n");
				printf("Email Id: pkulkarn@buffalo.edu\n");
				fflush(stdout);
			}
			else if(strcmp(input_1, "list")==0)		//list the connections
			{
				retrieveConnectionList();
				fflush(stdout);
			}
			else if(strcmp(input_1,"register")==0)				//register command
			{
				
						if(registerFlag!=1)
						{						
							scanf("%s %s",input_2,input_3);
							getMyIPAddr(0);
							if(strcmp(input_2,"")==0)	//check if no IP address provided		
							{
								printf("Incomplete command, provide the serverIP address\n");
							}
							else if(strcmp(input_2,ip_buffer)==0)		//check self-connection
							{
								printf("Cannot connect to self\n");
							}
							else if(validateIPAddress(input_2)==0)			//check IP address validity
							{
								printf("Improper IP address. re-enter the correct IP address\n");
							}
							else
							{
								
								if(strcmp(input_3,"")==0)
									{
										printf("Incomplete command, provide the port address\n");
									}
									else
									{
										portno=atoi(input_3);
										if(portno==0)
										{
											printf("Please provide a valid port number\n");
										}
										else
										{
										
											connectClient(input_2, input_3,argv);
											
										}
									}
							}
						}
						else
						{
							printf("Already registered on server,cannot re-register\n");							fflush(stdout);
						}
						

			}
	
			
			else if(strcmp(input_1,"connect")==0)			//on connect command
			{
				scanf("%s %s",input_2,input_3);
				if(registerFlag!=1)				//check if the client registered on server
				{
					printf("Cannot connect prior to registering\n");
				}
				else
				{
					if(count>3)				//not allow more than 4 connections
					{
							printf("Cannot connect to more than 4 hosts\n");
					}
					else
					{
						if(strcmp(input_2,"")==0)		
						{
							printf("Incomplete command, provide the serverIP address\n");
						}
						else
						{
							//for(i=0;i<count;i++)
							//{
								/*if(strcmp(input_2,cl[i].ipAddress)==0)  //check if already connected
								{
									printf("Already connected to this client,cannot connect again\n");								
								}
								else
								{*/
									
							
									if(strcmp(input_3,"")==0)
									{
										printf("Incomplete command, provide the port address\n");
										fflush(stdout);
									}
									else
									{
										portno=atoi(input_3);
										if(portno==0)
										{
											printf("Please provide a valid port number\n");
										}
										else
										{
										
											connectClient(input_2, input_3,argv);
											fflush(stdout);
										}
									}
						
							}
						}
			  		}
			}
		
			else if(strcmp(input_1, "terminate")==0)			//to terminate the connection
			{
				scanf("%d",&connectionId);
				if(connectionId==1)
				{
					printf("Cannot terminate server connection!!");
				}
				else
				{
					for(i=1;i<count;i++)
					{							//to check if the connectionId exists
						if(i==(connectionId-1))
						{
				
								terminate_fd=cl[i].fd;
								removeClient(connectionId);		//remove from the list
								close(terminate_fd);
								FD_CLR(terminate_fd, &master); // once closed remove from master
								printf("Connection terminated succesfully\n");
								fflush(stdout);
							
						
						}
						else
						{
								printf("The connection to terminate is-->%d\n",connectionId);
								fflush(stdout);
					
						}
					}
				}				
				
				fflush(stdout);
			}
			else if(strcmp(input_1,"exit")==0)
			{
				for(i=0;i<count;i++)
				{	
				
							terminate_fd=cl[i].fd;  		//intimate all clients about termination
							close(terminate_fd);
							FD_CLR(terminate_fd, &master); // once closed remove from master
				    			
							printf("\ncan delete successfully\n");
							fflush(stdout);
						
				}		
			}
			else if(strcmp(input_1,"download")==0)
			{
				
				int chunks;
				int clients_connected=0;
				char clients_connected_str[1];
				char file_size[2];
				scanf(" %s %d",input_1,&chunks);
				char download_buff[30];
				strcpy(download_buff,"download");
				strcat(download_buff,"!");
				strcat(download_buff,input_1);
				strcat(download_buff,"@");
				sprintf(file_size,"%d",chunks);
			
				strcat(download_buff,file_size);
				length = strlen(download_buff);
				download_buff[length]='\0';
				
				memset(buff,0,sizeof(buff));
				strcpy(buff,download_buff);
				length = strlen(download_buff);
				download_buff[length]='\0';
    				
					if(count>1)
					{
						for(i=1;i<count;i++)
						{
							      
							       int sent = send(cl[i].fd, buff, 21, 0);
			    					if (sent < 1)
			    				 	{
						
			       						 printf("\nCan't write to socket\n");
									
								}
								else
								{
								
							
								if ((rec_bytes = recv(cl[i].fd, buffer, 100, 0)) == -1) 
								{
										perror("recv");
			
								}
								else
								{
									length = strlen(buffer);
									buffer[length]='\0';
									file_size_download=atoi(buffer);  //file size received in char
									//calculate chunk of data to download
									each_client_download=(file_size_download/count); 
									sprintf(each_client_downloadStr,"%d",each_client_download);
									
								
								}
							}
						}
						if(each_client_download>0)
						{
							memset(file_download_request_buff,0,sizeof(file_download_request_buff)); //clear the buffer
							int start=0;
							char download_str[5];
							 int download=0;
							char recv_file_buffer[each_client_download];
							memset(recv_file_buffer,0,sizeof(recv_file_buffer));
							memset(download_str,0,sizeof(download_str));
							int file_left_download=0;
							int array_size=(file_size_download/chunks);
							int file_check[array_size];
							int check_array_size=0;
							int file_check_flag=0;
							received_buffer[0]='\0';
							FILE *file_write;
							FILE *file_append;
							char file_buff[file_size_download];
							
							while(download<file_size_download)
							{
								for(i=1;i<count;i++)
								{
									download=start+chunks;
									sprintf(clients_connected_str,"%d",start);
									sprintf(download_str,"%d",download);
									strcpy(file_download_request_buff,"file");
									strcat(file_download_request_buff,"@");
									strcat(file_download_request_buff,clients_connected_str);
									strcat(file_download_request_buff,"#");
									strcat(file_download_request_buff,download_str); //concat and send
									start=download;
									length = strlen(file_download_request_buff);	
									file_download_request_buff[length]='\0';
									file_left_download=file_size_download-download;
									int file_get_size=send(cl[i].fd,file_download_request_buff,sizeof (file_download_request_buff),0);
									if(file_get_size<0)
									{
										printf("Error sending file\n");
									}
									else
									{
										if(file_left_download>chunks)
										{
											//receive the chunks of data
											if ((rec_bytes = recv(cl[i].fd, recv_file_buffer,chunks ,0)) == -1) 
											{
													perror("recv");
			
											}
											else
											{
												//printf("buffer received is-->%s\n",recv_file_buffer);
											}
											check_array_size=check_array_size+1;
										}
										else
										{
											if ((rec_bytes = recv(cl[i].fd, recv_file_buffer,file_left_download ,0)) == -1) 
											{
													perror("recv");
			
											}
											else
											{
												//printf("buffer received is-->%s\n",recv_file_buffer);
											}
											check_array_size=check_array_size+1;	
										}
										
									}
								        strcat(file_buff,recv_file_buffer);
									memset(download_str,0, sizeof(download_str));
									memset(file_download_request_buff,0, sizeof(file_download_request_buff));
									memset(recv_file_buffer,0,sizeof(recv_file_buffer));
									
									file_buff[strlen(file_buff)]='\0';
								
								}
							} i=1;
							
							
							for(l=0;l<array_size;l++)
							{
								if(file_check[check_array_size]==0)
								{

									file_check_flag=file_check_flag+1;
								}
													

							}
							strcat(file_buff,recv_file_buffer);
							file_buff[strlen(file_buff)]='\0';
							printf("Number of bytes missing--%d\n",file_check_flag);
							printf("Number of bytes downloaded-->%d\n",sizeof(file_buff));
							file_write=fopen("fileDownloaded.txt","w+");
							if(file_write>0)
							{
								fwrite(file_buff,1,sizeof(file_buff),file_write);
							}
							else
							{
								printf("Failed to write\n");
							}
							fclose(file_write);
							memset(file_buff,0,sizeof(file_buff));
							

							
						}
					}
					else
					{
						printf("No clients to download\n");		//No clients to initiate download
					}
				

			}
			
			else
			{
				printf("Please enter the correct input,Used help command to know the commands and format\n");	
				fflush(stdout);
			}	
		}	

	
	    for(i = listener; i <= fdmax; i++) 			//to listen for the file descriptors(source:beej multi-chat server program)
	    {
            if (FD_ISSET(i, &readfds)) 
		{ 
		        if (i == listener)			//new connection received
			 {
		            

		           if((newfd = accept(listener, (struct sockaddr*)&clientaddr, &addrlen)) == -1) //accepting new connection
				    {
						perror("Server-accept() error:\n");					
				    }
				   
				   else 
				   {
						printf("New client connected\n");
						memset(buffer,0,sizeof(buffer));			//clear memset before receiving
						inet_ntop(clientaddr.sin_family,get_in_addr((struct sockaddr *)&clientaddr),s, sizeof s);
						if (send(newfd, "welcome to My Network!", 22, 0) == -1)
						{
								perror("send");
					
						}
				
						 if ((rec_bytes = recv(newfd, buffer, sizeof(buffer), 0)) == -1) 
						{
								perror("recv");
			
						}
					
			
								//assign the address in list
						memset(temp_ipaddr,0,sizeof(temp_ipaddr));
						
						ip.s_addr = inet_addr(s); 			//retrieving the client details to add in list 
	   					hostnames = gethostbyaddr(&ip, sizeof(ip), AF_INET);
	   				 	if (hostnames != NULL && hostnames[0].h_name != NULL) 
						{
	       						 
						
	    				 	} 
					 	else 
					 	{
							herror("gethostbyaddr");
		
	    				 	}
						 strcpy(portbuffer,buffer);   		//getting the port number
						 int portlen = strlen(portbuffer);
						 portbuffer[portlen]='\0';
						length = strlen(s);
						s[length]='\0';
						strcpy(temp_ipaddr,s);
						length=strlen(temp_ipaddr);
						temp_ipaddr[length]='\0';
						
						addTolist(portbuffer,temp_ipaddr,hostnames[0].h_name,newfd);
						 
						 FD_SET(newfd, &master);  // add to master set

						if (newfd > fdmax)
						{   
									 
						    fdmax = newfd;		//track of the newfd
						}
						
						  
					
						 fflush(stdout);

						
						
					    }
					} 
				else 
				{
				    memset(buff, 0, 300);
				    if ((rec_bytes = recv(i, buff, sizeof buff, 0)) <= 0)	//recv data from client
				     {
				        
				        if (rec_bytes == 0) 					//when connection gets closed
					{
				           

					    for(n=0;n<count;n++)
					    {
						if(i==cl[n].fd)
						{		
							
							connection_id=cl[n].connection_id;	//on terminate at client remove from list
							removeClient(connection_id);
						        printf("%d removed from the list\n",cl[n].fd);	
								
						}
					    }
				
				        } 
					else
					{
				            perror("recv");
				        }
				        close(i); 
				        FD_CLR(i, &master);  //remove from master on deletion
				    } 
				   else
				   {
				        
				         for(j = listener; j <= fdmax; j++)
					 {
				            
				            if (FD_ISSET(j, &master)) 
					    {
				               
				                if (j != listener) 
						{
							char download_buff[300];
							
							char chunks[20];
							int file;
							int size;
							char file_buffer[300];
							memset(file_buffer,0,sizeof (file_buffer));
							char file_read_buffer[100];
							int t=0;
							char chunks_to_download_from[10];
							char each_client_download[10];
							char send_buffer[300];
							 memset(download_buff, 0, 300);
							int client_size;
							
							buff[strlen(buff)]='\0';

							 memset(each_client_download, 0, 10);
							
							memset(chunks, 0, 20);
							strncpy(download_buff,buff,8);
							
							download_buff[strlen(download_buff)]='\0';

							if(strncmp(download_buff,"download",8)==0)
							{
								
								if(j==i)
								{
										break;

								}
								else
								{
								for(m=9;m<strlen(buff);m++)
								{
									if(buff[m]=='@')
									{
										break;
									}
									filename[m-9]=buff[m];
									
									
								}
								
								filename[strlen(filename)]='\0';

								for(k=9;k<strlen(buff);k++)
								{
									if(buff[k]=='@')
									{
										for(m=k+1;m<strlen(buff);m++)
										{
											chunks[l]=buff[m];
										        l=l+1;
										}
									}

								}
								
								chunks[strlen(chunks)]='\0';

								
								file_chunks_to_send=atoi(chunks);
								fd=fopen(filename,"rw+");
								if(fd==NULL)
								{
									printf("Error in opening file");

								}
								else
								{
									 
									      fseek(fd,0,SEEK_END);
									      size=ftell(fd);
									      printf("the file's length is %d\n",size);
									      fclose(fd);
									   

									
									char size_str[10];
									sprintf(size_str,"%d",size);
									
									size_str[strlen(size_str)]='\0';
									
									
								if (send(i, size_str, 100, 0) == -1)
									{
											perror("send");
					
									}
									else
									{
										printf("sent file size successfully\n");
										
																			}
								        }       
							           }//end else of unable to open file
							     }// end i!=j check
							      
							       else if(strncmp(download_buff,"file",4)==0)
								{	 									
									if(j==i)
									{
										break; 
									}
								else
								{	
								 memset(chunks_to_download_from,0,sizeof(chunks_to_download_from));								
									for(m=5;m<strlen(buff);m++)
									{
										if(buff[m]=='#')
										{
											break;
										}
										chunks_to_download_from[m-5]=buff[m];
									
								}
								
								chunks_to_download_from[strlen(chunks_to_download_from)]='\0';

								for(k=5;k<strlen(buff);k++)
								{
									if(buff[k]=='#')
									{
										for(m=k+1;m<strlen(buff);m++)
										{
											chunks_to[t]=buff[m];
										        t=t+1;
										}
									}

								}
							
									
									chunks_to_download_from[strlen(chunks_to_download_from)]='\0';
									
									chunks_to[strlen(chunks_to)]='\0';
									FILE *file;
									file_chunks_send_from=atoi(chunks_to_download_from);
									file_chunks_send_to=atoi(chunks_to);
									
									
									fd=fopen(filename,"r");		//open file in read mode
               												
 
       									if((fread(file_buffer,size, 1, fd)<0)) {printf("error\n");}
									l=0;
									for(k=file_chunks_send_from;k<file_chunks_send_to;k++)
									{
										send_buffer[l]=file_buffer[k];
										l=l+1;
									}
									
									send_buffer[strlen(send_buffer)]='\0';  //send the buffer


										if (send(i, send_buffer, strlen(send_buffer), 0) == -1)
										{
												perror("send");
					
										}
										

									}
								
								}
								
								else if(strncmp(buff,"The list is",11)==0)
								{


									printf("%s\n",buff);		//publish the list on new connection
								}
										
		
										
				                } //check on listener ends
				            }//send to all ends
				        }//check on client data ends
				    }//else for check on receiving data ends
				} //connection success ends
			    } //check on file descriptors action ends
			} //looping on all file descriptors ends
		
	}				//for(;;) ends
	return 0;   
}








