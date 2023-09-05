#include "Arduino_Comm_Constants.h"
#include <sys/types.h>
typedef struct {
  int row;
  int col;
} pos;

// Sets up serial port and checks it is the write device
// returns fd
int port_setup();

// Sends position to fd (i.e. arduino's serial port)
int write_pos(int fd, u_int8_t identifier, pos component_pos);

// Sends value for specified pin to output, on fd (i.e. arduino's serial port)
int write_pin(int fd, u_int8_t pin, u_int8_t value);

// Toggles output for specified pin, on fd (i.e. arduino's serial port)
int toggle_pin(int fd, u_int8_t pin);

// Reads pin value, on fd (i.e. arduino's serial port)
int read_pin(int fd, u_int8_t pin); // !!!TODO!!!