/**
 * Chatroom Lab
 * CS 241 - Fall 2016
 */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define READ_FAIL_NUM 2



static size_t read_counter;
static size_t write_counter;

int my_read(int fd, void *buff, size_t count) {
  read_counter++;

  if (read_counter == READ_FAIL_NUM) {
    read_counter = 0;

//	printf("HA\n");

    if (rand() % 2 > 0) {
      return read(fd, buff, count / 2 + 1);
    } else {
      errno = EINTR;
      return -1;
    }
  }

  return read(fd, buff, count);
}

int my_write(int fd, void *buff, size_t count) {
  write_counter++;

  if (write_counter == READ_FAIL_NUM) {
    write_counter = 0;

//	printf("HA\n");

    if (rand() % 2 > 0) {
      return write(fd, buff, count / 2 + 1);
    } else {
      errno = EINTR;
      return -1;
    }
  }

  return write(fd, buff, count);
}
