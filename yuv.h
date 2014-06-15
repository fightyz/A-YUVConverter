#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <string.h>
#include <errno.h>

typedef struct
{
	/* data */
	int y_width;
	int y_height;
	
	int u_width;
	int u_height;
	
	int v_width;
	int v_height;

	char * input_file_name;
	char * output_file_name;

	int bit_depth;
	long frame_num;
	long frame_size;

	int pixel_size;
} yuv_info;

void
GetInfo(int argc, char * const argv[], yuv_info * s_yuv_info);
void
YUVInit(yuv_info * s_yuv_info, unsigned char ** p_Y_space);
void
ReadImage(yuv_info * s_yuv_info, unsigned char * p_Y_space, FILE * fp_input_file, long offset);
void
WriteImage(yuv_info * s_yuv_info, unsigned char * p_Y_space, FILE * fp_output_file);