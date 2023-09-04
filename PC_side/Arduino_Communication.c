#include "Arduino_Communication.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

///////////////////////////////// Function Prototypes /////////////////////////////////

// Set serial port attributes
int set_interface_attribs(int fd);

// Exits program if initial message isn't correct
// Ensures message sent by device matches IDENTIFYING_PHRASE
void check_device(int fd);

///////////////////////////////////////////////////////////////////////////////////////

// Sends a 3 element array to fd
static int write_buff(int fd, u_int8_t buf[3]) {
  // Send component data
  int wlen = write(fd, buf, 3);
  if (wlen != 3) {
    printf("Error from write: %d, %d\n", wlen, errno);
    return EXIT_FAILURE;
  }
  tcdrain(fd); // delay for output

  printf("Buffer: %d %d %d\n", buf[0], buf[1], buf[2]);

  return EXIT_SUCCESS;
}

// Sets up serial port and checks it is the right device
// returns fd
int port_setup() {
  char *portname = SERIAL_TERMINAL;
  int fd;

  fd = open(portname, O_RDWR | O_NOCTTY | O_SYNC);
  if (fd < 0) {
    printf("Error opening %s: %s\n", portname, strerror(errno));
    sleep(TIME_BEFORE_EXIT);
    exit(EXIT_FAILURE);
  }

  // baudrate BAUDRATE, 8 bits, no parity, 1 stop bit
  if (set_interface_attribs(fd) == -1) {
    sleep(TIME_BEFORE_EXIT);
    exit(EXIT_FAILURE);
  }

  check_device(fd);

  return fd;
}

int set_interface_attribs(int fd) {
  int speed = BAUDRATE;

  struct termios tty;

  if (tcgetattr(fd, &tty) < 0) {
    printf("Error from tcgetattr: %s\n", strerror(errno));
    return -1;
  }

  cfsetospeed(&tty, (speed_t)speed);
  cfsetispeed(&tty, (speed_t)speed);

  tty.c_cflag |= CLOCAL | CREAD;
  tty.c_cflag &= ~CSIZE;
  tty.c_cflag |= CS8;      /* 8-bit characters */
  tty.c_cflag &= ~PARENB;  /* no parity bit */
  tty.c_cflag &= ~CSTOPB;  /* only need 1 stop bit */
  tty.c_cflag &= ~CRTSCTS; /* no hardware flowcontrol */

  tty.c_lflag |= ICANON | ISIG; /* canonical input */
  tty.c_lflag &= ~(ECHO | ECHOE | ECHONL | IEXTEN);

  tty.c_iflag &= ~IGNCR; /* preserve carriage return */
  tty.c_iflag &= ~INPCK;
  tty.c_iflag &= ~(INLCR | ICRNL | IUCLC | IMAXBEL);
  tty.c_iflag &= ~(IXON | IXOFF | IXANY); /* no SW flowcontrol */

  tty.c_oflag &= ~OPOST;

  tty.c_cc[VEOL] = 0;
  tty.c_cc[VEOL2] = 0;
  tty.c_cc[VEOF] = 0x04;

  if (tcsetattr(fd, TCSANOW, &tty) != 0) {
    printf("Error from tcsetattr: %s\n", strerror(errno));
    return -1;
  }
  return 0;
}

// Exits program if initial message isn't correct
// Ensures message sent by device matches IDENTIFYING_PHRASE
void check_device(int fd) {
  char buf[strlen(IDENTIFYING_PHRASE) + 1];
  int rdlen;

  time_t time_start = time(NULL);
  time_t time_passed;

  rdlen = read(fd, buf, sizeof(buf) - 1);

  if (rdlen < 0)
    printf("Error from read: %d: %s\n", rdlen, strerror(errno));

  time_passed = time(NULL) - time_start;

  if (time_passed >= TIMEOUT) {
    fprintf(stderr, "Timed Out waiting for Identity\n");
    fprintf(stderr, "Disconnecting...\n");
    sleep(TIME_BEFORE_EXIT);
    exit(EXIT_FAILURE);
  }

  buf[rdlen] = 0;
  if (strcmp(buf, IDENTIFYING_PHRASE) == 0) {
    return;
  } else {
    fprintf(stderr, "Wrong Identity given:\n\"%s\"\n", buf);
    fprintf(stderr, "Disconnecting...\n");
    sleep(TIME_BEFORE_EXIT);
    exit(EXIT_FAILURE);
  }
}

// Sends position to fd (i.e. arduino)
int write_pos(int fd, u_int8_t identifier, pos component_pos) {
  if (component_pos.row != -1 && component_pos.col != -1)
    printf("Row: %d, Column: %d\n", component_pos.row, component_pos.col);
  else {
    fprintf(stderr, "Not found\n");
    return EXIT_FAILURE;
  }

  // Load data points to be sent to fd
  u_int8_t buf[3];
  buf[0] = identifier; // Says which storage unit (For future proofing)
  buf[1] = component_pos.row;
  buf[2] = component_pos.col;

  return write_buff(fd, buf);
}

int write_pin(int fd, u_int8_t pin, u_int8_t value) {
  // Load data points to be sent to fd
  u_int8_t buf[3] = {GPIO_OUT_IDENTIFIER, pin, value};
  return write_buff(fd, buf);
}

int toggle_pin(int fd, u_int8_t pin) {
  u_int8_t buf[3] = {GPIO_TOGGLE_IDENTIFIER, pin, 0};
  return write_buff(fd, buf);
}

int read_pin(int fd, u_int8_t pin) {
  u_int8_t buf[3] = {GPIO_IN_IDENTIFIER, pin, 0};
  write_buff(fd, buf);

  // TODO
  return -1; 
}