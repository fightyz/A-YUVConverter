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
	int frame_num;
};