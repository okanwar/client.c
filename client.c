/**
 * client.c
 *
 * @author Om Kanwar
 *
 * USD COMP 375: Computer Networks
 * Project 1
 *
 *
 */

#define _XOPEN_SOURCE 600
#define BUFF_SIZE 1024
#define MAXLINE 100

#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>

long prompt();
int connectToHost(char *hostname, char *port);
void mainLoop(int fd);
void servefd(int fd, char *buff, size_t buff_len);
void recievefd(int fd, char *buff, size_t max_len);
void display_menu();
void option();
void parseArgs(const char *cmdline, char **argv);

static char cmdline_copy[MAXLINE];

void parseArgs(const char *cmdline, char **argv) {
	unsigned int k;
	for(k = 0; k < sizeof(argv)-1; k++) {
		argv[k] = NULL;
	}

	strcpy(cmdline_copy, cmdline);

	char *token;
	char *remainder = cmdline_copy;
	char delim[] = " \n";
	unsigned int i = 0;

	while((token = strtok_r(remainder, delim, &remainder))) {
		argv[i] = token;
		i++;
	}
}

int main() {
	int server_fd = connectToHost("comp375.sandiego.edu", 44144);
	mainLoop(server_fd);
	close(server_fd);
	return 0;
}

/**
 * Loop to keep asking user what they want to do and calling the appropriate
 * function to handle the selection.
 *
 * @param server_fd Socket file descriptor for communicating with the server
 */
void mainLoop(int fd) {
	while (1) {
		long selection = prompt();
		char *choice;

		switch (selection) {
			case 1:
				choice = "Air temperature";
				option(fd, choice);
				close(fd);
				fd = connectToHost("comp375.sandiego.edu", "47789");
				break;
			case 2:
				choice = "Relative humidity";
				option(fd, choice);
				close(fd);
				fd = connectToHost("comp375.sandiego.edu", "47789");
				break;
			case 3:
				choice = "Wind speed";
				option(fd, choice);
				close(fd);
				fd = connectToHost("comp375.sandiego.edu", "47789");
				break;
			case 4:
				printf("\n%s\n", "GOODBYE!!");
				close(fd);
				exit(1);
				break;
			default:
				fprintf(stderr, "ERROR: Invalid selection\n");
				break;
		}
	}
}

/** 
 * Print command prompt to user and obtain user input.
 *
 * @return The user's desired selection, or -1 if invalid selection.
 */
long prompt() {
	//Display menu options
	display_menu();
	printf("%s", "Selection: ");

	// Read in a value from standard input
	char input[10];
	memset(input, 0, 10); // set all characters in input to '\0' (i.e. nul)
	char *read_str = fgets(input, 10, stdin);

	// Check if EOF or an error, exiting the program in both cases.
	if (read_str == NULL) {
		if (feof(stdin)) {
			printf("Error");
			exit(0);
		}
		else if (ferror(stdin)) {
			perror("fgets");
			printf("Error");
			exit(1);
		}
	}

	// get rid of newline, if there is one
	char *new_line = strchr(input, '\n');
	if (new_line != NULL) new_line[0] = '\0';

	// convert string to a long int
	char *end;
	long selection = strtol(input, &end, 10);

	if (end == input || *end != '\0') {
		selection = -1;
	}

	return selection;
}

/**
 * Socket implementation of connecting to a host at a specific port.
 *
 * @param hostname The name of the host to connect to (e.g. "foo.sandiego.edu")
 * @param port The port number to connect to
 * @return File descriptor of new socket to use.
 */
int connectToHost(char *hostname, char *port) {
	// Step 1: fill in the address info in preparation for setting 
	//   up the socket
	int status;
	struct addrinfo hints;
	struct addrinfo *servinfo;  // will point to the results

	memset(&hints, 0, sizeof hints); // make sure the struct is empty
	hints.ai_family = AF_INET;       // Use IPv4
	hints.ai_socktype = SOCK_STREAM; // TCP stream sockets

	// get ready to connect
	if ((status = getaddrinfo(hostname, port, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
		exit(1);
	}

	// Step 2: Make a call to socket
	int fd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
	if (fd == -1) {
		perror("socket");
		exit(1);
	}

	// Step 3: connect!
	if (connect(fd, servinfo->ai_addr, servinfo->ai_addrlen) != 0) {
		perror("connect");
		exit(1);
	}
	freeaddrinfo(servinfo);
	return fd;
}

void display_menu() {
		
printf("WELCOME TO THE COMP375 SENSOR NETWORK \n\n\n");
	
	printf("%s", "Which sensor would you like to read: \n\n");
	printf("%s", "1. Air temperature\t\n");
	printf("%s", "2. Relative humidity\t\n");
	printf("%s", "3. Wind speed\t\n");
	printf("%s", "4. Quit Program\t\n\n");

}

void servefd(int fd, char *buff, size_t buff_len) {
	int message_sent = send(fd, buff, buff_len, 0);
	if (message_sent == 0) {
		printf("Server connection failed, goodbye.\n");
		exit(1);
	}
	else if (send == -1) {
		perror("send");
		exit(1);
	}
}

void recievefd( int fd, char *buff, size_t max_len) {
	int recvd = recv(fd, buff, max_len, 0);
	if (recvd == 0) {
		printf("Server connection failed, goodbye.\n");
		exit(1);
	}
	else if (recvd == -1) {
		perror("recv");
		exit(1);
	}
}

void option(int fd, char *choice) {
	
	char buff[BUFF_SIZE];
	char *buff_reset[BUFF_SIZE];
	
	//use servefd function to send server message
	servefd(fd, "AUTH password123\n", 17);
	
	//receive message from server and place into buffer	
	recievefd(fd, buff, BUFF_SIZE);
	
	//parse response into server
	parseArgs(buff, buff_reset);
	//Clears buffer so next message can be transmitted
	memset(buff, 0, BUFF_SIZE);
	parseArgs(buff, buff_reset);

	//connect to server using correct port
	fd = connectToHost(buff_reset[1],buff_reset[2]);

	servefd(fd, "AUTH sensorpass321\n", 21);
	recievefd(fd, buff, BUFF_SIZE);
	memset(buff, 0, BUFF_SIZE);
	servefd(fd, choice, 17);
	recievefd(fd, buff, BUFF_SIZE);
	memset(buff_reset,0,BUFF_SIZE);
	parseArgs(buff, buff_reset);
	
	//Translate time format into readable format for user
	time_t time;
	time = strtoul(ret_buff[0],NULL,0);
	
	//Format output of messages from program
	printf("\n%s%s%s", "The last ", choice,"reading was ");
	printf("%s%s%s", buff_reset[1], buff_reset[2], "taken at ");
	printf("%s\n", ctime(&time));

	memset(buff_reset, 0, BUFF_SIZE);
}
