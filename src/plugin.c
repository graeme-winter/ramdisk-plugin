#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/stat.h>
#include <sys/types.h>

#include "bitshuffle.h"
#include "cJSON.h"

// common block

int nx, ny, nbytes, number_of_frames, info[1024], error;
float qx, qy;

char directory[1024];

int get_named_int(cJSON *json, char *name, int *value) {
  cJSON *ptr = NULL;

  ptr = cJSON_GetObjectItemCaseSensitive(json, name);
  if (cJSON_IsNumber(ptr)) {
    *value = ptr->valueint;
    return 0;
  }
  return 1;
}

int get_named_float(cJSON *json, char *name, float *value) {
  cJSON *ptr = NULL;

  ptr = cJSON_GetObjectItemCaseSensitive(json, name);
  if (cJSON_IsNumber(ptr)) {
    *value = (float)ptr->valuedouble;
    return 0;
  }
  return 1;
}

void plugin_open(char *filename, int info[1024], int *error) {
  *error = 0;

  // not really sure what this is useful for...
  memset(info, 1024, sizeof(int));
  info[0] = 1;

  if (strlen(filename) > 1023) {
    fprintf(stderr, "filename too long: %s\n", filename);
    *error = 1;
    return;
  }

  strcpy(directory, filename);

  struct stat st;

  char scr[1280];

  sprintf(scr, "%s/start_1", directory);

  if (stat(scr, &st)) {
    fprintf(stderr, "Error reading %s\n", scr);
    *error = 1;
    return;
  }
}

void plugin_get_header(int *_nx, int *_ny, int *_nbytes, float *_qx, float *_qy,
                       int *_number_of_frames, int info[1024], int *error) {
  struct stat st;
  char scr[1280];

  sprintf(scr, "%s/start_1", directory);

  if (stat(scr, &st)) {
    fprintf(stderr, "Error reading %s\n", scr);
    *error = 1;
    return;
  }

  char *json_text = (char *)malloc(sizeof(char) * (st.st_size + 1));

  FILE *fin = fopen(scr, "r");
  fread(json_text, st.st_size, 1, fin);
  json_text[st.st_size] = 0;
  fclose(fin);

  cJSON *json = cJSON_Parse(json_text);

  if (json == NULL) {
    fprintf(stderr, "Error parsing %s\n", scr);
    *error = 1;
    free(json_text);
    return;
  }

  int status;

  status = get_named_int(json, "x_pixels_in_detector", &nx);
  if (status) {
    fprintf(stderr, "Failed to read %s\n", "x_pixels_in_detector");
  }

  status = get_named_int(json, "y_pixels_in_detector", &ny);
  if (status) {
    fprintf(stderr, "Failed to read %s\n", "y_pixels_in_detector");
  }

  status = get_named_int(json, "bit_depth_image", &nbytes);
  if (status) {
    fprintf(stderr, "Failed to read %s\n", "bit_depth_image");
  }

  status = get_named_int(json, "nimages", &number_of_frames);
  if (status) {
    fprintf(stderr, "Failed to read %s\n", "nimages");
  }

  status = get_named_float(json, "x_pixel_size", &qx);
  if (status) {
    fprintf(stderr, "Failed to read %s\n", "x_pixel_size");
  }

  status = get_named_float(json, "y_pixel_size", &qy);
  if (status) {
    fprintf(stderr, "Failed to read %s\n", "y_pixel_size");
  }

  free(json_text);

  // convert units
  nbytes /= 8;
  qx *= 1000.0;
  qy *= 1000.0;

  if ((nbytes != 2) && (nbytes != 4)) {
    fprintf(stderr, "Image depth of %d bytes unsupported\n", nbytes);
    *error = 1;
  }

  *_nx = nx;
  *_ny = ny;
  *_nbytes = nbytes;
  *_qx = qx;
  *_qy = qy;
  *_number_of_frames = number_of_frames;
}

void plugin_get_data(int *_frame_number, int *_nx, int *_ny, int *_data_array,
                     int info[1024], int *error) {
  *error = 0;

  char scr[1280];
  sprintf(scr, "%s/image_%06d_2", directory, (*_frame_number) - 1);

  struct stat st;

  if (stat(scr, &st)) {
    fprintf(stderr, "Error reading %s\n", scr);
    *error = 1;
    return;
  }

  char *chunk = (char *)malloc(sizeof(char) * (st.st_size));

  FILE *fin = fopen(scr, "r");
  fread(chunk, st.st_size, 1, fin);
  fclose(fin);

  // decompress => intermediate buffer if nbytes == 2 else straight to target
  // if nbytes is 4 - if 16 bit just copying not sign extending quite yet...
  if (nbytes == 2) {
    uint16_t *buffer = (uint16_t *)malloc(nbytes * ny * nx);
    bshuf_decompress_lz4((chunk) + 12, (void *)buffer, st.st_size, nbytes, 0);
    for (int j = 0; j < ny * nx; j++) {
      _data_array[j] = buffer[j];
    }
    free(buffer);
  } else if (nbytes == 4) {
    bshuf_decompress_lz4((chunk) + 12, (void *)_data_array, st.st_size, nbytes,
                         0);
  } else {
    // we should never get here
  }

  free(chunk);
}

void plugin_close(int *error) { *error = 0; }