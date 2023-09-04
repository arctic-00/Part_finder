#include <sys/types.h>

#define SERIAL_TERMINAL     "/dev/ttyUSB0"
#define BAUDRATE            B115200

// Time in seconds to wait for USB to give identity 
#define TIMEOUT             4
#define IDENTIFYING_PHRASE  "Part Finder\n"

// Time before exiting program after error
#define TIME_BEFORE_EXIT    5

#define GPIO_IN_IDENTIFIER 32
#define GPIO_OUT_IDENTIFIER 33
#define GPIO_TOGGLE_IDENTIFIER 34

#define CONTAINER_1_IDENTIFIER 1
#define CONTAINER_2_IDENTIFIER 2
#define CONTAINER_3_IDENTIFIER 3

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



