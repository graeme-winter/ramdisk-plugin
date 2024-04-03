#pragma once

void plugin_open(char *filename, int info[1024], int *error);
void plugin_get_header(int *_nx, int *_ny, int *_nbytes, float *_qx, float *_qy,
                       int *_number_of_frames, int info[1024], int *error);
void plugin_get_data(int *_frame_number, int *_nx, int *_ny, int *_data_array,
                     int info[1024], int *error);
void plugin_close(int *error);
