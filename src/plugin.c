#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/stat.h>
#include <sys/types.h>

#include "cJSON/cJSON.h"

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
  memset(info, sizeof(int), 1024);
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

  char scr[1280];

  sprintf(scr, "%s/start_1", directory);

  if (stat(scr, &st)) {
    fprintf(stderr, "Error reading %s\n", scr);
    *error = 1;
    return;
  }

  char *json_text = (char *)malloc(sizeof(char) * (st.st_size + 1));

  FILE *fin = fopen(argv[1], "r");
  fread(json_text, st.st_size, 1, fin);
  json_text[st.st_size] = 0;
  fclose(fin);

  cJSON *json = cJSON_Parse(json_text);

  if (json == NULL) {
    fprintf(stderr, "Error parsing %s\n", argv[1]);
    free(json_text);
    return 1;
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

  *_nx = nx;
  *_ny = ny;
  *_nbytes = nbytes;
  *_qx = qx;
  *_qy = qy;
  *_number_of_frames = number_of_frames;
}