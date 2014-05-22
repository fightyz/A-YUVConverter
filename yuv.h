#include <stdio.h>
#include <stdlib.h>

struct YUV_Info
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
};

void
GetInfo(int argc, char * const argv[], struct YUV_Info * s_yuv_info);
void
YUV_Init(struct YUV_Info * s_yuv_info, FILE * fp_input_file, unsigned char ** p_Y_space);
void
ReadImage(struct YUV_Info * s_yuv_info, unsigned char * p_Y_space, FILE * fp_input_file, long offset);
void
WriteImage(struct YUV_Info * s_yuv_info, unsigned char * p_Y_space, FILE * fp_output_file);