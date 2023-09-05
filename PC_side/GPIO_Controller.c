#include "Arduino_Communication.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define PIN_NAME_FILE_PATH "./GPIO_Pin_Names.txt"

#define MAX_LINE_LEN 254

///////////////////////////////// Function Prototypes
////////////////////////////////////

// Converts name of whatever you're controlling to its corresponding pin numbers
int pin_name_to_number(char *name);

///////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[]) {
  if (argc != 2 && argc != 3) {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "%s <GPIO Pin or Name of item> <Optional: Value (Otherwise value is toggled)>\n", argv[0]);
    return EXIT_FAILURE;
  }

  // Figure out whether a pin number or a string alias (defined in
  // ./output/GPIO_Pin_Names.txt) was given.
  int pin;
  if (isdigit(argv[1][0])) {
    char *str_ptr;
    pin = strtol(argv[1], &str_ptr, 10);

    // Check if the first argument only contains digits
    // Otherwise interpret it as a pin alias.
    // This makes "0 Pin Name" a valid pin alias  .
    if (str_ptr[0] != '\0')
      pin = pin_name_to_number(argv[1]);

  } else
    pin = pin_name_to_number(argv[1]);

  if (pin < 0) {
    fprintf(stderr, "Pin name '%s' was not found", argv[1]);
    sleep(TIME_BEFORE_EXIT);
    exit(EXIT_FAILURE);
  }

  int fd = port_setup();

  // Wait for arduino to setup and be able to receive
  sleep(2);

  if (argc == 2)
    toggle_pin(fd, pin);
  if (argc == 3)
    write_pin(fd, pin, strtol(argv[2], NULL, 10));
}

int pin_name_to_number(char *name) {
  FILE *fp = fopen(PIN_NAME_FILE_PATH, "r");

  if (fp == NULL) {
    printf("Path used: \"%s\"\n", PIN_NAME_FILE_PATH);
    perror("Error opening Pin names reference file");
    sleep(TIME_BEFORE_EXIT);
    exit(EXIT_FAILURE);
  }

  // Check for any matches to name
  char line[MAX_LINE_LEN + 1];
  while (fgets(line, MAX_LINE_LEN, fp)) {
    char *archived_name = strtok(line, ":");

    if (strcmp(archived_name, name) == 0) {
      fclose(fp);
      return strtol(strtok(NULL, ":"), NULL, 10);
    }
  }

  fclose(fp);
  return -1;
}