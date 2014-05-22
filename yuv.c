#include "yuv.h"
#include <unistd.h>
#include <getopt.h>

void GetInfo(int argc, char * const argv[], struct YUV_Info * s_yuv_info){
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

void YUV_Init(struct YUV_Info * s_yuv_info){

}