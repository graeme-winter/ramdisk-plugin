#include <stdio.h>
#include <stdlib.h>

#include "plugin.h"

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "%s <directory>\n", argv[0]);
    return 1;
  }
  int status;
  int info[1024];

  plugin_open(argv[1], info, &status);

  int nx, ny, nbytes, number_of_frames;
  float qx, qy;

  plugin_get_header(&nx, &ny, &nbytes, &qx, &qy, &number_of_frames, info,
                    &status);

  printf("nx=%d ny=%d nbytes=%d number_of_frames=%d qx=%.4f qy=%.4f\n", nx, ny,
         nbytes, number_of_frames, qx, qy);

  int *frame = (int *)malloc(sizeof(int) * nx * ny);

  for (int j = 1; j <= number_of_frames; j++) {
    int tx, ty;
    plugin_get_data(&j, &tx, &ty, frame, info, &status);
    int total = 0;
    for (int k = 0; k < nx * ny; k++) {
      if (frame[k] < 0xfffe)
        total += frame[k];
    }
    printf("%d %d\n", j, total);
  }

  plugin_close(&status);

  free(frame);

  return 1;
}