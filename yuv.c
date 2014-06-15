#include "yuv.h"
#include <unistd.h>
#include <getopt.h>
#include <sys/stat.h>

void
GetInfo(int argc,
		char * const argv[],
		yuv_info * s_yuv_info)
{
	int opt;
	while((opt = getopt(argc, argv, "i:o:r:b:")) != -1){
		switch(opt){
			case 'i':
				s_yuv_info->input_file_name = optarg;
				break;
			case 'o':
				s_yuv_info->output_file_name = optarg;
				break;
			case 'r':
				sscanf(optarg, "%dx%d", &s_yuv_info->y_width, &s_yuv_info->y_height);
				break;
			case 'b':
				sscanf(optarg, "%d", &s_yuv_info->bit_depth);
				break;
			default:
				exit(-1);
				break;
		}
	}
}

void
YUVInit(yuv_info * s_yuv_info,
		 unsigned char ** p_Y_space)
{
	s_yuv_info->u_width = s_yuv_info->v_width = s_yuv_info->y_width;
	s_yuv_info->u_height = s_yuv_info->v_height = s_yuv_info->y_height;

	int pixel_size = s_yuv_info->bit_depth % 8
	? s_yuv_info->bit_depth / 8 + 1
	: s_yuv_info->bit_depth / 8;
	s_yuv_info->pixel_size = pixel_size;

	struct stat st;
	stat(s_yuv_info->input_file_name, &st);
	off_t file_size = st.st_size;
	long frame_size = s_yuv_info->y_width * s_yuv_info->y_height / 2 * 3 * pixel_size;
	s_yuv_info->frame_size = frame_size;
	s_yuv_info->frame_num = file_size / frame_size;

	// allocate memory for convert buff Y_space
	p_Y_space[0] = malloc(s_yuv_info->y_width * s_yuv_info->y_height * pixel_size);
	p_Y_space[1] = malloc(s_yuv_info->y_width * s_yuv_info->y_height * pixel_size);
}

void
ReadImage(yuv_info * s_yuv_info,
		  unsigned char * p_Y_space,
		  FILE * fp_input_file,
		  long offset)
{
	unsigned char *p_work;
	int width = s_yuv_info->y_width;
	int height = s_yuv_info->y_height;
	int pixel_size = s_yuv_info->pixel_size;

	fseek(fp_input_file, offset, SEEK_SET);
	p_work = p_Y_space;
	fread(p_work, pixel_size, width * height, fp_input_file);
}

void
WriteImage(yuv_info * s_yuv_info,
		   unsigned char * p_Y_space,
		   FILE * fp_output_file)
{
	int y_width = s_yuv_info->y_width;
	int y_height = s_yuv_info->y_height;
	int frame_size = y_width * y_height;
	int pixel_size = s_yuv_info->pixel_size;

	fwrite(p_Y_space, pixel_size, frame_size, fp_output_file);
}