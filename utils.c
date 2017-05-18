/**
 * Chatroom Lab
 * CS 241 - Fall 2016
 */
#include <stdlib.h>
#include <string.h>

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>

#include "utils.h"
static const size_t MESSAGE_SIZE_DIGITS = 4;

char *create_message(char *name, char *message) {
  int name_len = strlen(name);
  int msg_len = strlen(message);
  char *msg = calloc(1, msg_len + name_len + 4);
  sprintf(msg, "%s: %s", name, message);

  return msg;
}

ssize_t get_message_size(int socket) {
  int32_t size;
  ssize_t read_bytes =
      read_all_from_socket(socket, (char *)&size, MESSAGE_SIZE_DIGITS);
  if (read_bytes == 0 || read_bytes == -1)
    return read_bytes;

  return (ssize_t)ntohl(size);
}

ssize_t write_message_size(size_t size, int socket) {
  // Your code here
//  return 9001;

	uint32_t thingy = htonl(size);
	ssize_t bytes = write_all_to_socket(socket, (char*)&thingy, sizeof(thingy));

	if(bytes == 0 || bytes == -1) return bytes;

	return bytes;
}

ssize_t read_all_from_socket(int socket, char *buffer, size_t count) {
  // Your Code Here
//  return 9001;

//	return read(socket, buffer, count);

	ssize_t sign_count = (ssize_t)count;

	ssize_t bytes = 0;
	ssize_t count_temp = count;
	void * buff_temp = (void*)buffer;

	while(1){

		bytes += read(socket, buff_temp, count_temp);

		if(bytes == 0) return 0;
		if(bytes == -1 && errno != EINTR) return -1;
		if(bytes == -1 && errno == EINTR){

			bytes = 0;
			count_temp = sign_count;
			buff_temp = (void*)buffer;
		}
		else if(bytes > 0 && bytes < sign_count){

			buff_temp += bytes;
			count_temp -= bytes;
		}
		else if(bytes == sign_count) return bytes;
		else if(bytes > sign_count) {printf("CALL JESUS\n"); return -69;}
	}

}

ssize_t write_all_to_socket(int socket, const char *buffer, size_t count) {
  // Your Code Here
//  return 9001;

//	return write(socket, buffer, count);

	ssize_t sign_count = (ssize_t)count;

	ssize_t bytes = 0;
	ssize_t count_temp = count;
	void * buff_temp = (void*)buffer;

	while(1){
		bytes += write(socket, buff_temp, count_temp);
		if(bytes == 0) return 0;
		if(bytes == -1 && errno != EINTR) return -1;
		if(bytes == -1 && errno == EINTR){
			bytes = 0;
			count_temp = sign_count;
			buff_temp = (void*)buffer;
		}
		else if(bytes > 0 && bytes < sign_count){
			buff_temp += bytes;
			count_temp -= bytes;
		}
		else if(bytes == sign_count) return bytes;
		else if(bytes > sign_count) {printf("CALL JESUS\n"); return -420;}
	}
}
