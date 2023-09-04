#define SERIAL_TERMINAL     "/dev/ttyUSB0"
#define BAUDRATE            B115200

// Time in seconds to wait for USB to give identity 
#define TIMEOUT             4
#define IDENTIFYING_PHRASE  "Part Finder\n"

// Time before exiting program after error
#define TIME_BEFORE_EXIT    5


typedef struct {
    int row;
    int col;
} pos;

// Sets up serial port and checks it is the write device
// returns fd
int port_setup();

int set_interface_attribs(int fd, int speed);

// Exits program if initial message isn't correct
// Ensures message sent by device matches IDENTIFYING_PHRASE
void check_device(int fd);

// Sends position to fd (i.e. arduino's serial port)
void write_pos(int fd, int identifier, pos component_pos);

