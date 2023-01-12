/*
File : server.c

description : server side of process controller adding, deleting and controlling the process.

*/

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include "common.h"
#include "server.h"

/* To count the number of clients connected to the server */
int cl_count = 0;

/* msg_action structure */
msg_action_t  msg_actions[MSG_MAX];
user_info_t  *user_list_start = NULL;

// Program to control the process 
int add() 
{
    int pid = fork();
    if (pid == -1)
    {
    return 1;
    }
    if (pid == 0) 
    {
      while (1) 
	{
            printf("Welcome To Process Controller\n");
            usleep(300000);
      }
    } 
    else 
    {
        kill(pid, SIGSTOP);
        int t;
        do 
	  {
            printf("Time in seconds for execution: ");
            scanf("%d", &t);
            if (t>0)
		{
                kill(pid, SIGCONT);
                sleep(t);
                kill(pid, SIGSTOP);
            }
        } 
	  while (t > 0);
          kill(pid, SIGKILL);
          wait(NULL);
    }
    return 0;
}

/*
//Program to add the process name
int add_data()
{
	FILE *p;
	st s1;
	if((p=fopen("Program","ab"))==NULL)
	{
		printf("\nCant open file\n");
		return EXIT_FAILURE;
	}
	while(1)
	{
		printf("Enter the program name -");
		scanf("%s",s1.name);
		printf("Program added successfully\n");
		break;
	}			
	fwrite(&s1,sizeof(st),1,p);
	fclose(p);
	return EXIT_SUCCESS;
}
*/

int add_data()
{
    /* Variable to store user content */
    char data[DATA_SIZE];

    /* File pointer to hold reference to our file */
    FILE * fPtr;


    /* 
     * Open file in w (write) mode. 
     * "data/file1.txt" is complete path to create file
     */
    fPtr = fopen("files/file1.c", "w");


    /* fopen() return NULL if last operation was unsuccessful */
    if(fPtr == NULL)
    {
        /* File not created hence exit */
        printf("Unable to create file.\n");
        exit(EXIT_FAILURE);
    }


    /* Input contents from user to store in file */
    printf("Enter contents to store in file : \n");
    fgets(data, DATA_SIZE, stdin);


    /* Write data to file */
    fputs(data, fPtr);


    /* Close file to save file data */
    fclose(fPtr);


    /* Success message */
    printf("File created and saved successfully. :) \n");


    return 0;
}


/*
//Program to delete the process name
int delete_data()
{
	char ch;
	FILE *p, *q;
	if((p=fopen("Program","rb"))==NULL)
	{
		printf("\nCant open file\n");
		return EXIT_FAILURE;
	}
	q=fopen("temp","wb");
	while((ch=getc(p))!=EOF)
	{
		if(ch!='E' && ch!='e')
		{
			putc(ch,q);
		}
	}

	fclose(p);
	fclose(q);
	remove("Program");
	rename("temp","Program");
	return EXIT_SUCCESS;
}
*/
int delete_data() 
{
	//printf("Enter the process to delete\n");
	//remove("files/file1.c");
    	if (remove("./Files/file1.c") == 0) 
	{
        printf("The file is deleted successfully.");
   	} 
	else 
	{
        printf("The file is not deleted.");
    	}
   	return 0;
}

user_info_t* add_user_to_list(char *uname)
{
  user_info_t *new_user = malloc(sizeof(user_info_t));
  user_info_t *temp = user_list_start;
  if (new_user == NULL)
  {
    printf("Failed to alloc memory \n");
    return NULL;
  }
  strncpy(new_user->uname, uname, strlen(uname));
  //strncpy(new_user->pwd, pwd, strlen(pwd));
  new_user->next = NULL;

  if (user_list_start == NULL)
  {
    user_list_start = new_user;
    printf("Added first user \n");
    return new_user;
  }
  while(temp->next != NULL)
  temp = temp->next;
  temp->next = new_user;
  printf("Added new user \n");
  return user_list_start;
}

int search_user_pwd_in_list(char *uname)
{
  user_info_t *temp = user_list_start;
  if (temp == NULL)
  {
    printf("List is empty \n");
    return USER_NOT_FOUND;
  }
  while(temp != NULL)
  {
    if (strcmp(temp->uname, uname) == 0)
    {
      printf("User %s found \n", uname);
    }	
    else
    {
      return USER_FOUND;
    }
    temp = temp->next;
  }
  printf("User %s not found \n", uname);
  return USER_NOT_FOUND;
}

void handle_login_msg (int sockfd, msg_t *msg)
{
  	msg_t reply_msg;
  	int ret;
  	printf("Handling message: %d \n", msg->msg_type);
  	printf("Username : %s \n", msg->msg_data);
  	ret = search_user_pwd_in_list(msg->msg_data);
  	ret = send(sockfd, (char *) &reply_msg, sizeof(msg), 0);
  	printf("Sent %d bytes reply .. \n", ret);
}

void handle_uname_passwd_msg (int sockfd, msg_t *msg)
{
  	msg_t reply_msg;
  	printf("Handling message: %d \n", msg->msg_type);
  	printf("Username : %s \n", msg->msg_data);
  	add_user_to_list(msg->msg_data);
  	reply_msg.msg_type = MSG_AUTH_SUCCESS;
  	int ret = send(sockfd, (char *) &reply_msg, sizeof(msg), 0);
  	printf("Sent %d bytes reply .. \n", ret);
}

void handle_process(int sockfd, msg_t *msg)
{
	printf("Process name : %s\n", msg->msg_data);
      if(msg->msg_data == "Add")
	{
		add();
	}
      else
	{
		add();
	}
}

void handle_add_process(int sockfd, msg_t *msg)
{
	printf("Process name : %s\n", msg->msg_data);
      if(msg->msg_data != NULL)
	{
		add_data();
	}
      else
	{
		printf("\nInvalid\n");
	}
}

void handle_delete_process(int sockfd, msg_t *msg)
{
	char *uname;
	printf("Process name : %s\n", msg->msg_data);
      if(msg->msg_data != NULL)
	{
		delete_data();
	}
      else
	{
		printf("\nInvalid\n");
	}
}


void setup_message_handlers()
{
  	msg_actions[MSG_UNAME_PASSWD].msg_action_function = handle_process;
  	msg_actions[MSG_ADD_PROCESS].msg_action_function= handle_add_process;
	msg_actions[MSG_DELETE_PROCESS].msg_action_function= handle_delete_process;
}


void handle_client_close(int sock_fd)
{

  	printf("Client on socket %d closed \n", sock_fd);
  	close(sock_fd);

}

void process_client_messages(int sockfd, char *recv_buffer)
{
  msg_t *m = (msg_t *) recv_buffer;
  switch (m->msg_type)
  {
    case MSG_UNAME_PASSWD:
                  printf("Received msg type %d socket = %d ... \n", m->msg_type, sockfd);
                  msg_actions[m->msg_type].msg_action_function(sockfd, m);
                  break;
    case MSG_LOGIN:
                  printf("Received msg type %d socket = %d ... \n", m->msg_type, sockfd);
                  msg_actions[m->msg_type].msg_action_function(sockfd, m);
                  break;

    case MSG_DELETE_PROCESS:
                  printf("Received msg type %d socket = %d ... \n", m->msg_type, sockfd);
                  msg_actions[m->msg_type].msg_action_function(sockfd, m);
                  break;

    case MSG_ADD_PROCESS:
                  printf("Received msg type %d socket = %d ... \n", m->msg_type, sockfd);
                  msg_actions[m->msg_type].msg_action_function(sockfd, m);
                  break;
    default:
                  printf("Received invalid msg type ... \n");
                  break;
  }
  return;
}

/* Thread to handle clients */
void* client_handler(void *client_sock_fd)
{
  	int *cl_sock_fd = (int *) client_sock_fd;
  	int ret = 0;
	char send_buffer[SEND_BUFFER_SIZE];
  	char recv_buffer[RECV_BUFFER_SIZE];
	printf("%s():%d Client Fd = %d\n",__FUNCTION__, __LINE__, *cl_sock_fd);
	while(1)
  	{
    		printf("%s():%d Waiting on recv for fd = %d \n",__FUNCTION__, __LINE__, *cl_sock_fd);
		ret = recv(*cl_sock_fd, recv_buffer, sizeof(recv_buffer), 0);
		if (ret == 0)
    		{
      		printf("%s():%d Client has closed on socket fd = %d \n",__FUNCTION__, __LINE__, *cl_sock_fd);
			handle_client_close(*cl_sock_fd);
			cl_count--;
    			pthread_exit(NULL);
            }
		process_client_messages(*cl_sock_fd, recv_buffer);
	}
	pthread_exit(NULL);
}

int main()
{

  int server_fd, ret;
  struct sockaddr_in server_addr;
  int cl_sock_fd[MAX_CLIENTS];  
  setup_message_handlers();
  pthread_t cl_threads[MAX_CLIENTS];
  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0) 
  {
    printf("Error in connection.\n");
    exit(1);
  }
  printf("TCP Server Socket is created.\n");
  memset(&server_addr, '\0',  sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  ret = bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
  if (ret < 0) 
  {
    printf("Error in binding.\n");
    exit(1);
  }
  if (listen(server_fd, 10) == 0) 
  {
    printf("Listening...\n\n");
  }
  while (1) 
  {
    ret = accept(server_fd, NULL, NULL);
    if (ret < 0) 
    {
      perror("accept failed: ");
      exit(1);
    }
    cl_sock_fd[cl_count] = ret;
    printf("cl_count = %d fd = %d clfd = %d \n",cl_count, ret, cl_sock_fd[cl_count]);
    ret = pthread_create(&cl_threads[cl_count], NULL, client_handler,&cl_sock_fd[cl_count]);
    if (ret == 0)
    {
      printf("A new thread is created for client on fd: %d \n",cl_sock_fd[cl_count]);
	cl_count++;
      printf("Total clients connected : %d\n\n", cl_count);
    }  
    if (cl_count == MAX_CLIENTS)
    {
      printf("Max clients %d are connected..No more connections will be accepted\n",cl_count);
      break;
    }
  }
  for (int i = 0; i < cl_count; i++)
  {  
    pthread_join(cl_threads[i], NULL);
  }
  return 0;
}

