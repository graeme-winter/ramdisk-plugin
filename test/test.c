#include <stdio.h>

#include "plugin.h"

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "%s <directory>\n", argv[0]);
    return 1;
  }
  int status;
  int info[1024];

  plugin_open(argv[1], info, &status);
  plugin_close(&status);

  return 1;
}