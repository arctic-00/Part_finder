#include "Arduino_Communication.h"

#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "%s <GPIO Pin or Name of item> <Optional: Value>\n", argv[0]);
  }

  


}