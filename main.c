//test.exe -i in.yuv -r 2560x1600 -o out.yuv -b 8

#include "threads.h"

int
main(int argc, char * argv[]){
	time_t start, finish;
	start = time(NULL);

	yuv_info * s_yuv_info = malloc(sizeof(yuv_info));
	GetInfo(argc, argv, s_yuv_info);

	FILE *fp_input_file_1 = fopen(s_yuv_info->input_file_name, "rb");
	FILE *fp_input_file_2 = fopen(s_yuv_info->input_file_name, "rb");
	FILE *fp_output_file = fopen(s_yuv_info->output_file_name, "wb");
	if(!fp_input_file_1 || !fp_input_file_2){
		fprintf(stderr, "open file error: %s\n", s_yuv_info->input_file_name);
		return EXIT_FAILURE;
	}
	unsigned char * p_Y_space[2];
	YUVInit(s_yuv_info, p_Y_space);
	printf("frame_num = %ld\n", s_yuv_info->frame_num);

	SemaphoreInit();

	pthread_t t_read_thread[2], t_write_thread;

	s_read_thread_data read_thread_data[2];
	SetReadThreadData(&read_thread_data[0], s_yuv_info, p_Y_space[0], fp_input_file_1, 0);
	SetReadThreadData(&read_thread_data[1], s_yuv_info, p_Y_space[1], fp_input_file_2, 1);

	s_write_thread_data write_thread_data;
	SetWriteThreadData(&write_thread_data, s_yuv_info, p_Y_space, fp_output_file);

	int ret_val_read_1 = pthread_create(&t_read_thread[0], NULL, ReadYUVFunction, (void *)&read_thread_data[0]);
	int ret_val_read_2 = pthread_create(&t_read_thread[1], NULL, ReadYUVFunction, (void *)&read_thread_data[1]);
	int ret_val_write = pthread_create(&t_write_thread, NULL, WriteYUVFunction, (void *)&write_thread_data);

	pthread_join(t_read_thread[0], NULL);
	pthread_join(t_read_thread[1], NULL);
	pthread_join(t_write_thread, NULL);

	free(p_Y_space[0]);
	free(p_Y_space[1]);
	free(s_yuv_info);
	fclose(fp_input_file_1);
	fclose(fp_input_file_2);
	fclose(fp_output_file);

	finish = time(NULL);
	fprintf(stdout, "time: %f\n", difftime(finish, start));

	return 0;
}