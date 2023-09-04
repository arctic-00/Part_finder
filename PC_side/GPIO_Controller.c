#include "Arduino_Communication.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>

///////////////////////////////// Function Prototypes /////////////////////////////////

// Converts name of whatever you're controlling to its corresponding pin numbers
int pin_name_to_number(char *name);

///////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[]) {
  if (argc != 2 && argc != 3) {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "%s <GPIO Pin or Name of item> <Optional: Value (Otherwise value is toggled)>\n", argv[0]);
  }

  int pin;
  if (isdigit(argv[1][0])) {
    char *str_ptr;
    pin = strtol(argv[1], &str_ptr, 10);

    // Check if it is only a number 
    if (str_ptr[0] != '\0') pin = pin_name_to_number(argv[1]);

  } else pin = pin_name_to_number(argv[1]);

  int fd = port_setup();

  if (argc == 2) {
    toggle_pin(fd, pin);
  }


}


int pin_name_to_number(char *name) {
  
}