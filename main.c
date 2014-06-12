//test.exe -i in.yuv -r 2560x1600 -o out.yuv -b 8

#include "yuv.h"
#include <sys/stat.h>
#include <pthread.h>
#include <time.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void * ReadYUVFunction(void * read_thread_data){
	struct YUV_Info * s_yuv_info = ((s_read_thread_data *)read_thread_data)->p_yuv_info;
	unsigned char * p_Y_space = ((s_read_thread_data *)read_thread_data)->p_Y_space;
	FILE * fp_input_file = ((s_read_thread_data *)read_thread_data)->fp_input_file;
	long origin = 0;
	for(long j = 0; j < s_yuv_info->frame_num; j++){
		pthread_mutex_lock(&mutex);
		ReadImage(s_yuv_info, p_Y_space, fp_input_file, origin + j * s_yuv_info->frame_size);
		pthread_mutex_unlock(&mutex);
	}
	return NULL;
}

void * WriteYUVFunction(void * write_thread_data){
	struct YUV_Info * s_yuv_info = ((s_write_thread_data *)write_thread_data)->p_yuv_info;
	unsigned char * p_Y_space = ((s_write_thread_data *)write_thread_data)->p_Y_space;
	FILE * fp_output_file = ((s_write_thread_data *)write_thread_data)->fp_output_file;

	for(long j = 0; j < s_yuv_info->frame_num; j++){
		pthread_mutex_lock(&mutex);
		WriteImage(s_yuv_info, p_Y_space, fp_output_file);
		pthread_mutex_unlock(&mutex);
	}
	return NULL;
}

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

	//long origin = 0;
	//for(long j = 0; j < s_yuv_info->frame_num; j++){
	//	ReadImage(s_yuv_info, p_Y_space, fp_input_file, origin + j * s_yuv_info->frame_size);
	//	WriteImage(s_yuv_info, p_Y_space, fp_output_file);
	//}

	// add 14-06-12
	//pthread_mutex_t mutex;
	//pthread_mutext_init(&mutex, NULL);
	pthread_t t_read_thread, t_write_thread;

	s_read_thread_data read_thread_data;
	//read_thread_data = malloc(sizeof(s_read_thread_data));
	read_thread_data.p_yuv_info = s_yuv_info;
	read_thread_data.p_Y_space = p_Y_space;
	read_thread_data.fp_input_file = fp_input_file;

	s_write_thread_data write_thread_data;
	//write_thread_data = malloc(sizeof(s_write_thread_data));
	write_thread_data.p_yuv_info = s_yuv_info;
	write_thread_data.p_Y_space = p_Y_space;
	write_thread_data.fp_output_file = fp_output_file;

	int ret_val_read = pthread_create(&t_read_thread, NULL, ReadYUVFunction, (void *)&read_thread_data);
	int ret_val_write = pthread_create(&t_write_thread, NULL, WriteYUVFunction, (void *)&write_thread_data);

	pthread_join(t_read_thread, NULL);
	pthread_join(t_write_thread, NULL);

	free(p_Y_space);
	fclose(fp_input_file);
	fclose(fp_output_file);

	finish = time(NULL);
	fprintf(stdout, "time: %f\n", difftime(finish, start));

	return 0;
}