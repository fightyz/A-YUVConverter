#include "yuv.h"
#include <unistd.h>
#include <getopt.h>

void
GetInfo(int argc, char * const argv[], struct YUV_Info * s_yuv_info){
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
YUV_Init(struct YUV_Info * s_yuv_info, FILE * fp_input_file, unsigned char ** p_Y_space){
	s_yuv_info->u_width = s_yuv_info->v_width = s_yuv_info->y_width;
	s_yuv_info->u_height = s_yuv_info->v_height = s_yuv_info->y_height;

	// calculate the frame numbers
	fseek(fp_input_file, 0, SEEK_END);
	long file_size = ftell(fp_input_file);
	fseek(fp_input_file, 0, SEEK_SET);
	long frame_size = s_yuv_info->y_width * s_yuv_info->y_height / 2 * 3;
	s_yuv_info->frame_size = frame_size;
	s_yuv_info->frame_num = file_size / frame_size;

	// allocate memory for convert buff Y_space
	*p_Y_space = malloc(s_yuv_info->y_width * s_yuv_info->y_height);
}

void
ReadImage(struct YUV_Info * s_yuv_info, unsigned char * p_Y_space, FILE * fp_input_file, long offset){
	printf("===%ld\n", offset);
	unsigned char *p_work;
	int width = s_yuv_info->y_width;
	int height = s_yuv_info->y_height;

	fseek(fp_input_file, offset, SEEK_SET);
	p_work = p_Y_space;
	for(int i = 0; i < height; i++, p_work += width){
		for(int j = 0; j < width; j++){
			fread(p_work + j, 1, 1, fp_input_file);
		}
	}
}

void
WriteImage(struct YUV_Info * s_yuv_info, unsigned char * p_Y_space, FILE * fp_output_file){
	printf("---\n");
	int y_width = s_yuv_info->y_width;
	int y_height = s_yuv_info->y_height;
	int frame_size = y_width * y_height;

	//fseek(fp_output_file, 0, SEEK_END);

	for(int i = 0; i < frame_size; i++){
		fwrite(p_Y_space + i, 1, 1, fp_output_file);
	}
}