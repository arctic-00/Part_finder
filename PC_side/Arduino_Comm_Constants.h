
#define SERIAL_TERMINAL "/dev/ttyUSB0"
#define BAUDRATE        B115200

// Container 1 size
#define C_1_NUM_ROWS 5
#define C_1_NUM_COLS 6

////////////////////////// CONSTANTS SHOULD NOT NEED TO BE CHANGED AFTER THIS POINT //////////////////////////

// Time in seconds to wait for USB to give identity
#define TIMEOUT            4
#define IDENTIFYING_PHRASE "P9;s\n"

// Time before exiting program after error
#define TIME_BEFORE_EXIT 5

#define GPIO_IN_IDENTIFIER     32
#define GPIO_OUT_IDENTIFIER    33
#define GPIO_TOGGLE_IDENTIFIER 34

#define CONTAINER_1_IDENTIFIER 1
#define CONTAINER_2_IDENTIFIER 2
#define CONTAINER_3_IDENTIFIER 3

// The Pin cannot be changed
#define PIN_BLOCKED -2
