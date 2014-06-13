//test.exe -i in.yuv -r 2560x1600 -o out.yuv -b 8

#include "yuv.h"
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
int
main(int argc, char * argv[]){
	time_t start, finish;
	start = time(NULL);

	struct YUV_Info * s_yuv_info = malloc(sizeof(struct YUV_Info));
	GetInfo(argc, argv, s_yuv_info);

	FILE *fp_input_file = fopen(s_yuv_info->input_file_name, "rb");
	FILE *fp_output_file = fopen(s_yuv_info->output_file_name, "wb");
	if(!fp_input_file){
		fprintf(stderr, "open file error: %s\n", s_yuv_info->input_file_name);
		return EXIT_FAILURE;
	}
	unsigned char * p_Y_space; //<buff for convert per frame
	YUV_Init(s_yuv_info, fp_input_file, &p_Y_space);
	printf("frame_num = %ld\n", s_yuv_info->frame_num);

	long origin = 0;
	for(long j = 0; j < s_yuv_info->frame_num; j++){
		ReadImage(s_yuv_info, p_Y_space, fp_input_file, origin + j * s_yuv_info->frame_size);
		WriteImage(s_yuv_info, p_Y_space, fp_output_file);
	}

	free(p_Y_space);
	free(s_yuv_info);
	fclose(fp_input_file);
	fclose(fp_output_file);

	finish = time(NULL);
	fprintf(stdout, "time: %f\n", difftime(finish, start));

	return 0;
}